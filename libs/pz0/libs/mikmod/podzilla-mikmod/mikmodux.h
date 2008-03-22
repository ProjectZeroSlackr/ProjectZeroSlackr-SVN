#ifndef MIKMODUX_H
#define MIKMODUX_H

#define mikversion "-= MikMod *For Unix* 3.0 =-\n"

#define mikbannerhead "\n" mikversion \
" - Original code by Jean-Paul Mikkers (MikMak) <mikmak@via.nl>\n" \
" - With additions (.IT support) by Jake Stine <dracoirs@epix.net>\n" 

#define mikbannerdriver \
" - Linux driver by Chris Conn <cconn@tohs.abacom.com>\n" \
" - Sun/Solaris driver by Valtteri Vuorikoski <vuori@sci.fi>\n" \
" - SGI driver by Stephan Kanthak <kanthak@i6.informatik.rwth-aachen.de>\n" \
" - FreeBSD support by Douglas Carmichael <dcarmich@mcs.com>\n" 

#define mikbannertail \
" - curses interface, zip code by Steve McIntyre <stevem@chiark.greenend.org.uk>\n" \
" - playlist, interface/archive improvements, & current maintainence by\n" \
"   Peter Amstutz <amstpi@freenet.tlh.lf.us> (I get two lines :)\n" \
" - This program is FREEWARE - Read MIKMOD.TXT for more info \n" \
" - Binary built on " __DATE__ " at " __TIME__ "\n" \
"\nType 'mikmod -h' for command line options!\n" 

/*
#ifdef OSS
#define mikbannerdriver1 \
#ifdef ULTRA
#define mikbannerdriver mikbannerdriver1 "" \
" - ultra driver by Andy Lo A Foe <arloafoe@cs.vu.nl>\n"
#else 
#define mikbannerdriver mikbannerdriver1 
#endif 
#elif defined(SUN) 
#define mikbannerdriver \
#elif defined(SOLARIS)
#define mikbannerdriver \
" - Sun/Solaris driver by Valtteri Vuorikoski <vuori@sci.fi>\n"
#elif defined(__alpha)
#define mikbannerdriver \
" - AudioFile driver by Roine Gustafsson <e93_rog@e.kth.se>\n"

#elif defined(__hpux)
#define mikbannerdriver \
" - HP-UX driver by Lutz Vieweg <lkv@mania.robin.de>\n"
#elif defined(SGI)
#define mikbannerdriver \
#elif defined(AIX)
#define mikbannerdriver \
" - AIX driver by Lutz Vieweg <lkv@mania.robin.de>\n"
#endif 
*/

#define mikbanner mikbannerhead mikbannerdriver mikbannertail
#define pausebanner \
"'||''|.    |   '||'  '|' .|'''.| '||''''| '||''|.\n" \
" ||   ||  |||   ||    |  ||..  '  ||  .    ||   || \n" \
" ||...|' |  ||  ||    |   ''|||.  ||''|    ||    ||\n" \
" ||     .''''|. ||    | .     '|| ||       ||    ||\n" \
".||.   .|.  .||. '|..'  |'....|' .||.....|.||...|'\n"

#define extractbanner \
"'||''''|          .                         .   ||\n" \
" ||  .   ... ....||. ... ..  ....    .... .||. ... .. ...   ... . \n" \
" ||''|    '|..'  ||   ||' '''' .|| .|   '' ||   ||  ||  || || || \n" \
" ||        .|.   ||   ||    .|' || ||      ||   ||  ||  ||  |'' \n" \
".||.....|.|  ||. '|.'.||.   '|..'|' '|...' '|.'.||..||. ||.'||||. \n" \
"                                                          .|....'\n"
#define loadbanner \
"'||'                          '||   ||\n" \
" ||         ...    ....     .. ||  ...  .. ...    ... . \n" \
" ||       .|  '|. '' .||  .'  '||   ||   ||  ||  || ||  \n" \
" ||       ||   || .|' ||  |.   ||   ||   ||  ||   |''   \n" \
".||.....|  '|..|' '|..'|' '|..'||. .||. .||. || .'||||. \n" \
"                                                .|....'\n"

typedef struct {
  unsigned int location;
  char  *marker;
  char  *command;
  char  *listcmd;
  unsigned int nameoffset;
 } ARCHIVE;

extern int firstinst, dispsamp;

/**************************************************************************
****** Playlist stuff: ****************************************************
**************************************************************************/

typedef struct {
	int current;
	int numused;
	char **file;
	char **archive;
} PLAYLIST;

extern PLAYLIST playlist;

void PL_ClearList(PLAYLIST *);
void PL_InitList(PLAYLIST *);
int PL_GetCurrent(PLAYLIST *, char *, char *);
int PL_GetNext(PLAYLIST *, char *, char *);
int PL_GetPrev(PLAYLIST *, char *, char *);
int PL_GetRandom(PLAYLIST *, char *, char *);
int PL_DelCurrent(PLAYLIST *);
void PL_Add(PLAYLIST *, char *, char *);
void PL_Load(PLAYLIST *, char *);

char *MA_dearchive(char *arc, char *file);
void MA_FindFiles(PLAYLIST *pl, char *filename);

#endif
