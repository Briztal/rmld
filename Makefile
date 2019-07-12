include /root/projects/mftk/mftk.mk

$(eval $(call mftk.utility.define,toolchain,target,x86_64))
$(eval $(call mftk.utility.execute,toolchain))

INC := -Ibuild/nostd/include -Ibuild/rmld/include

CFLAGS += -std=c89 -Wall -pedantic -O3

TCC := $(CC) $(INC) $(CFLAGS)

TS_BDIR = build/test

$(eval $(call mftk.node.define,nostd,0,build_dir,$(.wdir)/build/nostd))
$(eval $(call mftk.node.define,nostd,0,build_arch,x86_64))
$(eval $(call mftk.node.define,nostd,0,debug,1))

$(eval $(call mftk.node.define,rmld,0,nostd_inc_dir,$(.wdir)/build/nostd/include))
$(eval $(call mftk.node.define,rmld,0,build_dir,$(.wdir)/build/rmld))
$(eval $(call mftk.node.define,rmld,0,build_arch,x86_64))
$(eval $(call mftk.node.define,rmld,0,debug,1))

rmld.nostd.ar :
	rm -rf build/nostd
	$(call mftk.node.execute,nostd,0,all,)

rmld.ar :
	rm -rf build/rmld
	$(call mftk.node.execute,rmld,0,rmld,)

clean:
	rm -rf build

test: clean rmld.nostd.ar rmld.ar
	$(TCC) -o test/main.o -c test/main.c
	$(TCC) -o test/main.elf test/main.o build/rmld/rmld.ar build/nostd/nostd.ar
	test/main.elf

all: test
