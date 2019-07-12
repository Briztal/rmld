/*elf64.h - rmld - GPLV3, copyleft 2019 Raphael Outhier;*/

#ifndef KERNEL_TK_ELF64_H
#define KERNEL_TK_ELF64_H

#include <elf.h>

/*-------------------------------------------- elf types ---------------------------------------------*/

/*Unsigned program address, depends on the elf type;*/
#define elf64_addr u64

/*Unsigned file offset, depends on the elf type;*/
#define elf64_off u64



/*-------------------------------------------- elf header ---------------------------------------------*/

/**
 * elf64_hdr : the elf64 file header, located at the beginning of the file;
 */

struct elf64_hdr {
	
	/*The elf identifier;*/
	struct elf_identifier e_ident;
	
	/*The type of elf file;*/
	u16 e_type;
	
	/*The required architecture;*/
	u16 e_machine;
	
	/*The elf file version;*/
	u32 e_version;
	
	/*The virtual address at which the system transfers control;*/
	elf64_addr e_entry;
	
	/*The program header table offset from the beginning of the file;*/
	elf64_off e_phoff;
	
	/*The section header table offset, from the beginning of the file;*/
	elf64_off e_shoff;
	
	/*Processor specific flags;*/
	u32 e_flags;
	
	/*The elf header size in bytes*/
	u16 e_ehsize;
	
	/*The size in bytes of one entry in the program header table;*/
	u16 e_phentsize;
	
	/*The size in entries of the program header table;*/
	u16 e_phnum;
	
	/*The size in bytes of one entry in the section header table;*/
	u16 e_shentsize;
	
	/*The size in entries of the section header table;*/
	u16 e_shnum;
	
	/*The index in the section header table of the section header string table;*/
	u16 e_shstrndx;
	
};

/*-------------------------------------------- program header --------------------------------------------*/

/**
 * elf64_phdr : the format of a program segment header in a elf64 file;
 */

struct elf64_phdr {
	
	/*The type of segment related to this header;*/
	u32 p_type;
	
	/*A bit-mask of flags relevant to the segment;*/
	u32 p_flags;
	
	/*The offset from the beginning of the file at which the first byte of the segment resides;*/
	elf64_off p_offset;
	
	/*The virtual address at which the first byte of the segment resides;*/
	elf64_addr p_vaddr;
	
	/*The segment's physical address;*/
	elf64_addr p_paddr;
	
	/*The number of bytes in the on-file image of the segment; may be 0 (ex : segment containing only .bss);*/
	u64 p_filesz;
	
	/*The number of bytes in the on-memory image of the segment; may be 0;*/
	u32 p_memsz;
	
	/*The value to which the segments are aligned in memory and in the file;*/
	u32 p_align;
	
};

/*-------------------------------------------- section header ---------------------------------------------*/

/**
 * elf64_shdr : the format of a section header in an elf64_file;
 */

struct elf64_shdr {
	
	/*The index of the related section name in the string table;*/
	u32 sh_name;
	
	/*The type of the section the header refers;*/
	u32 sh_type;
	
	/*A bit mask flag set;*/
	u64 sh_flags;
	
	/*The address at which the first byte of the section should reside;*/
	elf64_addr sh_addr;
	
	/*The byte offset from the beginning of the file of the related section's first byte;*/
	elf64_off sh_offset;
	
	/*The section's size in bytes;*/
	u64 sh_size;
	
	/*A section header table index, whose interpretation depends on the section type;*/
	u32 sh_link;
	
	/*Extra information, whose interpretation depends on the section type;*/
	u32 sh_info;
	
	/*The alignment size of the section;*/
	u64 sh_addralign;
	
	/*If the related section contains fixed size entries, the size of each entry;*/
	u64 sh_entsize;
	
};

/*-------------------------------------------- symbol entries ---------------------------------------------*/

/**
 * elf64_sym : the format of an entry in the symbol table of an elf64 file;
 */

struct elf64_sym {
	
	/*The offset in the string table of the symbol's name;*/
	u32 sy_name;
	
	/*The symbol's type and binding attribute;*/
	u8 sy_info;
	
	/*The symbols's visibility;*/
	u8 sy_visibility;
	
	/*The section the symbol is related to;*/
	u16 sy_shndx;
	
	/*The value of the associated symbol;*/
	elf64_addr sy_value;
	
	/*The size if relevant of the symbol;*/
	u64 sy_size;
	
};


/*------------------------------------------ relocation entries --------------------------------------------*/

/*
 * Elf relocations have at most three attributes, whose descriptions are listed below : 
 * 
 * - r_offset ! This member gives the location at which to apply the relocation action.  
 *  For x relocatable file, the value is the byte offset from the beginning of the section to the storage
 *  unit affected by the relocation. For an executable file or shared object, the value is the virtual 
 *  address of the storage unit affected by the relocation.
 * 
 * - r_info : This member gives both the symbol table index with respect to which the relocation must be 
 * 	made and the type of relocation to apply. Relocation types are processor-specific. When the text refers
 *  to x relocation entry's relocation type or symbol table index, it means the result of applying
 * 	ELF[32|64]_R_TYPE or ELF[32|64]_R_SYM, respectively, to the entry's r_info member.
 * 
 * - r_addend : this member specifies x constant addend used to compute the value to be stored into the
 * 	relocatable field.
 */

/**
 * elf64_rel : a relocation structure that do not require an addend, in an elf64 file;
 */

struct elf64_rel {
	elf64_addr r_offset;
	u64 r_info;
};

/**
 * elf64_rela : a relocation structure that require an addend, in an elf64 file;
 */

struct elf64_rela {
	elf64_addr r_offset;
	u64 r_info;
	s64 r_addend;
};


#define ELF64_R_SYM(info) ((u32) ((info) >> 32))

#define ELF64_R_TYPE(info) ((u32) (info))

#define ELF64_R_INFO(sym, type) ((((u64) (sym))<<32) + (u64) (u32)(type) )



#endif /*KERNEL_TK_ELF64_H*/
