/* frog blast the vent core
 * GPL... (C) 2005 coob
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "pz.h"
#include "ipod.h"
#include "matrixfont.h"

static GR_WINDOW_ID		credits_wid;
static GR_WINDOW_INFO	credits_info;
static GR_GC_ID			credits_gc;
static GR_TIMER_ID		credits_timer;

static char *stream = NULL; 
static char *cnames[]={"Floydzilla", "by Dan Long", "DansFloyd.com",
		"Modded by Keripo", "for ZeroSlackr",
		"IPL DEVELOPERS", 
		"Bernard Leach", "Matthew J. Sahagian",
		"Courtney Cavin", "matz-josh", "Matthis Rouch",
       	"ansi", "Jens Taprogge", "Fredrik Bergholtz",
		"Jeffrey Nelson", "Scott Lawrence",
		"Cameron Nishiyama", "Prashant V", "Alastair S", "\0"};
static int txt_stream_start, txt_stream_end, txt_status, txt_len;
static int cols, curname = 0;
static int cnt = 0, cnt2 = 0, blipstep = 0;

#define PHOTO (screen_info.bpp == 16) 

#define NOWHERE  0
#define ALLIN    2
#define CENTRING 3
#define CENTRE   4
#define FADE	 6
#define NEXT     8

static void credits_free_var(void)
{
	if (stream != NULL) {
		free(stream);
		stream = NULL;
	}
	
	curname = 0; cnt = 0; cnt2 = 0;
}

static void credits_exit(void)
{
	credits_free_var();
	GrDestroyGC(credits_gc);
	GrDestroyTimer(credits_timer);
	pz_close_window(credits_wid);
}

static void credits_die(char *msg)
{
	pz_error(msg);
	credits_exit();
}

static void *credits_malloc(size_t howmuch)
{
	void *r;

	if (!(r = malloc(howmuch)))
		credits_die("This iPod will self-destruct in 5 seconds...");

	return r;
}

static void credits_var_init(void)
{
	int i;
	
	GrGetWindowInfo(credits_wid, &credits_info);
	GrClearWindow(credits_wid, GR_FALSE);

	cols = (credits_info.width/COL_W)+1;
	stream = credits_malloc(cols);
	for (i = 0; i < cols; i++) {
		stream[i] = ' '; 
	}
}

static void credits_init(void)
{
	static int seedinit = 0;
	
	if (!seedinit)
	{
		int fd, data;

		fd = open("/dev/random", O_RDONLY);
		if (fd == -1) {
			data = (int)time(NULL);
		}
		else {
			read(fd, &data, sizeof(data));
			close(fd);
		}
		srand(data);
		seedinit = 1;
	}
	
	credits_free_var(); 
	credits_var_init();
}

static void credits_blit_char(const int col, int cha, GR_COLOR colour,
                              const int fillbg)
{
	static int centre;
		
	if (!centre) {
		centre = (credits_info.height / 2) - (COL_H / 2);
	}
	
	if (fillbg) {
		GrSetGCUseBackground(credits_gc, GR_TRUE);
		GrSetGCBackground(credits_gc, BLACK);
	} else {
		GrSetGCUseBackground(credits_gc, GR_FALSE);
	}
	
	/* TRANSMUTATEULATE THE HOTDOG */
	if (cha == ' ') {
		cha = 0;
	} else if (cha >= '0' && cha <= '9') {
		cha = cha - 47;
	} else if (cha >= 'A' && cha <= 'Z') {
		cha = cha - 54;
	} else if (cha >= 'a' && cha <= 'z') { // lowercase -> upper
		cha = cha - 86;
	} else if (cha >= 1 && cha <= 26) { // c0d3
		cha = cha + 36;
	} else {
		cha = 34; // 'X'
	}

    GrSetGCForeground(credits_gc, colour);
	GrBitmap (credits_wid, credits_gc, COL_W*col, centre, COL_W, COL_H,
	          matrix_code_font[cha]);
}

static void credits_newtext(void)
{
	txt_status = NOWHERE;
	txt_len = strlen(cnames[curname]);
	txt_stream_start = ((cols - 1)/ 2) - (txt_len / 2) - 1;
	txt_stream_end = txt_stream_start + txt_len;
}

static void credits_clear_screen(const int exlcude_stream)
{
	GrSetGCForeground(credits_gc, BLACK);
	if (exlcude_stream) {
		GrFillRect(credits_wid, credits_gc, 0, 0, credits_info.width,
		           (credits_info.height / 2) - (COL_H / 2));
		GrFillRect(credits_wid, credits_gc, 0,
		           (credits_info.height / 2) + (COL_H / 2), credits_info.width,
		           (credits_info.height / 2) - (COL_H / 2));
	} else {
		GrFillRect(credits_wid, credits_gc, 0, 0,
		           credits_info.width, credits_info.height);
	}
}


static void credits_stream_push(void)
{
	int i, draw;
	GR_COLOR colour;
	
	for (i = cols - 1; i > 0; i--) {
		if (txt_status >= CENTRE
		    && i == txt_stream_end + 1) {
			stream[i] = stream[i - txt_len - 1];
			i -= txt_len;
		} else {
			stream[i] = stream[i - 1];
		}
		
		if (txt_status == ALLIN && i == txt_stream_end) {
			txt_status = CENTRING;
		}
		if (txt_status == CENTRING && i == txt_stream_start) {
			if (cnt2 == txt_stream_start)
				txt_status = CENTRE;
			else
				cnt2++;
		}
	} 
	
	if ((cols - cnt) == txt_stream_end && txt_status == NOWHERE) {
		txt_stream_end--; 
		stream[0] = cnames[curname][txt_stream_end - txt_stream_start];
		if (txt_stream_end == txt_stream_start) {
			txt_status = ALLIN;
			txt_stream_end = txt_stream_start + txt_len;
		}
	} else if (txt_status == FADE) {
		stream[0] = ' ';
	} else {
		stream[0] = rand() % 26 + 1;
	}

	for (i = 0; i < cols; i++) {
		draw = 1;
		if ((i > cnt2 && i <= cnt2 + txt_len && txt_status >= CENTRE) ||
		    (i >= cnt2 && i < cnt2 + txt_len && txt_status == CENTRING)) {
			colour = WHITE;
			draw = 0;
		} else if (i <= txt_stream_end) {
		    colour = PHOTO ? GR_RGB(0, (int) rand() % 35 + 220, 0) : LTGRAY;
		} else {
			colour = PHOTO ? GR_RGB(0, (int) rand() % 100 + 60, 0) : GRAY;
		}
		//if (txt_status > CENTRE && txt_status < NEXT && !draw) {
		//if ((txt_status == CENTRING || txt_status == CENTRE) && !draw) {
		//	credits_blit_char(i, stream[i], colour, 0);
		//} else {
			credits_blit_char(i, stream[i], colour, 1);
		//}
	}

/*	if (txt_status >= ALLIN && blipstep < txt_len / 2 && blipstep != -1) {
		blipstep++;
		GrSetGCForeground(credits_gc, LTGRAY);
		GrFillEllipse(credits_wid, credits_gc, (cols / 2) * COL_W,
		              credits_info.height / 2, blipstep * COL_W, COL_H);
	} else if (txt_status == CENTRE && blipstep == txt_len / 2) {
		credits_clear_screen(1);
		blipstep = -1;
	}*/
	
	if (cnt == cols) {
		cnt = 0;
		if (txt_status >= CENTRE) {
			txt_status++;
			if (txt_status == NEXT) {
				cnames[curname + 1] == "\0" ? curname = 0 : curname++;
				cnt = 0; blipstep = 0; cnt2 = 0;
				credits_newtext();
			}
		}
	} else { 
		cnt++;
	}
}

static void credits_do_draw(void)
{
	pz_draw_header("Credits");
}

static int credits_handle_event(GR_EVENT * event)
{
	int ret = 0;
	switch( event->type )
	{
	case( GR_EVENT_TYPE_TIMER ):
	credits_stream_push();
	break;

	case( GR_EVENT_TYPE_KEY_DOWN ):
	switch( event->keystroke.ch )
	{
		case 'h': /* hold */
			break;
		case '\r':
		case '\n': /* action */
			break;
		case 'p': /* play/pause */
		case 'd': /*or this */
			break;
		case 'l': /* CCW spin */
			break;
		case 'r': /* CW spin */
			break;
		case 'm':
			credits_exit();
			ret=1;
			break;
	}
	break;
	case( GR_EVENT_TYPE_KEY_UP ):
	switch( event->keystroke.ch )
	{
		case 'h': /* hold */
			break;
	}
	break;
	}

	return ret;
}

void new_credits_window( void )
{
	credits_gc = pz_get_gc(1);


	GrSetGCUseBackground(credits_gc, GR_FALSE);
	GrSetGCBackground(credits_gc, BLACK);

	credits_wid = pz_new_window(0, 0/*HEADER_TOPLINE + 1*/, screen_info.cols,
	                           screen_info.rows /*- (HEADER_TOPLINE + 1)*/, 
	                           credits_do_draw, credits_handle_event);

	GrSelectEvents( credits_wid, GR_EVENT_MASK_TIMER|
	GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_UP|GR_EVENT_MASK_KEY_DOWN);

	credits_init();
	credits_newtext();

    credits_timer = GrCreateTimer(credits_wid, 20);

	GrMapWindow(credits_wid);

	credits_clear_screen(0);
}

