# Last updated: Jun 1, 2008
# ~Keripo

# igpSP Makefile
# Build 0.9-2Xb K5
# Based on GP2X Makefile


# Global definitions

CC         = arm-uclinux-elf-gcc
BIN        = igpSP
OBJS       = main.o cpu.o memory.o video.o input.o gui.o                       \
             cheats.o cpu_threaded.o                                           \
			 ../gp2x/font.o ../gp2x/arm_stub.o video_blend.o                   \
             ipod_cop.o ipod_hw.o ipod_video.o ipod_input.o


# Platform specific definitions

# Use 'make' for normal, 'make NOSOUND=1' for without sound.
# Benchmark by adding 'BENCHMARK=1'

# 'BENCHMARK' only affects main.c while
# 'NOSOUND' affects gui.c, main.c and memory.c
# Delete only the affected .o files for faster re-compiling

VPATH      += ..
CFLAGS     += -DARM_ARCH -DIPOD_BUILD
ifdef NOSOUND
CFLAGS     += -DNOSOUND
else
OBJS       += sound.o ipod_sound.o
endif
ifdef BENCHMARK
CFLAGS     += -DBENCHMARK
endif
CFLAGS     += -O3 -msoft-float -funsigned-char -fno-common                    \
              -fno-builtin -fomit-frame-pointer -elf2flt                      \

INCLUDES   = -I../../hotdog
LIBS       = -Wl,-elf2flt -L../../hotdog/ipod -lhotdog -lm -lpthread


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

