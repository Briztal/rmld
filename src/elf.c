/*elf.c - rmld - GPLV3, copyleft 2019 Raphael Outhier;*/

#include <types.h>

#include <elf.h>


/**
 * elf_identifier_check : checks an elf identifier, to detect errors, and
 * 	analyse the elf file;
 */

rel_error_t elf_ident_check(struct elf_identifier *ident, u8 elf_class)
{
	
	u16 ident_lsb;
	u8 ei_data;
	u8 proc_little_endian;
	u8 elf_little_endian;
	
	/*The return flags variable;*/
	rel_error_t flags = 0;
	
	/*Sets bits in the flag variable and returns it;*/
	#define SET_RETURN(mask) {flags |= (mask);return flags;}
	
	
	/*If magic numbers do not match :*/
	if ((ident->ei_mag0 != ELFMAG0) || (ident->ei_mag1 != ELFMAG1) ||
		(ident->ei_mag2 != ELFMAG2) || (ident->ei_mag3 != ELFMAG3)) {
		
		/*Set the related flag;*/
		SET_RETURN(REL_ERROR_BAD_MAGIC_NUMBERS)
		
	}
	
	/*Fetch ei_data;*/
	ei_data = ident->ei_data;
	
	/*If the data format is unknown :*/
	if (!ei_data) {
		
		/*Set the related flag and return;*/
		SET_RETURN(REL_ERROR_UNKNOWN_DATA_FORMAT)
		
	}
	
	/*If bad elf class, set the related flag;*/
	if (ident->ei_class != elf_class) {
		flags |= REL_ERROR_BAD_ELF_CLASS;
	}

	/*
	 * Endianness; This check supposes that any software implementation of 
	 * 	integer numbers (ex 64 bits ints on 32 bits machine) follows
	 * 	the exact same endianness than the machine; If not, this code 
	 * 	can't be run on the platform;
	 */
	
	/*Read the two first bytes of elf_ident;*/
	ident_lsb = *((u16 *) (ident));
	
	/*little endian : lsb of ident_lsb is ELFMAG0*/
	proc_little_endian = (u8) (((u8) (ident_lsb)) == ELFMAG0);
	
	/*Fetch elf endianness;*/
	elf_little_endian = (u8) (ei_data == ELFDATA2LSB);
	
	/*If endiannesses differ, set the related flag;*/
	if (proc_little_endian != elf_little_endian) {
		flags |= REL_ERROR_BAD_ENDIANNESS;
	}
	
	
	/*Return the flags variable;*/
	return flags;
	
	/*Cleanup;*/
	#undef SET_RETURN
	
}


