#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <elf.h>
#include <loader.h>

#define FILE_NAME "test/test.o"

#define handle_error(msg) { printf("%s error;\n",msg); exit(1); }

u32 a;
u32 b;
u32 c;

int main(int argc, char *argv[])
{
	
	void *addr;
	int fd;
	struct stat sb;
	usize file_size;
	struct loader_symbol prtf;
	struct loader_symbol func;
	struct loading_env rel;
	u8 error;
	u32 (*fnc)(void);
	u32 res;
	
	fd = open(FILE_NAME, O_RDONLY);
	
	if (fd == -1) handle_error("open")
	
	if (fstat(fd, &sb) == -1)    /* To obtain file size */
	handle_error("fstat")
	
	file_size = (usize) sb.st_size;
	
	addr = mmap(NULL, file_size, PROT_WRITE | PROT_READ | PROT_EXEC,
				MAP_PRIVATE, fd, 0);
	
	if (addr == MAP_FAILED) handle_error("mmap")
	
	
	if (((struct elf_identifier *) addr)->ei_class != ELFCLASS64) {
		
		handle_error("elf class");
		
	}
	
	prtf.s_defined = 1;
	prtf.s_addr = (void*)&printf;
	prtf.s_next = 0;
	prtf.s_name = "printf";
	
	func.s_defined = 0;
	func.s_addr = 0;
	func.s_next = 0;
	func.s_name = "func";
	
	printf("hdr : %p\n", addr);
	
	loader_init(&rel, addr);
	
	error = loader_assign_sections(&rel);
	
	printf("sections allocations : %d\n", error);
	
	error = loader_assign_symbols(&rel, &prtf, &func);
	
	printf("symbols assignment : %d\n", error);
	
	error = loader_apply_relocations(&rel);
	
	printf("rellocation application : %d\n", error);
	
	printf("func : %p\n", func.s_addr);
	
	fnc = func.s_addr;
	
	printf("calling :\n");
	
	res = (*fnc)();
	
	printf("called : %d\n", res);
	
	close(fd);
	
	exit(EXIT_SUCCESS);
	
}


void ns_log(const char *str) {
	printf("%s", str);
}

void ns_abort() {
	abort();
}
