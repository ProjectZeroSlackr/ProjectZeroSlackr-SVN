/*
 *  tstep.h
 *  tstep the stept teah 
 *
 *  Created by Stephen on Mon Feb 14 2005.
 *  Copyright (c) 2005 __MyCompanyName__. All rights reserved.
 *
 */

// KERIPO MOD
#include "../_mods.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../pz.h"

#define pNum	256		//integer multiplied by this value for precision
						//used to lessen timing problems
// KERIPO MOD
// defined in "_mods.h"
//#define SONG_PATH "/hp/ddr/"
						
#define bLEFT_ARROW 1   //bits for arrow representation
#define bDOWN_ARROW 2
#define bUP_ARROW 4
#define bRIGHT_ARROW 8
#define MP(a,b,c,d) (((2* a + b)*2+ c)*2+d)
#define MASK(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) (MP(a,b,c,d)<<12)+(MP(e,f,g,h)<<8)+(MP(i,j,k,l)<<4)+MP(m,n,o,p)
#define tsBEGINNER  0
#define tsLIGHT	  1
#define tsSTANDARD  2
#define tsHEAVY	  3
#define tsCHALLENGE 4
#define NUM_DIFFICULTIES 5
#define ZAP(a)  {if(a) free(a)};

typedef struct
{
	char *title;
	char *artist;
	char *path1;
	char *path2;
	unsigned char difficulty[NUM_DIFFICULTIES];

} songInfo;

typedef struct
{
	int numSongs;
	char *path;
	char *name;
	songInfo **songs;
} albumInfo;


/*typedef struct
{
	char* tag;
	char* data;
} bPieces;*/

typedef struct
{
	int beat;
	int lengthms;
} songFreeze;

typedef struct
{
	int beat;
	int newBPM;
} beatChange;

typedef struct
{
	int beat;
	unsigned char stepByte;
	unsigned char score;
} stepType;

typedef struct
{
	char *songTitle;
	char *artist;
	int startBPM;
	int nSongFreeze;
	int currFreezeI;
	int currBPMI;
	int gap;
	int nBeatChange;
	int nFreezeArrows;
	int nSteps;
	songFreeze *songFreezes;
	beatChange *beatChanges;
	stepType *steps;
	stepType *stepFreezes;
//	GR_BITMAP arrows[56], leftArrowBM[56],
//		rightArrowBM[56],downArrowBM[56],upArrowBM[56];
	GR_BITMAP arrows[56], *leftArrowBM,
		*rightArrowBM,*downArrowBM,*upArrowBM;
	int currBeat;
	int currBPM;
	long int msCount,msTarget;
	int sfreeze;
	albumInfo ** albums;
	int numAlbums;
} tstep_global;

int getTag(char* data, char **tag, char ** tdata, const char d);
int getTimeDiff();
int getTagSafe(char* data, char* tag, char* tdata);
inline unsigned char getStepByte(char c);
char * cutLSpaces(const char * s);
int countCommas(const char *s);
void loadArrow(GR_BITMAP* a, int orientation);
void newSong(void);
int importSteps(const char *s);
int handleTag(char *tagd, char *data);
void registerHit(char unsigned step);
void tstep_init(void);
void findSongs(void);
void loadSong(char* filename, songInfo* song);
void writeAlbumList(char *filename);
