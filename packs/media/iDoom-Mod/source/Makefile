
CC      = arm-elf-gcc
CFLAGS  = -std=c99 -DIPOD -Dlinux -D__linux__ -DLINUX -DNORMALUNIX
LDFLAGS = -elf2flt

ifneq ($(DEBUG),)
# Debug mode.
 CFLAGS += \
	-DDEBUG \
	-g
else
 CFLAGS += \
	-O3
endif

ifneq ($(GRAY),)
 CFLAGS += -DGRAYIPOD
endif

ifneq ($(FPSCOUNT),)
# Use this to count fps
	CFLAGS += -DFPSCOUNT
endif

CFLAGS +=\
	-pedantic\
	-Wall \
	-fpack-struct \
	-mstructure-size-boundary=8 \

LDFLAGS +=\
	#-lm

# Gamelib objects
OBJS =\
	keyHandler.o \
	file.o \
	blit.o \
	ipodhw.o

# iDoom objects
OBJS +=\
	am_map.o \
	d_items.o \
	d_main.o \
	d_net.o \
	doomdef.o \
	doomstat.o \
	dstrings.o \
	f_finale.o \
	f_wipe.o \
	g_game.o \
	hu_lib.o \
	hu_stuff.o \
	i_main.o \
	i_net.o \
	i_system.o \
	\
	info.o \
	m_argv.o \
	m_bbox.o \
	m_cheat.o \
	m_fixed.o \
	m_menu.o \
	m_misc.o \
	m_random.o \
	m_swap.o \
	p_ceilng.o \
	p_doors.o \
	p_enemy.o \
	p_floor.o \
	p_inter.o \
	p_lights.o \
	p_map.o \
	p_maputl.o \
	p_mobj.o \
	p_plats.o \
	p_pspr.o \
	p_saveg.o \
	p_setup.o \
	p_sight.o \
	p_spec.o \
	p_switch.o \
	p_telept.o \
	p_tick.o \
	p_user.o \
	r_bsp.o \
	r_data.o \
	r_draw.o \
	r_main.o \
	r_plane.o \
	r_segs.o \
	r_sky.o \
	r_things.o \
	st_lib.o \
	st_stuff.o \
	tables.o \
	v_video.o \
	w_wad.o \
	wi_stuff.o \
	z_zone.o \
	i_video.o \
	sounds.o
	#\ s_sound.o \
	#i_sound.o \
	
all: iDoom-Mod

iDoom-Mod: $(OBJS) Makefile
	$(CC) $(OBJS) -o iDoom-Mod $(CFLAGS) $(LDFLAGS)

clean: 
	$(RM) $(OBJS) *~ iDoom-Mod iDoom-Mod.gdb
