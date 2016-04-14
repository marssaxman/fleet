# Configure the build target: bare metal i386, statically linked ELF.
ARCH:=i686
BITS:=32
ASFLAGS+=-march=$(ARCH) --$(BITS) --strip-local-absolute
CPPFLAGS+=-march=$(ARCH) -m$(BITS) -ffreestanding -nostdlib -nostdinc
LDFLAGS+=-static -nostdlib -melf_i386

