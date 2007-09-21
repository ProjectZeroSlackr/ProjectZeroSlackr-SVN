/*
 * Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include <stdio.h>
#include "../pz.h"

/* - - - PERIODIC TABLE DATA - - - */

typedef struct periodic_element_ {
	int group;
	int period;
	char * symbol;
	char * name;
	float mass;
	float density;
	float fpoint; /* freezing point */
	float bpoint; /* boiling point */
	char block; /* s, p, d, f */
	int color; /* in periodic_color_palette below */
} periodic_element;

const GR_COLOR periodic_color_palette[] = {
	GR_RGB(255,255,255), /* 0 hydrogen */
	GR_RGB(221,221,221), /* 1 alkali metals */
	GR_RGB(187,187,187), /* 2 earth metals */
	GR_RGB(255,204,255), /* 3 transition metals */
	GR_RGB(255,204,204), /* 4 other metals */
	GR_RGB(255,204,153), /* 5 metalloids */
	GR_RGB(255,255,204), /* 6 nonmetals */
	GR_RGB(204,255,204), /* 7 halogens */
	GR_RGB(204,204,153), /* 8 astatine */
	GR_RGB(204,255,255), /* 9 noble gases */
	GR_RGB(153,204,255), /* 10 lanthanides */
	GR_RGB(204,153,255)  /* 11 actinides */
};

const GR_COLOR periodic_color_palette_bw[] = {
	GR_RGB(255,255,255),	/* 0 hydrogen */
	GR_RGB(50,50,50),	/* 1 alkali metals */
	GR_RGB(50,50,50),	/* 2 earth metals */
	GR_RGB(255,255,255),	/* 3 transition metals */
	GR_RGB(255,255,255),	/* 4 other metals */
	GR_RGB(50,50,50),	/* 5 metalloids */
	GR_RGB(255,255,255),	/* 6 nonmetals */
	GR_RGB(255,255,255),	/* 7 halogens */
	GR_RGB(50,50,50),	/* 8 astatine */
	GR_RGB(255,255,255),	/* 9 noble gases */
	GR_RGB(50,50,50),	/* 10 lanthanides */
	GR_RGB(50,50,50)	/* 11 actinides */
};

const periodic_element periodic_elements[] = {
	{	1,	1,	"H",	"Hydrogen",	1.00794,	0.09,	-259,	-253,	's',	0	},
	{	18,	1,	"He",	"Helium",	4.0026,	0.18,	-272,	-269,	's',	9	},
	{	1,	2,	"Li",	"Lithium",	6.941,	0.53,	180,	1347,	's',	1	},
	{	2,	2,	"Be",	"Beryllium",	9.01218,	1.85,	1278,	2970,	's',	2	},
	{	13,	2,	"B",	"Boron",	10.81,	2.34,	2300,	2550,	'p',	5	},
	{	14,	2,	"C",	"Carbon",	12.0111,	2.26,	3500,	4827,	'p',	6	},
	{	15,	2,	"N",	"Nitrogen",	14.0067,	1.25,	-210,	-196,	'p',	6	},
	{	16,	2,	"O",	"Oxygen",	15.9994,	1.43,	-218,	-183,	'p',	6	},
	{	17,	2,	"F",	"Fluorine",	18.998403,	1.7,	-220,	-188,	'p',	7	},
	{	18,	2,	"Ne",	"Neon",	20.179,	0.9,	-249,	-246,	'p',	9	},
	{	1,	3,	"Na",	"Sodium",	22.98977,	0.97,	98,	883,	's',	1	},
	{	2,	3,	"Mg",	"Magnesium",	24.305,	1.74,	639,	1090,	's',	2	},
	{	13,	3,	"Al",	"Aluminum",	26.98154,	2.7,	660,	2467,	'p',	4	},
	{	14,	3,	"Si",	"Silicon",	28.0855,	2.33,	1410,	2355,	'p',	5	},
	{	15,	3,	"P",	"Phosphorus",	30.97376,	1.82,	44,	280,	'p',	6	},
	{	16,	3,	"S",	"Sulfur",	32.06,	2.07,	113,	445,	'p',	6	},
	{	17,	3,	"Cl",	"Chlorine",	35.453,	3.21,	-101,	-35,	'p',	7	},
	{	18,	3,	"Ar",	"Argon",	39.948,	1.78,	-189,	-186,	'p',	9	},
	{	1,	4,	"K",	"Potassium",	39.0983,	0.86,	64,	774,	's',	1	},
	{	2,	4,	"Ca",	"Calcium",	40.08,	1.55,	839,	1484,	's',	2	},
	{	3,	4,	"Sc",	"Scandium",	44.9559,	2.99,	1539,	2832,	'd',	3	},
	{	4,	4,	"Ti",	"Titanium",	47.88,	4.54,	1660,	3287,	'd',	3	},
	{	5,	4,	"V",	"Vanadium",	50.9415,	6.11,	1890,	3380,	'd',	3	},
	{	6,	4,	"Cr",	"Chromium",	51.996,	7.19,	1857,	2672,	'd',	3	},
	{	7,	4,	"Mn",	"Manganese",	54.938,	7.43,	1245,	1962,	'd',	3	},
	{	8,	4,	"Fe",	"Iron",	55.847,	7.87,	1535,	2750,	'd',	3	},
	{	9,	4,	"Co",	"Cobalt",	58.9332,	8.9,	1495,	2870,	'd',	3	},
	{	10,	4,	"Ni",	"Nickel",	58.69,	8.9,	1453,	2732,	'd',	3	},
	{	11,	4,	"Cu",	"Copper",	63.546,	8.96,	1083,	2567,	'd',	3	},
	{	12,	4,	"Zn",	"Zinc",	65.39,	7.13,	420,	907,	'd',	3	},
	{	13,	4,	"Ga",	"Gallium",	69.72,	5.91,	30,	2403,	'p',	4	},
	{	14,	4,	"Ge",	"Germanium",	72.59,	5.32,	937,	2830,	'p',	5	},
	{	15,	4,	"As",	"Arsenic",	74.9216,	5.72,	81,	613,	'p',	5	},
	{	16,	4,	"Se",	"Selenium",	78.96,	4.79,	217,	685,	'p',	6	},
	{	17,	4,	"Br",	"Bromine",	79.904,	3.12,	-7,	59,	'p',	7	},
	{	18,	4,	"Kr",	"Krypton",	83.8,	3.75,	-157,	-153,	'p',	9	},
	{	1,	5,	"Rb",	"Rubidium",	85.4678,	1.63,	39,	688,	's',	1	},
	{	2,	5,	"Sr",	"Strontium",	87.62,	2.54,	769,	1384,	's',	2	},
	{	3,	5,	"Y",	"Yttrium",	88.9059,	4.47,	1523,	3337,	'd',	3	},
	{	4,	5,	"Zr",	"Zirconium",	91.224,	6.51,	1852,	4377,	'd',	3	},
	{	5,	5,	"Nb",	"Niobium",	92.9064,	8.57,	2468,	4927,	'd',	3	},
	{	6,	5,	"Mo",	"Molybdenum",	95.94,	10.22,	2617,	4612,	'd',	3	},
	{	7,	5,	"Tc",	"Technetium",	98.906,	11.5,	2200,	4877,	'd',	3	},
	{	8,	5,	"Ru",	"Ruthenium",	101.07,	12.37,	2250,	3900,	'd',	3	},
	{	9,	5,	"Rh",	"Rhodium",	102.9055,	12.41,	1966,	3727,	'd',	3	},
	{	10,	5,	"Pd",	"Palladium",	106.42,	12.02,	1552,	2927,	'd',	3	},
	{	11,	5,	"Ag",	"Silver",	107.868,	10.5,	962,	2212,	'd',	3	},
	{	12,	5,	"Cd",	"Cadmium",	112.41,	8.65,	321,	765,	'd',	3	},
	{	13,	5,	"In",	"Indium",	114.82,	7.31,	157,	2000,	'p',	4	},
	{	14,	5,	"Sn",	"Tin",	118.71,	7.31,	232,	2270,	'p',	4	},
	{	15,	5,	"Sb",	"Antimony",	121.75,	6.68,	630,	1750,	'p',	5	},
	{	16,	5,	"Te",	"Tellurium",	127.6,	6.24,	449,	990,	'p',	5	},
	{	17,	5,	"I",	"Iodine",	126.905,	4.93,	114,	184,	'p',	7	},
	{	18,	5,	"Xe",	"Xenon",	131.29,	5.9,	-112,	-108,	'p',	9	},
	{	1,	6,	"Cs",	"Caesium",	132.905,	1.87,	29,	678,	's',	1	},
	{	2,	6,	"Ba",	"Barium",	137.33,	3.59,	725,	1140,	's',	2	},
	{	3,	6,	"La",	"Lanthanium",	138.906,	6.15,	920,	3469,	'd',	3	},
	{	3,	9,	"Ce",	"Cerium",	140.12,	6.77,	795,	3257,	'f',	10	},
	{	4,	9,	"Pr",	"Praseodymium",	140.908,	6.77,	935,	3127,	'f',	10	},
	{	5,	9,	"Nd",	"Neodymium",	144.24,	7.01,	1010,	3127,	'f',	10	},
	{	6,	9,	"Pm",	"Promethium",	146.92,	7.3,	1100,	3000,	'f',	10	},
	{	7,	9,	"Sm",	"Samarium",	150.36,	7.52,	1072,	1900,	'f',	10	},
	{	8,	9,	"Eu",	"Europium",	151.96,	5.24,	822,	1597,	'f',	10	},
	{	9,	9,	"Gd",	"Gadolinium",	157.25,	7.9,	1311,	3233,	'f',	10	},
	{	10,	9,	"Tb",	"Terbium",	158.925,	8.23,	1360,	3041,	'f',	10	},
	{	11,	9,	"Dy",	"Dysprosium",	162.5,	8.55,	1412,	2562,	'f',	10	},
	{	12,	9,	"Ho",	"Holmium",	164.93,	8.8,	1470,	2720,	'f',	10	},
	{	13,	9,	"Er",	"Erbium",	167.26,	9.07,	1522,	2510,	'f',	10	},
	{	14,	9,	"Tm",	"Thulium",	168.934,	9.32,	1545,	1727,	'f',	10	},
	{	15,	9,	"Yb",	"Ytterbium",	173.04,	6.9,	824,	1466,	'f',	10	},
	{	16,	9,	"Lu",	"Lutetium",	174.967,	9.84,	1656,	3315,	'f',	10	},
	{	4,	6,	"Hf",	"Hafnium",	178.49,	13.31,	2150,	5400,	'd',	3	},
	{	5,	6,	"Ta",	"Tantalum",	180.9479,	16.65,	2996,	5425,	'd',	3	},
	{	6,	6,	"W",	"Tungsten",	183.85,	19.35,	3410,	5660,	'd',	3	},
	{	7,	6,	"Re",	"Rhenium",	186.207,	21.04,	3180,	5627,	'd',	3	},
	{	8,	6,	"Os",	"Osmium",	190.2,	22.6,	3045,	5027,	'd',	3	},
	{	9,	6,	"Ir",	"Iridium",	192.22,	22.4,	2410,	4527,	'd',	3	},
	{	10,	6,	"Pt",	"Platinum",	195.08,	21.45,	1772,	3827,	'd',	3	},
	{	11,	6,	"Au",	"Gold",	196.9665,	19.32,	1064,	2807,	'd',	3	},
	{	12,	6,	"Hg",	"Mercury",	200.59,	13.55,	-39,	357,	'd',	3	},
	{	13,	6,	"Tl",	"Thallium",	204.383,	11.85,	303,	1457,	'p',	4	},
	{	14,	6,	"Pb",	"Lead",	207.2,	11.35,	327,	1740,	'p',	4	},
	{	15,	6,	"Bi",	"Bismuth",	208.9804,	9.75,	271,	1560,	'p',	4	},
	{	16,	6,	"Po",	"Polonium",	208.98,	9.3,	254,	962,	'p',	4	},
	{	17,	6,	"At",	"Astatine",	209.99,	0,	302,	337,	'p',	8	},
	{	18,	6,	"Rn",	"Radon",	222.02,	9.73,	-71,	-62,	'p',	9	},
	{	1,	7,	"Fr",	"Francium",	223.02,	0,	27,	677,	's',	1	},
	{	2,	7,	"Ra",	"Radium",	226.025,	5.5,	700,	1737,	's',	2	},
	{	3,	7,	"Ac",	"Actinium",	227.0278,	10.07,	1050,	3200,	'd',	3	},
	{	3,	10,	"Th",	"Thorium",	232.0381,	11.72,	1750,	4790,	'f',	11	},
	{	4,	10,	"Pa",	"Protactinium",	231.0359,	15.4,	1568,	0,	'f',	11	},
	{	5,	10,	"U",	"Uranium",	238.029,	18.95,	1132,	3818,	'f',	11	},
	{	6,	10,	"Np",	"Neptunium",	237.0482,	20.2,	640,	3902,	'f',	11	},
	{	7,	10,	"Pu",	"Plutonium",	244.06,	19.84,	640,	3235,	'f',	11	},
	{	8,	10,	"Am",	"Americiam",	243.06,	13.67,	994,	2607,	'f',	11	},
	{	9,	10,	"Cm",	"Curium",	247.07,	13.5,	1340,	0,	'f',	11	},
	{	10,	10,	"Bk",	"Berkelium",	247.07,	14.78,	986,	0,	'f',	11	},
	{	11,	10,	"Cf",	"Californium",	251.08,	15.1,	900,	0,	'f',	11	},
	{	12,	10,	"Es",	"Einsteinium",	252.08,	0,	860,	0,	'f',	11	},
	{	13,	10,	"Fm",	"Fermium",	257.1,	0,	1527,	0,	'f',	11	},
	{	14,	10,	"Md",	"Mendelevium",	258.1,	0,	0,	0,	'f',	11	},
	{	15,	10,	"No",	"Nobelium",	259.1,	0,	827,	0,	'f',	11	},
	{	16,	10,	"Lr",	"Lawrencium",	260.11,	0,	1627,	0,	'f',	11	},
	{	4,	7,	"Rf",	"Rutherfordium",	261,	0,	0,	0,	'd',	3	},
	{	5,	7,	"Db",	"Dubnium",	262,	0,	0,	0,	'd',	3	},
	{	6,	7,	"Sg",	"Seaborgium",	266,	0,	0,	0,	'd',	3	},
	{	7,	7,	"Bh",	"Bhorium",	264,	0,	0,	0,	'd',	3	},
	{	8,	7,	"Hs",	"Hassium",	269,	0,	0,	0,	'd',	3	},
	{	9,	7,	"Mt",	"Meitnerium",	268,	0,	0,	0,	'd',	3	},
	{	10,	7,	"Ds",	"Darmstadtium",	271,	0,	0,	0,	'd',	3	},
	{	11,	7,	"Rg",	"Roentgenium",	272,	0,	0,	0,	'd',	3	},
	{	12,	7,	"Uub",	"Ununbium",	285,	0,	0,	0,	'd',	3	},
	{	13,	7,	"Uut",	"Ununtrium",	284,	0,	0,	0,	'p',	4	},
	{	14,	7,	"Uuq",	"Ununquadium",	289,	0,	0,	0,	'p',	4	},
	{	15,	7,	"Uup",	"Ununpentium",	288,	0,	0,	0,	'p',	4	},
	{	16,	7,	"Uuh",	"Ununhexium",	292,	0,	0,	0,	'p',	4	},
	{	17,	7,	"Uus",	"Ununseptium",	0,	0,	0,	0,	'p',	4	},
	{	18,	7,	"Uuo",	"Ununoctium",	0,	0,	0,	0,	'p',	9	}
};

/* - - - PERIODIC TABLE VARIABLES - - - */

int periodic_sel = 0;

GR_WINDOW_ID periodic_wid;
GR_WINDOW_ID periodic_bufwid;
GR_GC_ID periodic_gc;

/* - - - TINYFONT - - - */

const long int periodic_tinyfont_bitmaps[] = {
	0x00000000, 0x00220200, 0x00550000, 0x00575750, 0x00236200, 0x00514500, 0x00236300, 0x00220000,
	0x00244200, 0x00211200, 0x00272720, 0x00027200, 0x00000240, 0x00007000, 0x00000200, 0x00122400,
	0x00255200, 0x00262700, 0x00612700, 0x00631600, 0x00571100, 0x00761600, 0x00365200, 0x00712200,
	0x00725200, 0x00353100, 0x00020200, 0x00020240, 0x00024200, 0x00070700, 0x00021200, 0x00610200,
	
	0x00254300, 0x00257500, 0x00675600, 0x00344300, 0x00655600, 0x00764700, 0x00746400, 0x00345300,
	0x00577500, 0x00722700, 0x00722400, 0x00566500, 0x00444700, 0x00775500, 0x00575500, 0x00255200,
	0x00656400, 0x00255300, 0x00656500, 0x00363600, 0x00722200, 0x00555200, 0x00552200, 0x00557700,
	0x00522500, 0x00522200, 0x00714700, 0x00322300, 0x00422100, 0x00622600, 0x00250000, 0x00000700,
	
	0x00420000, 0x00063300, 0x00465600, 0x00034300, 0x00135300, 0x00026300, 0x00127200, 0x00035360,
	0x00465500, 0x00202200, 0x00202240, 0x00456500, 0x00222200, 0x00067500, 0x00065500, 0x00025200,
	0x00065640, 0x00035310, 0x00056400, 0x00032600, 0x00272100, 0x00055300, 0x00057200, 0x00057700,
	0x00052500, 0x00055360, 0x00072700, 0x00162100, 0x00222220, 0x00432400, 0x00630000, 0x00525250
};

void periodic_tinyfont_draw_char(GR_WINDOW_ID wid, GR_GC_ID gc, GR_COORD x, GR_COORD y, char ch)
{
	int i,j;
	int r;
	long int bmp = periodic_tinyfont_bitmaps[ch-' '];
	for (i=7; i>=0; i--) {
		r = bmp & 15;
		bmp /= 16;
		for (j=3; j>=0; j--) {
			if (r & 1) {
				GrPoint(wid, gc, x+j, y+i);
			}
			r /= 2;
		}
	}
}

void periodic_tinyfont_draw_string(GR_WINDOW_ID wid, GR_GC_ID gc, GR_COORD x, GR_COORD y, char * s)
{
	char * t = s;
	GR_COORD xx = x;
	GR_COORD yy = y;
	while (*t) {
		if ((*t) == 10) {
			xx = x;
			yy += 8;
		} else {
			periodic_tinyfont_draw_char(wid, gc, xx, yy, *t);
			xx += 4;
		}
		t++;
	}
}

void periodic_tinyfont_draw_chart(GR_WINDOW_ID wid, GR_GC_ID gc, GR_COORD x, GR_COORD y)
{
	char ch;
	GR_COORD xx = x;
	GR_COORD yy = y;
	for (ch = 32; ch < 128; ch++) {
		periodic_tinyfont_draw_char(wid, gc, xx, yy, ch);
		if ((ch % 16) == 15) {
			xx = x;
			yy += 8;
		} else {
			xx += 4;
		}
	}
}

/* - - - PERIODIC TABLE HELPER FUNCTIONS - - - */

/* textview.c */
extern void new_stringview_window(char *buf, char *title);

int periodic_fix_period(int p)
{
	return (p>8)?(p-3):p;
}

int periodic_fix_group(int g, int p)
{
	return (p>8)?0:g;
}

char * periodic_group_name(int i)
{
	if (i==1) {
		return _("Noble Gases");
	} else if (periodic_elements[i].period > 8) {
		switch (periodic_elements[i].period) {
		case 9:
			return _("Lanthanides");
			break;
		case 10:
			return _("Actinides");
			break;
		}
		return "";
	} else if (i>1) {
		switch (periodic_elements[i].group) {
		case 1:
			return _("Alkali Metals");
			break;
		case 2:
			return _("Alkaline Earth Metals");
			break;
		case 15:
			return _("Pnictogens");
			break;
		case 16:
			return _("Chalcogens");
			break;
		case 17:
			return _("Halogens");
			break;
		case 18:
			return _("Noble Gases");
			break;
		}
	}
	return "";
}

void periodic_draw_element(GR_WINDOW_ID wid, GR_GC_ID gc, int i, int bx, int by, int cellsize)
{
	int ex = (bx+((periodic_elements[i].group-1)*cellsize));
	int ey = (by+((periodic_elements[i].period-1)*cellsize));
	int tx, ty;
	char tt[4];
	if (i == periodic_sel) {
		GrSetGCForeground(gc, GR_RGB(0,0,0));
		GrFillRect(wid, gc, ex, ey, cellsize+1, cellsize+1);
		GrSetGCForeground(gc, GR_RGB(255,255,255));
	} else {
		if (screen_info.bpp < 8) {
			GrSetGCForeground(gc, periodic_color_palette_bw[periodic_elements[i].color]);
		} else {
			GrSetGCForeground(gc, periodic_color_palette[periodic_elements[i].color]);
		}
		GrFillRect(wid, gc, ex, ey, cellsize+1, cellsize+1);
		GrSetGCForeground(gc, GR_RGB(0,0,0));
		GrRect(wid, gc, ex, ey, cellsize+1, cellsize+1);
	}
	if (cellsize >= 8) {
		strcpy(tt, periodic_elements[i].symbol);
		if (strlen(tt) > 2) {
			tt[1] = tt[strlen(tt)-1];
			tt[2] = 0;
			tx = ex + cellsize/2 - 4;
		} else if (tt[1] != 0) {
			tx = ex + cellsize/2 - 4;
		} else {
			tx = ex + cellsize/2 - 2;
		}
		ty = ey + cellsize/2 - 4;
		periodic_tinyfont_draw_string(wid, gc, tx, ty, tt);
	}
}

void periodic_draw_table(GR_WINDOW_ID wid, GR_GC_ID gc)
{
	int cellsize = (screen_info.cols-1)/18;
	int bx = (screen_info.cols-(cellsize*18 + 1))/2;
	int by = 4;
	int i;
	for (i=0; i<118; i++) {
		periodic_draw_element(wid, gc, i, bx, by, cellsize);
	}
}

void periodic_element_info(int i)
{
	char buf[1000];
	periodic_element e;
	e = periodic_elements[i];
	snprintf(buf, 1000,
		_("Number: %d\nSymbol: %s\nName: %s\nGroup: %d %s\nPeriod: %d\nBlock: %c\nMass: %g amu\nDensity: %g mol/L\nFreezing Point: %g%cC\nBoiling Point: %g%cC"),
		(i+1), e.symbol, e.name, periodic_fix_group(e.group, e.period), periodic_group_name(i), periodic_fix_period(e.period),
		e.block, e.mass, e.density, e.fpoint, 0xB0, e.bpoint, 0xB0);
	new_stringview_window(buf, _("Periodic"));
}

/* - - - PERIODIC TABLE EVENT HANDLERS - - - */

void periodic_draw(void)
{
	int cellsize = (screen_info.cols-1)/18;
	int bx = (screen_info.cols-(cellsize*18 + 1))/2;
	int by = 4;
	
	char buf[40];
	
	int tw, th, tb;
	int ty = by + cellsize*11 + 4;
	GrGetGCTextSize(periodic_gc, " ", 1, GR_TFASCII, &tw, &th, &tb);
	ty += (th-tb);
	
	/* copy the table */
	GrCopyArea(periodic_wid, periodic_gc, 0, 0, screen_info.cols, screen_info.rows - HEADER_TOPLINE, periodic_bufwid, 0, 0, MWROP_SRCCOPY);
	
	/* draw the element */
	periodic_draw_element(periodic_wid, periodic_gc, periodic_sel, bx, by, cellsize);
	
	/* draw element text */
	snprintf(buf, 40, _("#%d - %s - %s"), (periodic_sel+1), periodic_elements[periodic_sel].symbol, periodic_elements[periodic_sel].name);
	GrSetGCForeground(periodic_gc, GR_RGB(0,0,0));
	GrText(periodic_wid, periodic_gc, bx, ty, buf, -1, GR_TFASCII);
}

void periodic_event_draw(void)
{
	pz_draw_header(_("Periodic"));
	periodic_draw();
}

int periodic_handle_event(GR_EVENT * event)
{
	if (event->type == GR_EVENT_TYPE_KEY_DOWN) {
		switch (event->keystroke.ch) {
		case 'l':
			if (periodic_sel > 0) {
				periodic_sel--;
			} else {
				periodic_sel = 117;
			}
			periodic_draw();
			break;
		case 'r':
			if (periodic_sel < 117) {
				periodic_sel++;
			} else {
				periodic_sel = 0;
			}
			periodic_draw();
			break;
		case '\n':
		case '\r':
			periodic_element_info(periodic_sel);
			break;
		case 'm':
		case 'q':
			pz_close_window(periodic_wid);
			GrDestroyWindow(periodic_bufwid);
			GrDestroyGC(periodic_gc);
			break;
		default:
			return 0;
			break;
		}
		return 1;
	} else {
		return 0;
	}
}

void new_periodic_window(void)
{
	periodic_gc = pz_get_gc(1);
	
	GrSetGCUseBackground(periodic_gc, GR_TRUE);
	GrSetGCForeground(periodic_gc, GR_RGB(0,0,0));
	GrSetGCBackground(periodic_gc, GR_RGB(255,255,255));
	
	periodic_wid = pz_new_window(0, HEADER_TOPLINE + 1,
		screen_info.cols, screen_info.rows - (HEADER_TOPLINE + 1),
		periodic_event_draw, periodic_handle_event);
	periodic_bufwid = GrNewPixmap(screen_info.cols, screen_info.rows - HEADER_TOPLINE, NULL);
	
	periodic_sel = -1;
	periodic_draw_table(periodic_bufwid, periodic_gc);
	periodic_sel = 0;
	
	GrSelectEvents(periodic_wid, GR_EVENT_MASK_EXPOSURE| GR_EVENT_MASK_KEY_UP| GR_EVENT_MASK_KEY_DOWN);
	GrMapWindow(periodic_wid);
}

