/*
 * apple2_disk.c
 *
 * Disk ][ card emulation
 */

/* $Id: apple2_disk.c,v 1.11 2000/08/21 01:13:32 nyef Exp $ */

#include <stdlib.h>
#include <string.h>

#include "apple2_cards.h"
#include "types.h"
#include "tool.h"
#include "ui.h"
#include "menu.h"


/* manifest constants */

#define DISK_2_ROM_FILE "disk_ii.rom"

#define DSK_FILE_TRACK_LENGTH 0x1000
#define NIB_FILE_TRACK_LENGTH 0x1a00

#define BYTES_PER_NIB_SECTOR 0x1a0

#define TRACKS_PER_DISK 35 /* 0x23 */

#define DISK_SEEK_IN 0 /* higher tracks */
#define DISK_SEEK_OUT 1 /* lower tracks */


/* data structures */

struct a2_disk {
    int is_inserted;
    int is_readonly;

    int last_phase;
    
    int cur_track;

    u8 *disk_data;
    
    u8 *track_data;
    int track_data_index;

    struct ui_menu *menu;
};

struct a2_disk_card {
    struct apple2_card api;

    struct a2_disk disks[2];
    
    int motor_on;
    int drive_select;

    u8 write_latch;

    int qlines;
};


/* drive emulation */

void a2_disk_seek(struct a2_disk *disk, int direction)
{
    int new_track;

    new_track = disk->cur_track;

    if (direction == DISK_SEEK_OUT) {
	if (new_track == 0) { /* already at beginning */
	    return;
	}
	new_track--;
    } else if (direction == DISK_SEEK_IN) {
	new_track++;
	if (new_track >= TRACKS_PER_DISK) { /* already at end */
	    return;
	}
    } else {
	deb_printf("a2_disk: BUG: bogus direction parameter to seek.\n");
	return;
    }

    disk->cur_track = new_track;
    disk->track_data = disk->disk_data + (new_track * NIB_FILE_TRACK_LENGTH);
}

void a2_line_toggle(struct a2_disk_card *card, u16 address)
{
    int addr;

    addr = address & 0xf;
    
    switch (addr) {
    case 0x0: case 0x1: case 0x2: case 0x3:
    case 0x4: case 0x5: case 0x6: case 0x7:
	if (!(addr & 1)) {
	    return; /* stepper phase off, ingore it */
	}
	if (addr & 2) {
	    card->disks[card->drive_select].last_phase = addr;
	    return; /* odd stepper phase off, set phase in disk struct */
	}
	if ((addr & 4) == (card->disks[card->drive_select].last_phase & 4)) {
	    /* outward bound */
	    a2_disk_seek(&card->disks[card->drive_select], DISK_SEEK_OUT);
	} else {
	    /* inward bound */
	    a2_disk_seek(&card->disks[card->drive_select], DISK_SEEK_IN);
	}
	return;

    case 0x8:
	card->motor_on = 0;
	return;
    case 0x9:
	card->motor_on = 1;
	return;

    case 0xa:
	card->drive_select = 0;
	return;
    case 0xb:
	card->drive_select = 1;
	return;

    case 0xc: /* Q6L */
	card->qlines &= ~1;
	return;
    case 0xd: /* Q6H */
	card->qlines |= 1;
	return;
    case 0xe: /* Q7L */
	card->qlines &= ~2;
	return;
    case 0xf: /* Q7H */
	card->qlines |= 2;
	return;
    }
}

u8 a2_disk_read(struct a2_disk_card *card, u16 address)
{
    struct a2_disk *cur_disk;
    
    a2_line_toggle(card, address);

    cur_disk = &card->disks[card->drive_select];
    
    switch (card->qlines & 3) {
    case 0: /* read data */
	if (card->motor_on && cur_disk->is_inserted) {
	    u8 retval;

	    retval = cur_disk->track_data[cur_disk->track_data_index];
	    cur_disk->track_data_index++;
	    cur_disk->track_data_index %= NIB_FILE_TRACK_LENGTH;

	    return retval;
	}
	break;
    case 1: /* sense write protect */
	return cur_disk->is_readonly << 7;
    case 2: /* write byte */
	if (card->motor_on && cur_disk->is_inserted && (!cur_disk->is_readonly)) {
	    cur_disk->track_data[cur_disk->track_data_index] = card->write_latch;
	    cur_disk->track_data_index++;
	    cur_disk->track_data_index %= NIB_FILE_TRACK_LENGTH;
	}
	break;
    case 3: /* load write latch, SHOULD NOT HAPPEN */
	deb_printf("a2_disk: load write latch during read.\n");
	break;
    }
    
    return 0x00;
}

void a2_disk_write(struct a2_disk_card *card, u16 address, u8 data)
{
    a2_line_toggle(card, address);

    switch (card->qlines & 3) {
    case 0: /* read data */
	break;
    case 1: /* sense write protect */
	break;
    case 2: /* write byte */
	break;
    case 3: /* load write latch */
	card->write_latch = data;
	break;
    }
}


/* disk image conversion */

u8 a2_disk_6_2_write_table[64] = {
    0x96, 0x97, 0x9a, 0x9b, 0x9d, 0x9e, 0x9f, 0xa6,
    0xa7, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb2, 0xb3,
    0xb4, 0xb5, 0xb6, 0xb7, 0xb9, 0xba, 0xbb, 0xbc,
    0xbd, 0xbe, 0xbf, 0xcb, 0xcd, 0xce, 0xcf, 0xd3,
    0xd6, 0xd7, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde,
    0xdf, 0xe5, 0xe6, 0xe7, 0xe9, 0xea, 0xeb, 0xec,
    0xed, 0xee, 0xef, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6,
    0xf7, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};

int disk2_nibblize_sector_6_2(u8 *sector_data, u8 *output_buffer)
{
    u8 aux_data_buffer[86]; /* 0x56 bytes */
    const u8 bit_reverse_table[4] = {0, 2, 1, 3,};
    int i;
    int j;
    u8 checksum;
    u8 last_byte;

    /* set up aux_data_buffer[] */
    memset(aux_data_buffer, 0, 86);
#if 1
    j = 85;
    for (i = 0; i < 0x100; i++) {
	aux_data_buffer[j] >>= 2;
	aux_data_buffer[j] |= bit_reverse_table[sector_data[i] & 3] << 4;
	if (!j--) {
	    j = 85;
	}
    }
#endif
    aux_data_buffer[0] >>= 2;
    aux_data_buffer[1] >>= 2;

    /* output data to disk buffer */
    checksum = 0;
    j = 0;
    for (i = 85; i >= 0; i--) {
	last_byte = aux_data_buffer[i];
	output_buffer[j++] = a2_disk_6_2_write_table[checksum ^ last_byte];
	checksum = last_byte;
    }
    for (i = 0; i < 0x100; i++) {
	last_byte = sector_data[i] >> 2;
	output_buffer[j++] = a2_disk_6_2_write_table[checksum ^ last_byte];
	checksum = last_byte;
    }
    output_buffer[j++] = a2_disk_6_2_write_table[checksum];

    return j;
}

int disk2_write_address_prologue(u8 *output_buffer)
{
    output_buffer[0] = 0xd5;
    output_buffer[1] = 0xaa;
    output_buffer[2] = 0x96;
    return 3;
}

int disk2_write_data_prologue(u8 *output_buffer)
{
    output_buffer[0] = 0xd5;
    output_buffer[1] = 0xaa;
    output_buffer[2] = 0xad;
    return 3;
}

int disk2_write_epilogue(u8 *output_buffer)
{
    output_buffer[0] = 0xde;
    output_buffer[1] = 0xaa;
    output_buffer[2] = 0xeb;
    return 3;
}

int disk2_write_data_field(u8 *sector_data, u8 *output_buffer)
{
    int length;

    length = disk2_write_data_prologue(output_buffer);
    length += disk2_nibblize_sector_6_2(sector_data, output_buffer + length);
    length += disk2_write_epilogue(output_buffer + length);

    return length;
}

int disk2_write_address_field(u8 volume, u8 track, u8 sector, u8 *output_buffer)
{
    int length;
    u8 checksum;

    length = disk2_write_address_prologue(output_buffer);

    output_buffer[length++] = (volume >> 1) | 0xaa;
    output_buffer[length++] = volume | 0xaa;

    output_buffer[length++] = (track >> 1) | 0xaa;
    output_buffer[length++] = track | 0xaa;

    output_buffer[length++] = (sector >> 1) | 0xaa;
    output_buffer[length++] = sector | 0xaa;

    checksum = volume ^ track ^ sector;
    
    output_buffer[length++] = (checksum >> 1) | 0xaa;
    output_buffer[length++] = checksum | 0xaa;

    length += disk2_write_epilogue(output_buffer + length);

    return length;
}

int disk2_write_sector(u8 volume, u8 track, u8 sector, u8 *sector_data, u8 *output_buffer)
{
    int length;
    int i;

    for (i = 0; i < 0x25; i++) {
	output_buffer[i] = 0xff;
    }

    length = i;

    length += disk2_write_address_field(volume, track, sector, output_buffer + length);
    for (i = 0; i < 0x10; i++) {
	output_buffer[length++] = 0xff;
    }

    length += disk2_write_data_field(sector_data, output_buffer + length);

    if (length != BYTES_PER_NIB_SECTOR) {
	deb_printf("disk2_write_sector(): BUG: wrote 0x%x bytes, expected 0x%x bytes.\n", length, BYTES_PER_NIB_SECTOR);
    }
    return length;
}

void disk2_convert_track(u8 volume, u8 track, u8 *input_buffer, u8 *output_buffer, u8 *translate_table)
{
    int i;
    
    for (i = 0; i < 16; i++) {
	disk2_write_sector(volume, track, i,
			   input_buffer + (translate_table[i] * 0x100),
			   output_buffer + (i * BYTES_PER_NIB_SECTOR));
    }
}

u8 *disk2_convert_disk(u8 *input_buffer, u8 *translate_table)
{
    u8 *retval;
    int i;
    
    retval = malloc(NIB_FILE_TRACK_LENGTH * TRACKS_PER_DISK);

    if (!retval) {
	return NULL;
    }

    for (i = 0; i < TRACKS_PER_DISK; i++) {
	disk2_convert_track(254, i, input_buffer + (i * DSK_FILE_TRACK_LENGTH),
			    retval + (i * NIB_FILE_TRACK_LENGTH),
			    translate_table);
    }

    return retval;
}

u8 disk2_po_translate_table[0x10] = {
    0x00, 0x08, 0x01, 0x09, 0x02, 0x0a, 0x03, 0x0b,
    0x04, 0x0c, 0x05, 0x0d, 0x06, 0x0e, 0x07, 0x0f,
};

u8 disk2_do_translate_table[0x10] = {
    0x00, 0x07, 0x0e, 0x06, 0x0d, 0x05, 0x0c, 0x04,
    0x0b, 0x03, 0x0a, 0x02, 0x09, 0x01, 0x08, 0x0f,
};

void a2_disk_load_disk(struct a2_disk_card *card)
{
    rom_file romfile;

/*      romfile = read_romimage("../roms/apple2/dc/Dos_3.3.dsk"); */
/*      romfile = read_romimage("../roms/apple2/dc/DOS33.po"); */
/*      romfile = read_romimage("../roms/apple2/wizardry/dc/Wizardry_I_boot.dsk"); */
    romfile = read_romimage("../roms/apple2/sysroms/tarnover/dc/2400Game.do");
/*      romfile = read_romimage("../roms/apple2/rockys_boots.dsk"); */
/*      romfile = read_romimage("../roms/apple2/autodul1.dsk"); */
/*      romfile = read_romimage("../roms/apple2/acs/AdvConstSet1of6.dsk"); */
/*      romfile = read_romimage("../roms/apple2/ultima_1/U1Boot.dsk"); */
/*      romfile = read_romimage("../roms/apple2/Conan1.dsk"); */
/*      romfile = read_romimage("../roms/apple2/softporn.dsk"); */
/*      romfile = read_romimage("../roms/apple2/ProDOS.do"); */
    card->disks[0].disk_data = disk2_convert_disk(romfile->data, disk2_do_translate_table);
    card->disks[0].track_data = card->disks[0].disk_data;
    card->disks[0].is_inserted = 1;

    romfile = read_romimage("../roms/apple2/sysroms/tarnover/dc/2400Plyr.do");
    card->disks[1].disk_data = disk2_convert_disk(romfile->data, disk2_do_translate_table);
    card->disks[1].track_data = card->disks[1].disk_data;
    card->disks[1].is_inserted = 1;
    card->disks[1].is_readonly = 0;
}

void disk2_remove_disk(struct a2_disk *disk);

void disk2_open_callback(struct a2_disk *disk, char *filename)
{
    rom_file romfile;

    /* FIXME: is limited to DOS order only */
    /* FIXME: leaks memory */
    /* FIXME: is always readonly */
    romfile = read_romimage(filename);
    disk->disk_data = disk2_convert_disk(romfile->data, disk2_do_translate_table);
    disk->track_data = disk[0].disk_data;
    disk->cur_track = 0;
    disk->is_inserted = 1;
    disk->is_readonly = 1;
    
    disk->menu[1].callback = (ui_menu_callback)disk2_remove_disk;
    disk->menu[1].name = "Remove disk";
    menu_rename_item(&disk->menu[1]);
}

#define DISK2_FILE_FILTER \
    "All Supported Files\0*.dsk;*.do;*.po\0" \
    "Disk images (*.DSK)\0*.dsk\0" \
    "ProDOS-order images (*.PO)\0*.po" \
    "DOS-order images (*.DO)\0*.do\0\0"

void disk2_insert_disk(struct a2_disk *disk)
{
    menu_file_open_box((ui_open_callback)disk2_open_callback,
		       disk, DISK2_FILE_FILTER);
}

void disk2_remove_disk(struct a2_disk *disk)
{
    /* FIXME: leaks memory */
    /* FIXME: should sync modified images to disk */
    disk->is_inserted = 0;
    
    disk->menu[1].callback = (ui_menu_callback)disk2_insert_disk;
    disk->menu[1].name = "Insert disk";
    menu_rename_item(&disk->menu[1]);
}

/* initialization */

void a2_disk_load_rom(struct a2_disk_card *card)
{
    rom_file romfile;

    romfile = read_romimage(DISK_2_ROM_FILE);

    if (!romfile) {
	/* rom not found */
	deb_printf("unable to load \"" DISK_2_ROM_FILE "\", the disk rom file.\n");
	return;
    }

    card->api.rom = romfile->data;
}

struct ui_menu disk2_menu[] = {
    {"disk",        MF_NONE}, /* menu title (maybe) */
    {"Insert disk", MF_NONE, (ui_menu_callback)disk2_insert_disk},
    {NULL, 0, NULL}, /* must be last entry */
};

void disk2_init_menu(struct a2_disk *disk, int disk_num)
{
    struct ui_menu *menu;

    menu = malloc(sizeof(disk2_menu));
    memcpy(menu, disk2_menu, sizeof(disk2_menu));

    menu[1].callback_data = disk;
    disk->menu = menu;
    
    apple2_set_child_menu(menu, disk_num);
}

struct apple2_card *apple2_disk_init(struct apple2_mainboard *apple2)
{
    struct a2_disk_card *retval;

    retval = malloc(sizeof(*retval));

    if (!retval) {
	return NULL;
    }

    a2_disk_load_rom(retval);

    disk2_init_menu(&retval->disks[0], 0);
    disk2_init_menu(&retval->disks[1], 1);

    retval->api.read = (a2_card_read)a2_disk_read;
    retval->api.write = (a2_card_write)a2_disk_write;

    retval->motor_on = 0;
    retval->drive_select = 0;
    retval->qlines = 0;

    retval->disks[0].is_inserted = 0;
    retval->disks[0].is_readonly = 1;
    retval->disks[0].cur_track = 0;

    retval->disks[1].is_inserted = 0;
    retval->disks[1].is_readonly = 1;
    retval->disks[1].cur_track = 0;
    
    return (struct apple2_card *)retval;
}

/*
 * $Log: apple2_disk.c,v $
 * Revision 1.11  2000/08/21 01:13:32  nyef
 * added preliminary support for file open boxes
 *
 * Revision 1.10  2000/08/21 00:33:46  nyef
 * fixed to initialize the per-disk menus (oops)
 *
 * Revision 1.9  2000/08/05 15:42:45  nyef
 * really fixed warning in disk2_nibblize_sector_6_2()
 *
 * Revision 1.8  2000/07/01 15:43:32  nyef
 * started adding preliminary per-disk menus
 *
 * Revision 1.7  2000/05/06 22:35:27  nyef
 * fixed warning in disk2_nibblize_sector_6_2()
 *
 * Revision 1.6  2000/03/13 02:18:05  nyef
 * fixed track seeks (I hope)
 *
 * Revision 1.5  2000/03/12 00:13:59  nyef
 * fixed the DO skewing table
 *
 * Revision 1.4  2000/03/11 22:55:00  nyef
 * fixed the PO skewing table
 * added a DO skewing table
 *
 * Revision 1.3  2000/03/11 22:13:00  nyef
 * added code to convert PO image files to NIB format (used internally)
 *
 * Revision 1.2  2000/03/11 16:32:00  nyef
 * added preliminary track seeks, disk reading, and disk writing
 *
 * Revision 1.1  2000/03/11 02:29:58  nyef
 * Initial revision
 *
 */
