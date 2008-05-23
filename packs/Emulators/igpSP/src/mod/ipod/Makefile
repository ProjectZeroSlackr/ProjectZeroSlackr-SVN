# Last updated: May 22, 2008
# ~Keripo

# igpSP Makefile
# Build 0.9-2Xb K4
# Based on GP2X Makefile


# Global definitions

CC         = arm-uclinux-elf-gcc

OBJS       = main.o cpu.o memory.o video.o input.o sound.o gui.o               \
             cheats.o cpu_threaded.o ../gp2x/font.o ../gp2x/arm_stub.o         \
             ipod_cop.o ipod_hw.o ipod_video.o ipod_input.o
BIN        = igpSP


# Platform specific definitions

# Use 'make' for normal, 'make NOSOUND=1' for without sound.
# Benchmark with 'make NOSOUND=1 BENCHMARK=1'
# Using benchmark with sound doesn't seem to work for some reason.

# 'BENCHMARK' only affects main.c while
# 'NOSOUND' affects gui.c, main.c and memory.c
# Delete only the affected .o files for faster re-compiling

VPATH      += ..
CFLAGS     += -DARM_ARCH -DIPOD_BUILD
ifdef NOSOUND
CFLAGS     += -DNOSOUND
endif
ifdef BENCHMARK
CFLAGS     += -DBENCHMARK
endif
CFLAGS     += -O3 -std=c99 -msoft-float -funsigned-char -fno-common           \
              -fno-builtin -elf2flt                                           \

INCLUDES   = -I../../hotdog
LIBS       = -lm -lpthread -L../../hotdog/ipod -lhotdog -Wl,-elf2flt


# Compilation:

.SUFFIXES: .c

%.z: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.u: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: %.S
	$(CC) $(ASFLAGS) $(INCLUDES) -c -o $@ $<

all:	$(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(BIN)  

clean:
	rm -f *.o ../gp2x/*.o $(BIN).gdb $(BIN)

