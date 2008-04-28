

/* chopper2: Originally by Joshua Oreman, improved by Prashant Varanasi */
/* Feel free to improve  */


/*Still To do:*/
/* - Make original speed and further increases in speed depend more on screen size*/
/* - attempt to make the tunnels get narrower as the game goes on*/
/* - make the chopper2 look better, maybe a picture, and scale according to screen size*/
/* - use textures for the color screens for background and terrain, eg stars on background*/
/* - allow choice of different levels [later: different screen themes]*/
/* - better high score handling, improved screen etc. */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#include <stdlib.h>
#include <errno.h>
#include "pz.h"


#define NUMBER_OF_BLOCKS 8
#define NUMBER_OF_PARTICLES 20
#define MAX_TERRAIN_NODES 15

#define LEVEL_MODE_NORMAL 1
#define LEVEL_MODE_STEEP 2




static TWindow *mywindow=NULL;
static TWidget *mywidget=NULL;

//static ttk_surface mysrf;

static ttk_color blackcolor;

/*chopper2's local variables to track the terrain position etc*/
static int chopCounter;

static int iRotorOffset;
static int iScreenX;
static int iScreenY;

static int iPlayerPosX;
static int iPlayerPosY;
static int iCameraPosX;
static int iPlayerSpeedX;
static int iPlayerSpeedY;

static int iLastBlockPlacedPosX;

static int iGravityTimerCountdown;
static int bPlayerPressingUp;

static int iPlayerAlive;

static int iLevelMode;

static int blockh,blockw;

static char chopper2Paused=0;

static char newhighscore=0;

typedef struct
{
	int iWorldX;
	int iWorldY;
	
	int iSizeX;
	int iSizeY;
	
	int bIsActive;
} CBlock;

typedef struct
{
	int iWorldX;
	int iWorldY;
	
	int iSpeedX;
	int iSpeedY;
	
	int bIsActive;
} CParticle;

typedef struct
{
	int x;
	int y;
} CTerrainNode;

typedef struct
{
	CTerrainNode mNodes[MAX_TERRAIN_NODES];
	int iNodesCount;
	int iLastNodePlacedPosX;
} CTerrain;

CBlock mBlocks[NUMBER_OF_BLOCKS];
CParticle mParticles[NUMBER_OF_PARTICLES];

CTerrain mGround;
CTerrain mRoof;



/*Colors to be used in the game - dependant on generation */
static ttk_color cHeliB,cHeliH,cRotorConnect,cRotor,cHeliBack,cParticle,cBackground,cBlock,cTerrain,cScore;



/*Function declarations*/
static void chopDrawParticle(CParticle *mParticle,ttk_surface mysrf);
static void chopDrawBlock(CBlock *mBlock,ttk_surface mysrf);
static void chopRenderTerrain(CTerrain *ter,ttk_surface mysrf);
void chopper2_load();
void cleanup_chopper2();
int hs;
#define HIGHSCORE 0
#define chopper2LEVEL 1
#define DISABLESETTINGS 0 
/*just to see the speed difference on the iPod*/

static void LoadSettings()
{
	if(DISABLESETTINGS) return;
// KERIPO MOD
//hs=readHighScore("Data/.chopper");
hs=readHighScore("/opt/Zillae/ZacZilla/Data/chopper.txt");
}
static void SaveSettings(int closeonly)
{
	if(DISABLESETTINGS) return;
	// KERIPO MOD	
	//writeHighScore("Data/./chopper",hs);	
	writeHighScore("/opt/Zillae/ZacZilla/Data/chopper.txt",hs);
		
}

static void PutSetting(int setting, int value)
{
	if(DISABLESETTINGS) return;
	hs=value;
}
static int GetSetting(int setting)
{
	if(DISABLESETTINGS) return 0;
	// KERIPO MOD	
	//return readHighScore("Data/.chopper");
	return readHighScore("/opt/Zillae/ZacZilla/Data/chopper.txt");
}
	


static void chopDrawPlayer(int x,int y,ttk_surface mysrf) /*These are SCREEN coords, not world!*/
{
	if(iPlayerAlive == 0)
	{
		ttk_text(mysrf, ttk_menufont, mywindow->w/2 - 30 ,mywindow->h/2-20, blackcolor, "Game Over");
		ttk_text(mysrf, ttk_menufont, mywindow->w/2 - 60, mywindow->h/2, blackcolor, "Press Action to try again!");
		if(newhighscore)
			ttk_text(mysrf, ttk_menufont, mywindow->w/2 - 40, mywindow->h/2+20, blackcolor, "New High Score!");
			
		return;
	}
	
	  //chopGC = GrNewGC();       /* Get the graphics context */
	  /*Homer: mmm...magic numbers*/
	ttk_fillrect(mysrf,x+6,y+2,x+6+12,y+2+9,cHeliH);
	ttk_fillrect(mysrf,x-3,y+6,x-3 + 23,y+6+3,cHeliB);
	ttk_fillrect(mysrf,x+10,y,x+10+2,y+3,cRotorConnect);
	ttk_fillrect(mysrf,x+10,y,x+11,y+3,cRotorConnect);
	ttk_line(mysrf,x,y+iRotorOffset,x+20,y-iRotorOffset,cRotor);

	ttk_fillrect(mysrf,x-2,y+5,x-2+2,y+5+5,cHeliBack);
	
		
}



static void loadColors()
{
	static char init=0;
	if (init) return;
	blackcolor = ttk_makecol(0,0,0);
	
	if (ttk_screen->bpp==2)
	{
		cHeliB=ttk_makecol(BLACK);
		cHeliH=ttk_makecol(BLACK);
		cRotorConnect=ttk_makecol(BLACK);
		cRotor=ttk_makecol(BLACK);
		cHeliBack=ttk_makecol(BLACK);
		cParticle=ttk_makecol(GREY);
		cBackground=ttk_makecol(WHITE);
		cBlock=ttk_makecol(DKGREY);
		cTerrain=ttk_makecol(BLACK);
		cScore=ttk_makecol(DKGREY);
	}
	else
	{
		cHeliB=ttk_makecol(50,50,200);	
		cHeliH=ttk_makecol(50,50,200);	
		cRotorConnect=ttk_makecol(50,50,50);
		cRotor=ttk_makecol(30,30,30);
		cHeliBack=ttk_makecol(10,10,10);
		cParticle=ttk_makecol(150,150,150);
		cBackground=ttk_makecol(145,197,255);
		cBlock=ttk_makecol(30,30,30);
		cTerrain=ttk_makecol(50,100,250);
		cScore=ttk_makecol(20,20,20);
	}

	init=1;
}


static void chopClearTerrain(CTerrain *ter)
{
	ter->iNodesCount = -1;
}


static int iR(int low,int high)
{
	return low+rand()%(high-low+1);
}


static void chopCopyTerrain(CTerrain *src,CTerrain *dest,int xOffset,int yOffset)
{
	int i=0;
	
	while(i < src->iNodesCount)
	{
		dest->mNodes[i].x = src->mNodes[i].x + xOffset;
		dest->mNodes[i].y = src->mNodes[i].y + yOffset;
		
		i++;
	}
	
	dest->iNodesCount = src->iNodesCount;
	dest->iLastNodePlacedPosX = src->iLastNodePlacedPosX;

}
	

static void chopAddTerrainNode(CTerrain *ter,int x,int y)
{
	int i=0;
	
	if(ter->iNodesCount + 1 >= MAX_TERRAIN_NODES)
	{
		//printf("ERROR: Not enough nodes!\n");
		return;
	}
	
	ter->iNodesCount++;
	
	i = ter->iNodesCount - 1;
	
	ter->mNodes[i].x = x;
	ter->mNodes[i].y= y;
	
	ter->iLastNodePlacedPosX = x;
	
	//printf("placed node %d at %d\n",i,x);
	//chopPrintTerrainSummery(ter);
	
}


static void chopTerrainNodeDeleteAndShift(CTerrain *ter,int nodeIndex)
{
	int i=nodeIndex;
	
	while( i < ter->iNodesCount )
	{
		ter->mNodes[i - 1] = ter->mNodes[i];	
		i++;
	}
	
	ter->iNodesCount--;
	
		
}

static int chopUpdateTerrainRecycling(CTerrain *ter)
{
	int i=1;
	int ret = 0;
	int iNewNodePos,g,v;
	while(i < ter->iNodesCount)
	{
		
		if( iCameraPosX > ter->mNodes[i].x)
		{
//			int x = ter->mNodes[i].x;
			//it's off the screen
			chopTerrainNodeDeleteAndShift(ter,i);
			
			
			
			iNewNodePos = ter->iLastNodePlacedPosX + 50; //magic number
			g = iScreenY - 10;
			
			v = 3*iPlayerSpeedX;
			if(v>50) v=50;
			if(iLevelMode == LEVEL_MODE_STEEP)
				v*=5;
				
			chopAddTerrainNode(ter,iNewNodePos,g - iR(-v,v));
			ret=1;
			
		}

		i++;
		
	}

	return 1;
}

static int chopTerrainHeightAtPoint(CTerrain *ter,int pX)
{
	//this is pretty hacked. But it's 3am and I don't care ;)
	
	int iNodeIndexOne,iNodeIndexTwo,h,terY1,terY2,terX1,terX2,a,b;
	float c,d;
	
	int i=0;
	for(i=1;i<MAX_TERRAIN_NODES;i++)
	{
		if(ter->mNodes[i].x > pX)
		{
			iNodeIndexOne = i - 1;
			break;
		}
	
	}
	
	iNodeIndexTwo = iNodeIndexOne + 1;
		
	terY1 = ter->mNodes[iNodeIndexOne].y;
	terY2 = ter->mNodes[iNodeIndexTwo].y;
	
	terX1 = 0;
	terX2 = ter->mNodes[iNodeIndexTwo].x - ter->mNodes[iNodeIndexOne].x;
	
	pX-= ter->mNodes[iNodeIndexOne].x;
	
	a = terY2 - terY1;
	b = terX2;
	c = pX;
	d = (c/b) * a;
	
	h = d + terY1;
	
	//printf("\n\nter1 = %d/%d, ter2 = %d/%d, pos = %d\n",terX1,terY1,terX2,terY2,pX);	
	//printf("a = %f, b = %f, c = %f, d = %f, h = %d\n",a,b,c,d,h);
	//printf("c/b * a :: %f/%f * %f (%f)\n\n",c,b,a,c/b);
	
	return h;

}


static int chopPointInTerrain(CTerrain *ter,int pX,int pY,int iTestType)
{
	int h = chopTerrainHeightAtPoint(ter,pX);
	
	if(iTestType == 0)
		return (pY > h);
	else
		return (pY < h);
}


static void chopAddBlock(int x,int y,int sx,int sy, int indexOverride)/*World coords*/
{
	int i=0;
	
	if(indexOverride < 0)
	{
		while(mBlocks[i].bIsActive && i < NUMBER_OF_BLOCKS)
			i++;	
		if(i==NUMBER_OF_BLOCKS)
		{
			printf("No blocks!\n");
			return;
		}
	}
	else
		i = indexOverride;
	
			
	mBlocks[i].bIsActive = 1;
	
	mBlocks[i].iWorldX = x;
	mBlocks[i].iWorldY = y;
	mBlocks[i].iSizeX = sx;
	mBlocks[i].iSizeY = sy;
	
	iLastBlockPlacedPosX = x;
	
}

static void chopAddParticle(int x,int y,int sx,int sy)
{
	int i=0;
	

	while(mParticles[i].bIsActive && i < NUMBER_OF_PARTICLES)
		i++;	
	if(i==NUMBER_OF_BLOCKS)
	{
		return;
	}

			
	mParticles[i].bIsActive = 1;
	
	mParticles[i].iWorldX = x;
	mParticles[i].iWorldY = y;
	mParticles[i].iSpeedX = sx;
	mParticles[i].iSpeedY = sy;
	
}

static void chopGenerateBlockIfNeeded()
{
	int i=0;
	int DistSpeedX = iPlayerSpeedX * 5;
	if(DistSpeedX<200) DistSpeedX = 200;
	
	while(i < NUMBER_OF_BLOCKS)
	{
		if(!mBlocks[i].bIsActive)
		{
			
			int iX,iY,sX,sY;
			
			iX = iLastBlockPlacedPosX + (350-DistSpeedX);
			sX = blockw;
			
			iY = iR(0,iScreenY);
			sY = blockh + iR(1,blockh/3);//iR(0,50);
			
			chopAddBlock(iX,iY,sX,sY,i);
			
		}
			
		i++;
	}		

}


static int chopPointInBox(int px,int py,int x,int y,int x2,int y2)
{
	if(px > x && px < x2 && py > y && py < y2)
		return 1;
	else
		return 0;
}

static int chopBlockCollideWithPlayer(CBlock *mBlock)
{
	int px = iPlayerPosX;
	int py = iPlayerPosY;
	
	int x = mBlock->iWorldX-17;
	int y = mBlock->iWorldY-11;
// 	
	int x2 = x + mBlock->iSizeX+17;
	int y2 = y + mBlock->iSizeY+11;
	
	if(px>x && px<x2)
	{
		if(py>y && py<y2)
		{
			return 1;
		}
	}
	//if(chopPointInBox(px + 20,py,x,y,x2,y2))return 1;
	//if(chopPointInBox(px + 20,py+10,x,y,x2,y2))return 1;
	
	return 0;	
}

	

static int chopBlockOffscreen(CBlock *mBlock)
{
	if(mBlock->iWorldX + mBlock->iSizeX < iCameraPosX)
	{
		return 1;
	}
	else
		return 0;
}

static int chopParticleOffscreen(CParticle *mParticle)
{
	if(mParticle->iWorldX < iCameraPosX || mParticle->iWorldY < 0 || mParticle->iWorldY > iScreenY || mParticle->iWorldX > iCameraPosX + iScreenX)
	{
		return 1;
	}
	else
		return 0;
}

static void chopKillPlayer()
{
	
	int i=0;
	
	while(i < NUMBER_OF_PARTICLES)
	{
		mParticles[i].bIsActive = 0;
		chopAddParticle(iPlayerPosX + iR(0,20),iPlayerPosY + iR(0,20),iR(-2,2),iR(-2,2));
		i++;
	}
	iPlayerSpeedX = 0;
	iPlayerAlive = 0;
	if (iPlayerPosX>GetSetting(HIGHSCORE))
	{
		newhighscore=1;
		PutSetting(HIGHSCORE,iPlayerPosX);
	}

}


static void chopDrawTheWorld(ttk_surface mysrf)
{
	int i=0;	
	
	while(i < NUMBER_OF_BLOCKS)
	{
		if(mBlocks[i].bIsActive)
		{
			if(chopBlockOffscreen(&mBlocks[i]) == 1)
				mBlocks[i].bIsActive = 0;
			else
				chopDrawBlock(&mBlocks[i],mysrf);
		}
		
		i++;
	}

	i=0;
	
	while(i < NUMBER_OF_PARTICLES)
	{
		if(mParticles[i].bIsActive)
		{
			if(chopParticleOffscreen(&mParticles[i]) == 1)
				mParticles[i].bIsActive = 0;
			else
				chopDrawParticle(&mParticles[i],mysrf);
		}
		
		i++;
	}
	
	chopRenderTerrain(&mGround,mysrf);
	chopRenderTerrain(&mRoof,mysrf);

}


static void chopDrawParticle(CParticle *mParticle,ttk_surface mysrf)
{
	
	int iPosX = (mParticle->iWorldX - iCameraPosX);
	int iPosY = (mParticle->iWorldY);

	ttk_fillrect(mysrf, iPosX,iPosY,iPosX+3,iPosY+3,cParticle); 

}

static void chopDrawScene(ttk_surface mysrf)
{
	char           s[30];
	ttk_fillrect(mysrf, 0,0,mywindow->w,mywindow->h,cBackground); 
	chopDrawPlayer(iPlayerPosX - iCameraPosX,iPlayerPosY,mysrf);
	
	/*
	
	If I was less lazy I could make a nifty shadow effect with this stuff...
	
	int h = chopTerrainHeightAtPoint(&mGround, iPlayerPosX);
	GrSetGCForeground(gc, BLACK);
	GrFillRect(chopBuffer,gc,iPlayerPosX - iCameraPosX,h - 1,2,2);
	
	*/
	
	chopDrawTheWorld(mysrf);
	sprintf(s,"%d",iPlayerPosX);
	ttk_text(mysrf,ttk_menufont,2,2,cScore,s);
	//Flip the buffer
	ttk_gfx_update(mysrf);
}

static void chopGameLoop(ttk_surface mysrf)
{
	if (chopper2Paused || !iPlayerAlive) 
	{
		chopDrawScene(mysrf); 
		return;
	}
	   int i=0;
	
	if(chopUpdateTerrainRecycling(&mGround)==1)
		chopCopyTerrain(&mGround,&mRoof,0,-( iScreenY * 0.75)); //mirror the sky if we've changed the ground
	
	iRotorOffset = iR(-1,1);
	

	
	//We need to have this here so particles move when we're dead
	i=0;
	while(i < NUMBER_OF_PARTICLES)
	{
		if(mParticles[i].bIsActive == 1)
		{
			mParticles[i].iWorldX += mParticles[i].iSpeedX;
			mParticles[i].iWorldY += mParticles[i].iSpeedY;
		}
		
		i++;
	}
	
	
	/* Redraw the main window: */
	chopDrawScene(mysrf); 
	
	
	
	iCameraPosX = iPlayerPosX - 25;
	iPlayerPosX+=iPlayerSpeedX;
	iPlayerPosY+=iPlayerSpeedY;
	
	iGravityTimerCountdown--;
	
	if(iGravityTimerCountdown <= 0)
	{
		//iPlayerSpeedY++;
		iGravityTimerCountdown = 3;
		chopAddParticle(iPlayerPosX,iPlayerPosY+5,0,0); //pretty hax putting this here :S
	}
	
	//cap player vel
	if(iPlayerSpeedY < -2)
		iPlayerSpeedY = -2;
	
	if(iLevelMode == LEVEL_MODE_NORMAL)
		chopGenerateBlockIfNeeded();
    
    
    
	if(bPlayerPressingUp == 1)
	{
		if(iPlayerSpeedY > 0)
			iPlayerSpeedY = 0;
		else
			iPlayerSpeedY-=2;
	}
	else
	iPlayerSpeedY++;

	chopCounter++;
	//printf("Chop Counter: %d\n",chopCounter);
	//increase speed as we go along
	if(chopCounter == 50){
		iPlayerSpeedX++;
// 		printf("Speed increase!\n");
		chopCounter=0;
	}
		
	//goto nextpart;
	if(iPlayerPosY > iScreenY-10 || iPlayerPosY < -5 ||  
	chopPointInTerrain(&mGround,iPlayerPosX,iPlayerPosY + 10,0) || chopPointInTerrain(&mRoof,iPlayerPosX,iPlayerPosY,1)
	)
	{
		chopKillPlayer();
	}
	
	i=0;
	while(i < NUMBER_OF_BLOCKS)
	{
		if(mBlocks[i].bIsActive == 1)
		{
			if(chopBlockCollideWithPlayer(&mBlocks[i]))
			{
				//pwned
				chopKillPlayer();
			}
		}
		
		i++;
	}
}


/*
static void chopPrintTerrainSummery(CTerrain *ter)
{

	printf("Terrain:\n");
	
	int i=0;
	
	while(i < ter->iNodesCount)
	{
		printf("%d - %d/%d\n",i,ter->mNodes[i].x,ter->mNodes[i].y);
		i+;
	}
	
	printf("Number of nodes: %d. Last pos: %d\n\n",ter->iNodesCount,ter->iLastNodePlacedPosX);
}*/




static void chopDrawBlock(CBlock *mBlock,ttk_surface mysrf)
{ 
	//gc = GrNewGC();   
	int iPosX = (mBlock->iWorldX - iCameraPosX);
	int iPosY = (mBlock->iWorldY);
	ttk_fillrect(mysrf, iPosX,iPosY,iPosX+mBlock->iSizeX,iPosY+mBlock->iSizeY,cBlock); 
}


static void chopRenderTerrain(CTerrain *ter,ttk_surface mysrf)
{

	int i=1;

	int oldx=0;

	int ay=0;
	if(ter->mNodes[0].y<mywindow->h/2) ay=0; else ay=mywindow->h;
	
	while(i < ter->iNodesCount && oldx<mywindow->w)
	{
		
		static p=0;
		int x = ter->mNodes[i-1].x -  iCameraPosX;
		int y = ter->mNodes[i-1].y;
		
		int x2 = ter->mNodes[i].x -  iCameraPosX;
		int y2 = ter->mNodes[i].y;

		 				
		//ttk_line(mysrf,x,y,x2,y2,cTerrain);
		//ttk_line(mysrf, x,y - 1,x2,y2 - 1,cTerrain);
		//ttk_line(mysrf, x,y + 1,x2,y2 + 1,cTerrain);
		short polyx[5]={x,x,x2,x2,x};
		short polyy[5]={ay,y,y2,ay,ay};
		ttk_fillpoly(mysrf,5,polyx,polyy,cTerrain);
				
		//Draw a number above it to aid with debugging
		/*
		char num[2];
		sprintf(num,"%d",i);
		GrText(chopBuffer, gc, x2, y2, num, -1, GR_TFASCII);*/
		oldx=x;
		i++;
		
	}

}


static int chopper2_button(TWidget *this,int button,int time)
{
			int ret=0;
			switch (button)
			{
				case TTK_BUTTON_ACTION:
					bPlayerPressingUp=0;
					break;
				case TTK_BUTTON_HOLD:
					chopper2Paused=0;	
					break;
				case TTK_BUTTON_MENU:
					SaveSettings(0);
					ttk_hide_window (this->win);
					break;
				default:
					ret |= TTK_EV_UNUSED;
			}
			return ret;
		}
	static int chopper2_down(TWidget *this,int button)
	{
			int ret=0;
			switch (button)
			{
				case TTK_BUTTON_ACTION:
					if (iPlayerAlive==0)
					{
						chopper2_load();
					}
					else
					{
						bPlayerPressingUp=1;
					}
					break;
				case TTK_BUTTON_HOLD:
					chopper2Paused=1;
					break;
				default:
					ret |= TTK_EV_UNUSED;
			}
			return ret;
		}
		

static int chopper2_buttonhi(TWidget *this,int button,int time)
{
		int ret=0;
			switch (button)
			{
				case TTK_BUTTON_ACTION:
					ttk_hide_window(this->win);
					SaveSettings(1);
					break;
				case TTK_BUTTON_MENU:
					SaveSettings(1);
					ttk_hide_window (this->win);
					break;
				default:
					ret |= TTK_EV_UNUSED;
			}
			
	    
    return ret;
}

int timer_call (struct TWidget *this)
{
	mywindow->dirty=1;
}

void draw_chopper2 (TWidget *wid,ttk_surface srf) 
{
	//chopGameLoop();
	chopGameLoop(srf);
	
}

void draw_chopper2_highscore(TWidget *wid,ttk_surface srf)
{
	int score = GetSetting(HIGHSCORE);
	char scoretext[30];
	sprintf(scoretext,"High Score: %d",score);
	int w,h;
	ttk_surface_get_dimen(srf,&w,&h);
	ttk_text(srf, ttk_menufont, w/2 - 30 ,h/2-20, blackcolor, scoretext);
\
}



TWindow *new_chopper2_window()
{
	printf("new_chopper2_window\n");
	TWindow *ret;
	mywindow = ttk_new_window ();
	mywindow->title="Chopper";
	mywidget=ttk_new_widget(0,0);
	mywidget->focusable=1;
	mywidget->draw=draw_chopper2;
	mywidget->down=chopper2_down;
	mywidget->button=chopper2_button;
	ttk_add_widget (mywindow, mywidget);
	ttk_widget_set_timer(mywidget,65);
	mywidget->timer=timer_call;
	printf("Timer Created\n");
	chopper2_load();
	chopGameLoop(mywindow->srf);
	LoadSettings();
	ttk_show_window (mywindow);
    	return mywindow;
}

TWindow *new_chopper2_highscore()
{
	TWindow *scorewindow;
	TWindow *ret;
	TWidget *myhi;
	scorewindow=ttk_new_window ();
	scorewindow->title="Chopper2 High Scores";
	myhi=ttk_new_widget(0,0);
	myhi->focusable=1;
	myhi->button=chopper2_buttonhi;
	ttk_add_widget (scorewindow, myhi);
	LoadSettings();
	ttk_show_window (scorewindow);
    	return scorewindow;
}
	

void chopper2_load()
{
		/*chopDrawScene();*/
	loadColors();
	
	iScreenX = mywindow->w;
	iScreenY = mywindow->h;
	blockh=0.2*iScreenY;
	blockw=0.04*iScreenX;
	
	iRotorOffset = 0;

	//chopAddBlock(100,100,20,20);
	
		
	iPlayerPosX = 60;
	iPlayerPosY = iScreenY * 0.4;
	
	iLastBlockPlacedPosX = 0;
	iGravityTimerCountdown = 2;
	
	iPlayerAlive = 1;
	
	chopCounter = 0;
	
	newhighscore=0;

	int i;
	int g;
	
	i=0;
	while(i < NUMBER_OF_PARTICLES)
	{
		mParticles[i].bIsActive = 0;
		i++;
	}
	
	i=0;
	while(i < NUMBER_OF_BLOCKS)
	{
		mBlocks[i].bIsActive = 0;
		i++;
	}
	
	g = iScreenY - 10;
	
	chopClearTerrain(&mGround);
	
	i=0;
	while(i < MAX_TERRAIN_NODES) //we could add less, probably.
	{
		chopAddTerrainNode(&mGround,i * 30,g - iR(0,20));
		
		i++;
	}
	i=0;
	//chopCopyTerrain(&mGround,&mRoof,0,-( iScreenY * 0.75)); //mirror the sky if we've changed the ground
	if(chopUpdateTerrainRecycling(&mGround)==1)
		chopCopyTerrain(&mGround,&mRoof,0,-( iScreenY * 0.75)); //mirror the sky if we've changed the ground

	//iLevelMode = iR(1,2);
	iLevelMode = LEVEL_MODE_NORMAL;
	
	iPlayerSpeedX = 4;
	iPlayerSpeedY = 0;

	
	if(iLevelMode == LEVEL_MODE_NORMAL)
		iPlayerSpeedX*=2; //make it a bit more exciting, cause it's easy terrain...
	//printf("level mode: %d",iLevelMode);
	
	iCameraPosX=0; bPlayerPressingUp=0;

}


