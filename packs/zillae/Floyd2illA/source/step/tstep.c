#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "tstep.h"
#include "../pz.h"
#include "../ipod.h"

static GR_WINDOW_ID tstep_wid;
static GR_WINDOW_ID tstep_temp_wid;
static GR_TIMER_ID tstep_timer_id;
static int tstep_counter;
//static int tstep_gc;
tstep_global *tstep_g;

static void tstep_do_draw()
{
//  pz_draw_header("tstep");
}

static void tstep_DrawScene()
{
	GR_GC_ID	gc;
	int i, offset, y;
	GR_SCREEN_INFO si;
	GrGetScreenInfo(&si);
	gc = GrNewGC();
//	printf("hello worlds\n");
	GrSetGCForeground(gc, WHITE);
	GrFillRect(tstep_temp_wid,gc,0,0, 24*4, si.rows);	
//	GrClearWindow(tstep_temp_wid, GR_FALSE);
//	GrClearWindow(tstep_wid, GR_FALSE);
	GrSetGCForeground(gc, BLACK);
	GrSetGCUseBackground(gc, GR_FALSE);
	
	GrBitmap(tstep_temp_wid,
		gc,	0,4,24,24,tstep_g->leftArrowBM);

	GrBitmap(tstep_temp_wid,
		gc,	24,4,24,24,tstep_g->downArrowBM);

	GrBitmap(tstep_temp_wid,
		gc, 48,4,24,24,tstep_g->upArrowBM);

	GrBitmap(tstep_temp_wid,
		gc, 72,4,24,24,tstep_g->rightArrowBM);

	offset=-4+(tstep_g->msCount*24/4/tstep_g->msTarget)+(tstep_g->currBeat*24)/(4);
	for(i=0;i<tstep_g->nSteps;i++)
	{
		y=(tstep_g->steps[i].beat*24)/(pNum)-offset;
		if(tstep_g->steps[i].stepByte>>4){
		if(y<128)
		{
			if(y>-24)
			{
				if(tstep_g->steps[i].stepByte&bLEFT_ARROW)
					GrBitmap(tstep_temp_wid, gc,
					0,y,24,24,tstep_g->leftArrowBM);

				if(tstep_g->steps[i].stepByte&bDOWN_ARROW)
					GrBitmap(tstep_temp_wid, gc,
					24,y,24,24,tstep_g->downArrowBM);

				if(tstep_g->steps[i].stepByte&bUP_ARROW)
					GrBitmap(tstep_temp_wid, gc,
					48,y,24,24,tstep_g->upArrowBM);

				if(tstep_g->steps[i].stepByte&bRIGHT_ARROW)
					GrBitmap(tstep_temp_wid, gc,
					72,y,24,24,tstep_g->rightArrowBM);
			}
		}
		else
			break;}
	}
	GrCopyArea(tstep_wid,gc,si.cols/2-24*2,0,24*4,si.rows,tstep_temp_wid,0,0,MWROP_SRCCOPY);
}

static void tstep_game_Loop()
{

// currBeat=(msCount*currBPM*pNum)/60000; //current beat * 256
  int msDiff=getTimeDiff();
  if(tstep_g->sfreeze)
  {
 	tstep_g->sfreeze-=msDiff*pNum;
	if(tstep_g->sfreeze<=0)
	{
		tstep_g->msCount+=-tstep_g->sfreeze;
		tstep_g->sfreeze=0;
	}
  }
  else
	tstep_g->msCount+=msDiff*pNum;
  while(tstep_g->msCount>=tstep_g->msTarget)
  {
	tstep_g->msCount-=tstep_g->msTarget;
	tstep_g->currBeat++;
	if(tstep_g->currBPMI<tstep_g->nBeatChange&&tstep_g->beatChanges[tstep_g->currBPMI].beat==tstep_g->currBeat)
	{
		tstep_g->currBPM=tstep_g->beatChanges[tstep_g->currBPMI].newBPM;
		tstep_g->msTarget=(60000/4)*pNum*pNum/tstep_g->currBPM; //may want to optimize
		tstep_g->currBPMI++;
	}
	if(tstep_g->currFreezeI<tstep_g->nSongFreeze&&tstep_g->songFreezes[tstep_g->currFreezeI].beat==tstep_g->currBeat)
	{
		tstep_g->sfreeze=tstep_g->songFreezes[tstep_g->currFreezeI].lengthms*pNum;
		tstep_g->sfreeze-= -tstep_g->msCount;
		tstep_g->msCount=0;
		tstep_g->currFreezeI++;
	}
  }
//  printf("%f %f\n", (float)msTarget, (float)msCount);
//  if(msCount>=msTarget)
//	printf("Program running too slow!");  
	tstep_counter++;

	tstep_DrawScene();
}

static int tstep_handle_event (GR_EVENT *event)
{
	int ret = 0;
	switch(event->type)
	{
	case GR_EVENT_TYPE_TIMER:
		tstep_game_Loop();
		break;
	case GR_EVENT_TYPE_KEY_DOWN:
		switch(event->keystroke.ch)
		{
		case '\r':
			{
			GR_GC_ID gc=GrNewGC();
			pz_close_window(tstep_wid);
			GrDestroyTimer(tstep_timer_id);
			GrDestroyGC(gc);
			break;}
		case '5':
		case 'd':
			//play pause
			registerHit(bDOWN_ARROW);
			break;
		case '4':
		case 'w':
			registerHit(bLEFT_ARROW);
			break;
		case '6':
		case 'f':
			registerHit(bRIGHT_ARROW);
			break;
		case 'l':
			break;
		case 'r':
			break;
		case '8':
		case 'm':
			registerHit(bUP_ARROW);
			ret=1;
			break;
		default:
			break;
		}
		break;
	}
	return ret;
}

void new_tstep_window(void)
{
	GR_GC_ID gc;
	GR_SCREEN_INFO si;
	gc = GrNewGC();
	GrGetScreenInfo(&si);
	tstep_init();
/*tstep_wid = pz_new_window (0,
21,
si.cols,
si.rows-21,
tstep_do_draw,
tstep_handle_event);*/
	tstep_wid = pz_new_window (0,
	0,
	si.cols,
	si.rows,
	tstep_do_draw,
	tstep_handle_event);

GrSelectEvents(tstep_wid,
	GR_EVENT_MASK_KEY_UP | GR_EVENT_MASK_KEY_DOWN
	| GR_EVENT_MASK_TIMER);

GrMapWindow(tstep_wid);
GrClearWindow(tstep_wid, GR_FALSE);
tstep_timer_id = GrCreateTimer(tstep_wid, 20); //timer interval, milliseconds
tstep_temp_wid= GrNewPixmap(24*4,si.rows,NULL);
//findSongs();
//writeAlbumList(".steptest");

}
 