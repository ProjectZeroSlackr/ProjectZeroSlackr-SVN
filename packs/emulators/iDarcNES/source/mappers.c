/*
 * mappers.c
 *
 * mapper emulation routines
 */

/* $Id: mappers.c,v 1.98 2000/12/14 21:47:07 nyef Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include "mappers.h"
#include "ui.h"
#include "tool.h"
#include "nes_ppu.h"
#include "nes.h"
#include "cal.h"

/* defines for mapper interfaces */
typedef nes_mapper (* mapinit_t)(nes_ppu ppu, nes_rom romfile);

nes_mapper map0_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map1_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map2_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map3_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map4_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map7_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map9_init(nes_ppu ppu, nes_rom romfile);
nes_mapper mapA_init(nes_ppu ppu, nes_rom romfile);
nes_mapper mapB_init(nes_ppu ppu, nes_rom romfile);
nes_mapper mapF_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map14_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map17_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map18_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map20_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map42_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map44_init(nes_ppu ppu, nes_rom romfile);
nes_mapper map4E_init(nes_ppu ppu, nes_rom romfile);

struct mapper_support {
    int mapper;
    const char *name;
    mapinit_t create;
    int support; /* mapper support. 0: none. 1: partial. 2: full. */
};

struct mapper_support mappers[] = {
    {0,   "None",               &map0_init,  2},
    {1,   "MMC1",               &map1_init,  2},
    {2,   "UNROM",              &map2_init,  2},
    {3,   "CNROM",              &map3_init,  2},
    {4,   "MMC3",               &map4_init,  1},
    {7,   "AOROM",              &map7_init,  2},
    {9,   "MMC2",               &map9_init,  2},
    {10,  "MMC4",               &mapA_init,  2},
    {11,  "Color Dreams",       &mapB_init,  2},
#if 0
    {15,  "100-in-1",           &mapF_init,  2},
#endif
    {20,  "Famicom DiskSystem", &map14_init, 1},
    {23,  "Konami VRC2 type B", &map17_init, 1},
    {24,  "Konami VRC6",        &map18_init, 1},
#if 0
    {32,  "Irem G-101",         &map20_init, 1},
#endif
    {66,  "GNROM",              &map42_init, 1},
    {68,  "Sunsoft Mapper 4",   &map44_init, 2},
    {78,  "Irem 74HC161/32",    &map4E_init, 1},
    {-1,  NULL,                 NULL,         0},
};

nes_mapper create_mapper(nes_ppu ppu, nes_rom romfile)
{
    int i;
    
    for (i = 0; mappers[i].mapper != -1; i++) {
	if ((mappers[i].mapper == romfile->mapper) && (mappers[i].support)) {
	    deb_printf("mappers: initializing%s completed mapper %d (%s).\n",
		   (mappers[i].support == 1)? " partially": "",
		   romfile->mapper, mappers[i].name);
	    return mappers[i].create(ppu, romfile);
	}
    }
    return NULL;
}

int mapper_supported(int mapper)
{
    int i;

    for (i = 0; mappers[i].mapper != -1; i++) {
	if (mappers[i].mapper == mapper) {
	    return mappers[i].support;
	}
    }
    return 0;
}


/* dummy routines for when a mapper doesn't need it. */

void dummy_hsync(nes_mapper mapper, cal_cpu cpu, int display_active)
{
}


/* MAPPER 0 Implementation */

void map0_write(nes_mapper mapper, unsigned short address, unsigned char value)
{
}

nes_mapper map0_init(nes_ppu ppu, nes_rom romfile)
{
    nes_mapper retval;

    retval = malloc(sizeof(struct nes_mapper));

    if (retval) {
	retval->write = map0_write;
	retval->hsync = dummy_hsync;
	
	ppu_latchfunc = NULL;
	nesppu_map_8k(ppu, 0, 0);

	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return retval;
}


/* MAPPER 1 Implementation */

struct mapper_1 {
    struct nes_mapper interface;
    nes_ppu ppu;
    nes_rom romfile;
    int sequence;
    int accumulator;
    int data[4];
};

void map1_write(struct mapper_1 *mapper, unsigned short address, unsigned char value)
{
    int bank_select;
    
    if (value & 0x80) {
	mapper->data[0] |= 0x0c;
	mapper->accumulator = mapper->data[(address >> 13) & 3];
	mapper->sequence = 5;
    } else {
	mapper->accumulator |= ((value & 1) << (mapper->sequence));
	mapper->sequence++;
    }

    if ((mapper->sequence) == 5) {
	mapper->data[(address >> 13) & 3] = mapper->accumulator;
	mapper->sequence = 0;
	mapper->accumulator = 0;

	switch (mapper->data[0] & 3) {
 	case 0:
	    PPU_mirror_one_low();
	    break;
	case 1:
	    PPU_mirror_one_high();
	    break;
	case 2:
	    PPU_mirror_vertical();
	    break;
 	case 3:
	    PPU_mirror_horizontal();
	    break;
	}
	
	if (mapper->romfile->prg_size == 0x20) { /* 512k cart */
	    bank_select = mapper->data[1] & 0x10;
	} else { /* other carts */
	    bank_select = 0;
	}
	
	if (!(mapper->data[0] & 8)) {
	    nesprg_map_32k(0, (mapper->data[3] & 15) + (bank_select >> 1));
	} else if (mapper->data[0] & 4) {
	    nesprg_map_16k(0, (mapper->data[3] & 15) + bank_select);
	    nesprg_map_16k(1, -1);
	} else {
	    nesprg_map_16k(0, 0);
	    nesprg_map_16k(1, (mapper->data[3] & 15) + bank_select);
	}
	
	if (mapper->data[0] & 0x10) {
	    nesppu_map_4k(mapper->ppu, 0, mapper->data[1]);
	    nesppu_map_4k(mapper->ppu, 1, mapper->data[2]);
	} else {
	    nesppu_map_8k(mapper->ppu, 0, mapper->data[1] >> 1);
	}
    }
}

nes_mapper map1_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_1 *retval;

    retval = malloc(sizeof(struct mapper_1));

    if (retval) {
	retval->interface.write = (mapwrite_t)map1_write;
	retval->interface.hsync = dummy_hsync;
	retval->ppu = ppu;
	retval->romfile = romfile;
	
	ppu_latchfunc = NULL;
	retval->data[0] = 0x1f; /* FIXME: may be incorrect */
	retval->data[3] = 0x00;

	/* FIXME: may want to set consistent internal state */
	/* NOTE: does nothing when not using CHR-ROM */
	nesppu_map_8k(ppu, 0, 0);
	
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper)retval;
}


/* MAPPER 2 Implementation */

void map2_write(nes_mapper mapper, unsigned short address, unsigned char value)
{
    nesprg_map_16k(0, value);
}

nes_mapper map2_init(nes_ppu ppu, nes_rom romfile)
{
    nes_mapper retval;

    retval = malloc(sizeof(struct nes_mapper));

    if (retval) {
	retval->write = map2_write;
	retval->hsync = dummy_hsync;

	/* NOTE: does nothing when not using CHR-ROM */
	nesppu_map_8k(ppu, 0, 0);
	
	ppu_latchfunc = NULL;
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return retval;
}


/* MAPPER 3 Implementation */

struct mapper_3 {
    struct nes_mapper interface;
    nes_ppu ppu;
};

void map3_write(struct mapper_3 *mapper, unsigned short address, unsigned char value)
{
    nesppu_map_8k(mapper->ppu, 0, value);
}

nes_mapper map3_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_3 *retval;

    retval = malloc(sizeof(struct mapper_3));

    if (retval) {
	retval->interface.write = (mapwrite_t)map3_write;
	retval->interface.hsync = dummy_hsync;
	retval->ppu = ppu;
	
	ppu_latchfunc = NULL;
	map3_write(retval, 0, 0);
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper)retval;
}


/* MAPPER 4 Implementation */

struct mapper_4 {
    struct nes_mapper interface;
    nes_ppu ppu;
    nes_rom romfile;

    unsigned char command;
    
    unsigned char irq_count;
    unsigned char irq_latch;
    unsigned char irq_state;
    
    unsigned char chr_switch[6];
    unsigned char prg_switch[2];
};

void map4_sync_prg(struct mapper_4 *mapper)
{
    if (mapper->command & 0x40) {
	nesprg_map_8k(0, 0xfe);
	nesprg_map_8k(1, mapper->prg_switch[1]);
	nesprg_map_8k(2, mapper->prg_switch[0]);
	nesprg_map_8k(3, 0xff);
    } else {
	nesprg_map_8k(0, mapper->prg_switch[0]);
	nesprg_map_8k(1, mapper->prg_switch[1]);
	nesprg_map_16k(1, -1);
    }
}

void map4_sync_chr(struct mapper_4 *mapper)
{
    if (mapper->command & 0x80) {
	nesppu_map_1k(mapper->ppu, 0, mapper->chr_switch[2]);
	nesppu_map_1k(mapper->ppu, 1, mapper->chr_switch[3]);
	nesppu_map_1k(mapper->ppu, 2, mapper->chr_switch[4]);
	nesppu_map_1k(mapper->ppu, 3, mapper->chr_switch[5]);
	nesppu_map_1k(mapper->ppu, 4, mapper->chr_switch[0]);
	nesppu_map_1k(mapper->ppu, 5, mapper->chr_switch[0] + 1);
	nesppu_map_1k(mapper->ppu, 6, mapper->chr_switch[1]);
	nesppu_map_1k(mapper->ppu, 7, mapper->chr_switch[1] + 1);
    } else {
	nesppu_map_1k(mapper->ppu, 0, mapper->chr_switch[0]);
	nesppu_map_1k(mapper->ppu, 1, mapper->chr_switch[0] + 1);
	nesppu_map_1k(mapper->ppu, 2, mapper->chr_switch[1]);
	nesppu_map_1k(mapper->ppu, 3, mapper->chr_switch[1] + 1);
	nesppu_map_1k(mapper->ppu, 4, mapper->chr_switch[2]);
	nesppu_map_1k(mapper->ppu, 5, mapper->chr_switch[3]);
	nesppu_map_1k(mapper->ppu, 6, mapper->chr_switch[4]);
	nesppu_map_1k(mapper->ppu, 7, mapper->chr_switch[5]);
    }
}

void map4_write(struct mapper_4 *mapper, unsigned short address, unsigned char value)
{
    if (address == 0x8000) {
        mapper->command = value;
	map4_sync_chr(mapper);
	map4_sync_prg(mapper);
    } else if (address == 0x8001) {
        switch (mapper->command & 7) {
	case 0:
	    mapper->chr_switch[0] = value;
	    break;
	case 1:
	    mapper->chr_switch[1] = value;
	    break;
	case 2:
	    mapper->chr_switch[2] = value;
	    break;
	case 3:
	    mapper->chr_switch[3] = value;
	    break;
	case 4:
	    mapper->chr_switch[4] = value;
	    break;
	case 5:
	    mapper->chr_switch[5] = value;
	    break;
	case 6:
            mapper->prg_switch[0] = value;
	    break;
	case 7:
            mapper->prg_switch[1] = value;
	    break;
        }
	map4_sync_chr(mapper);
	map4_sync_prg(mapper);
    } else if (address == 0xa000) {
	if (value & 0x40) {
	    PPU_mirror_one_high();
	} else {
	    if (value & 0x01) {
		PPU_mirror_horizontal();
	    } else {
		PPU_mirror_vertical();
	    }
	}
    } else if (address == 0xc000) {
	mapper->irq_count = value;
    } else if (address == 0xc001) {
	mapper->irq_latch = value;
    } else if (address == 0xe000) {
	mapper->irq_state = 0;
    } else if (address == 0xe001) {
	mapper->irq_state = 1;
    } else {
        deb_printf("map4_write: unknown address. (0x%04hx, 0x%02x)\n", address, value);
    }
}

void map4_hsync(struct mapper_4 *mapper, cal_cpu cpu, int display_active)
{
    if (display_active && mapper->irq_state && (mapper->irq_count-- == 0)) {
	mapper->irq_count = mapper->irq_latch;
	cpu->irq(cpu, 0);
    }
}

nes_mapper map4_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_4 *retval;

    retval = malloc(sizeof(struct mapper_4));

    if (retval) {
	retval->interface.write = (mapwrite_t)map4_write;
	retval->interface.hsync = (maphsync_t)map4_hsync;
	retval->ppu = ppu;
	retval->romfile = romfile;
	
	ppu_latchfunc = NULL;
	
	retval->command = 0;
	retval->prg_switch[0] = 0;
	retval->prg_switch[1] = 1;
	map4_sync_prg(retval);
	retval->chr_switch[0] = 0;
	retval->chr_switch[1] = 2;
	retval->chr_switch[2] = 4;
	retval->chr_switch[3] = 5;
	retval->chr_switch[4] = 6;
	retval->chr_switch[5] = 7;
	map4_sync_chr(retval);
    }

    return (nes_mapper)retval;
}


/* MAPPER 7 Implementation */

struct mapper_7 {
    struct nes_mapper interface;
    nes_ppu ppu;
};

void map7_write(struct mapper_7 *mapper, unsigned short address, unsigned char value)
{
    if (value & 0x10) {
	PPU_mirror_one_high();
    } else {
	PPU_mirror_one_low();
    }

    nesprg_map_32k(0, value & 0x0f);
}

nes_mapper map7_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_7 *retval;

    retval = malloc(sizeof(struct mapper_7));

    if (retval) {
	retval->interface.write = (mapwrite_t)map7_write;
	retval->interface.hsync = dummy_hsync;
	retval->ppu = ppu;
	
	ppu_latchfunc = NULL;
	nesprg_map_32k(0, 0);
    }

    return (nes_mapper)retval;
}


/* MAPPER 9 Implementation */

struct map9_latch {
    unsigned char lo_bank;
    unsigned char hi_bank;
    unsigned char state;
};

struct mapper_9 {
    struct nes_mapper interface;
    nes_ppu ppu;
    struct map9_latch latch1;
    struct map9_latch latch2;
};

void map9_latchfunc(struct mapper_9 *mapper, unsigned short address)
{
    if ((address & 0x3ff0) == 0x0fd0) {
	mapper->latch1.state = 0;
	nesppu_map_4k(mapper->ppu, 0, mapper->latch1.lo_bank);
    } else if ((address & 0x3ff0) == 0x0fe0) {
	mapper->latch1.state = 1;
	nesppu_map_4k(mapper->ppu, 0, mapper->latch1.hi_bank);
    } else if ((address & 0x3ff0) == 0x1fd0) {
	mapper->latch2.state = 0;
	nesppu_map_4k(mapper->ppu, 1, mapper->latch2.lo_bank);
    } else if ((address & 0x3ff0) == 0x1fe0) {
	mapper->latch2.state = 1;
	nesppu_map_4k(mapper->ppu, 1, mapper->latch2.hi_bank);
    }
}

void map9_write(struct mapper_9 *mapper, unsigned short address, unsigned char value)
{
    if (address < 0xa000) {
	/* Ignore it. It's not in spec. */
    } else if (address < 0xb000) {
	nesprg_map_8k(0, value);
    } else if (address < 0xc000) {
	mapper->latch1.lo_bank = value;
	if (mapper->latch1.state == 0) {
	    nesppu_map_4k(mapper->ppu, 0, value);
	}
    } else if (address < 0xd000) {
	mapper->latch1.hi_bank = value;
	if (mapper->latch1.state == 1) {
	    nesppu_map_4k(mapper->ppu, 0, value);
	}
    } else if (address < 0xe000) {
	mapper->latch2.lo_bank = value;
	if (mapper->latch2.state == 0) {
	    nesppu_map_4k(mapper->ppu, 1, value);
	}
    } else if (address < 0xf000) {
	mapper->latch2.hi_bank = value;
	if (mapper->latch2.state == 1) {
	    nesppu_map_4k(mapper->ppu, 1, value);
	}
    } else {
	if (value & 1) {
	    PPU_mirror_horizontal();
	} else {
	    PPU_mirror_vertical();
	}
    }
}

nes_mapper map9_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_9 *retval;

    retval = malloc(sizeof(struct mapper_9));

    if (retval) {
	retval->interface.write = (mapwrite_t)map9_write;
	retval->interface.hsync = dummy_hsync;
	retval->ppu = ppu;

	retval->latch1.lo_bank = 0;
	retval->latch1.hi_bank = 0;
	retval->latch1.state = 0;
	retval->latch2.lo_bank = 0;
	retval->latch2.hi_bank = 0;
	retval->latch2.state = 0;
	ppu_latchfunc = (ppulatch_t)map9_latchfunc;
	nesprg_map_8k(0, 0);
	nesprg_map_8k(1, 0xfd);
	nesprg_map_8k(2, 0xfe);
	nesprg_map_8k(3, 0xff);
    }

    return (nes_mapper)retval;
}


/* MAPPER A (10) Implementation */

struct mapA_latch {
    unsigned char lo_bank;
    unsigned char hi_bank;
    unsigned char state;
};

struct mapper_A {
    struct nes_mapper interface;
    nes_ppu ppu;
    struct mapA_latch latch1;
    struct mapA_latch latch2;
};

void mapA_latchfunc(struct mapper_A *mapper, unsigned short address)
{
    if ((address & 0x3ff0) == 0x0fd0) {
	mapper->latch1.state = 0;
	nesppu_map_4k(mapper->ppu, 0, mapper->latch1.lo_bank);
    } else if ((address & 0x3ff0) == 0x0fe0) {
	mapper->latch1.state = 1;
	nesppu_map_4k(mapper->ppu, 0, mapper->latch1.hi_bank);
    } else if ((address & 0x3ff0) == 0x1fd0) {
	mapper->latch2.state = 0;
	nesppu_map_4k(mapper->ppu, 1, mapper->latch2.lo_bank);
    } else if ((address & 0x3ff0) == 0x1fe0) {
	mapper->latch2.state = 1;
	nesppu_map_4k(mapper->ppu, 1, mapper->latch2.hi_bank);
    }
}

void mapA_write(struct mapper_A *mapper, unsigned short address, unsigned char value)
{
    if (address < 0xa000) {
	/* Ignore it. It's not in spec. */
    } else if (address < 0xb000) {
	nesprg_map_16k(0, value);
    } else if (address < 0xc000) {
	mapper->latch1.lo_bank = value;
	if (mapper->latch1.state == 0) {
	    nesppu_map_4k(mapper->ppu, 0, value);
	}
    } else if (address < 0xd000) {
	mapper->latch1.hi_bank = value;
	if (mapper->latch1.state == 1) {
	    nesppu_map_4k(mapper->ppu, 0, value);
	}
    } else if (address < 0xe000) {
	mapper->latch2.lo_bank = value;
	if (mapper->latch2.state == 0) {
	    nesppu_map_4k(mapper->ppu, 1, value);
	}
    } else if (address < 0xf000) {
	mapper->latch2.hi_bank = value;
	if (mapper->latch2.state == 1) {
	    nesppu_map_4k(mapper->ppu, 1, value);
	}
    } else {
	if (value & 1) {
	    PPU_mirror_horizontal();
	} else {
	    PPU_mirror_vertical();
	}
    }
}

nes_mapper mapA_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_A *retval;

    retval = malloc(sizeof(struct mapper_A));

    if (retval) {
	retval->interface.write = (mapwrite_t)mapA_write;
	retval->interface.hsync = dummy_hsync;
	retval->ppu = ppu;

	retval->latch1.lo_bank = 0;
	retval->latch1.hi_bank = 0;
	retval->latch1.state = 0;
	retval->latch2.lo_bank = 0;
	retval->latch2.hi_bank = 0;
	retval->latch2.state = 0;
	ppu_latchfunc = (ppulatch_t)mapA_latchfunc;
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper)retval;
}


/* MAPPER B (11) Implementation */

struct mapper_B {
    struct nes_mapper interface;
    nes_ppu ppu;
};

void mapB_write(struct mapper_B *mapper, unsigned short address, unsigned char value)
{
    unsigned char vromptr;
    unsigned char romptr;

    vromptr = (value >> 4) & 0x0f;
    romptr = value & 0x0f;

    nesppu_map_8k(mapper->ppu, 0, vromptr);

    nesprg_map_32k(0, romptr);
}

nes_mapper mapB_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_B *retval;

    retval = malloc(sizeof(struct mapper_B));

    if (retval) {
	retval->interface.write = (mapwrite_t)mapB_write;
	retval->interface.hsync = dummy_hsync;
	retval->ppu = ppu;
	
	ppu_latchfunc = NULL;
	mapB_write(retval, 0, 0);
    }

    return (nes_mapper)retval;
}


#if 0
/* MAPPER F (15) Implementation */

void mapF_write(nes_mapper mapper, unsigned short address, unsigned char value)
{
    int bank = (value << 1);
    
    if (address == 0x8000) {
	bank8_reg = bank;
	bankA_reg = bank + 1;
	bankC_reg = bank + 2;
	bankE_reg = bank + 3;
	sync_banks();
	
	if (value & 64) {
	    PPU_mirror_horizontal();
	} else {
	    PPU_mirror_vertical();
	}
    } else if (address == 0x8001) {
	bankC_reg = bank;
	bankE_reg = bank + 1;
	sync_banks();
    } else if (address == 0x8002) {
	bank8_reg = bank + ((value & 0x80)? 1 : 0);
	bankA_reg = bank8_reg;
	bankC_reg = bank8_reg;
	bankE_reg = bank8_reg;
	sync_banks();
    } else if (address == 0x8003) {
	bankC_reg = bank;
	bankE_reg = bank + 1;
	sync_banks();
	
	if (value & 64) {
	    PPU_mirror_horizontal();
	} else {
	    PPU_mirror_vertical();
	}
    } else {
	/* FIXME: what about other addresses? */
    }
}

nes_mapper mapF_init(nes_ppu ppu)
{
    nes_mapper retval;

    retval = malloc(sizeof(struct nes_mapper));

    if (retval) {
	retval->write = mapF_write;
	retval->hsync = dummy_hsync;

	ppu_latchfunc = NULL;
	mapF_write(retval, 0, 0);
	bank8_reg = 0;
	bankA_reg = 1;
	bankC_reg = 2;
	bankE_reg = 3;
	sync_banks();
    }
    return retval;
}
#endif


/* MAPPER 17 (23) Implementation */

struct mapper_17 {
    struct nes_mapper interface;
    nes_ppu ppu;
    unsigned char bank_regs[16];
};

void map17_sync_vrom(struct mapper_17 *mapper, int bank)
{
    unsigned char value;

    value = mapper->bank_regs[bank];
    value |= mapper->bank_regs[bank + 1] << 4;

    nesppu_map_1k(mapper->ppu, bank >> 1, value);
}

void map17_write(struct mapper_17 *mapper, unsigned short address, unsigned char value)
{
    if (address == 0x8000) {
	nesprg_map_8k(0, value);
    } else if (address == 0x9000) {
	switch (value & 3) {
	case 0:
	    PPU_mirror_vertical();
	    break;
	case 1:
	    PPU_mirror_horizontal();
	    break;
	case 2:
	    PPU_mirror_one_low();
	    break;
	case 3:
	    PPU_mirror_one_high();
	    break;
	}
    } else if (address == 0xa000) {
	nesprg_map_8k(1, value);
    } else if (((address & 0x8ffc) == 0x8000)
	       && (address >= 0xb000)
	       && (address < 0xf000)) {
	u16 base;

	base = (address >> 10) - 0x2c;
	mapper->bank_regs[base + (address & 3)] = value & 15;
	map17_sync_vrom(mapper, base + (address & 2));
    } else {
	deb_printf("map23: unknown addr 0x%04hx = 0x%02x.\n", address, value);
    }
}

nes_mapper map17_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_17 *retval;

    retval = malloc(sizeof(struct mapper_17));

    if (retval) {
	retval->interface.write = (mapwrite_t)map17_write;
	retval->interface.hsync = dummy_hsync;
	retval->ppu = ppu;

	ppu_latchfunc = NULL;
#if 0
	nesppu_map_8k(ppu, 0, 0);
#endif
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper)retval;
}


/* MAPPER 18 (24) Implementation */

struct mapper_18 {
    struct nes_mapper interface;
    nes_ppu ppu;
    unsigned char irq_latch;
    unsigned char irq_count;
    unsigned char irq_state;
};

void map18_write(struct mapper_18 *mapper, unsigned short address, unsigned char value)
{
    if (address == 0x8000) {
	nesprg_map_16k(0, value);
    } else if (address < 0x9000) {
	deb_printf("map18_write: unknown write in 0x8001-0x8fff range.\n");
    } else if (address == 0xb003) {
	switch (value & 0x0c) {
	case 0x00:
	    PPU_mirror_vertical();
	    break;
	case 0x04:
	    PPU_mirror_horizontal();
	    break;
	case 0x08:
	    PPU_mirror_one_low();
	    break;
	case 0x0c:
	    PPU_mirror_one_high();
	    break;
	}
    } else if ((address & 0xfffc) == 0xd000) {
	nesppu_map_1k(mapper->ppu, address & 3, value);
    } else if ((address & 0xfffc) == 0xe000) {
	nesppu_map_1k(mapper->ppu, 4 + (address & 3), value);
    } else if ((address & 0xfffc) == 0xf000) {
	switch (address & 3) {
	case 0:
	    mapper->irq_latch = value;
	    break;
	case 1:
	    mapper->irq_state = value;
	    mapper->irq_count = mapper->irq_latch;
	    break;
	case 2:
	    mapper->irq_state = (mapper->irq_state << 1) | (mapper->irq_state & 1);
	    break;
	case 3:
	    deb_printf("map18: unknown write 0x%02x to 0xf003.\n", value);
	    break;
	}
    }
}

void map18_hsync(struct mapper_18 *mapper, cal_cpu cpu, int display_active)
{
    if ((mapper->irq_state & 2) && (++mapper->irq_count == 0)) {
	cpu->irq(cpu, 0);
	mapper->irq_count = mapper->irq_latch;
    }
}

nes_mapper map18_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_18 *retval;

    retval = malloc(sizeof(struct mapper_18));

    if (retval) {
	retval->interface.write = (mapwrite_t)map18_write;
	retval->interface.hsync = (maphsync_t)map18_hsync;
	retval->ppu = ppu;
	
	ppu_latchfunc = NULL;
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper)retval;
}


#if 0 /* doesn't work, not high priority */

/* MAPPER 20 (32) Implementation */

unsigned char map20_saved_9fff;

void map20_write(unsigned short address, unsigned char value)
{
    if (address == 0x8fff) {
	if (map20_saved_9fff & 0x02) {
	    bankC_reg = value;
	} else {
	    bank8_reg = value;
	}
	sync_banks();
    } else if (address == 0x9fff) {
	map20_saved_9fff = value;
	if (value & 1) {
	    PPU_mirror_horizontal();
	} else {
	    PPU_mirror_vertical();
	}
    } else if (address == 0xafff) {
	bankA_reg = value;
	sync_banks();
    } else if ((address & 0xfff8) == 0xbff0) {
	nesppu_map_1k(mapper->ppu, address & 7, value);
    } else {
	deb_printf("map32: unknown address 0x%04hx = 0x%02x.\n", address, value);
    }
}

void map20_init(void)
{
    ppu_latchfunc = NULL;
    bank8_reg = 0;
    bankA_reg = 0;
    bankC_reg = 0xfe;
    bankE_reg = 0xff;
    sync_banks();
}

mapper_iface_t map20_iface = {map20_init, map20_write, dummy_hsync, dummy_vsync};

#endif


/* MAPPER 42 (66) Implementation */

struct mapper_42 {
    struct nes_mapper interface;
    nes_ppu ppu;
};

void map42_write(struct mapper_42 *mapper, unsigned short address, unsigned char value)
{
    unsigned char vromptr;
    unsigned char romptr;

    vromptr = value & 0x03;
    romptr = (value >> 4) & 0x03;

    nesppu_map_8k(mapper->ppu, 0, vromptr);
    nesprg_map_32k(0, romptr);
}

nes_mapper map42_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_42 *retval;

    retval = malloc(sizeof(struct mapper_42));

    if (retval) {
	retval->interface.write = (mapwrite_t)map42_write;
	retval->interface.hsync = dummy_hsync;
	retval->ppu = ppu;
	
	ppu_latchfunc = NULL;
	map42_write(retval, 0, 0);
    }

    return (nes_mapper)retval;
}


/* MAPPER 44 (68) Implementation */

struct mapper_44 {
    struct nes_mapper interface;
    nes_ppu ppu;
    nes_rom romfile;
    unsigned char ppu_a10l;
    unsigned char ppu_a10h;
    unsigned char mirror;
};

void map44_sync_mirrors(struct mapper_44 *mapper)
{
    unsigned char *namebase;
    
    if (mapper->mirror & 0x10) {
	namebase = mapper->romfile->chr_data + 0x20000;
	switch (mapper->mirror & 3) {
	case 0:
	    PPU_nametables[0] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[1] = namebase + (mapper->ppu_a10h << 10);
	    PPU_nametables[2] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[3] = namebase + (mapper->ppu_a10h << 10);
	    break;
	case 1:
	    PPU_nametables[0] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[1] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[2] = namebase + (mapper->ppu_a10h << 10);
	    PPU_nametables[3] = namebase + (mapper->ppu_a10h << 10);
	    break;
	case 2:
	    PPU_nametables[0] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[1] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[2] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[3] = namebase + (mapper->ppu_a10l << 10);
	    break;
	case 3:
	    PPU_nametables[0] = namebase + (mapper->ppu_a10h << 10);
	    PPU_nametables[1] = namebase + (mapper->ppu_a10h << 10);
	    PPU_nametables[2] = namebase + (mapper->ppu_a10h << 10);
	    PPU_nametables[3] = namebase + (mapper->ppu_a10h << 10);
	    break;
	}
    } else {
	switch (mapper->mirror & 3) {
	case 0:
	    PPU_mirror_horizontal();
	    break;
	case 1:
	    PPU_mirror_vertical();
	    break;
	case 2:
	    PPU_mirror_one_low();
	    break;
	case 3:
	    PPU_mirror_one_high();
	    break;
	}
    }
}

void map44_write(struct mapper_44 *mapper, unsigned short address, unsigned char value)
{
    switch ((address & 0x7000) >> 12) {
    case 0:
	nesppu_map_2k(mapper->ppu, 0, value);
	break;
    case 1:
	nesppu_map_2k(mapper->ppu, 1, value);
	break;
    case 2:
	nesppu_map_2k(mapper->ppu, 2, value);
	break;
    case 3:
	nesppu_map_2k(mapper->ppu, 3, value);
	break;
    case 4:
	mapper->ppu_a10l = value;
	map44_sync_mirrors(mapper);
	break;
    case 5:
	mapper->ppu_a10h = value;
	map44_sync_mirrors(mapper);
	break;
    case 6:
	mapper->mirror = value;
	map44_sync_mirrors(mapper);
	break;
    case 7:
	nesprg_map_16k(0, value);
	break;
    }
}

nes_mapper map44_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_44 *retval;

    retval = malloc(sizeof(struct mapper_44));

    if (retval) {
	retval->interface.write = (mapwrite_t)map44_write;
	retval->interface.hsync = dummy_hsync;
	retval->ppu = ppu;
	retval->romfile = romfile;
	
	ppu_latchfunc = NULL;
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper)retval;
}


/* MAPPER 4E (78) Implementation */

struct mapper_4E {
    struct nes_mapper interface;
    nes_ppu ppu;
};

void map4E_write(struct mapper_4E *mapper, unsigned short address, unsigned char value)
{
    unsigned char vromptr;
    unsigned char romptr;

    vromptr = (value >> 4) & 0x0f;
    romptr = value & 0x0f;

    nesppu_map_8k(mapper->ppu, 0, vromptr);
    nesprg_map_16k(0, romptr);
}

nes_mapper map4E_init(nes_ppu ppu, nes_rom romfile)
{
    struct mapper_4E *retval;

    retval = malloc(sizeof(struct mapper_4E));

    if (retval) {
	retval->interface.write = (mapwrite_t)map4E_write;
	retval->interface.hsync = dummy_hsync;
	retval->ppu = ppu;

	ppu_latchfunc = NULL;
	
	nesprg_map_16k(1, -1);
	map4E_write(retval, 0, 0);
    }

    return (nes_mapper)retval;
}


/*
 * $Log: mappers.c,v $
 * Revision 1.98  2000/12/14 21:47:07  nyef
 * removed some small silliness surrounding bank_select in map1_write()
 *
 * Revision 1.97  2000/10/05 06:53:03  nyef
 * converted to use the new PRG-ROM mapping interface
 * this may break the following mappers (untested/insufficiently tested
 * changes): 1, 9, a (10), 17 (23), 18 (24), 42 (66), 44 (68), 4e (78)
 *
 * Revision 1.96  2000/06/28 01:35:04  nyef
 * moved map14_* (mapper 20, FDS) out to fds.c
 *
 * Revision 1.95  2000/06/27 02:08:30  nyef
 * hacked in FDS timer IRQs (with 113 cycle resolution)
 * removed some FDS debug code
 *
 * Revision 1.94  2000/06/25 20:45:56  nyef
 * fixed FDS (moved the diskptr hack to where it does some good)
 *
 * Revision 1.93  2000/06/25 19:23:17  nyef
 * added a menu item to switch FDS disk sides
 *
 * Revision 1.92  2000/06/25 18:59:49  nyef
 * added a preliminary FDS menu
 *
 * Revision 1.91  2000/06/07 00:39:28  nyef
 * deleted all mention of vsync routines
 *
 * Revision 1.90  2000/06/06 02:12:47  nyef
 * removed the mapper 4 vsync hook (it didn't appear to be doing anything)
 *
 * Revision 1.89  2000/06/02 02:21:14  nyef
 * cleaned up mapper 17 (23) a little bit (untested)
 *
 * Revision 1.88  2000/05/27 04:50:15  nyef
 * removed almost all references to romfile->prg_size
 *
 * Revision 1.87  2000/03/04 04:46:34  nyef
 * fixed bug in mapper 4 IRQs (SMB3 fortress stages now work)
 *
 * Revision 1.86  2000/02/21 20:28:22  nyef
 * much cleanuppance of FDS code
 * added (inaccessible save via GDB) support for ejecting disks and
 * changing disk sides
 *
 * Revision 1.85  2000/02/21 03:49:53  nyef
 * cleaned up the FDS control register emulation a bit
 *
 * Revision 1.84  2000/02/19 20:03:41  nyef
 * fixed FDS mapper to work with the new bank 4 and 6 hook interface
 *
 * Revision 1.83  2000/02/19 19:43:58  nyef
 * fixed FDS mapper to compile again
 * removed some of the more broken debug code from the FDS mapper
 *
 * Revision 1.82  2000/02/05 19:35:24  nyef
 * changed mapper 18 (24) IRQs to improve CV3j vertical scrolling areas
 *
 * Revision 1.81  2000/01/04 01:29:11  nyef
 * fixed mapper 2 to behave correctly should it ever encounter CHR-ROM
 *
 * Revision 1.80  1999/11/20 18:32:37  nyef
 * converted mappers 42 (66) (not tested), 44 (68), and 4E (68) (not tested)
 *
 * Revision 1.79  1999/11/20 18:23:27  nyef
 * converted mappers 17 (23) and 18 (24)
 *
 * Revision 1.78  1999/11/20 18:20:40  nyef
 * converted mapper B (11)
 *
 * Revision 1.77  1999/11/20 18:11:24  nyef
 * converted mappers 7, 9, and A (10)
 *
 * Revision 1.76  1999/11/20 18:02:13  nyef
 * converted mapper 4
 *
 * Revision 1.75  1999/11/20 17:57:45  nyef
 * converted mappers 2 and 3
 *
 * Revision 1.74  1999/11/20 17:43:40  nyef
 * converted mapper 1
 *
 * Revision 1.73  1999/11/20 05:40:50  nyef
 * changed to use new rom file interface
 * all mappers other than 0 disabled
 *
 * Revision 1.72  1999/11/16 01:53:49  nyef
 * half-converted mapper 20 (32) (doesn't work)
 * stripped out all the vestiges of the old CHR-ROM switching
 *     scheme that I could find
 *
 * Revision 1.71  1999/11/16 01:11:14  nyef
 * converted mapper 4E (78) (not tested)
 *
 * Revision 1.70  1999/11/15 05:43:51  nyef
 * converted mapper 44 (68)
 *
 * Revision 1.69  1999/11/15 05:34:55  nyef
 * converted mapper 42 (66) (not tested)
 *
 * Revision 1.68  1999/11/15 05:27:03  nyef
 * converted mapper 18 (24)
 *
 * Revision 1.67  1999/11/15 05:19:53  nyef
 * converted mapper 17 (23) (not tested)
 *
 * Revision 1.66  1999/11/15 05:08:00  nyef
 * converted mapper F (15)
 *
 * Revision 1.65  1999/11/15 04:05:42  nyef
 * all functioning mapper converted to new CHR ROM mapping interface
 *
 * Revision 1.64  1999/11/14 17:36:33  nyef
 * removed support for 1024k MMC1 games (there's no such thing)
 *
 * Revision 1.63  1999/11/14 17:29:50  nyef
 * converted mapper B (11)
 *
 * Revision 1.62  1999/11/14 16:19:42  nyef
 * converted mappers 7, 9, and A (10)
 *
 * Revision 1.61  1999/11/14 15:34:29  nyef
 * converted mappers 2, 3, and 4
 *
 * Revision 1.60  1999/11/14 07:01:14  nyef
 * changed to use new mapper interface
 * only mappers 0 and 1 have been converted, all other mappers are disabled
 *
 * Revision 1.59  1999/10/28 01:04:28  nyef
 * stripped out all the so-called "outrange bank compensation" code as
 * sync_banks() in nes.c now handles this
 *
 * Revision 1.58  1999/10/22 03:14:34  nyef
 * added preliminary IRQ support to mapper 18 (24), VRC6.
 * added mirroring support to VRC6.
 *
 * Revision 1.57  1999/10/21 23:35:42  nyef
 * changed to use new mapper hblank interface
 *
 * Revision 1.56  1999/09/03 00:31:35  nyef
 * fixed mapper 1 mirroring
 *
 * Revision 1.55  1999/08/29 13:05:51  nyef
 * updated to new IRQ interface
 *
 * Revision 1.54  1999/07/05 01:30:02  nyef
 * fixed outrange rom bank compensation on mapper 4
 *
 * Revision 1.53  1999/05/04 01:55:37  nyef
 * added preliminary support for mapper 24
 *
 * Revision 1.52  1999/05/03 00:35:52  nyef
 * added ppu mirroring control to FDS
 *
 * Revision 1.51  1999/05/02 21:34:11  nyef
 * fixed typo in FDS mapper (14 (20)) that was causing problems
 *
 * Revision 1.50  1999/02/20 19:08:58  nyef
 * added large amounts of stuff to the FDS mapper (still doesn't work)
 *
 * Revision 1.49  1999/02/14 06:39:30  nyef
 * added preliminary support for mapper 14 (20), the FDS mapper
 *
 * Revision 1.48  1999/01/03 03:16:05  nyef
 * added Kritz's changes to mapper 20 (32).
 *
 * Revision 1.47  1999/01/02 19:09:25  nyef
 * added support for mapper F (15) by kritz.
 *
 * Revision 1.46  1999/01/02 16:56:28  nyef
 * fixed infinite loop in mapper 7.
 *
 * Revision 1.45  1999/01/02 01:17:17  nyef
 * added preliminary mapper 17 (23) support.
 *
 * Revision 1.44  1999/01/02 00:51:52  nyef
 * added preliminary (nonfunctional) mapper 20 (32) support.
 *
 * Revision 1.43  1999/01/01 22:11:40  nyef
 * added support for mapper 44 (68).
 *
 * Revision 1.42  1998/12/31 22:35:47  nyef
 * added outrange vrom bank compensation to mapper 4.
 * Crystalis no longer breaks at the name screen.
 *
 * Revision 1.41  1998/12/31 19:06:49  nyef
 * added preliminary (untested) support for mapper 42 (66).
 *
 * Revision 1.40  1998/12/31 18:59:20  nyef
 * added preliminary (untested) support for mapper 4E (78).
 *
 * Revision 1.39  1998/12/31 03:38:31  nyef
 * changed map4_hsync() to only check irq counter if screen is on.
 *
 * Revision 1.38  1998/12/29 02:15:35  nyef
 * fixed map4_sync_rom() so that the SMB3 intro sortof works.
 *
 * Revision 1.37  1998/12/26 01:02:58  nyef
 * changed mapper system to support support for mappers > 15.
 *
 * Revision 1.36  1998/12/20 03:43:13  nyef
 * updated mapper_support[] table.
 *
 * Revision 1.35  1998/12/20 03:31:32  nyef
 * added second latch to mapper 9.
 * mapper 9 now works just about perfectly.
 *
 * Revision 1.34  1998/12/15 04:41:27  nyef
 * added support for 512k and 1024k mapper 1 carts.
 *
 * Revision 1.33  1998/12/15 03:50:14  nyef
 * fixed bank mask bug in mapper 1.
 *
 * Revision 1.32  1998/12/15 03:43:20  nyef
 * reorganized the mapper 1 implementation to be much cleaner looking.
 *
 * Revision 1.31  1998/12/15 01:50:24  nyef
 * changed mapper hsync and vsync routines to conform to new spec.
 *
 * Revision 1.30  1998/11/28 03:59:31  nyef
 * changed map4_hsync() to postdecrement rather than predecrement the irq
 * count. Crystalis looks slightly better.
 *
 * Revision 1.29  1998/11/16 23:25:53  nyef
 * fixed mistake in the mapper 10 latchfunc.
 *
 * Revision 1.28  1998/11/05 03:07:40  nyef
 * added default VRom mapping to mapper 1.
 *
 * Revision 1.27  1998/11/03 01:41:24  nyef
 * added preliminary support for mapper 10.
 *
 * Revision 1.26  1998/11/01 22:09:40  nyef
 * commented out mapper 4 mirror control deb_printf.
 *
 * Revision 1.25  1998/10/04 14:14:16  nyef
 * fixed out-range bank compensation on mappers 11, 7, 3, 2 and 1.
 *
 * Revision 1.24  1998/10/04 14:10:52  nyef
 * fixed out-range rom bank compensation on mapper 4.
 * this will probably need to be propagated to other mappers.
 *
 * Revision 1.23  1998/10/03 23:18:36  nyef
 * added out-range rom bank compensation to mapper 4.
 *
 * Revision 1.22  1998/10/03 23:10:58  nyef
 * changed mapper 1 to only have 1 shift register.
 * this may improve some games.
 *
 * Revision 1.21  1998/10/03 13:46:18  nyef
 * added more code to mapper 11. (still doesn't work)
 *
 * Revision 1.20  1998/10/01 02:43:16  nyef
 * added preliminary support for mapper B (11).
 *
 * Revision 1.19  1998/10/01 02:35:17  nyef
 * added support for mapper 9.
 *
 * Revision 1.18  1998/09/01 01:00:01  nyef
 * fixed mapper 0. (added vrom stuff. oops.)
 *
 * Revision 1.17  1998/08/30 17:07:10  nyef
 * changed to use the new ppu_flags system instead of PPU_use_vrom.
 *
 * Revision 1.16  1998/08/24 01:57:53  nyef
 * fixed bug with 8k vrom pages on mapper 1.
 *
 * Revision 1.15  1998/08/24 01:24:19  nyef
 * more work on mapper 4.
 *
 * Revision 1.14  1998/08/24 00:29:06  nyef
 * more changes to mapper 4.
 *
 * Revision 1.13  1998/08/23 22:42:46  nyef
 * added hooks for calling mappers on vsync and hsync.
 * added preliminary irq support to mapper 4.
 *
 * Revision 1.12  1998/08/23 18:59:16  nyef
 * more changes to mapper 1. effectiveness unknown.
 *
 * Revision 1.11  1998/08/22 19:33:57  nyef
 * changes to mapper 4. effectiveness unknown.
 *
 * Revision 1.10  1998/08/22 18:41:37  nyef
 * more changes to mapper 1.
 *
 * Revision 1.9  1998/08/22 17:53:22  nyef
 * fixed more bugs in the mapper 1 emulation.
 *
 * Revision 1.8  1998/08/22 16:37:59  nyef
 * added support for mapper 1 roms that don't use VROM.
 *
 * Revision 1.7  1998/08/22 16:33:12  nyef
 * added preliminary VROM support to mapper 4.
 *
 * Revision 1.6  1998/08/04 21:56:31  nyef
 * changed the bit masks on the vrom selectors for mapper 1.
 *
 * Revision 1.5  1998/08/04 02:03:04  nyef
 * added an initialization message to set_mapper().
 *
 * Revision 1.4  1998/08/02 13:46:03  nyef
 * changed every printf to deb_printf for debug console.
 *
 * Revision 1.3  1998/07/31 02:28:00  nyef
 * upgraded the mapper 1 emulation somewhat. still incomplete, but better.
 *
 * Revision 1.2  1998/07/18 21:55:52  nyef
 * added preliminary mapper 7 support
 *
 * Revision 1.1  1998/07/11 22:18:27  nyef
 * Initial revision
 *
 */
