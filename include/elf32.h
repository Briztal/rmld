/*elf32.h - rmld - GPLV3, copyleft 2019 Raphael Outhier;*/


#ifndef KERNEL_TK_ELF32_H
#define KERNEL_TK_ELF32_H

#include <elf.h>

/*--------------------------- elf types -------------------------------------*/

/*Unsigned program address, depends on the elf type;*/
#define elf32_addr u32

/*Unsigned file offset, depends on the elf type;*/
#define elf32_off u32


/*------------------------------ elf header ---------------------------------*/

/**
 * elf32_hdr : the elf32 file header, located at the beginning of the file;
 */
 
struct elf32_hdr {

	/*The elf identifier;*/
	struct elf_identifier e_ident;
	
	/*The type of elf file;*/
	u16 e_type;
	
	/*The required architecture;*/
	u16 e_machine;

	/*The elf file version;*/
	u32 e_version;
	
	/*The virtual address at which the system transfers control;*/
	elf32_addr e_entry;

	/*The program header table offset from the beginning of the file;*/
 	elf32_off e_phoff;

	/*The section header table offset, from the beginning of the file;*/
	elf32_off e_shoff;

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
		
	/*The index in the section header table of the string table header;*/
	u16 e_shstrndx;

};

/*Change the endianness of an elf header;*/
void elf32_hdr_change_endian(struct elf32_hdr *);

/*Check an elf header;*/
elf_hdcheck_t elf32_hdr_check(const struct elf32_hdr *);

/*Get the size in bytes of the elf section table;*/
#define elf32_hdr_get_stable_size(hdr) ((hdr)->e_shentsize * (hdr)->e_shnum)

/*Get the size in bytes of the elf program table;*/
#define elf32_hdr_get_ptable_size(hdr) ((hdr)->e_phentsize * (hdr)->e_phnum)


/*----------------------------- program header ------------------------------*/

/**
 * elf32_phdr : the format of a program segment header in a elf32 file; 
 */

struct elf32_phdr {

	/*The type of segment related to this header;*/
	u32 p_type;

	/*The segment's first byte offset from the beginning of the file;*/
	elf32_off p_offset;
	
	/*The virtual address at which the first byte of the segment resides;*/
	elf32_addr p_vaddr;

	/*The segment's physical address;*/
	elf32_addr p_paddr;

	/*The number of bytes in the on-file image of the segment;*/
	u32 p_filesz;

	/*The number of bytes in the on-memory image of the segment;*/
	u32 p_memsz;

	/*A bit-mask of flags relevant to the segment;*/
	u32 p_flags;

	/*The value to which the segments are aligned*/
	u32 p_align;

};


/*Change the endianness of x program header;*/
void elf32_phdr_change_endian(struct elf32_phdr *);


/*--------------------------- section header --------------------------------*/

/**
 * elf32_shdr : the format of a section header in an elf32_file;
 */

struct elf32_shdr {

	/*The index of the related section name in the string table;*/
	u32 sh_name;

	/*The type of the section the header refers;*/
	u32 sh_type;

	/*A bit mask flag set;*/
	u32 sh_flags;

	/*The address at which the first byte of the section should reside;*/
	elf32_addr sh_addr;

	/*The related section's first byte's offset from the file's start;*/
	elf32_off sh_offset;

	/*The section's size in bytes;*/
	u32 sh_size;

	/*A section header table index, meaning depends on the section type;*/
	u32 sh_link;

	/*Extra information, meaning depends on the section type;*/ 
	u32 sh_info;

	/*The alignment size of the section;*/	
	u32 sh_addralign;

	/*If the related section contains fixed size entries, their size;*/
	u32 sh_entsize;

};


/*Change the endianness of x section header;*/
void elf32_shdr_change_endian(struct elf32_shdr *);


/*---------------------------- symbol entries --------------------------*/

/**
 * elf32_sym : the format of an entry in the symbol table of an elf32 file;
 */

struct elf32_sym {

	/*The offset in the string table of the symbol's name;*/
	u32 sy_name;

	/*The value of the associated symbol;*/
	elf32_addr sy_value;

	/*The size if relevant of the symbol;*/
	u32 sy_size;

	/*The symbol's type and binding attribute;*/
	u8 sy_info;

	/*The symbols's visibility;*/
	u8 sy_visibility;
	
	/*The section the symbol is related to;*/
	u16 sy_shndx;

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
 * elf32_rel : a relocation structure that do not require an addend, in an elf32 file;
 */

struct elf32_rel {
	elf32_addr r_offset;
	u32 r_info;
};


/**
 * elf32_rela : a relocation structure that require an addend, in an elf32 file;
 */

struct elf32_rela {
	elf32_addr r_offset;
	u32 r_info;
	s32 r_addend;
};


#endif
