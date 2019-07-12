/*loader.h - rmld - GPLV3, copyleft 2019 Raphael Outhier;*/

#ifndef KERNEL_TK_LOADER_H
#define KERNEL_TK_LOADER_H

#include <exec.h>

#include <elf64.h>

/**
 * The byte table struct contains data to describe an abstract byte table,
 * that contains a given number of entries, of a constant size;
 * The table must not start at address 0;
 */
struct elf_table {

	/*Address of the lowest byte of the table; must not be 0;*/
	void *t_start;

	/*Address of the table's last byte's successor;*/
	void *t_end;

	/*Size of one block in the table;*/
	usize t_bsize;

};

/**
 * The loader symbol struct either provides or receives definition for a symbol
 * in memory;
 */
struct loader_symbol {
	
	/*symbols are referenced in a linked list;*/
	struct loader_symbol *s_next;
	
	/*The address of the symbol;*/
	void *s_addr;
	
	/*A flag, set if the symbol is defined;*/
	u8 s_defined;
	
	/*The name of the symbol;*/
	const char *s_name;
	
	
};

/*
 * Loading error codes;
 */

/*A non-empty no-bits section was encountered;*/
#define LOADER_ERROR_NON_EMPTY_NOBITS_SECTION ((u8) 1)

/*A table was queried with an invalid index;*/
#define LOADER_ERROR_BAD_TABLE_INDEX ((u8) 2)

/*A section header was found with an unexpected type;*/
#define LOADER_ERR_BAD_SECTION_TYPE ((u8) 3)

/*A section header's entry size was unexpectedly null;*/
#define LOADER_ERR_TYPE_SECT_ENTSIZE_NULL ((u8) 4)

/*A relocation symbol had a null value;*/
#define LOADER_ERROR_REL_SYMBOL_NULL_INDEX ((u8) 5)

/*A relocation symbol had a null address;*/
#define LOADER_ERROR_REL_SYMBOL_NULL_ADDRESS ((u8) 6)

/*A relocation symbol had a null address;*/
#define LOADER_ERROR_REL_BAD_TYPE ((u8) 7)

/*A relocation symbol had a null address;*/
#define LOADER_ERROR_REL_VALUE_OVERFLOW ((u8) 8)


/**
 * The loading environment contains data related to a relocatable elf file
 * that must be loaded into memory;
 */
struct loading_env {

	/*The elf header;*/
	struct elf64_hdr *r_hdr;

	/*Section header table descriptor;*/
	struct elf_table r_shtable;
	
	/*The an internal context to restore in case of internal error;*/
	struct rest_ctx *r_error_ctx;

};

/**
 * loader_init : initializes the loading environment for the provided elf file;
 * @param env : the environment to initialize;
 * @param ram_start : the address of the file's first byte in RAM;
 * @param file_size : the size in bytes of the file in RAM;
 */
void loader_init(
		struct loading_env *env,
		void *ram_start
);

/**
 * loader_assign_sections : update all section's values to their RAM addresses;
 * @param env : the loading environment;
 * @param env : an error descriptor;
 * @return 0 if all section were assigned correctly, 1 if an error occurred;
 * if the latter occurs, @err will be initialized with the error context for
 * further debug; any error should halt the loading;
*/
u8 loader_assign_sections(
		struct loading_env *env
);

/**
 * loader_assign_symbols : for each symbol in the environment :
 * - if the symbol is defined updates the symbol's address internally and
 *   updated the list of symbol queries if required;
 * - if the symbol is not defined, search the list of external definitions
 *   for an eventual matching symbol;
 * It is possible that undefined symbols remain after the execution of this
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
);

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
u8 loader_apply_relocations(struct loading_env *env);


#endif /*KERNEL_TK_LOADER_H*/
