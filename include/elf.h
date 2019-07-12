/*elf.h - rmld - GPLV3, copyleft 2019 Raphael Outhier;*/

#ifndef KERNEL_TK_ELF_H
#define KERNEL_TK_ELF_H

#include <types.h>

/*------------------------------ elf identifier -----------------------------*/

/**
 * elf_identifier : the commont part for elf32 and elf64 file formats.
 * 	Any kind of elf file starts with this structure, which allows unified 
 * 	first interaction; It is firstly loaded, to determine the exact file 
 * 	format and to verify the compatibilty with the system;
 */

struct elf_identifier {
	
	/*Bytes 0 - 3 : magic numbers; must be '0x77f', 'E', 'L' and 'F':*/	
	u8 ei_mag0;
	u8 ei_mag1;
	u8 ei_mag2;
	u8 ei_mag3;

	/*Byte 4 : the architecture the related binary concerns;*/
	u8 ei_class;

	/*Byte 5 : data encoding of processor-specific data in file;*/
	u8 ei_data;

	/*Byte 6 : the version number of the elf specification;*/
	u8 ei_version;

	/*Byte 7 : the targetted OS ABI;*/
	u8 ei_osabi;

	/*Byte 8 : the tagetted OS ABI version;*/
	u8 ei_abiversion;

	/*Byte 9 - 15 : rfu*/
	u8 ei_padding[7];

};


/**
 * Elf Magic numbers; expected values for ei_mag bytes;
 */

#define ELFMAG0 ((u8) (0x7f))
#define ELFMAG1 ((u8) 'E')
#define ELFMAG2 ((u8) 'L')
#define ELFMAG3 ((u8) 'F')


/**
 * ei class values;
 */

/*Unknown class;*/
#define ELFCLASSNONE ((u8) 0)

/*32 bit architecture;*/
#define ELFCLASS32 ((u8) 1)

/*64 bit architecture;*/
#define ELFCLASS64 ((u8) 2)


/**
 * ei data formats;
 */

/*Unknown data format*/
#define ELFDATANONE 0      /* e_ident[EI_DATA] */

/*Two's complement, little endian;*/
#define ELFDATA2LSB 1

/*Two's complement, little endian;*/
#define ELFDATA2MSB 2


/**
 * ei version;
 */

/*Invalid version;*/
#define EV_NONE    0       /* e_version, EI_VERSION */

/*Current version;*/
#define EV_CURRENT      1


/*
 * ei OS ABI;
 */

#define ELFOSABI_NONE       0
#define ELFOSABI_LINUX      3


/*
 * loading_env init status;
 */

/*The type returned when a loading_env struct is initialsed;*/
#define rel_error_t u8

/*If set, elf magic numbers are incorrect;*/
#define REL_ERROR_BAD_MAGIC_NUMBERS ((rel_error_t) (1 << 0))

/*If set, the elf class is unknown;*/
#define REL_ERROR_UNKNOWN_CLASS ((rel_error_t) (1 << 1))

/*If set, the data format is unknown;*/
#define REL_ERROR_UNKNOWN_DATA_FORMAT ((rel_error_t) (1 << 2))

/*If set, the elf file endianness is incompatible with the architecture;*/
#define REL_ERROR_BAD_ENDIANNESS ((rel_error_t) (1 << 3))

/*If set, the elf class is incompatible with the called initializer;*/
#define REL_ERROR_BAD_ELF_CLASS ((rel_error_t) (1 << 4))


/*Check an elf identifier;*/
rel_error_t elf_identifier_check(struct elf_identifier *, u8 elf_64);


/*-------------------------- elf header constants ---------------------------*/

/*
 * Elf files types : 
 */

/*Unknown file type;*/
#define ET_NONE 0

/*Relocatable file;*/
#define ET_REL 1

/*Executable file;*/
#define ET_EXEC 2

/*Shared object;*/
#define ET_DYN 3

/*Core file;*/ 
#define ET_CORE 4


/*
 * Elf machine types : copy of linux/elf.h
 */

/* These constants define the various ELF target machines */
#define EM_NONE     0
#define EM_M32      1
#define EM_SPARC    2
#define EM_386      3
#define EM_68K      4
#define EM_88K      5
#define EM_486      6   /* Perhaps disused */
#define EM_860      7
#define EM_MIPS     8   /* MIPS R3000 (officially, big-endian only) */
                /* Next two are historical and binaries and
                   modules of these types will be rejected by
                   Linux.  */
#define EM_MIPS_RS3_LE  10  /* MIPS R3000 little-endian */
#define EM_MIPS_RS4_BE  10  /* MIPS R4000 big-endian */

#define EM_PARISC   15  /* HPPA */
#define EM_SPARC32PLUS  18  /* Sun's "v8plus" */
#define EM_PPC      20  /* PowerPC */
#define EM_PPC64    21   /* PowerPC64 */
#define EM_SPU      23  /* Cell BE SPU */
#define EM_ARM      40  /* ARM 32 bit */
#define EM_SH       42  /* SuperH */
#define EM_SPARCV9  43  /* SPARC v9 64-bit */
#define EM_H8_300   46  /* Renesas H8/300 */
#define EM_IA_64    50  /* HP/Intel IA-64 */
#define EM_X86_64   62  /* AMD x86-64 */
#define EM_S390     22  /* IBM S/390 */
#define EM_CRIS     76  /* Axis Communications 32-bit embedded processor */
#define EM_M32R     88  /* Renesas M32R */
#define EM_MN10300  89  /* Panasonic/MEI MN10300, AM33 */
#define EM_OPENRISC     92     /* OpenRISC 32-bit embedded processor */
#define EM_BLACKFIN     106     /* ADI Blackfin Processor */
#define EM_ALTERA_NIOS2 113 /* Altera Nios II soft-core processor */
#define EM_TI_C6000 140 /* TI C6X DSPs */
#define EM_AARCH64  183 /* ARM 64 bit */
#define EM_TILEPRO  188 /* Tilera TILEPro */
#define EM_MICROBLAZE   189 /* Xilinx MicroBlaze */
#define EM_TILEGX   191 /* Tilera TILE-Gx */
#define EM_BPF      247 /* Linux BPF - in-kernel virtual machine */
#define EM_FRV      0x5441  /* Fujitsu FR-V */


/*
 * Elf Versions : 
 */

/*Invalid version;*/
#define EV_NONE     0

/*Current version;*/
#define EV_CURRENT  1

/*Unknown value. TODO;*/
#define EV_NUM      2


/*
 * elf header check;
 */

/*The type of an elf header check variable;*/
#define elf_hdcheck_t u8

/*If set, the type of elf file is unknown;*/
#define ELF_HDCHECK_UNKNOWN_TYPE ((elf_idcheck_t) (1 << 0))

/*If set, the elf header size is lesser than a standard elf header size;*/
#define ELF_HDCHECK_INVALID_HEADER_SIZE ((elf_idcheck_t) (1 << 1))


/*----------------------- program header constants -----------------------*/ 

/*
 * Program header types :  
 */

/*Unused program header;*/
#define PT_NULL    0

/*Segment contains loadable data;*/
#define PT_LOAD    1

/*Segment contains dynamic linking information;*/
#define PT_DYNAMIC 2

/*Segment specifies the location of the program interpreter;*/
#define PT_INTERP  3

/*Specifies the location of notes;*/
#define PT_NOTE    4

/*Reserved, segment has unspecified sepantics;*/ 
#define PT_SHLIB   5

/*Segment contains the program header table;*/
#define PT_PHDR    6

/*Segment contains thread local data;*/
#define PT_TLS     7

/*Range reserved for OS specifics semantics;*/
#define PT_LOOS    0x60000000
#define PT_HIOS    0x6fffffff

/*Range reserved for processor specifics semantics;*/
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff

/*GNU extensions;*/
#define PT_GNU_EH_FRAME     0x6474e550
#define PT_GNU_STACK    (PT_LOOS + 0x474e551)


/*
 * Program header flags;
 */

/*Executable segment;*/
#define PF_X        (1 << 0)

/*Writable segent;*/
#define PF_W        (1 << 1)

/*Readable segment:*/
#define PF_R        (1 << 2)


/*------------------------  section header constants ------------------------*/

/* 
 * Special section indexes :
 */

/*Meaningless section reference;*/
#define SHN_UNDEF   0

/*Reserved index range;*/
#define SHN_HIRESERVE   0xffff
#define SHN_LORESERVE   0xff00

/*Processor reserved index range;*/
#define SHN_LOPROC  0xff00
#define SHN_HIPROC  0xff1f

/*Speifies the absolute value for the corresponding reference;*/
#define SHN_ABS     0xfff1

/*Common symbols;*/
#define SHN_COMMON  0xfff2


/* 
 * Section header types; 
 */

/*Inactive section header;*/
#define SHT_NULL 0

/*Section holds program data;*/
#define SHT_PROGBITS 1

/*Section holds a symbol table;*/
#define SHT_SYMTAB 2

/*Section holds a string table;*/
#define SHT_STRTAB 3

/*Section holds relocation entries requiring explicit addend;*/
#define SHT_RELA 4

/*Section holds a symbol hash table;*/
#define SHT_HASH 5

/*Section holds data required for dynamic linking;*/
#define SHT_DYNAMIC 6

/*Section holds notes;*/
#define SHT_NOTE 7

/*Like SHT_PROGBITS but section occupies no space in file image;*/
#define SHT_NOBITS 8

/*Section holds relocation entries requiring no explicit addend;*/
#define SHT_REL 9

/*Reserved, has no specified semantics;*/
#define SHT_SHLIB 10

/*Section holds synamic linking symbols;*/
#define SHT_DYNSYM 11

/*Unknown section type;*/
#define SHT_NUM 12

/*Range reserves for processor-specified semantics;*/
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7fffffff

/*Range reserved for application programs;*/
#define SHT_LOUSER 0x80000000
#define SHT_HIUSER 0xffffffff


/*
 * Section header flags;
 */

/*Section contains data that should be writable during process execution;*/
#define SHF_WRITE (1 << 0)

/*Section occupies memory during process execution;*/
#define SHF_ALLOC (1 << 1)

/*Section contains executable machine instructions;*/
#define SHF_EXECINSTR (1 >> 2)

/*Reserved flags;*/
#define SHF_MASKPROC 0xf0000000


/*------------------------ symbol entries constants -------------------------*/

/*
 * Symbol (info) types : occupy the four lsbits of sy_info;
 */

/*The symbol's type is not defined;*/ 
#define SYT_NOTYPE  0

/*The symbol is associated with data objects;*/
#define SYT_OBJECT  1

/*The symbol is associated with a function of executable code;*/
#define SYT_FUNC    2

/*The symbol is associated with a section;*/
#define SYT_SECTION 3

/*The symbol is related to a file; See 'man elf' for details;*/
#define SYT_FILE    4


/*
 * Symbol (info) bindings : occupies the four msbits of sy_info;
 */

/*The symbol not visible outside the object file containing its definition;*/
#define SYB_LOCAL  0

/*The symbol is visible to all objects being combined;*/
#define SYB_GLOBAL 1

/*Global symbol with lower precedence;*/
#define SYB_WEAK   2


/*
 * sy_info <-> (sy_type, sy_bind)
 */

/*Extract sy_type from sy_info;*/
#define ELF_SY_INFO_TO_BIND(info)\
	((u8) ((u8)(0x0f) & (((u8)(info)) >> 4)))

/*Extract sy_bind form sy_info;*/
#define ELF_SY_INFO_TO_TYPE(info)\
	((u8) ((u8) (0x0f) & ((u8)(info))))

/*Combine sy_bind and sy_type to sy_info;*/
#define ELF_SY_BIND_TYPE_TO_INFO(bind, type)\
	((u8) (((0x0f & ((u8)(bind))) << 4) \
					| (0x0f & ((u8)(info)))))


/*
 * Symbol visibility:
 */

/*Symbol is visible if its binding is WEAK or GLOBAL;*/
#define SYV_DEFAULT 0

/*Processor specific hidden class;*/
#define SYV_INTERNAL 1

/*Symbol is unavailable to local module;*/
#define SYV_HIDDEN 2

/*Symbol is available to other modules, 
 * but reference in the local module resolve to the local symbol;*/
#define SYV_PROTECTED 3



#endif /*KERNEL_TK_ELF_H*/
