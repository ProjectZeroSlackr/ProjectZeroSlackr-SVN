#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#include "pz.h"
#include "ipod.h"


#define NUMBER_OF_BLOCKS 5
#define NUMBER_OF_PARTICLES 10
#define MAX_TERRAIN_NODES 10

#define LEVEL_MODE_NORMAL 1
#define LEVEL_MODE_STEEP 2

static GR_WINDOW_ID chopper_wid;
static GR_GC_ID chopper_gc;
static GR_WINDOW_ID chopBuffer;
static GR_TIMER_ID chopTimer;
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

static int bWaitingToStart;


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

static void chopClearTerrain(CTerrain *ter)
{
	ter->iNodesCount = -1;
}


int iR(int low,int high)
{
	return low+rand()%(high-low+1);
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

static void chopRenderTerrain(CTerrain *ter)
{

	int i=1;
	GrSetGCForeground(chopper_gc, BLACK);
	
	while(i < ter->iNodesCount)
	{
		int x = ter->mNodes[i-1].x -  iCameraPosX;
		int y = ter->mNodes[i-1].y;
		
		int x2 = ter->mNodes[i].x -  iCameraPosX;
		int y2 = ter->mNodes[i].y;
				
	
		GrLine(chopBuffer, chopper_gc, x,y,x2,y2);
		GrLine(chopBuffer, chopper_gc, x,y - 1,x2,y2 - 1);
		GrLine(chopBuffer, chopper_gc, x,y + 1,x2,y2 + 1);
				
		//Draw a number above it to aid with debugging
		/*
		char num[2];
		sprintf(num,"%d",i);
		GrText(chopBuffer, gc, x2, y2, num, -1, GR_TFASCII);*/
		
		i++;
		
	}

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

int chopUpdateTerrainRecycling(CTerrain *ter)
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
			
			v = 10;
			if(iLevelMode == LEVEL_MODE_STEEP)
				v*=5;
				
			chopAddTerrainNode(ter,iNewNodePos,g - iR(-v,v));
			
			ret=1;
			
			
		}

		i++;
		
	}

	return ret;
}

int chopTerrainHeightAtPoint(CTerrain *ter,int pX)
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


int chopPointInTerrain(CTerrain *ter,int pX,int pY,int iTestType)
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
	
	while(i < NUMBER_OF_BLOCKS)
	{
		if(!mBlocks[i].bIsActive)
		{
			
			int iX,iY,sX,sY;
			
			iX = iLastBlockPlacedPosX + 300;
			sX = 5;
			
			iY = iR(0,iScreenY);
			sY = 25;//iR(0,50);
			
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
	
	int x = mBlock->iWorldX;
	int y = mBlock->iWorldY;
	
	int x2 = x + mBlock->iSizeX;
	int y2 = y + mBlock->iSizeY;
	
	if(chopPointInBox(px + 20,py,x,y,x2,y2))return 1;
	if(chopPointInBox(px + 20,py+10,x,y,x2,y2))return 1;
	
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
	
	iPlayerAlive = 0;

}

static void chopDrawPlayer(int x,int y) /*These are SCREEN coords, not world!*/
{
	if(iPlayerAlive == 0)return;
	
	  //chopper_gc = GrNewGC();       /* Get the graphics context */
	  
	  /*Homer: mmm...magic numbers*/
	GrSetGCForeground(chopper_gc, BLACK);
	GrFillRect(chopBuffer, chopper_gc, 	x+6,		y+2,		12,		9	); 
	GrFillRect(chopBuffer, chopper_gc, 	x - 3,		y+6,		23,		3	); 
	GrFillRect(chopBuffer, chopper_gc, 	x + 10,		y,			2,		3	); 
	GrFillRect(chopBuffer, chopper_gc, 	x -2,		y+5	,		2,		5	); 
	GrFillRect(chopBuffer, chopper_gc, 	x +18,		y+5	,		2,		5	); 
	
	GrLine(chopBuffer, chopper_gc, x,y+iRotorOffset, x+20,y-iRotorOffset); //rotor
	
	
}

static void chopDrawBlock(CBlock *mBlock)
{ 
	//gc = GrNewGC();   
	
	int iPosX = (mBlock->iWorldX - iCameraPosX);
	int iPosY = (mBlock->iWorldY);

	GrSetGCForeground(chopper_gc, BLACK);
	GrFillRect(chopBuffer, chopper_gc, iPosX,iPosY,mBlock->iSizeX,	mBlock->iSizeY	); 
}

static void chopDrawParticle(CParticle *mParticle)
{
	int iPosX = (mParticle->iWorldX - iCameraPosX);
	int iPosY = (mParticle->iWorldY);

	GrSetGCForeground(chopper_gc, GRAY);
	GrFillRect(chopBuffer, chopper_gc, iPosX,iPosY,3,3); 

}


static void chopDrawTheWorld()
{
	int i=0;	
	
	while(i < NUMBER_OF_BLOCKS)
	{
		if(mBlocks[i].bIsActive)
		{
			if(chopBlockOffscreen(&mBlocks[i]) == 1)
				mBlocks[i].bIsActive = 0;
			else
				chopDrawBlock(&mBlocks[i]);
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
				chopDrawParticle(&mParticles[i]);
		}
		
		i++;
	}
	
	chopRenderTerrain(&mGround);
	chopRenderTerrain(&mRoof);

}

static void chopDrawScene()
{
	char           s[30];
  
	GrClearWindow (chopBuffer, GR_FALSE);
	
	sprintf (s, "Chopper - %d", chopCounter);
		
	pz_draw_header (s);
	
	GrSetGCForeground(chopper_gc, WHITE);
	GrFillRect(chopBuffer, chopper_gc, 0,0,screen_info.cols,screen_info.rows	); 
	
	chopDrawPlayer(iPlayerPosX - iCameraPosX,iPlayerPosY);
	
	/*
	
	If I was less lazy I could make a nifty shadow effect with this stuff...
	
	int h = chopTerrainHeightAtPoint(&mGround, iPlayerPosX);
	GrSetGCForeground(gc, BLACK);
	GrFillRect(chopBuffer,gc,iPlayerPosX - iCameraPosX,h - 1,2,2);
	
	*/
	
	chopDrawTheWorld();
	
	
	//Flip the buffer
	GrCopyArea(chopper_wid, chopper_gc, 0, 0,
	screen_info.cols, (screen_info.rows - (HEADER_TOPLINE + 1)),
	chopBuffer, 0, 0, MWROP_SRCCOPY);
}

static void chopGameLoop()
{
   int i=0;
	
	
	iRotorOffset = iR(-1,1);
	
	/* Redraw the main window: */
	chopDrawScene(); 
	
	if(bWaitingToStart == 1)return;
	
	//We need to have this here so particles move when we're dead
	while(i < NUMBER_OF_PARTICLES)
	{
		if(mParticles[i].bIsActive == 1)
		{
			mParticles[i].iWorldX += mParticles[i].iSpeedX;
			mParticles[i].iWorldY += mParticles[i].iSpeedY;
		}
		
		i++;
	}
	
	i=0;
	
	
	if(iPlayerAlive == 0)return;
	
	
	//increase score
	chopCounter++;
	
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
			iPlayerSpeedY-=1;
	}
	else
		iPlayerSpeedY++;

	
	//increase speed as we go along
	if(chopCounter % 300 == 0)
		iPlayerSpeedX++;
		
	
	if(iPlayerPosY > iScreenY-10 || iPlayerPosY < -5 ||  
	chopPointInTerrain(&mGround,iPlayerPosX,iPlayerPosY + 10,0) || chopPointInTerrain(&mRoof,iPlayerPosX,iPlayerPosY,1)
	)
	{
		chopKillPlayer();
	}
	
	
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
	
	if(chopUpdateTerrainRecycling(&mGround)==1)
		chopCopyTerrain(&mGround,&mRoof,0,-( iScreenY * 0.75)); //mirror the sky if we've changed the ground
}

static int chopHandleEvent (GR_EVENT *event)
{
    switch (event->type)
    {
    case GR_EVENT_TYPE_TIMER:
        chopGameLoop();
        break;
	
	case GR_EVENT_TYPE_KEY_UP:
	if(event->keystroke.ch == '\r')
		bPlayerPressingUp = 0;
		break;

    case GR_EVENT_TYPE_KEY_DOWN:
    	if(bWaitingToStart == 1)
	{
	
	iPlayerSpeedY = 5;
    	bWaitingToStart = 0;
	
	}
        switch (event->keystroke.ch)
        {
        case '\r': /* Wheel button */
            bPlayerPressingUp = 1;
	    break;
         case 'm': /* Menu button */
            pz_close_window (chopper_wid);
            GrDestroyTimer (chopTimer);
            GrDestroyGC(chopper_gc);
            break;
        default:
            break;
        }
        break;   /* key down */
    }
    return 1;
}

void new_chopper_window(void)
{

	int i,g;
	i=0;
	chopper_gc = pz_get_gc(1);       /* Get the graphics context */
	GrGetScreenInfo(&screen_info); /* Get screen info */
	
	/* Open the window: */
	chopper_wid = pz_new_window (0,
					21,
					screen_info.cols,
					screen_info.rows - 21, /* Height of screen - header  */
					chopDrawScene,
					chopHandleEvent);
					
	chopBuffer = GrNewPixmap(screen_info.cols,
							(screen_info.rows - (HEADER_TOPLINE + 1)),
							NULL);
	
					
	iScreenX = screen_info.cols;
	iScreenY = screen_info.rows - 21;
	
	/* Select the types of events you need for your window: */
	GrSelectEvents (chopper_wid,
			GR_EVENT_MASK_KEY_DOWN
			| GR_EVENT_MASK_TIMER | GR_EVENT_MASK_KEY_UP);
	
	/* Display the window: */
	GrMapWindow (chopper_wid);
	
	/* Create the timer used for animating your application: */
	chopTimer = GrCreateTimer (chopper_wid,
					75); /* Timer interval, millisecs */
					
	/*chopDrawScene();*/
	
	iRotorOffset = 0;
	//chopAddBlock(100,100,20,20);
	
		
	iPlayerPosX = 60;
	iPlayerPosY = iScreenY * 0.2;
	
	iLastBlockPlacedPosX = 0;
	iGravityTimerCountdown = 2;
	
	iPlayerAlive = 1;
	
	chopCounter = 0;
	
	
	
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
	
	if(chopUpdateTerrainRecycling(&mGround)==1)
		chopCopyTerrain(&mGround,&mRoof,0,-( iScreenY * 0.75)); //mirror the sky if we've changed the ground
	
	iLevelMode = iR(1,2);
	
	iPlayerSpeedX = 4;
	iPlayerSpeedY = 0;

	
	if(iLevelMode == LEVEL_MODE_NORMAL)
		iPlayerSpeedX*=2; //make it a bit more exciting, cause it's easy terrain...
	//printf("level mode: %d",iLevelMode);
	
	bWaitingToStart = 1;
}