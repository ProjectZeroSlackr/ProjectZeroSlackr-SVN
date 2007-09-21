# gpSP makefile
# Gilead Kutnick - Exophase
# GP2X port(ion) - Z

# Global definitions

#PREFIX  = /opt/open2x/gcc-4.1.1-glibc-2.3.6
CC        = arm-elf-gcc
#STRIP     = arm-elf-strip

 OBJS      = main.o cpu.o memory.u video.o input.o sound.o  \
             cheats.o cpu_threaded.z gui.o cop.o \
             font.o  \
      	    arm_stub.o
 BIN       = igpSP

# Platform specific definitions 

VPATH      += ..
CFLAGS     += -DARM_ARCH #-DGP2X_BUILD
# NOTE: -funroll-loops will slow down compiling considerably
CFLAGS     += -O3 -elf2flt -std=c99 -msoft-float -funsigned-char -fno-common           \
              -fno-builtin                                                    \

#INCLUDES   = `$(PREFIX)/bin/arm-open2x-linux-sdl-config --cflags` 
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
	rm -f *.o *.u *.z $(BIN) 

