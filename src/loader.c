/*loader.c - rmld - GPLV3, copyleft 2019 Raphael Outhier;*/


#include <loader.h>

#include <except.h>

#include <string.h>

#define DEBUG 1

#include <debug.h>

/*TODO CHECKS*/
/*TODO CHECKS*/
/*TODO CHECKS*/
/*TODO CHECKS*/
/*TODO CHECKS*/
/*TODO CHECKS*/
/*TODO CHECKS*/

/*------------------------------------------------------------------ internals*/

/**
 * TABLE_ITERATE : iterates on each entry of @table, saving the ref of the
 * current entry in @entry_p;
 */
#define TABLE_ITERATE(table, entry_p) \
    for ((entry_p) = (table).t_start;\
        (usize)(entry_p) < (usize)(table).t_end;\
        (entry_p) = ptr_sum_byte_offset(entry_p,(table).t_bsize)\
    )

/**
 * check_section_index : verifies the provided index is undefined or reserved;
 * @param index : the index to check;
 * @return 1 if the index is reserved or undefined, 0 if it is valid.
 */
static __inline__ u8 check_section_index(u16 index)
{
	/*If the section index in undefined or reserved, assert;*/
	return (u8) ((index == SHN_UNDEF) || (index >= SHN_LORESERVE));
}

/**
 * loader_error : shortcut for error throw;
 * @param env : the loading environment;
 * @param err_type : the type of error to throw;
 */
static __inline__ void loading_error(struct loading_env *env, u8 err_type)
{
	throw_error(env->r_error_ctx, err_type);
}

/*----------------------------------------------------------------- debug only*/


static const char *section_name(
	struct loading_env *env, struct elf64_shdr *shdr
)
{
	
	struct elf64_hdr *hdr;
	struct elf64_shdr *str_hdr;
	
	/*Cache the elf header;*/
	hdr = env->r_hdr;
	
	/*Cache the string section header;*/
	str_hdr = ptr_sum_byte_offset(
		hdr,
		hdr->e_shoff + hdr->e_shstrndx * hdr->e_shentsize
	);
	
	return ptr_sum_byte_offset(hdr, str_hdr->sh_offset + shdr->sh_name);
	
}


static const char *symbol_name(
	struct loading_env *env, struct elf64_shdr *shdr, struct elf64_sym *sym
) {
	
	
	struct elf64_hdr *hdr;
	struct elf64_shdr *str_hdr;
	
	/*Cache the elf header;*/
	hdr = env->r_hdr;
	
	/*Cache the string section header;*/
	str_hdr = ptr_sum_byte_offset(
		hdr,
		hdr->e_shoff + shdr->sh_link * hdr->e_shentsize
	);
	
	return ptr_sum_byte_offset(hdr, str_hdr->sh_offset + sym->sy_name);
	
	
}

/*---------------------------------------------------------------- loader init*/

/**
 * loader_init : initializes the loading environment for the provided elf file;
 * @param env : the environment to initialize;
 * @param ram_start : the address of the file's first byte in RAM;
 * @param file_size : the size in bytes of the file in RAM;
 */
void loader_init(
	struct loading_env *env,
	void *ram_start
)
{
	
	struct elf64_hdr *hdr;
	u8 *shtable;
	usize shentry_size;
	
	/*Initialize the elf header;*/
	env->r_hdr = hdr = ram_start;
	
	/*Determine the address of the section table;*/
	env->r_shtable.t_start = shtable =
		ptr_sum_byte_offset(ram_start, hdr->e_shoff);
	
	/*Determine section table desc vars;*/
	env->r_shtable.t_bsize = shentry_size = hdr->e_shentsize;
	
	/*Determine the end of the section table;*/
	env->r_shtable.t_end =
		ptr_sum_byte_offset(shtable, shentry_size * hdr->e_shnum);
	
}

/*-------------------------------------------------------- sections assignment*/

/**
 * loader_assign_sections : update all section's values to their RAM addresses;
 * @param env : the loading environment;
 * @return 0 if all section were assigned correctly, 1 if an error occurred;
 * if the latter occurs, @err will be initialized with the error context for
 * further debug; any error should halt the loading;
*/
u8 loader_assign_sections(
	struct loading_env *env
)
{
	void *hdr;
	struct elf_table shtable;
	struct elf64_shdr *shdr;
	
	/*Fetch vars;*/
	hdr = env->r_hdr;
	shtable = env->r_shtable;
	
	debug_("loader assigning sections");
	
	/*Iterate over the section table :*/
	TABLE_ITERATE(shtable, shdr) {
		
		void *address;
		u64 offset;
		
		debug("assigning section %s", section_name(env, shdr));
		
		/*If the section is of type nobits, with non-null size, fail;*/
		if ((shdr->sh_type == SHT_NOBITS) && (shdr->sh_size != 0))
			return LOADER_ERROR_NON_EMPTY_NOBITS_SECTION;
		
		/*Fetch the offset and size of the section;*/
		offset = shdr->sh_offset;
		
		/*Use the section address;*/
		address = ptr_sum_byte_offset(hdr, offset);
		
		/*Update the section's address;*/
		shdr->sh_addr = (u64) address;
		
		debug("assigned at %h", address);
		
	}
	
	debug_("loader done assigning sections");
	
	/*Complete;*/
	return 0;
	
}

/*------------------------------------------------------------------ internals*/

/**
 * table_get_entry : if @index is a valid subscript of @table, returns the
 * related entry; if not, throws a loading error;
 * @param env : the loading environment;
 * @param table : the table to query the entry of;
 * @param index : the required table subscript;
 * @return the entry at @index.
 */
static void *__get_table_entry(
	struct loading_env *env,
	struct elf_table *table,
	usize index
)
{
	/*Fetch the block ref;*/
	void *bref = ptr_sum_byte_offset(table->t_start, index * table->t_bsize);
	
	/*If bad index, fail;*/
	if (bref >= table->t_end) {
		loading_error(env, LOADER_ERROR_BAD_TABLE_INDEX);
	}
	
	/*If the index was valid, complete;*/
	return bref;
	
}

/**
 * get_section_header : if @section_id is a valid subscript in the section
 * header table, and if the related header has the required type, returns this
 * header; if one of these checks fail, throws an loading error;
 * @param section_id : a subscript if the section header table;
 * @param section_type : the expected type of the section, 0 to ignore check;
 * @return the ref of the section header at subscript @index;
 */
static struct elf64_shdr *__get_section_header(
	struct loading_env *env,
	u16 section_id,
	u32 section_type
)
{
	
	struct elf64_shdr *shdr;
	
	/*Fetch the ref of the section header;*/
	shdr = __get_table_entry(env, &env->r_shtable, section_id);
	
	/*If type check is enabled and bad type, fail;*/
	if (section_type && (shdr->sh_type != section_type)) {
		loading_error(env, LOADER_ERR_BAD_SECTION_TYPE);
	}
	
	/*Complete;*/
	return shdr;
	
}

/**
 * __section_header_to_table : updates @table with information regarding on
 * the section referred by section header @hdr; if the entry size is null, a
 * loading error is thrown;
 * @param env : the loading environment;
 * @param hdr : the header of the section to get the table of;
 * @param table : the location where to save table data;
 */
static void __section_header_to_table(
	struct loading_env *env,
	struct elf64_shdr *hdr,
	struct elf_table *table,
	u8 byte_table
)
{
	
	void *start;
	usize ent_size;
	
	/*Cache the entry size;*/
	ent_size = hdr->sh_entsize;
	
	/*If the entry size is null, fail or set it to 1;*/
	if (!ent_size) {
		if (byte_table) {
			ent_size = 1;
		} else {
			loading_error(env, LOADER_ERR_TYPE_SECT_ENTSIZE_NULL);
		}
	}
	
	/*Initialize the table;*/
	table->t_start = start = ptr_sum_byte_offset(env->r_hdr, hdr->sh_offset);
	table->t_end = ptr_sum_byte_offset(start, hdr->sh_size);
	table->t_bsize = ent_size;
	
}

/*---------------------------------------------------------- symbol definition*/

/*Search a symbol table for a symbol definition;*/
void *sym_def_find(struct loader_symbol *defs, const char *name)
{
	
	while (defs) {
		
		/*If undefined or names do not match, skip;*/
		if ((!defs->s_defined) || (str_cmp(name, defs->s_name) != 0)) {
			
			defs = defs->s_next;
			continue;
			
		}
		
		/*If names match, return the defined value;*/
		return defs->s_addr;
		
	}
	
	/*If no def was found, return 0;*/
	return 0;
	
}

/*--------------------------------------------------------- symbols assignment*/

/**
 * symbol_update_address : attempts to determine the address (value) of a
 * symbol, from its related section; If the index of the symbol's section is
 * undefined or reserved, the symbol's value is reset to 0.
 * If a file format / access error is detected, the provided context is
 * restored with value 1.
 * @param env : a the loading environment.
 * @param sym : the symbol to update the value of.
 * @param rest : the context to restore in case of elf error.
 */
static void update_symbol_address(
	struct loading_env *env,
	struct elf64_sym *sym
)
{
	
	u16 section_id;
	struct elf64_shdr *shdr;
	u8 bad_index;
	usize value;
	
	/*Reset the value;*/
	value = 0;
	
	/*Fetch the symbol's section's index;*/
	section_id = sym->sy_shndx;
	
	debug("related to section at index %h", section_id);
	
	/*Check the section index;*/
	bad_index = check_section_index(section_id);
	
	/*If the index is invalid :*/
	if (bad_index) {
		
		debug("bad section index : %d", section_id);
		
	} else {
		
		
		/*Get the section header; the section should contain program data*/
		shdr = __get_section_header(env, section_id, 0);
		
		if (shdr->sh_type == SHT_PROGBITS) {
			
			debug("assigning to section : %s", section_name(env, shdr));
			
			/*If the offset is valid determine the symbol's address;*/
			value = sym->sy_value + shdr->sh_addr;
			
		} else {
			
			debug("section %s holds no prog_bits : %d", section_name(env, shdr), shdr->sh_type);
			
		}
		
	}
	
	/*Save the symbol's value;*/
	sym->sy_value = value;
	
}

/**
 * assing_symbol_table : for each symbol in the symbol table :
 * - if the symbol is defined updates the symbol's address internally and
 *   updated the list of symbol queries if required;
 * - if the symbol is not defined, search the list of external definitons
 *   for an eventual matching symbol;
 * It it possible that undefined symbols remain after the execution of this
 * function. Those will have their value assigned to 0;
 * @param env : the loading environment
 * @param symtbl_hdr : symbol table's section header;
 * @param definitions : a list of defined symbols, that are accessible to the
 * executable; if undefined symbols with matching names are found in the
 * executable, their value will be set to the value provided in the list;
 * @param queries : a set of symbols the executable may define; if defined
 * symbols with matching names are found in the executable, the list will be
 * updated with the value of the symbol in the executable;
 */
static void assing_symbol_table(
	struct loading_env *env,
	struct elf64_shdr *sym_table_header,
	struct loader_symbol *definitions,
	struct loader_symbol *queries
)
{
	
	struct elf_table symtable;
	
	u16 str_table_index;
	struct elf64_shdr *str_table_hdr;
	struct elf_table str_table;
	struct elf64_sym *sym;
	
	debug("assigning symbols in %s", section_name(env, sym_table_header));
	
	/*Fetch the symbol table;*/
	__section_header_to_table(env, sym_table_header, &symtable, 0);
	
	/*Fetch the string table id;*/
	str_table_index = (u16) sym_table_header->sh_link;
	
	/*Get the string table section header;*/
	str_table_hdr = __get_section_header(env, str_table_index, SHT_STRTAB);
	
	/*Fetch table data;*/
	__section_header_to_table(env, str_table_hdr, &str_table, 1);
	
	/*Iterate over the symbol table;*/
	TABLE_ITERATE(symtable, sym) {
		
		const char *s_name;
		struct loader_symbol *ext_sym;
		
		/*Fetch the name start;*/
		s_name = __get_table_entry(env, &str_table, sym->sy_name);
		
		/*
		 * Internal symbol definition;
		 */
		
		debug("symbol %s", s_name);
		
		/*If the symbol is undefined :*/
		if (sym->sy_shndx == SHN_UNDEF) {
			
			debug("undefined, searching for external def", s_name);
			
			/*If a definition exists, update the value;
			 * if not, set the symbol's value to 0;*/
			sym->sy_value = (u64) sym_def_find(definitions, s_name);
			
		} else {
			
			/*If the symbol is defined, update its value;*/
			update_symbol_address(env, sym);
			
		}
		
		debug("assigned at %h", sym->sy_value);
		
		/*If the symbol's value is null, stop here;*/
		if (!sym->sy_value) {
			
			continue;
		}
		
		/*
		 * External symbols definition;
		 */
		
		/*Initialise the current symbol;*/
		ext_sym = queries;
		
		/*For each external symbol:*/
		while (ext_sym) {
			
			/*If the symbol is already defined, skip;*/
			/*If symbols names do not match, skip;*/
			if ((ext_sym->s_defined) ||
				(str_cmp(s_name, ext_sym->s_name) != 0)) {
				ext_sym = ext_sym->s_next;
				continue;
				
			}
			
			/*Define the external symbol;*/
			ext_sym->s_defined = 1;
			ext_sym->s_addr = (void *) sym->sy_value;
			
			/*Stop here;*/
			break;
			
		}
		
	}
	
}

/**
 * loader_assign_symbols : for each symbol in the environment :
 * - if the symbol is defined updates the symbol's address internally and
 *   updated the list of symbol queries if required;
 * - if the symbol is not defined, search the list of external definitions
 *   for an eventual matching symbol;
 * It it possible that undefined symbols remain after the execution of this
 * function. Those will have their value assigned to 0;
 * @param env : the loading environment
 * @param definitions : a list of defined symbols, that are accessible to the
 * executable; if undefined symbols with matching names are found in the
 * executable, their value will be set to the value provided in the list;
 * @param queries : a set of symbols the executable may define; if defined
 * symbols with matching names are found in the executable, the list will be
 * updated with the value of the symbol in the executable;
 * @return 0 if all symbols had their value assigned, or, if a symbol table's
 * string table index was invalid (only source of error), the index of the
 * symbol table's section header; this error should stop the loading;
 */
u8 loader_assign_symbols(
	struct loading_env *env,
	struct loader_symbol *defs,
	struct loader_symbol *undefs
)
{
	
	struct elf_table shtable;
	struct elf64_shdr *sheader;
	u8 error_id;
	
	debug_("loader assigning symbols");
	
	try(ctx, error_id) {
			
			/*Update the internal error context;*/
			/*Reset at exception exit, to avoid scope escapism;*/
			env->r_error_ctx = &ctx;
			
			/*Fetch section header table descriptor;*/
			shtable = env->r_shtable;
			
			/*Iterate over the section table :*/
			TABLE_ITERATE(shtable, sheader) {
				
				/*If the section holds a symbol table :*/
				if (sheader->sh_type == SHT_SYMTAB) {
					
					/*Assign symbols in the symbol table;*/
					assing_symbol_table(env, sheader, defs, undefs);
					
				}
				
			}
			
		}
	
	
	try_end
	
	debug_("loader done assigning symbols");
	
	/*Reset the internal error context to avoid scope escapism;*/
	env->r_error_ctx = 0;
	
	/*Return the error id;*/
	return error_id;
	
}

/*--------------------------------------------------------------- relocations */

/**
 * loader_apply_relocation : apply the relocation @rel_type to @rel_addr,
 * regarding symbol at @sym_addr and @addend; If the relocation fails to be
 * applied, the function stops throws the related error;
 * This function is processor-defined;
 * @param rel_addr : the address to apply the relocation to;
 * @param sym_addr : the address of the symbol the relocation concerns;
 * @param addend : the relocation addend, null if none;
 * @param rel_type : the relocation type;
 * @return 0 if the relocation was applied correctly, LOADER_ERROR_REL_BAD_TYPE
 * if bad relocation type, LOADER_ERROR_REL_VALUE_OVERFLOW if relocation value
 * overflow.
 */
u8 loader_apply_relocation(
	u64 rel_addr,
	u64 sym_addr,
	s64 addend,
	u32 rel_type
);

/**
 * apply_reloaction_table : for each relocation in the relocation table,
 * verifies the relocation can be applied (symbol valid and defined), then
 * calls the processor-defined function @loader_apply_relocation, to actually
 * apply the relocation. If a relocation fails to be applied, the function
 * stops throws the related error;
 * @param env : the relocation environment;
 * @param rel_table_hdr : the relocation table header;
 */
static void apply_reloaction_table(
	struct loading_env *env,
	struct elf64_shdr *rel_table_hdr
)
{
	
	u8 explicit_addend;
	struct elf_table reltable;
	u16 symtbl_id;
	
	u16 rel_sect_id;
	struct elf64_shdr *rel_sect_hdr;
	u64 rel_sect_start;
	
	struct elf64_shdr *sym_table_hdr;
	struct elf_table sym_table;
	struct elf64_rela *rel;
	
	debug("applying relocations in %s", section_name(env, rel_table_hdr));
	
	/*Determine whether an explicit addend is provided;*/
	explicit_addend = (u8) (rel_table_hdr->sh_type == SHT_RELA);
	
	/*Fetch the relocation table;*/
	__section_header_to_table(env, rel_table_hdr, &reltable, 0);
	
	/*Fetch the symbol table header identifier;*/
	symtbl_id = (u16) rel_table_hdr->sh_link;
	
	/*Fetch the symbol table;*/
	sym_table_hdr = __get_section_header(env, symtbl_id, SHT_SYMTAB);
	
	debug("using symbols from table %s", section_name(env, sym_table_hdr));
	
	/*Fetch table data;*/
	__section_header_to_table(env, sym_table_hdr, &sym_table, 0);
	
	/*Fetch the index of the section to relocate;*/
	rel_sect_id = (u16) rel_table_hdr->sh_info;
	
	/*Fetch the header of the section to relocate;*/
	rel_sect_hdr = __get_section_header(env, rel_sect_id, SHT_PROGBITS);
	
	debug("updating content of section %s", section_name(env, rel_sect_hdr));
	
	/*Fetch the start and size of the section whose content will be changed;*/
	rel_sect_start = rel_sect_hdr->sh_addr;
	
	/*Iterate over the relocation table;*/
	TABLE_ITERATE(reltable, rel) {
		u64 rel_addr;
		u64 rel_info;
		u32 sym_index;
		u32 rel_type;
		struct elf64_sym *sym;
		u64 sym_addr;
		s64 addend;
		u8 rel_error;
		
		/*Determine the relocation's address;*/
		rel_addr = (u64) ptr_sum_byte_offset(rel_sect_start, rel->r_offset);
		
		/*Fetch relocation information, get index and type;*/
		rel_info = rel->r_info;
		sym_index = ELF64_R_SYM(rel_info);
		rel_type = ELF64_R_TYPE(rel_info);
		
		/*If the symbol's index is null :*/
		if (!sym_index)
			loading_error(env, LOADER_ERROR_REL_SYMBOL_NULL_INDEX);
		
		/*Fetch the symbol reference;*/
		sym = __get_table_entry(env, &sym_table, sym_index);
		
		/*Fetch the symbol's value;*/
		sym_addr = sym->sy_value;
		
		/*If the symbol's address is null :*/
		if (!sym_addr)
			loading_error(env, LOADER_ERROR_REL_SYMBOL_NULL_ADDRESS);
		
		/*Initialise the addend;*/
		addend = (explicit_addend) ? rel->r_addend : 0;
		
		debug("applying relocation of type %d at %h with addend %h using "
				  "symbol %s of value %h ",rel_type, rel_addr, addend,
			  symbol_name(env, sym_table_hdr, sym), sym_addr);
		
		/*Apply the relocation;*/
		rel_error = loader_apply_relocation(
			rel_addr, sym_addr, addend, rel_type
		);
		
		/*If the relocation failed, throw an error;*/
		if (rel_error) {
			loading_error(env, rel_error);
		}
		
	}
	
}

/**
 * loader_apply_relocations : for each relocation in the environment, verifies
 * the relocation can be applied (symbol valid and defined), then calls the
 * processor-defined function @loader_apply_relocation, to actually apply the
 * relocation.
 * If a relocation fails to be applied, the function stops;
 * @param env : the relocation environment;
 * @param reltbl_hdr : the relocation table header;
 * @return an loading error code;
 */
u8 loader_apply_relocations(struct loading_env *env)
{
	
	struct elf_table shtable;
	struct elf64_shdr *shdr;
	u8 error_id;
	
	/*Fetch the symbol table descriptor;*/
	shtable = env->r_shtable;
	
	debug_("loader applying relocations");
	
	try(ctx, error_id) {
			
			/*Update the internal error context;*/
			/*Reset at exception exit, to avoid scope escapism;*/
			env->r_error_ctx = &ctx;
			
			/*Iterate over the section table :*/
			TABLE_ITERATE(shtable, shdr) {
				
				u32 sh_type;
				
				/*Fetch the section type;*/
				sh_type = shdr->sh_type;
				
				/*If the section contains a relocation table :*/
				if ((sh_type == SHT_REL) || (sh_type == SHT_RELA)) {
					
					/*Attempt to apply relocations;*/
					apply_reloaction_table(env, shdr);
					
				}
				
			}
			
		}
	
	try_end
	
	debug_("loader done applying relocations");
	
	/*Reset the internal error context to avoid scope escapism;*/
	env->r_error_ctx = 0;
	
	/*Complete;*/
	return 0;
	
}
