/*
 * fds.c
 *
 * Famicom Disk System emulation
 */

/* $Id: fds.c,v 1.6 2000/07/01 15:44:32 nyef Exp $ */

#include <stdlib.h>
#include "mappers.h"
#include "ui.h"
#include "menu.h"

struct mapper_14 {
    struct nes_mapper interface;
    nes_ppu ppu;
    nes_rom romfile;

    u8 *rom;
    u8 *disk;
    u8 irqflag1;
    u8 irqflag2;
    u32 diskptr;
    u8 *ram;

    int reg_4025;

    u16 irq_counter;
    u16 irq_latch;

    u8 dsr0;
    int write_skip;
    
    int drive_has_power;
    int disk_is_inserted;
};

void map14_set_drive_power(struct mapper_14 *mapper, int has_power)
{
    mapper->drive_has_power = has_power;
}

void map14_seek_disk_to_beginning(struct mapper_14 *mapper)
{
    mapper->diskptr = 0;
}

void map14_set_disk_side(struct mapper_14 *mapper, u8 side)
{
    mapper->disk = mapper->romfile->prg_data
	+ (65500 * (side % mapper->romfile->prg_size));
}

void map14_set_disk_inserted(struct mapper_14 *mapper, int is_inserted)
{
    mapper->disk_is_inserted = is_inserted;
}

u8 map14_read_disk_status_0(struct mapper_14 *mapper)
{
    u8 retval;
    
    /* Disk Status Register 0 */
/*     deb_printf("FDS: DSR0 read.\n"); */
    retval = mapper->dsr0;
    mapper->dsr0 &= 0xfe;
    return retval;
}

u8 map14_read_disk_status_1(struct mapper_14 *mapper)
{
    /* Disk Status Register 1 */
    u8 retval;

    retval = 0x00;
    
    if (!mapper->disk_is_inserted) {
	retval |= 0x01;
    }
    return retval;
}

u8 map14_read_disk_data(struct mapper_14 *mapper)
{
    /* FIXME: incrementing the pointer should probably be done at interrupt generation */
    return mapper->disk[mapper->diskptr++];
}

u8 map14_b4read(struct mapper_14 *mapper, u16 addr)
{
    if (addr == 0x4030) {
	return map14_read_disk_status_0(mapper);
    } else if (addr == 0x4031) {
	return map14_read_disk_data(mapper);
    } else if (addr == 0x4032) {
	return map14_read_disk_status_1(mapper);
    } else if (addr == 0x4033) { /* ExPort Input */
	deb_printf("FDS: ExPort read.\n");
	if (mapper->drive_has_power) {
	    return 0x80; /* FIXME: may be wrong */
	} else {
	    return 0x00; /* FIXME: may be wrong */
	}
    } else {
	deb_printf("FDS: unknown read from 0x%04x.\n", addr);
	return 0;
    }
}

void map14_4025_write(struct mapper_14 *mapper, u8 data)
{
#ifdef DEBUG_SPEW
    deb_printf("4025: %c%c%c%c %c%c%c.\n",
	       (data & 0x80)? '*': '-',
	       (data & 0x40)? '*': '-',
	       (data & 0x20)? '*': '-',
	       (data & 0x10)? '*': '-',
	       (data & 0x04)? '*': '-',
	       (data & 0x02)? '*': '-',
	       (data & 0x01)? '*': '-');
#endif
    mapper->irqflag1 = 0;
    if (data & 0x01) {
	/* motor on switch */
    }
    if (data & 0x02) {
	map14_seek_disk_to_beginning(mapper);
    }
    if (!(data & 0x04)) {
	/* disk write switch */
#ifdef DEBUG_SPEW
	deb_printf("map14: disk write not yet emulated.\n");
#endif
	mapper->write_skip = 2;
    }
    if (data & 0x10) {
	/* something to do with a block end mark */
#ifdef DEBUG_SPEW
	deb_printf("map14: unknown phase E2 bit set.\n");
#endif
    }
    if (!(data & 0x20)) {
	/* unknown, not covered by document */
	/* one text states that it is always 1 */
#ifdef DEBUG_SPEW
	deb_printf("map14: unknown bit reset.\n");
#endif
    }
    if (data & 0x40) {
	/* some sort of enable switch to do with disk data read/write */
    } else if (mapper->reg_4025 & 0x40) {
	/* hack up the diskptr on the downward edge of 4025 & 0x40 */
	mapper->diskptr -= 2;
    }
    if (data & 0x80) {
	/* IRQ enable */
	mapper->irqflag1 = 1;
    }

    /* not a disk register, so separate from the rest */
    if (data & 0x08) {
	PPU_mirror_horizontal();
    } else {
	PPU_mirror_vertical();
    }

    mapper->reg_4025 = data;
}

void map14_b4write(struct mapper_14 *mapper, u16 addr, u8 data)
{
    if (addr < 0x4040) {
	if (addr == 0x4020) {
	    mapper->irq_latch &= 0xff00;
	    mapper->irq_latch |= data;
	} else if (addr == 0x4021) {
	    mapper->irq_latch &= 0x00ff;
	    mapper->irq_latch |= (data << 8);
	} else if (addr == 0x4022) {
	    mapper->irq_counter = mapper->irq_latch;
	    mapper->irqflag2 = data;
	} else if (addr == 0x4024) {
/* 	    deb_printf("FDS: 4024 0x%02x.\n", data); */
	    if (!(mapper->reg_4025 & 0x04)) {
#ifdef DEBUG_SPEW
		deb_printf("data write 0x%02x.\n", data);
		deb_printf("disk: %02x %02x>%02x<%02x %02x\n",
			   mapper->disk[mapper->diskptr - 2],
			   mapper->disk[mapper->diskptr - 1],
			   mapper->disk[mapper->diskptr],
			   mapper->disk[mapper->diskptr + 1],
			   mapper->disk[mapper->diskptr + 2]);
#endif
		if (mapper->write_skip) {
		    mapper->write_skip--;
		} else {
		    mapper->disk[mapper->diskptr - 2] = data;
		}
	    }
	} else if (addr == 0x4025) {
	    map14_4025_write(mapper, data);
	} else {
	    deb_printf("FDS: unknown write (0x%04x, 0x%02x).\n", addr, data);
	}
    } else {
	/* It's a sound reg, ignore it for now */
    }
}

void map14_hsync(struct mapper_14 *mapper, cal_cpu cpu, int display_active)
{
/*     if (map14_irqflag1 && map14_irqflag2) { */
    if (mapper->irqflag1) {
	cpu->irq(cpu, 0);
    }

    if (mapper->irqflag2) {
	if (mapper->irq_counter < 113) {
	    mapper->irqflag2 = 0;
	    mapper->dsr0 |= 1;
/* 	    mapper->irq_counter = mapper->irq_latch; */
	    cpu->irq(cpu, 0);
	} else {
	    mapper->irq_counter -= 113;
	}
    }
}

void map14_write(struct mapper_14 *mapper, u16 address, u8 value)
{
    if (address < 0xe000) {
	mapper->ram[address - 0x8000] = value;
    } else {
	deb_printf("map14_write(0x%04hx, 0x%02x).\n", address, value);
    }
}

struct ui_menu map14_menu[];
void map14_insert_disk(struct mapper_14 *mapper);

void map14_remove_disk(struct mapper_14 *mapper)
{
    map14_set_disk_inserted(mapper, 0);
    map14_menu[1].name = "Insert disk";
    map14_menu[1].callback = (ui_menu_callback)&map14_insert_disk;
    menu_rename_item(&map14_menu[1]);
    menu_enable_item(&map14_menu[2]);
}

void map14_insert_disk(struct mapper_14 *mapper)
{
    map14_set_disk_inserted(mapper, 1);
    map14_menu[1].name = "Remove disk";
    map14_menu[1].callback = (ui_menu_callback)&map14_remove_disk;
    menu_rename_item(&map14_menu[1]);
    menu_disable_item(&map14_menu[2]);
}

void map14_set_side(struct mapper_14 *mapper)
{
    int side;
    
    if (mapper->disk_is_inserted) {
	return;
    }

    side = map14_menu[2].name[9] - '1';
    map14_set_disk_side(mapper, side);
    side++;
    if (side >= mapper->romfile->prg_size) {
	side = 0;
    }
    map14_menu[2].name[9] = '1' + side;
    menu_rename_item(&map14_menu[2]);
}

struct ui_menu map14_menu[] = {
    {"FDS",         MF_NONE, NULL},
    {"Insert disk", MF_NONE, (ui_menu_callback)map14_insert_disk},
    {"Set side 1",  MF_NONE, (ui_menu_callback)map14_set_side},
    {NULL, 0, NULL}, /* must be last entry */
};

#include <string.h>

void map14_init_menu(struct mapper_14 *mapper)
{
    struct ui_menu *menu;

    for (menu = &map14_menu[1]; menu->name; menu++) {
	menu->callback_data = mapper;
    }

    map14_menu[2].name = strdup(map14_menu[2].name);
    
    menu_init(map14_menu);
}

void map14_load_bios(struct mapper_14 *mapper)
{
    rom_file romfile;
    
    deb_printf("attempting to load FDS bios file.\n");
    
    romfile = read_romimage("disksys.rom");

    if (!romfile) {
	deb_printf("could not open bios file.\n");
	deb_printf("things are about to get ugly.\n");
	return;
    }
    
    mapper->rom = romfile->data;

    /* FIXME: memory leak */
}

nes_mapper map14_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_14 *retval;

    retval = malloc(sizeof(struct mapper_14));

    if (retval) {
	retval->interface.write = (mapwrite_t)map14_write;
	retval->interface.hsync = (maphsync_t)map14_hsync;
	retval->romfile = romfile;
	retval->ppu = ppu;

	ppu_latchfunc = NULL;

	map14_load_bios(retval);

	map14_init_menu(retval);

	map14_set_drive_power(retval, 1);
	map14_set_side(retval);
	map14_set_disk_inserted(retval, 0);
	map14_seek_disk_to_beginning(retval);

	retval->reg_4025 = 0;
	retval->irqflag2 = 0;
	retval->dsr0 = 0;
	
	retval->ram = calloc(1, 0x6000);
	
	bank_8 = retval->ram + 0x0000;
	bank_A = retval->ram + 0x2000;
	bank_C = retval->ram + 0x4000;
	bank_E = retval->rom;
	
	nes_bank4_read_hook = (readhook_t)map14_b4read;
	nes_bank4_write_hook = (writehook_t)map14_b4write;
    }

    return (nes_mapper)retval;
}

/*
 * $Log: fds.c,v $
 * Revision 1.6  2000/07/01 15:44:32  nyef
 * fixed to work with the child menu changes
 *
 * Revision 1.5  2000/06/29 01:24:46  nyef
 * renamed the menu functions more appropriately
 *
 * Revision 1.4  2000/06/29 01:05:46  nyef
 * moved menu interface out from ui.h to menu.h
 *
 * Revision 1.3  2000/06/28 01:55:18  nyef
 * disabled lots of debug output
 *
 * Revision 1.2  2000/06/28 01:45:56  nyef
 * added preliminary disk write support
 *
 * Revision 1.1  2000/06/28 01:34:32  nyef
 * Initial revision
 *
 */
