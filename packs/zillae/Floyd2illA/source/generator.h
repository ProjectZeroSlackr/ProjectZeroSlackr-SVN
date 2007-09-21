// KERIPO MOD
#include "_mods.h"

#define SINE_TABLE_VERSION	1	//data table version number (int)

// KERIPO MOD
// defined in "_mods.h"
//#define SINE_TABLE_DIR		"/usr/share/musictools/"
//#define SINE_TABLE_FILE		"sinetable.dat"
#define GENERATOR_NUM_NOTES	88

#define FREQUENCY_MAX	4000 // maximum freq
#define FREQUENCY_MIN 	20	// minimum freq
#define SAMPLING_RATE	44100 // this is the sampling rate of dsp write
#define MAX_SAMPLES     11000/8  // this is the max # of samples collected

#define HALF_WAVE		(SAMPLING_RATE>>1) 	// /2
#define QUARTER_WAVE		(SAMPLING_RATE>>2)	// /4
#define MAX_LOOKUP_SAMPLES	(SAMPLING_RATE>>2)	// Quadrant 1 only
#define ZERO_SAMPLE_VALUE 	8192	// 14 bits (0 to 16k-1) = fast_sine(0)

#define GENERATOR_MODE_FREQ		0
#define GENERATOR_MODE_VOLUME	        1	
#define GENERATOR_MODE_NOTES		2
#define GENERATOR_NUM_MODES		3
#define START_FREQ			440	//freq of Note A4 on the tempered scale


static int generator_quit_app ;
static int generator_paused;	//paused
static int generator_mode=0;  
static int generator_freq = START_FREQ;
static int generator_freq_delta = 1;
static int generator_notes_delta = 1;
 
void generate_tone()	;
void generator_destruct();
void generator_draw_text(char *wave );
// number drawing function declarations
void generator_zero( int x, int y );
void generator_one( int x, int y );
void generator_two( int x, int y );
void generator_three( int x, int y );
void generator_four( int x, int y );
void generator_five( int x, int y );
void generator_six( int x, int y );
void generator_seven( int x, int y );
void generator_eight( int x, int y );
void generator_nine( int x, int y );
void generator_dash( int x, int y );


static int generator_cur_note = GENERATOR_NUM_NOTES/2;
static int generator_notes[] = {
33,
35,
37,
39,
41,
44,
46,
49,
52,
55,
58,
62,
65,
69,
73,
78,
82,
87,
93,
98,
104,
110,
117,
123,
131,
139,
147,
156,
165,
175,
185,
196,
208,
220,
233,
247,
262,
277,
294,
311,
330,
349,
370,
392,
415,
440,
466,
494,
523,
554,
587,
622,
659,
698,
740,
784,
831,
880,
932,
988,
1047,
1109,
1175,
1245,
1319,
1397,
1480,
1568,
1661,
1760,
1865,
1976,
2093,
2217,
2349,
2489,
2637,
2794,
2960,
3136,
3322,
3520,
3729,
3951,
4186,
4435,
4699,
4978,
};
static char *generator_note_names[] = {"C1",
"C#1/Db1",
"D1",
"D#1/Eb1",
"E1",
"F1",
"F#1/Gb1",
"G1",
"G#1/Ab1",
"A1",
"A#1/Bb1",
"B1",
"C2",
"C#2/Db2",
"D2",
"D#2/Eb2",
"E2",
"F2",
"F#2/Gb2",
"G2",
"G#2/Ab2",
"A2",
"A#2/Bb2",
"B2",
"C3",
"C#3/Db3",
"D3",
"D#3/Eb3",
"E3",
"F3",
"F#3/Gb3",
"G3",
"G#3/Ab3",
"A3",
"A#3/Bb3",
"B3",
"C4",
"C#4/Db4",
"D4",
"D#4/Eb4",
"E4",
"F4",
"F#4/Gb4",
"G4",
"G#4/Ab4",
"A4",
"A#4/Bb4",
"B4",
"C5",
"C#5/Db5",
"D5",
"D#5/Eb5",
"E5",
"F5",
"F#5/Gb5",
"G5",
"G#5/Ab5",
"A5",
"A#5/Bb5",
"B5",
"C6",
"C#6/Db6",
"D6",
"D#6/Eb6",
"E6",
"F6",
"F#6/Gb6",
"G6",
"G#6/Ab6",
"A6",
"A#6/Bb6",
"B6",
"C7",
"C#7/Db7",
"D7",
"D#7/Eb7",
"E7",
"F7",
"F#7/Gb7",
"G7",
"G#7/Ab7",
"A7",
"A#7/Bb7",
"B7",
"C8",
"C#8/Db8",
"D8",
"D#8/Eb8" };
