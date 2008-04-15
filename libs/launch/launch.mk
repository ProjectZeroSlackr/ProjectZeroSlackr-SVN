# Last updated: Apr 15, 2008
# ~Keripo
# Stripped down Makefile for ZeroLauncher launch modules
# pz.h file is from SVN r2408

CC = arm-elf-gcc
LD = arm-elf-ld
TTKCONF=../ttk/ttk-config-here
LIBS += -Wl,-elf2flt -Wl,-whole-archive -lc `$(TTKCONF) --ipod --sdl --libs` contrib/ucdl/libuCdl.a -lintl `$(CC) -print-libgcc-file-name` -lsupc++ -Wl,-no-whole-archive -mapcs

include Makefile

cobj-m  := $(patsubst %.c,%.o,$(wildcard $(patsubst %.o,%.c,$(obj-m))))

all: $(obj-m)
	@$(LD) -Ur -d -o $(MODULE).zl $(obj-m)
	@rm -rf *.c *.h *.o *~ Makefile
	@mv Module PackInfo
	
$(cobj-m): %.o: %.c
	@$(CC) -c -o $@ $< -I../launch `$(TTKCONF) --ipod --sdl --cflags` -D__PZ_MODULE_NAME=\"$(MODULE)\" -DPZ_MOD

clean:
	@rm -rf *.zl *.o
