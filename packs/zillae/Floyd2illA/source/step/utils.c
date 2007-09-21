// KERIPO MOD
#include "../_mods.h"

#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include "tstep.h"
extern tstep_global *tstep_g;
void newSong()
{
	tstep_g->nSongFreeze=0;
	tstep_g->currFreezeI=0;
	tstep_g->currBPMI=0;
	tstep_g->gap=0;
	tstep_g->nBeatChange=0;
	tstep_g->nFreezeArrows=0;
	tstep_g->nSteps=0;
	tstep_g->currBeat=0;
	tstep_g->currBPM=0;
	tstep_g->msCount=0;
	tstep_g->msTarget=0;
	tstep_g->sfreeze=0;
}

void freesong()
{
	if(tstep_g!=NULL)
	{
		if(tstep_g->songFreezes!=NULL)
			free(tstep_g->songFreezes);
		if(tstep_g->beatChanges!=NULL)
			free(tstep_g->beatChanges);
		if(tstep_g->steps!=NULL)
			free(tstep_g->steps);
		if(tstep_g->stepFreezes!=NULL)
			free(tstep_g->stepFreezes);
		free(tstep_g);
	}
}


int getTimeDiff()
{
	static struct timeval cur_st;
	static long int oldTime=0,currTime=0;
	oldTime=currTime;
	gettimeofday(&cur_st,NULL);
	currTime = cur_st.tv_sec*1000+cur_st.tv_usec/1000;
	return (int)(currTime-oldTime);
}


void loadArrow(GR_BITMAP* a, int orientation)
{
  unsigned char arrow[24][32] = 
 {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
  {0,0,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
  char b[32][32];
  int i,j;
  for(i=0;i<32;i++)
	for(j=0;j<32;j++)
		b[i][j]=0;
  for(i=0;i<24;i++)
	for(j=0;j<24;j++)
	{
		switch(orientation)
		{
			case bLEFT_ARROW:
				b[i][j]=arrow[j][i];
			break;
			case bRIGHT_ARROW:
				b[i][j]=arrow[24-1-j][i];
			break;
			case bDOWN_ARROW:
				b[i][j]=arrow[24-1-i][j];
			break;
			case bUP_ARROW:
				b[i][j]=arrow[i][j];
			break;
		}
	}
	for(i=0;i<24;i++)
	{
		a[i*2]=MASK(b[i][0],b[i][1],b[i][2],b[i][3],b[i][4],b[i][5],b[i][6],b[i][7],
			b[i][8],b[i][9],b[i][10],b[i][11],b[i][12],b[i][13],b[i][14],b[i][15]);
		a[i*2+1]=MASK(b[i][16],b[i][17],b[i][18],b[i][19],b[i][20],b[i][21],b[i][22],b[i][23],
			b[i][24],b[i][25],b[i][26],b[i][27],b[i][28],b[i][29],b[i][30],b[i][31]);
	}
	a[48]=0x0000;
	a[49]=0x0000;
	a[50]=0xFFFF;
	a[51]=0xFFFF;
	a[52]=0xFFFF;
	a[53]=0xFFFF;
	a[54]=0x0000;
	a[55]=0x0000;

/*  arrows[0] = MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);arrows[1]= MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[2] = MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);arrows[3]= MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[4] = MASK(0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0);arrows[5]= MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[6] = MASK(0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0);arrows[7]= MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[8] = MASK(0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0);arrows[9]= MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[10]= MASK(0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1);arrows[11]=MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[12]= MASK(0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1);arrows[13]=MASK(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[14]= MASK(0,0,0,0,0,0,1,1,1,1,1,0,0,1,1,1);arrows[15]=MASK(1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[16]= MASK(0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1);arrows[17]=MASK(1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[18]= MASK(0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1);arrows[19]=MASK(1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[20]= MASK(0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0);arrows[21]=MASK(1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0);
  arrows[22]= MASK(0,0,1,1,1,1,1,1,1,1,1,0,0,1,1,1);arrows[23]=MASK(1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0);
  arrows[24]= MASK(0,0,1,1,1,1,1,1,1,1,1,0,0,1,1,1);arrows[25]=MASK(1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0);
  arrows[26]= MASK(0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1);arrows[27]=MASK(1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0);
  arrows[28]= MASK(0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1);arrows[29]=MASK(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[30]= MASK(0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1);arrows[31]=MASK(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[32]= MASK(0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1);arrows[33]=MASK(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[34]= MASK(0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1);arrows[35]=MASK(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[36]= MASK(0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1);arrows[37]=MASK(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[38]= MASK(0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1);arrows[39]=MASK(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[40]= MASK(0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1);arrows[41]=MASK(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[42]= MASK(0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1);arrows[43]=MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[44]= MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);arrows[45]=MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  arrows[46]= MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);arrows[47]=MASK(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);*/
}
void registerHit(unsigned char step)
{
	int i;
	int b;
	//extremely simple and wildly inaccurate, just to try it out
	for(i=0;i<tstep_g->nSteps;i++)
	{
		b=tstep_g->steps[i].beat*4/pNum;
		if(b==tstep_g->currBeat||b+1==tstep_g->currBeat)
		{
			//we will count this as a hit just for giggles..
			tstep_g->steps[i].stepByte &= (step<<4)^0xFF;
			tstep_g->steps[i].score=1;
		}
	}
}
void tstep_init(void)
{

	int i=0;
	tstep_g = (tstep_global *)malloc(sizeof(tstep_global));
	newSong(); //initialize variables to zero
	// KERIPO MOD
	//loadSong( "/hp/ddr/metallo.dwi" , NULL );
	loadSong( DWI_SAMPLE , NULL );
		printf("Title: %s\n",tstep_g->songTitle);
		printf("Artist: %s\n",tstep_g->artist);
		printf("BPM: %d\n",tstep_g->startBPM);
		printf("Gap: %d\n",tstep_g->gap);
		printf("Freezes:\n");
		for(i=0;i<tstep_g->nSongFreeze;i++)
			printf("Beat: %d\tMs: %d\n",tstep_g->songFreezes[i].beat,tstep_g->songFreezes[i].lengthms);
		printf("BPM Changes:\n");
		for(i=0;i<tstep_g->nBeatChange;i++)
			printf("Beat: %d\tNew BPM: %d\n",tstep_g->beatChanges[i].beat,tstep_g->beatChanges[i].newBPM);
		printf("Size of long long: %d\n", (int) sizeof (long long));
	tstep_g->upArrowBM=malloc(sizeof(GR_BITMAP)*56);
	tstep_g->downArrowBM=malloc(sizeof(GR_BITMAP)*56);
	tstep_g->leftArrowBM=malloc(sizeof(GR_BITMAP)*56);
	tstep_g->rightArrowBM=malloc(sizeof(GR_BITMAP)*56);
  loadArrow(tstep_g->upArrowBM,bUP_ARROW);
  loadArrow(tstep_g->leftArrowBM,bLEFT_ARROW);
  loadArrow(tstep_g->rightArrowBM,bRIGHT_ARROW);
  loadArrow(tstep_g->downArrowBM,bDOWN_ARROW);
  tstep_g->msCount=-tstep_g->gap*256;
  tstep_g->currBPM=tstep_g->startBPM;
  tstep_g->msTarget=(60000/4)*pNum*pNum/tstep_g->currBPM; //may want to optimize
  getTimeDiff(); //initialize
}