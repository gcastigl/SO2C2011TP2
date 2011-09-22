# Binary Name
BINARY := bin/kernel.bin

CFLAGS := -fno-builtin -fno-stack-protector
LDFLAGS := -Tlink.ld

# Directories belonging to the project
PROJDIRS := src include
# All source files of the project
CSRCS := $(shell find -L $(PROJDIRS) -type f -name "*.c")
# All header files of the project
CHDRS := $(shell find -L $(PROJDIRS) -type f -name "*.h")
# All asm files of the project
ASMSRCS := $(shell find -L $(PROJDIRS) -type f -name "*.asm")

# All object files in the library
COBJS := $(patsubst src/%.c,bin/%.o,$(CSRCS))
ASMOBJS := $(patsubst src/%.asm,bin/%.o,$(ASMSRCS))

OBJS := $(ASMOBJS) $(COBJS)
OBJDIRS := $(sort $(dir $(OBJS)))

.PHONY: all clean

all: $(OBJDIRS) $(BINARY)

$(OBJDIRS):
	mkdir -p $@

$(BINARY): $(ASMOBJS) $(COBJS)
	$(LD) $(LDFLAGS) -o $(BINARY) $(ASMOBJS) $(COBJS)

clean:
	-@$(RM) -rf $(OBJDIRS)

$(ASMOBJS): bin/%.o : src/%.asm
	nasm -f aout $< -o $@

$(COBJS): bin/%.o : src/%.c
	gcc $(CFLAGS) -MMD -MP -c $< -o $@
