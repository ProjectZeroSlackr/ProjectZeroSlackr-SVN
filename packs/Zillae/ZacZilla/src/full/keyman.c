/*
keyman
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "pz.h"
#include "ipod.h"

#define TATEHABA 73
#define MAXSTAGE 10
#define MAXTRA 10

#define TRUE 1
#define FALSE 0

#define TWIDTH 16
#define THEIGHT 16

#define MENU 0
#define INGAME 1
#define PAUSE 2
#define END 3
#define NODATA 4

#define SORA 1
#define ZIM 0

static GR_WINDOW_ID	keyman_wid;
static GR_WINDOW_ID	yomipict_pixmap;
static GR_WINDOW_ID	allmap_pixmap;
static GR_WINDOW_ID	mainmenu_pixmap;
static GR_WINDOW_ID	shu_pixmap;
static GR_WINDOW_ID	dialogall_pixmap;
static GR_WINDOW_ID	dialog_pixmap;

static GR_GC_ID		keyman_gc;
static GR_TIMER_ID      keyman_timer;
static GR_IMAGE_ID kmimage_id;
static GR_SCREEN_INFO screen_info;


static unsigned short keymap1_black[16]={0x0000,0x0780,0x1860,0x2010,0x2010,0x7ffe,0x4008,0x4048,0x4048,0x2110,0x20d0,0x1860,0x1780,0x20c0,0x4040,0x7fc0};
static unsigned short keymap2_black[16]={0x0000,0x01e0,0x0618,0x0804,0x0804,0x7ffe,0x1002,0x1202,0x1202,0x0886,0x0b04,0x0618,0x01e8,0x0304,0x0202,0x03fe};
static unsigned short keymap3_black[16]={0x0000,0x07e0,0x0810,0x1008,0x2814,0x27e4,0x2004,0x2004,0x2004,0x1008,0x0810,0x07e0,0x0810,0x1008,0x1008,0x1ff8};

static unsigned short keymap1_gray[16] = {0x0000,0x0000,0x0180,0x07e0,0x0fe0,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0180,0x0000};
static unsigned short keymap2_gray[16] = {0x0000,0x0000,0x0180,0x07e0,0x07f0,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0180,0x0000};
static unsigned short keymap3_gray[16] = {0x0000,0x0000,0x07c0,0x0fe0,0x07c0,0x0180,0x0000,0x1000,0x1000,0x0000,0x0800,0x0400,0x0000,0x1008,0x03c0,0x0000};

static unsigned short keymap1_ltgray[16] = {0x0000,0x0000,0x0600,0x1800,0x0000,0x0000,0x1ff0,0x1fb0,0x1fb0,0x0ee0,0x0720,0x0180,0x0000,0x0f00,0x1e00,0x0000};
static unsigned short keymap2_ltgray[16] ={0x0000,0x0000,0x0060,0x0018,0x0008,0x0000,0x0ff8,0x0df8,0x0df8,0x0770,0x04e0,0x0180,0x0000,0x00e0,0x0078,0x0000};
static unsigned short keymap3_ltgray[16] ={0x0000,0x0000,0x0020,0x0010,0x1020,0x1810,0x1ff0,0x0ff0,0x0ff0,0x07e0,0x03c0,0x0000,0x03c0,0x0420,0x07e0,0x0000};

static unsigned short keymap1_white[16] = {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x2000,0x2000,0x1000,0x1800,0x0600,0x0800,0x1000,0x2000,0x0000};
static unsigned short keymap2_white[16] = {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0004,0x0004,0x0004,0x0008,0x0018,0x0060,0x0010,0x0008,0x0004,0x0000};
static unsigned short keymap3_white[16] = {0x0000,0x0000,0x0000,0x0000,0x0008,0x0008,0x0008,0x0008,0x0008,0x0010,0x0020,0x0000,0x0420,0x0810,0x0810,0x0000};

static int GAMEN;
static int scr;
static int ketumatu;
static int training;
static int tranum;
static char hensuu[20];

static struct afterblock{
	int x;
	int y;
	char mozi;

}wafter;

static struct deady{
	int deadcount;
	float sp;
	float g;
	int nowt;
	int kaiten;
}wdeady;

static struct menukey{
	int BA;
	int KEY;
	int STAGE;
	int ANI;
}wmenukey;
static struct menukey wmenukey={0,1,1,0};

static int dialogkey;

static struct lastkey{
	int AR;
	int AL;
	int AU;
	int AD;
	int REE;
	int PAU;
}wlastkey;
static struct lastkey wlastkey={0,0,0,0,0};

static struct setkey{
	char KLEFT;
	char KRIGHT;
	char KUP;
	char KDOWN;
	char KJUMP;
	char KPAUSE;
}wsetkey;
static struct setkey wsetkey={0,0,0,0,0,0};

static struct wnowconditon{
	int LR;
	int XLOC;
	int YLOC;
	double XKANSEI;
	double sXKANSEI;
	double YKANSEI;
	int MUKI;
	int JIMEN;
	int MAPSCROLL;
	int NUPDW;
	int HASIGO;
	int KAGI;
	int shupict;
}wnow;
static struct wnowconditon wnow={0,0,0,0,0,0,0,0,0,0,0};


static struct yomidata {
	int sttop;
	int stleft;
	int stscroll;
	int updw;
	char mapd[10][TATEHABA];
	char tokud[500];
}wyomidata;

static char pathimage[]="/opt/Zillae/ZacZilla/Data/KMData/pict.gif";
static char pathmap[]="/opt/Zillae/ZacZilla/Data/KMData/mapdata/stage1.txt";

static void kmbitmap(void){
	
	GrSetGCUseBackground(keyman_gc, GR_FALSE); 
	GrSetGCForeground(keyman_gc, GR_RGB(0,0,0)); 
	switch(wnow.shupict){
		case 0:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap1_black);break;
		case 1:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap2_black);break;
		case 2:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap3_black);break;
	}
	GrSetGCForeground(keyman_gc, GR_RGB(80,80,80)); 
	switch(wnow.shupict){
		case 0:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap1_gray);break;
		case 1:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap2_gray);break;
		case 2:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap3_gray);break;
	} 
	GrSetGCForeground(keyman_gc, GR_RGB(160,160,160)); 
	switch(wnow.shupict){
		case 0:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap1_ltgray);break;
		case 1:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap2_ltgray);break;
		case 2:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap3_ltgray);break;
	}
	GrSetGCForeground(keyman_gc, GR_RGB(255,255,255)); 
	switch(wnow.shupict){
		case 0:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap1_white);break;
		case 1:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap2_white);break;
		case 2:GrBitmap(mainmenu_pixmap, keyman_gc, wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)), 16, 16, keymap3_white);break;
	}

}

static void makedialog(void){
	GrCopyArea(dialog_pixmap,keyman_gc,0,0,32,32,yomipict_pixmap,0,32,0);
	GrCopyArea(dialog_pixmap,keyman_gc,32,0,16,32,yomipict_pixmap,16,32,0);
	GrCopyArea(dialog_pixmap,keyman_gc,48,0,16,32,yomipict_pixmap,16,32,0);
	GrCopyArea(dialog_pixmap,keyman_gc,64,0,32,32,yomipict_pixmap,16,32,0);
	GrCopyArea(dialog_pixmap,keyman_gc,64,32,32,16,yomipict_pixmap,16,48,0);
	GrCopyArea(dialog_pixmap,keyman_gc,64,48,32,32,yomipict_pixmap,16,48,0);
	GrCopyArea(dialog_pixmap,keyman_gc,32,48,16,32,yomipict_pixmap,16,48,0);
	GrCopyArea(dialog_pixmap,keyman_gc,48,48,16,32,yomipict_pixmap,16,48,0);
	GrCopyArea(dialog_pixmap,keyman_gc,0,48,32,32,yomipict_pixmap,0,48,0);
	GrCopyArea(dialog_pixmap,keyman_gc,0,32,32,16,yomipict_pixmap,0,48,0);
	
	GrCopyArea(dialog_pixmap,keyman_gc,32,32,32,16,yomipict_pixmap,8,48,0);
	
	GrCopyArea(dialog_pixmap,keyman_gc,16,26,64,16,yomipict_pixmap,48,48,0);
	GrCopyArea(dialog_pixmap,keyman_gc,16,48,64,16,yomipict_pixmap,48,32,0);
	
	GrCopyArea(dialog_pixmap,keyman_gc,16,8,64,16,yomipict_pixmap,112,32,0);
}

static void makemenu(void){
	char itiziteki[10];
	GrSetGCForeground(keyman_gc, GR_RGB(255,255,255));
	GrFillRect(mainmenu_pixmap,keyman_gc,0,0,160,128);
	
	GrCopyArea(mainmenu_pixmap,keyman_gc,40,16,80,16,yomipict_pixmap,48,64,0);
	
	GrCopyArea(mainmenu_pixmap,keyman_gc,26,46,32,48,yomipict_pixmap,0,32,0);
	GrCopyArea(mainmenu_pixmap,keyman_gc,58,46,32,48,yomipict_pixmap,8,32,0);
	GrCopyArea(mainmenu_pixmap,keyman_gc,90,46,40,48,yomipict_pixmap,8,32,0);
	
	GrCopyArea(mainmenu_pixmap,keyman_gc,26,78,32,32,yomipict_pixmap,0,48,0);
	GrCopyArea(mainmenu_pixmap,keyman_gc,58,78,32,32,yomipict_pixmap,8,48,0);
	GrCopyArea(mainmenu_pixmap,keyman_gc,90,78,40,32,yomipict_pixmap,8,48,0);
	
	//GrCopyArea(mainmenu_pixmap,keyman_gc,120,80,32,32,yomipict_pixmap,0,80,MWROP_BLACKNESS);
	
	GrSetGCForeground(keyman_gc, GR_RGB(0,0,0));
	GrText(mainmenu_pixmap,keyman_gc,81,32,"for iPod",-1,GR_TFASCII|GR_TFTOP);
	GrSetGCForeground(keyman_gc, GR_RGB(0,0,0));
	GrText(mainmenu_pixmap,keyman_gc,31,50,"Key ",-1,GR_TFASCII|GR_TFTOP);
	GrText(mainmenu_pixmap,keyman_gc,31,60,"Stage",-1,GR_TFASCII|GR_TFTOP);
	GrText(mainmenu_pixmap,keyman_gc,31,70,"Start",-1,GR_TFASCII|GR_TFTOP);
	GrText(mainmenu_pixmap,keyman_gc,31,80,"Training",-1,GR_TFASCII|GR_TFTOP);
	GrText(mainmenu_pixmap,keyman_gc,31,90,"Quit",-1,GR_TFASCII|GR_TFTOP);
	
	GrSetGCForeground(keyman_gc, GR_RGB(0,0,0));
	if (wmenukey.KEY==1){strcpy(itiziteki,"for G3 type");}
	else {strcpy(itiziteki,"for G4 type");}
	GrText(mainmenu_pixmap,keyman_gc,71,50,itiziteki,-1,GR_TFASCII|GR_TFTOP);
	sprintf(itiziteki,"%d",wmenukey.STAGE);
	GrText(mainmenu_pixmap,keyman_gc,101,60,itiziteki,-1,GR_TFASCII|GR_TFTOP);
			
			//GrText(mainmenu_pixmap,keyman_gc,-150,0,pathmap,-1,GR_TFASCII|GR_TFTOP);
			
	if (wmenukey.ANI<3){wmenukey.ANI++;}
	else {wmenukey.ANI=0;}
	GrCopyArea(mainmenu_pixmap,keyman_gc,85,73,32,32,yomipict_pixmap,(wmenukey.ANI*32),80,0);
			
	GrCopyArea(mainmenu_pixmap,keyman_gc,12,(48+(wmenukey.BA*10)),14,16,yomipict_pixmap,112,48,0);
	
	
}


static void MINIPHOTO(int a,int b,int c,int d){
	GrCopyArea(keyman_wid,keyman_gc,a+((screen_info.cols-160)/2),b+((screen_info.rows-128)/2),c,d,mainmenu_pixmap,0,0,0);
}


static int INIT(int bx,int by,char zi){
	int scr= (wnow.MAPSCROLL/16);
	int a=(((by-5)/16)+1+scr);
	int a1=((bx-5)/16)+1;
	int b=(((by+5)/16)+1+scr);
	int b1=((bx+5)/16)+1;
if ( wyomidata.mapd[a1-1][a-1] == zi || wyomidata.mapd[b1-1][a-1] == zi || wyomidata.mapd[a1-1][b-1] == zi || wyomidata.mapd[b1-1][b-1] == zi ){ return TRUE; } else {return FALSE;}
}


static int tuka(char x){
switch (x){
	case 'A':return TRUE;break;
	case 'B':return FALSE;break;
	case 'C':return FALSE;break;
	case 'D':return TRUE;break;
	case 'E':return TRUE;break;
	case 'F':return TRUE;break;
	case 'G':return FALSE;break;
	case 'H':return TRUE;break;
	case 'I':return FALSE;break;
	case 'J':return TRUE;break;
	case 'K':return FALSE;break;
	case 'L':return TRUE;break;
	case 'M':return FALSE;break;
	case 'N':return TRUE;break;
	case 'O':return FALSE;break;
	case 'P':return TRUE;break;
	case 'Q':return FALSE;break;
	case 'R':return FALSE;break;
	case 'S':return FALSE;break;
	case 'T':return FALSE;break;
	case 'U':return FALSE;break;
	case 'V':return FALSE;break;
	case 'W':return FALSE;break;
	case 'X':return FALSE;break;
	default :return FALSE;break;
	}
}

static int tuka2(char x){
	switch (x){
	case 'G':return FALSE;break;
	default:return TRUE;break;
	}
}

void usleep(long usec)
{
    struct timeval tv;
    tv.tv_sec =  usec / 1000000;
    tv.tv_usec = usec % 1000000;
    select(0, NULL, NULL, NULL, &tv);
}

static char kmsearch(int bx,int by){
	int scr= (wnow.MAPSCROLL/16);
	int a=(bx/16)+1;
	int b=((by/16)+1)+scr;
	return wyomidata.mapd[a-1][b-1];
}

static int pictx (char mozi){
	switch(mozi){
			case 'A':return 0;break;
			case 'B':return 16;break;
			case 'C':return 32;break;
			case 'D':return 48;break;
			case 'E':return 64;break;
			case 'F':return 80;break;
			case 'G':return 96;break;
			case 'H':return 112;break;
			case 'I':return 128;break;
			case 'J':return 144;break;
			case 'K':return 160;break;
			case 'L':return 176;break;
			case 'M':return 192;break;
			case 'N':return 208;break;
			case 'O':return 224;break;
			case 'P':return 240;break;
			case 'Q':return 256;break;
			case 'R':return 272;break;
			case 'S':return 288;break;
			case 'T':return 304;break;
			case 'U':return 320;break;
			case 'V':return 336;break;
			case 'W':return 352;break;
			case 'X':return 368;break;
			default :return 0;break;
			}
	
	
} 

static void tuikapaint(int x,int y,char mozi){
	int kari=0;
	wyomidata.mapd[x-1][y-1]=mozi;
	kari=pictx(mozi);
	GrCopyArea(allmap_pixmap,keyman_gc,((x-1)*16),((y-1)*16),16,16,yomipict_pixmap,kari,0,0);
	GrCopyArea(mainmenu_pixmap,keyman_gc,0,0,160,128,allmap_pixmap,0,wnow.MAPSCROLL,0);
	kmbitmap();
	//GrCopyArea(mainmenu_pixmap,keyman_gc,wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)),160,128,shu_pixmap,0,0,0);
	
	MINIPHOTO(0,0,160,128);
}

static void poppaint(int x,int y,char mozi){
	int kari=0;int Bkari=16;
	wyomidata.mapd[x-1][y-1]=mozi;
	if (mozi=='B'){kari=pictx('F');}
	else{kari=pictx(mozi);}
	
	GrCopyArea(allmap_pixmap,keyman_gc,((x-1)*16),((y-1)*16),16,16,yomipict_pixmap,Bkari,0,0);
	GrCopyArea(allmap_pixmap,keyman_gc,((x-1)*16),((y-1)*16)-4,16,16,yomipict_pixmap,kari,0,0);
	GrCopyArea(mainmenu_pixmap,keyman_gc,0,0,160,128,allmap_pixmap,0,wnow.MAPSCROLL,0);
	kmbitmap();
	//GrCopyArea(mainmenu_pixmap,keyman_gc,wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)),160,128,shu_pixmap,0,0,0);
	
	MINIPHOTO(0,0,160,128);
	wafter.mozi=wyomidata.mapd[x-1][y-1];
	wafter.x=x;
	wafter.y=y;
	
	
}


static void afterzure(void){
	int kari;
	wafter.mozi=wyomidata.mapd[wafter.x-1][wafter.y-1];
	kari=pictx(wafter.mozi);
	GrCopyArea(allmap_pixmap,keyman_gc,((wafter.x-1)*16),((wafter.y-1)*16),16,16,yomipict_pixmap,kari,0,0);
	wafter.mozi=wyomidata.mapd[wafter.x-1][wafter.y-1-1];
	kari=pictx(wafter.mozi);
	GrCopyArea(allmap_pixmap,keyman_gc,((wafter.x-1)*16),((wafter.y-1-1)*16),16,16,yomipict_pixmap,kari,0,0);
}

static void block (int x,int y,char mozi){
	int alllength,i,itar,p,flag;
	int xza,yza;
	
	char matome[10];
	char target[10];
	switch(mozi){
		case 'L':
			poppaint(x,y,'M');
			break;
		case 'N':
			poppaint(x,y,'O');
			break;
		case 'P':
			poppaint(x,y,'Q');
			break;
	}
	
	
	alllength=strlen(wyomidata.tokud);
	
	sprintf(target,"%c %d,%d",mozi,x,y);
	
	/*searchpoint*/
	for (p=0;p < alllength;p++){
		if (wyomidata.tokud[p]==mozi){
			p=p+2;
			itar=0;
			while (wyomidata.tokud[p]!=' '){
				
				if (wyomidata.tokud[p]==target[itar+2]){flag=1;}
				////else {flag=0;break;}
				//else {break;}
				else {flag=0;break;}
				p++;itar++;
			}
		}
		if (flag==1){break;}
		
	}
	//sprintf(hensuu,"%c%c%c%c%c%c%c",wyomidata.tokud[p-4],wyomidata.tokud[p-3],wyomidata.tokud[p-2],wyomidata.tokud[p-1],wyomidata.tokud[p],wyomidata.tokud[p+1],wyomidata.tokud[p+2]);	
	
	/*rewrite*/
	if (flag==1){
	i=0;
	
	while(wyomidata.tokud[p] != '\n'){
		if (wyomidata.tokud[p]==' ' && wyomidata.tokud[p+1] == '\n'){break;}
		
		else if (wyomidata.tokud[p]==' '){
			while(wyomidata.tokud[p] !=','){
				matome[i]=wyomidata.tokud[p];
				matome[i+1]=0;
				p++;
				i++;
			}
			
			xza=atoi(matome);
			i=0;
		}
		
		
		
		if (wyomidata.tokud[p] ==','){
			p++;
			while(wyomidata.tokud[p] != ' '){
				matome[i]=wyomidata.tokud[p];
				matome[i+1]=0;
				p++;
				i++;
			}
			yza=atoi(matome);
			switch (mozi){
			case 'L':tuikapaint(xza,yza,'A');break;
			case 'N':tuikapaint(xza,yza,'B');break;
			case 'P':tuikapaint(xza,yza,'G');break;
			}
			usleep(50000);
			//p--;
		}
		
		
		
		i=0;
		//p++;
		
	}
	}
	
}

static void Punch (int bx, int by){
char mozi=kmsearch(bx,(by-12));
int x=(bx/16)+1;
int y=((by/16)+1)+scr;
scr= (wnow.MAPSCROLL/16);
	switch(mozi){
	case 'D':wnow.YLOC=wnow.YLOC-5;break;
	case 'E':ipod_beep();poppaint(x,y-1,'F');afterzure();break;
	case 'F':ipod_beep();poppaint(x,y-1,'B');afterzure();break;
	case 'H':ipod_beep();poppaint(x,y-1,'I');wnow.NUPDW=-1;afterzure();break;
	case 'J':ipod_beep();poppaint(x,y-1,'K');wnow.NUPDW=1;afterzure();break;
	case 'U':ipod_beep();poppaint(x,y-1,'A');afterzure();break;
	case 'L':ipod_beep();block(x,y-1,'L');afterzure();break;
	case 'N':ipod_beep();block(x,y-1,'N');afterzure();break;
	case 'P':ipod_beep();block(x,y-1,'P');afterzure();break;
	}
}



static void mappaint(){
int x=0;
int i=0;
int p=0;
	GrSetGCForeground(keyman_gc, GR_RGB(0,0,0));
	kmimage_id=GrLoadImageFromFile(pathimage,0);
	yomipict_pixmap=GrNewPixmap(384,128,NULL);
	allmap_pixmap=GrNewPixmap(160,(TATEHABA*16),NULL);
	GrDrawImageToFit(yomipict_pixmap,keyman_gc,0,0,384,128,kmimage_id);
	
	for (i=0;i<=(TATEHABA-1);i++){
	
		for (p=0;p<=9;p++){
			x=pictx(wyomidata.mapd[p][i]);
			GrCopyArea(allmap_pixmap,keyman_gc,(p*16),(i*16),16,16,yomipict_pixmap,x,0,0);
			

		}
	}
}

static void textread(void){
	FILE *fp;
	int length;
	int c;
	char cs[10]="";
	int line=0;
	int mozi=0;
	strcpy(wyomidata.tokud,"");
	if ((fp=fopen(pathmap,"r")) == NULL){GAMEN=NODATA;}
	
	while ((c=fgetc(fp)) !=EOF)
	{
		if (line<TATEHABA){
			if (c=='\n'){line++;mozi=0;}
			else {wyomidata.mapd[mozi][line]=c;mozi++;}
			}
		else if (line<=TATEHABA+4) {
			if (c=='\n'){line++;mozi=0;
				if (line==TATEHABA+1){wyomidata.stleft=atoi(cs);strcpy(cs,"");}
				if (line==TATEHABA+2){wyomidata.sttop=atoi(cs);strcpy(cs,"");}
				if (line==TATEHABA+3){wyomidata.stscroll=atoi(cs);strcpy(cs,"");}
				if (line==TATEHABA+4){wyomidata.updw=atoi(cs);strcpy(cs,"");}
			}
			else {  length=strlen(cs);
					cs[length]=c;
					cs[length+1]=0;
					mozi++;
					}
		}
		else {
			length=strlen(wyomidata.tokud);
			wyomidata.tokud[length]=c;
			wyomidata.tokud[length+1]=0;
			mozi++;
					
			
		}
	}
	
	
	fclose(fp);
}

//static void draw_menu()
//{		 
		
//}



static void opencard(void){
	wnow.XLOC=wyomidata.stleft;
	wnow.YLOC=wyomidata.sttop;
	wnow.MAPSCROLL=wyomidata.stscroll;
	scr= (wnow.MAPSCROLL/16);
	wnow.NUPDW=wyomidata.updw;
	wnow.XKANSEI=0;
	wnow.YKANSEI=0;
	wnow.MUKI=1;
	wnow.JIMEN=ZIM;
	wnow.KAGI=FALSE;
	wnow.HASIGO=FALSE;
	wnow.LR=0;
	wnow.sXKANSEI=0;
	
}

static void startgame(void){
// KERIPO MOD
	//if (training==0){sprintf(pathmap,"/etc/KMData/mapdata/stage%d.txt",wmenukey.STAGE);}
	if (training==0){sprintf(pathmap,"/opt/Zillae/ZacZilla/Data/KMData/mapdata/stage%d.txt",wmenukey.STAGE);}
	//else if (training==1){tranum++;sprintf(pathmap,"/etc/KMData/tra/tra%d.txt",tranum);}
	else if (training==1){tranum++;sprintf(pathmap,"/opt/Zillae/ZacZilla/Data/KMData/tra/tra%d.txt",tranum);}
	switch (wmenukey.KEY){
		case 1: wsetkey.KLEFT='l';
			wsetkey.KRIGHT='r';
			wsetkey.KUP='w';
			wsetkey.KDOWN='f';
			wsetkey.KJUMP='\r';
			wsetkey.KPAUSE='m';
			break;
		
		case 2: wsetkey.KLEFT='l';
			wsetkey.KRIGHT='r';
			wsetkey.KUP='f';
			wsetkey.KDOWN='d';
			wsetkey.KJUMP='\r';
			wsetkey.KPAUSE='m';
			break;	
		}
	textread();
	mappaint();
	opencard();
}

static void hensu(void){
	GrSetGCForeground(keyman_gc, GR_RGB(0,0,0));
	GrText(mainmenu_pixmap,keyman_gc,50,50,hensuu,-1,GR_TFASCII|GR_TFTOP);
}

static void ATARI2(void){

int STAGED=((wnow.YLOC+8-7)/16+1)+scr;
int STAGEE=((wnow.YLOC+8+7.99)/16+1)+scr;

//int STAGEE=((wnow.YLOC+8)/16+1)+scr;

int a=((wnow.XLOC+8-6)/16+1);
int b=((wnow.XLOC+8+5.99)/16+1);

int MIDLEFT=0;
int MIDRIGHT=0;

int TOPLEFT=0;
int TOPRIGHT=0;
int BOTTOMLEFT=0;
int BOTTOMRIGHT=0;
double XMERI=0;
double YMERI=0;

int he1=((wnow.XLOC+8+8)/16);
int he2=((wnow.YLOC+8+8)/16);
int he3=((wnow.XLOC+8-6)/16+1);

int he42=((wnow.YLOC+8-7)/16+1);

int he5=((wnow.XLOC+8+6-16)/16+1);
int he6=((wnow.XLOC+8)/16+1);
int he7=(((wnow.YLOC+8)/16+1)+scr);
int he8=((wnow.YLOC+8+8)/16+1);
//int he9=((wnow.XLOC+8+5.99)/16+1);
int he10=((wnow.YLOC+8+8-16)/16+1);

int he32=((wnow.XLOC+8-5)/16+1);
int he92=((wnow.XLOC+8+4.99)/16+1);

	if (wnow.HASIGO == 0){
		TOPLEFT=tuka(wyomidata.mapd[a-1][STAGED-1]);
		TOPRIGHT=tuka(wyomidata.mapd[b-1][STAGED-1]);
		BOTTOMLEFT=tuka(wyomidata.mapd[a-1][STAGEE-1]);
		BOTTOMRIGHT=tuka(wyomidata.mapd[b-1][STAGEE-1]);
	}
	else {
		TOPLEFT=tuka2(wyomidata.mapd[a-1][STAGED-1]);
		TOPRIGHT=tuka2(wyomidata.mapd[b-1][STAGED-1]);
		BOTTOMLEFT=tuka2(wyomidata.mapd[a-1][STAGEE-1]);
		BOTTOMRIGHT=tuka2(wyomidata.mapd[b-1][STAGEE-1]);
	
	}
	
	
	
	if (wnow.HASIGO == 0 && wyomidata.mapd[a-1][STAGED-1]=='U'){MIDLEFT=1;}
	if (wnow.HASIGO == 0 && wyomidata.mapd[b-1][STAGED-1]=='U'){MIDRIGHT=1;}
	
	if (TOPLEFT+TOPRIGHT+MIDLEFT+MIDRIGHT==1){
		if (MIDLEFT==1){
			XMERI=(wnow.XLOC+8-6-(16*he1))/(wnow.XKANSEI+0.0001);
			YMERI=(wnow.YLOC+8-7-(16*he2))/(wnow.YKANSEI+0.0001);
			if (wnow.YKANSEI<0){
				if ((XMERI*XMERI < YMERI*YMERI && XMERI >= 0)||(YMERI<0)){
				}
				else {
					if (kmsearch(wnow.XLOC-1,(wnow.YLOC+6-15))=='U'){
					Punch(wnow.XLOC-1,wnow.YLOC+6);
					wnow.YLOC=16*he42+7-8;
					wnow.YKANSEI=0;
					}
				}
			}	
		}
		else if(MIDRIGHT==1){
			XMERI=(wnow.XLOC+8+6-(16*he1))/(wnow.XKANSEI+0.0001);
			YMERI=(wnow.YLOC+8-7-(16*he2))/(wnow.YKANSEI+0.0001);
			if (wnow.YKANSEI<0){
				if ((XMERI*XMERI < YMERI*YMERI && XMERI >= 0)||(YMERI<0)){
				}
				else {
					if (kmsearch(wnow.XLOC+17,(wnow.YLOC+6-15))=='U'){
					Punch(wnow.XLOC+17,wnow.YLOC+6);
					wnow.YLOC=16*he42+7-8;
					wnow.YKANSEI=0;
					}
				}
			}	
		}
	}
	else {
		if (TOPLEFT+TOPRIGHT+MIDLEFT+MIDRIGHT==2){
			if (wnow.YKANSEI<0){
				if (kmsearch(wnow.XLOC+8,wnow.YLOC+8-16) != 'D'){
					if (kmsearch(wnow.XLOC+8,(wnow.YLOC+8-15))=='U'){
					Punch(wnow.XLOC+8,wnow.YLOC+8);
					wnow.YLOC=16*he42+7-8;
					wnow.YKANSEI=0;
					}
				}
				else{
					wnow.YLOC=wnow.YLOC-5-8;
				}
			}
		}
	}
	
	if (TOPLEFT+BOTTOMLEFT+TOPRIGHT+BOTTOMRIGHT==1)	{
		if (TOPLEFT==1){
			XMERI=(wnow.XLOC+8-6-(16*he1))/(wnow.XKANSEI+0.0001);
			YMERI=(wnow.YLOC+8-7-(16*he2))/(wnow.YKANSEI+0.0001);
			if ((XMERI*XMERI < YMERI*YMERI && XMERI >= 0)||(YMERI<0)){
				wnow.XLOC=16*he3+6-8;
				wnow.XKANSEI=0;
			}
			else {
				Punch(wnow.XLOC-1,wnow.YLOC+6);
				wnow.YLOC=16*he42+7-8;
				wnow.YKANSEI=0;
			}
		}
		else if(TOPRIGHT==1){
			XMERI=(wnow.XLOC+8+6-(16*he1))/(wnow.XKANSEI+0.0001);
			YMERI=(wnow.YLOC+8-7-(16*he2))/(wnow.YKANSEI+0.0001);
			if ((XMERI*XMERI < YMERI*YMERI && XMERI >= 0)||(YMERI<0)){
				wnow.XLOC=16*he5-6-8;
				wnow.XKANSEI=0;
			}
			else {
				Punch(wnow.XLOC+17,wnow.YLOC+6);
				wnow.YLOC=16*he42+7-8;
				wnow.YKANSEI=0;
			}
		}
		else if(BOTTOMLEFT==1){
			XMERI=(wnow.XLOC+8-6-(16*he1))/(wnow.XKANSEI+0.0001);
			YMERI=(wnow.YLOC+8+8-(16*he2))/(wnow.YKANSEI+0.0001);
			if ((XMERI*XMERI < YMERI*YMERI && XMERI >= 0)||(YMERI<0)){
				wnow.XLOC=16*he3+6-8;
				wnow.XKANSEI=0;
			}
		}
		else if(BOTTOMRIGHT==1){
			XMERI=(wnow.XLOC+8+6-(16*he1))/(wnow.XKANSEI+0.0001);
			YMERI=(wnow.YLOC+8+8-(16*he2))/(wnow.YKANSEI+0.0001);
			if ((XMERI*XMERI < YMERI*YMERI && XMERI >= 0)||(YMERI<0)){
				wnow.XLOC=16*he5-6-8;
				wnow.XKANSEI=0;
			}
		}
	}
	else if (TOPLEFT+BOTTOMLEFT+TOPRIGHT+BOTTOMRIGHT==0 || TOPLEFT+BOTTOMLEFT+TOPRIGHT+BOTTOMRIGHT==4){}
	else{
		if (TOPLEFT+BOTTOMLEFT==2){
			wnow.XLOC=16*he3+6-8;
			wnow.XKANSEI=0;
		}
		if (TOPRIGHT+BOTTOMRIGHT==2){
			wnow.XLOC=16*he5-6-8;
			wnow.XKANSEI=0;
		}
		if (TOPLEFT+TOPRIGHT==2){
			if (kmsearch(wnow.XLOC+8,wnow.YLOC+8-16) != 'D'){
				Punch(wnow.XLOC+8,wnow.YLOC+8);
				wnow.YLOC=16*he42+7-8;
				wnow.YKANSEI=0;
			}
			else{
				wnow.YLOC=wnow.YLOC-5-8;
			}
		}
		if (TOPLEFT+BOTTOMRIGHT==TOPLEFT+BOTTOMLEFT+TOPRIGHT+BOTTOMRIGHT || TOPRIGHT+BOTTOMLEFT==TOPLEFT+BOTTOMLEFT+TOPRIGHT+BOTTOMRIGHT){
			if (wnow.XKANSEI>0){wnow.XLOC=(16*he5)-6-8;}
			else if (wnow.XKANSEI<0){wnow.XLOC=(16*he3)+6-8;}
			wnow.XKANSEI=0;
			if (wnow.YKANSEI<0){wnow.YLOC=(16*he42)+7-8;}
			wnow.YKANSEI=0;
		}
	}	
	
	if (INIT(wnow.XLOC+8,wnow.YLOC+8,'G')==TRUE){wnow.JIMEN=TRUE;}
	else if (kmsearch(wnow.XLOC+8,wnow.YLOC+8)=='R'){
		tuikapaint(he6,he7,'S');
		wnow.KAGI=TRUE;
	}
	else if (INIT(wnow.XLOC+8,wnow.YLOC+8,'T')==TRUE){
		if (wnow.KAGI==TRUE){
			ketumatu=1;
			GAMEN=END;
		}
	}
	else if (INIT(wnow.XLOC+8,wnow.YLOC+8,'C')==TRUE){
		ketumatu=1;
		GAMEN=END;
	}
	else if (INIT(wnow.XLOC+8,wnow.YLOC+8,'W')==TRUE){
		wdeady.sp=-16;
		wdeady.g=1.6;
		wdeady.nowt=wnow.YLOC;
		GAMEN=END;
		wdeady.kaiten=0;
		ketumatu=0;
		wdeady.deadcount=1;
	}
	else if (INIT(wnow.XLOC+8,wnow.YLOC+8,'X')==TRUE){
		wdeady.sp=12;
		wdeady.g=0;
		wdeady.nowt=wnow.YLOC;
		GAMEN=END;
		wdeady.kaiten=0;
		ketumatu=0;
		wdeady.deadcount=1;
	}
	
	he32=((wnow.XLOC+8-5)/16+1);
	he92=((wnow.XLOC+8+4.99)/16+1);
	he8=((wnow.YLOC+8+8)/16+1);
			
	if (wnow.HASIGO==0){
		if (TOPLEFT==1){BOTTOMLEFT=0;}
		else if (BOTTOMLEFT==1){BOTTOMLEFT=tuka(wyomidata.mapd[he32-1][he8+scr-1]);}
		if (TOPRIGHT==1){BOTTOMRIGHT=0;}
		else if (BOTTOMRIGHT==1){BOTTOMRIGHT=tuka(wyomidata.mapd[he92-1][he8+scr-1]);}
	}
	else if (wnow.HASIGO==1){
		if (TOPLEFT==1){BOTTOMLEFT=0;}
		else if (BOTTOMLEFT==1){BOTTOMLEFT=tuka2(wyomidata.mapd[he32-1][he8+scr-1]);}
		if (TOPRIGHT==1){BOTTOMRIGHT=0;}
		else if (BOTTOMRIGHT==1){BOTTOMRIGHT=tuka2(wyomidata.mapd[he92-1][he8+scr-1]);}
	}
	
	if (BOTTOMLEFT+BOTTOMRIGHT>=1 && BOTTOMLEFT+BOTTOMRIGHT<=4){
		wnow.JIMEN=ZIM;
		wnow.YLOC=16*he10-8-8;
		wnow.YKANSEI=0;
	}
	else {wnow.JIMEN=SORA;}

	if (wnow.NUPDW!=1 && (wnow.YLOC-(wnow.MAPSCROLL % 16))<0){
		wnow.YLOC=(wnow.MAPSCROLL % 16);
		wnow.YKANSEI=0;
	
	}
	
	
}




static int keyman_handle_event(GR_EVENT * event)
{
	int ret = 0;
    switch( event->type )
    {
    case( GR_EVENT_TYPE_TIMER ):
	
	switch(GAMEN){
		
		
		case MENU:
			makemenu();
			
			MINIPHOTO(0,0,160,128);
			wlastkey.AR=0;wlastkey.AL=0;wlastkey.AU=0;wlastkey.AD=0;wlastkey.REE=0;wlastkey.PAU=0;
			break;
		case INGAME:
			
		
		
		
			if (wlastkey.AL==1){wnow.LR=-1;wnow.MUKI=-1;}
			else if(wlastkey.AR==1){wnow.LR=1;wnow.MUKI=1;}
			else {wnow.LR=0;}
			
			
			if(wlastkey.REE==1){
					if(wnow.JIMEN==ZIM){
					wnow.YKANSEI=-9;
					wnow.JIMEN=SORA;
					}
					else if (wnow.HASIGO==1){
					wnow.YKANSEI=-9;
					wnow.JIMEN=SORA;
					wnow.HASIGO=0;
					
					}
			}
			
			if (INIT(wnow.XLOC+8,wnow.YLOC+8,'G')==FALSE){wnow.HASIGO=0;}
			
			
			
			if (INIT(wnow.XLOC+8,wnow.YLOC+8,'G')== TRUE && wlastkey.AU !=0){
				wnow.HASIGO=1;
				wnow.shupict=2;
				//GrCopyArea(shu_pixmap,keyman_gc,0,0,16,16,yomipict_pixmap,32,112,0);
				wnow.XKANSEI=0;
				//wnow.YLOC=wnow.YLOC-3;
				}
			else if (INIT(wnow.XLOC+8,wnow.YLOC+8,'G')== TRUE && wlastkey.AD !=0){
				wnow.HASIGO=0;
				//GrCopyArea(shu_pixmap,keyman_gc,0,0,16,16,yomipict_pixmap,32,112,0);
				wnow.XKANSEI=0;
				//wnow.YLOC=wnow.YLOC+3;	
				}
				
			
			if (wnow.JIMEN==ZIM){wnow.XKANSEI=(0.9*wnow.XKANSEI)+(1*wnow.LR);}
			else {
				if (wnow.LR != 0){
				wnow.XKANSEI=(0.99*wnow.XKANSEI)+(0.3*wnow.LR);
				}
			}
			if (wnow.HASIGO==0 ){
				if (wnow.YKANSEI <= 6){
					wnow.YKANSEI++;
				}
			}
			else {wnow.YKANSEI=0;}
			
			
			

			
			wnow.XLOC=wnow.XLOC+wnow.XKANSEI;
			wnow.YLOC=wnow.YLOC+wnow.YKANSEI;
			
			
			if (wnow.NUPDW==-1 && ((wnow.MAPSCROLL) % 16)==0){wnow.YLOC=wnow.YLOC+16;}
			else if (wnow.NUPDW==1 && (wnow.MAPSCROLL % 16)==15){wnow.YLOC=wnow.YLOC-16;}
			
			
			
			if (wnow.MAPSCROLL>0 && wnow.MAPSCROLL<TATEHABA*16){
				wnow.MAPSCROLL=wnow.MAPSCROLL+wnow.NUPDW;
			}
			scr= (wnow.MAPSCROLL/16);
			ATARI2();
						
			
			if (wlastkey.AU != 1 && wlastkey.AD != 1){
			 if (wnow.MUKI==1){
			 wnow.shupict=0;
			 //GrCopyArea(shu_pixmap,keyman_gc,0,0,16,16,yomipict_pixmap,0,112,0);
			 }
			 else {
			 wnow.shupict=1;
			 //GrCopyArea(shu_pixmap,keyman_gc,0,0,16,16,yomipict_pixmap,16,112,0);
			 }
			}			
			
						
			if (wnow.NUPDW==-1 && wnow.YLOC>136){
				wdeady.sp=-20;
				wdeady.g=1.6;
				wdeady.nowt=128;
				GAMEN=END;
				wdeady.kaiten=0;
				ketumatu=0;
				wdeady.deadcount=1;}
				
			else if(wnow.NUPDW==1 && wnow.YLOC<0){
				wdeady.sp=12;
				wdeady.g=0;
				wdeady.nowt=-16;
				GAMEN=END;
				wdeady.kaiten=0;
				ketumatu=0;
				wdeady.deadcount=1;}
			else if (wnow.NUPDW==0 && (wnow.YLOC>136)) {
			
				wdeady.sp=-10;
				wdeady.g=1.6;
				wdeady.nowt=128;
				GAMEN=END;
				wdeady.kaiten=0;
				ketumatu=0;
				wdeady.deadcount=1;}
					
				
			
			GrCopyArea(mainmenu_pixmap,keyman_gc,0,0,160,128,allmap_pixmap,0,wnow.MAPSCROLL,0);
			kmbitmap();
			//GrCopyArea(mainmenu_pixmap,keyman_gc,wnow.XLOC,(wnow.YLOC-(wnow.MAPSCROLL % 16)),160,128,shu_pixmap,0,0,0);
			hensu();		
			
			MINIPHOTO(0,0,160,128);
			if (wlastkey.PAU==1){GAMEN=PAUSE;}
			
			wlastkey.AR=0;wlastkey.AL=0;wlastkey.AU=0;wlastkey.AD=0;wlastkey.REE=0;wlastkey.PAU=0;
			break;
		case PAUSE:
			GrCopyArea(dialogall_pixmap,keyman_gc,0,0,96,80,dialog_pixmap,0,0,0);
			GrCopyArea(dialogall_pixmap,keyman_gc,3,24+(dialogkey*24),13,16,yomipict_pixmap,115,48,0);
			GrCopyArea(keyman_wid,keyman_gc,32+((screen_info.cols-160)/2),24+((screen_info.rows-128)/2),96,80,dialogall_pixmap,0,0,0);
			
			break;
		case END:
				if (ketumatu==0){
					wdeady.sp=wdeady.sp+wdeady.g;
					wdeady.nowt=wdeady.nowt+wdeady.sp;
					//sprintf(hensuu,"%d",wnow.MAPSCROLL+(wdeady.deadcount*wnow.NUPDW));
					
					if (wnow.MAPSCROLL+(wdeady.deadcount*wnow.NUPDW)>0 && wnow.MAPSCROLL+(wdeady.deadcount*wnow.NUPDW)<TATEHABA*16){
						GrCopyArea(mainmenu_pixmap,keyman_gc,0,0,160,128,allmap_pixmap,0,wnow.MAPSCROLL+(wdeady.deadcount*wnow.NUPDW),0);
					}
					
					GrCopyArea(shu_pixmap,keyman_gc,0,0,16,16,yomipict_pixmap,wdeady.kaiten*16,16,0);
					GrCopyArea(mainmenu_pixmap,keyman_gc,wnow.XLOC,wdeady.nowt,160,128,shu_pixmap,0,0,0);
					hensu();
					
					MINIPHOTO(0,0,160,128);
					if (wdeady.kaiten<7){wdeady.kaiten++;}else{wdeady.kaiten=0;}
					if (training==0){
						if (wdeady.deadcount>31){GAMEN=MENU;
					
						}   
					}
					else if (training==1){
						if (wdeady.deadcount>15){opencard();GAMEN=INGAME;
						}
						
					}
					wdeady.deadcount++;
				}
				else if (ketumatu==1){
					if (training==0){
					GAMEN=MENU;
					}
					if (training==1){
						if (tranum>=MAXTRA){
							GAMEN=MENU;
						}
						else{
							startgame();
							GAMEN=INGAME;
						}
					
					}
				}
			break;
		case NODATA:
			GrSetGCForeground(keyman_gc, GR_RGB(255,255,255));
			GrFillRect(keyman_wid,keyman_gc,0,0,160,128);
			
			GrSetGCForeground(keyman_gc, GR_RGB(80,80,80));
			GrLine(keyman_wid,keyman_gc,80,20,34,100);
			GrLine(keyman_wid,keyman_gc,80,20,126,100);
			GrLine(keyman_wid,keyman_gc,34,100,126,100);
			GrEllipse(keyman_wid,keyman_gc,80,55,5,20);
			GrEllipse(keyman_wid,keyman_gc,80,85,5,5);
			
			GrSetGCForeground(keyman_gc, GR_RGB(0,0,0));
			GrText(keyman_wid,keyman_gc,50,48,"Please Copy",-1,GR_TFASCII|GR_TFTOP);
			GrText(keyman_wid,keyman_gc,30,68,"KMData folder to /etc",-1,GR_TFASCII|GR_TFTOP);
			GrText(keyman_wid,keyman_gc,50,100,"Click to Quit",-1,GR_TFASCII|GR_TFTOP);
			break;
		}
		
			
	break;
	

    case( GR_EVENT_TYPE_KEY_DOWN ):
	
	switch(GAMEN){
		case INGAME:
			if (event->keystroke.ch==wsetkey.KLEFT){wlastkey.AL=1;}else{wlastkey.AL=0;}
			if (event->keystroke.ch==wsetkey.KRIGHT){wlastkey.AR=1;}else{wlastkey.AR=0;}
			if (event->keystroke.ch==wsetkey.KUP){wlastkey.AU=1;}else{wlastkey.AU=0;}
			if (event->keystroke.ch==wsetkey.KDOWN){wlastkey.AD=1;}else{wlastkey.AD=0;}
			if (event->keystroke.ch==wsetkey.KJUMP){wlastkey.REE=1;}else{wlastkey.REE=0;}
			if (event->keystroke.ch==wsetkey.KPAUSE){wlastkey.PAU=1;}else{wlastkey.PAU=0;}
			break;
		case MENU:
			switch( event->keystroke.ch )
			{
	    
			case '\r': /* action */
				switch (wmenukey.BA){
				case 0:if (wmenukey.KEY<2){wmenukey.KEY++;}else{wmenukey.KEY=1;}break;	
				case 1:if (wmenukey.STAGE<MAXSTAGE){wmenukey.STAGE++;}
						else {wmenukey.STAGE=1;}
						break;
				
				case 2: GAMEN=INGAME; 
					training=0;
					startgame();
					break;
				case 3:GAMEN=INGAME;
					tranum=0;
					training=1;
					startgame();
					break;
				case 4: pz_close_window(keyman_wid);
					GrDestroyTimer(keyman_timer);
					GrUnmapWindow(keyman_wid);
					GrDestroyWindow(keyman_wid);
					GrFreeImage(kmimage_id);
					GrDestroyGC(keyman_gc);
					ret = 1;
					break;
				}
			break;
			case 'r': /* CCW spin */
			if (wmenukey.BA<4){ipod_beep();wmenukey.BA++;}		
			break;

			case 'l': /* CW spin */
			if (wmenukey.BA>0){ipod_beep();wmenukey.BA--;}
			break;
			
			case 'm': /* menu */
					pz_close_window(keyman_wid);
					GrDestroyTimer(keyman_timer);
					GrUnmapWindow(keyman_wid);
					GrDestroyWindow(keyman_wid);
					GrFreeImage(kmimage_id);
					GrDestroyGC(keyman_gc);
					ret = 1;		
			break;
			}
			break;
			
					
		case PAUSE:
			if (event->keystroke.ch==wsetkey.KLEFT && dialogkey == 1){ipod_beep();dialogkey=0;}
			if (event->keystroke.ch==wsetkey.KRIGHT && dialogkey == 0){ipod_beep();dialogkey=1;}
			if (event->keystroke.ch==wsetkey.KJUMP && dialogkey == 0){GAMEN=INGAME;}
			else if (event->keystroke.ch==wsetkey.KJUMP && dialogkey == 1){GAMEN=MENU;}
			
			break;
		case END:break;
		case NODATA:
			if (event->keystroke.ch=='\r'){
			GrDestroyTimer(keyman_timer);
			GrUnmapWindow(keyman_wid);
			GrDestroyWindow(keyman_wid);
			GrFreeImage(kmimage_id);
			GrDestroyGC(keyman_gc);
			ret = 1;
			}
			break;
		}
	
	
	
	break;
    }

    return ret;
}

static void keyman_do_draw() {
	//pz_draw_header("KeyMan");
}


void new_keyman_window( void )
{
	
    keyman_gc = GrNewGC();
    GrSetGCUseBackground(keyman_gc, GR_FALSE);
    GrSetGCForeground(keyman_gc, GR_RGB(255,255,255));
	GrGetScreenInfo(&screen_info);
	
	yomipict_pixmap=GrNewPixmap(384,128,NULL);
	mainmenu_pixmap=GrNewPixmap(160,128,NULL);
	shu_pixmap=GrNewPixmap(16,16,NULL);
	dialogall_pixmap=GrNewPixmap(96,80,NULL);
	dialog_pixmap=GrNewPixmap(96,80,NULL);
	
	
    keyman_wid = pz_new_window(0, 0, 
	screen_info.cols, screen_info.rows, 
	keyman_do_draw, keyman_handle_event);

		
	if (!(kmimage_id=GrLoadImageFromFile("/opt/Zillae/ZacZilla/Data/KMData/pict.gif",0))){
		GAMEN=NODATA;
		
	}
	else {
		GAMEN=MENU;
		GrDrawImageToFit(yomipict_pixmap,keyman_gc,0,0,384,128,kmimage_id);
		makedialog();
	}
	
	

    GrSelectEvents( keyman_wid, GR_EVENT_MASK_TIMER|
	GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_UP|GR_EVENT_MASK_KEY_DOWN);

    keyman_timer = GrCreateTimer( keyman_wid, 90 );
    GrMapWindow( keyman_wid );
    pz_draw_header( "KeyMan" );
}
