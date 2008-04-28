/*
 * Copyright (C) 2006 Morten Proschowsky
 */

/* includes */
#include "../pz.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

/* dependent on appereance items */
extern ttk_color ti_ap_get(int);
extern TApItem * ti_ap_getx(int);

/* dependent on textinput module */
extern int ti_register(TWidget *(* cr)(), TWidget *(* ncr)(), char *, int);
extern TWidget * ti_create_tim_widget(int ht, int wd);

/* dependent on texteval module */
extern void texteval_add_to_input_log(char c);
extern void texteval_add_to_touch_log(int touch);

/* dependent on inl function */ 
extern int inl(int i);

/* definitions */
#define titup_TIMER_VALUE 20
#define CW 10 /* width of the space for each char */
#define CH 14 /* height of the space for each char */

/* charset definitions */
#define CHARS 32
#define CHARSETS 3
const int charset[CHARSETS][CHARS] = { {' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','!','?',',','.','\''}, {' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','!','?',',','.','\''}, {' ','1','2','3','4','5','6','7','8','9','0',')','(',']','[','}','{','+','-','%','*','#','/','\\','_',':',';','!','?',',','.','\''}};

#define MAXNUM 255
int numval[MAXNUM];

/* settings */

#define CONF_RELEASE 1
static const char* cfg_release[] =  { "On", "Off", NULL };


/* variables */


static int charlist_pos = 0;
static int ppchar = 0;
static int pchar = 0;
static double predict[CHARS+1][CHARS+1][CHARS+1];
static double var = 0.5;
static int lastp = 0; // position of last touch
static int lastt = 0; // time of last touch
static int lastc = 0; // character
static int lastct = 0; // character change time
static int ignore_remove = 0;
static int finger_on_wheel = 0;

static int titup_sin[CHARS];
static int titup_cos[CHARS];
static int rad = 0;
static int sel_cs = 0;

static double cp[CHARS]; // used for visualization
static double maxcp; // used for visualization


// char til num - unknwon is 32
// simple...
#define C2N(c) ((c<MAXNUM&&c>=0)?numval[c]:0)
#define N2C(n) ((n<CHARS&&n>=0)?charset[sel_cs][n]:0)
		
void titup_build_numlog() {
	int i, j, c;
	for (i=0; i<MAXNUM; i++) {
		numval[i] = 0;
		// str to lower
		c = (i>64 && i < 91)? i+32 : i;
		for (j=0; j<CHARS; j++) {
			if (charset[1][j] == c)
				numval[i] = j;
		}
	}
}
						    
void titup_predict_clear(void)
{
	int i,j,k;
	for (i=0; i<CHARS; i++)
		for (j=0; j<CHARS; j++)
			for (k=0; k<CHARS; k++)
				predict[i][j][k] = 0.00;
}

void titup_predict_load(const char * fn)
{
	FILE * fp;
	char buf[120];
	int c1, c2, c3;
	double cv;
	titup_predict_clear();
	fp = fopen(fn, "r");
	if (fp) {
		while (fgets(buf, 120, fp)) {
			if (strlen(buf)>=5 && !(buf[0]=='#' && buf[1]=='#')) {
				c1 = C2N(buf[0]);
				c2 = C2N(buf[1]);
				c3 = C2N(buf[2]);
				cv = atof(buf+4) / 1000000;
				predict[c1][c2][c3] = cv;
				//pz_error("Loaded (%s)  %d %d %d - %.3f\n", buf+4, c1, c2,c3, cv);

				//printf("Loaded (%s)  %d %d %d - %.3f\n", buf+4, c1, c2,c3, cv);
			}
		}
		fclose(fp);
	}
}


void titup_reset()
{
	charlist_pos = 0;
	ppchar = 0;
	pchar = 0;
}
		

void titup_draw(TWidget * wid, ttk_surface srf) {
	int i, px, py, tw;
	uc16 s[2] = {0,0};
	ttk_ap_fillrect(srf, ti_ap_getx(0), 0, srf->h-2*rad-CH, srf->w, srf->h);
		
	ttk_text(srf, ttk_textfont, srf->w/2 - rad - 72, srf->h-rad-CH-titup_sin[CHARS*3/4], ti_ap_get(1), "|<< Delete");

	ttk_text(srf, ttk_textfont, srf->w/2 + rad +15, srf->h-rad-CH-titup_sin[CHARS/4], ti_ap_get(1), "Charset >>|");
	ttk_text(srf, ttk_textfont, srf->w/2 + 50, srf->h-1*CH, ti_ap_get(1), "New line >||");


	for (i = 0; i<CHARS; i++)
	{
		px = titup_cos[i];
		py = titup_sin[i];
		s[0] = charset[sel_cs][i];
		tw = ttk_text_width_uc16(ttk_menufont, s)/2;
		if (i == charlist_pos)
			ttk_fillellipse(srf, srf->w/2-1+px, srf->h-1-rad-CH/2-py, 7, 7, ti_ap_getx(2)->color);
		ttk_text_uc16(srf, ttk_textfont, srf->w/2+px-tw, srf->h-rad-CH-py, ti_ap_get((i==charlist_pos)?3:1), s);

	}
		
}

int titup_dist(int p, int q) {
	int dist = abs(p-q);
	return (dist>48?96-dist:dist);
}

int titup_rotatechar(int a) {
        ppchar = pchar;
        pchar = a;
	return a;
}

int titup_input_highlighted_char() {
// roolback
	int chg_time = ttk_getticks() - lastct;
	if (chg_time < 100 && titup_dist(charlist_pos, lastc)>=2)
		return charlist_pos; // remove with out writing...

	if (chg_time < 100)
		charlist_pos = lastc;

	titup_rotatechar(charlist_pos);

	titup_input_char(charset[sel_cs][charlist_pos]);
	return charlist_pos;
}

void update_probabilities() {
	int i;
	float e = 0;
	maxcp = 0;
	for (i=0;i<CHARS;i++) {
		maxcp = (maxcp > predict[ppchar][pchar][i]?maxcp:predict[ppchar][pchar][i]);
		cp[i] = predict[ppchar][pchar][i];
		if (cp[i] > 0) // avoid NaN
			e = e - cp[i]*log(cp[i])/log(2);
	}
	var = e>0?1/e:0.5;
}

int titup_input_char(int c) {
	// send to ttk
	ttk_input_char(c);

	// update variables
	update_probabilities();
	return c;
}

int titup_down(TWidget * wid, int btn) {
	ignore_remove = 3;

	switch (btn) {
		case TTK_BUTTON_ACTION:
			titup_input_highlighted_char();
			break;
		case TTK_BUTTON_PREVIOUS:
			pchar = ppchar = 0;
			titup_input_char(TTK_INPUT_BKSP);
			break;
		case TTK_BUTTON_NEXT:
			sel_cs = (sel_cs + 1) % CHARSETS;
			break;
		case TTK_BUTTON_PLAY:
			titup_rotatechar(0);
			titup_input_char(TTK_INPUT_ENTER);
			break;
		case TTK_BUTTON_MENU:
			ttk_input_end();
			break;
		default:
			return TTK_EV_UNUSED;
			break;
	}
	return TTK_EV_CLICK;
}

int titup_touch(TWidget * wid, int p)
{
	if (p == lastp) return TTK_EV_UNUSED;
	
	int i, maxchar;
	double prob, maxprob;
	// speed in radians/second
	double speed = abs(65.4*titup_dist(p, lastp)/(ttk_getticks()-lastt));
	lastt = ttk_getticks();	
	var = 0.7 * var + 0.3 * (0.01 + 2/(1+exp(-0.2*speed))-1);
	if (sel_cs == 2) var = 0.001;
	lastp = p;	
	maxchar = 0;
	maxprob = 0;
	
	for (i=0; i<CHARS; i++) {
		prob = pow(0.1 + predict[ppchar][pchar][i], var) * 
			exp(-pow((double)titup_dist(p,3*i)/15.2, 2.0)/(2.0*var));

		if (prob>maxprob) {
			maxchar = i;
			maxprob = prob;
		}
	
		cp[i] = prob;
	}
	maxcp = maxprob;
		
	if (charlist_pos != maxchar) {
	    wid->dirty = 1;
	    lastc = charlist_pos;
	    lastct = ttk_getticks();
	    charlist_pos = maxchar;
		if (ignore_remove) ignore_remove--;
	}
	return TTK_EV_CLICK;
}

#ifdef IPOD
int titup_timer(TWidget * wid)
{
	int in = inl (0x7000C140);
	int st = (in & 0x40000000);
	int touch = (in & 0x007F0000) >> 16;
	if (st) {
		titup_touch(wid, touch);
		finger_on_wheel = 1;
	}
	else
	{
		if (1 && finger_on_wheel) {
			finger_on_wheel = 0;
			if  (ignore_remove)
			{
				ignore_remove = 0;
			}
			else
			{
				titup_input_highlighted_char();
			}
		}
	}
	return 0;
}
#endif

TWidget * titup_create()
{
	TWidget * wid = ti_create_tim_widget(2*rad+CW, 0); // 2*rad+CH);
	wid->down = titup_down;
	wid->draw = titup_draw;
	titup_reset();
#ifdef IPOD
	wid->timer = titup_timer;
	ttk_widget_set_timer(wid, titup_TIMER_VALUE);
#else
	wid->stap = titup_touch;
#endif

	return wid;
}


void titup_init_settings()
{
	//pz_menu_add_setting ("/Settings/TUP: Select on Release", CONF_RELEASE, titup_config, cfg_release);
}

void titup_cleanup()
{

}


void titup_init()
{
		titup_init_settings();
		ti_register(titup_create, titup_create, N_("Transparant User guided Prediction"), 25);
		titup_build_numlog();
		titup_predict_load("/opt/Zillae/ZacZilla/Data/predict.txt");
		int i;
		double r;
		ttk_get_screensize(&i, NULL, NULL);
		rad = sqrt(i)*3.6;
		for (i=0;i<CHARS;i++) {
			r = 3.14159/2.0 - 3.14159*2.0*((double) i)/CHARS;
			titup_sin[i] = (int) (sin(r)*rad);
			titup_cos[i] = (int) (cos(r)*rad);
		}
		update_probabilities();
}


