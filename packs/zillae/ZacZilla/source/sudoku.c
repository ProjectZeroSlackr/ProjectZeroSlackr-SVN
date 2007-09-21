#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "pz.h"
#include "matrixfont.h"

// preprocessor
#define sudoku_digit_mask 0x0000000F		// displayed digit mask
#define sudoku_solve_mask 0x000000F0		// solution digit mask
#define sudoku_allow_mask 0x0001FF00		// digits allowed in this cell mask
#define sudoku_locks_mask 0x00020000		// cell locked mask
#define sudoku_lock_flag  0x00020000		// cell locked
#define sudoku_dirty_flag 0x00040000		// if the cell needs redrawing

// global variables

static GR_WINDOW_ID sudoku_wid;
static GR_GC_ID sudoku_gc;

static GR_TIMER_ID sudoku_timer_id;

static int sudoku_counter;

int sudoku_board[81];									// sudoku_board on display

int sudoku_allow_bits[9] = {0x00100, 0x00200, 0x00400, 0x00800, 0x01000, 0x02000, 0x04000, 0x08000, 0x10000};

int sudoku_box_index[9][9] = {{0,1,2,9,10,11,18,19,20},
											 {3,4,5,12,13,14,21,22,23},
											 {6,7,8,15,16,17,24,25,26},
											 {27,28,29,36,37,38,45,46,47},
											 {30,31,32,39,40,41,48,49,50},
											 {33,34,35,42,43,44,51,52,53},
											 {54,55,56,63,64,65,72,73,74},
											 {57,58,59,66,67,68,75,76,77},
											 {60,61,62,69,70,71,78,79,80}};

int sudoku_row_index[9][9] = {{0,1,2,3,4,5,6,7,8},
											 {9,10,11,12,13,14,15,16,17},
											 {18,19,20,21,22,23,24,25,26},
											 {27,28,29,30,31,32,33,34,35},
											 {36,37,38,39,40,41,42,43,44},
											 {45,46,47,48,49,50,51,52,53},
											 {54,55,56,57,58,59,60,61,62},
											 {63,64,65,66,67,68,69,70,71},
											 {72,73,74,75,76,77,78,79,80}};

int sudoku_col_index[9][9] = {{0,9,18,27,36,45,54,63,72},
											 {1,10,19,28,37,46,55,64,73},
											 {2,11,20,29,38,47,56,65,74},
											 {3,12,21,30,39,48,57,66,75},
											 {4,13,22,31,40,49,58,67,76},
											 {5,14,23,32,41,50,59,68,77},
											 {6,15,24,33,42,51,60,69,78},
											 {7,16,25,34,43,52,61,70,79},
											 {8,17,26,35,44,53,62,71,80}};

int sudoku_solved;													// flag if solution found
int sudoku_maxlevel = 0;

int sudoku_cur_row = 4;
int sudoku_cur_col = 4;

int sudoku_setallowed(int sudoku_board[81], int cc, int num);

// get row index from cell number
int sudoku_getrow(int cell) {
	return (floor(cell / 9));
}

// get column from cell number
int sudoku_getcol(int cell) {
	return cell % 9;
}

// get box from cell number
int sudoku_getbox(cell) {
	int row=sudoku_getrow(cell);
	int col=sudoku_getcol(cell);

	return 3 * floor(row/3) + floor(col/3);
}

void sudoku_doclear(void) {
	int i;

	for (i = 0; i < 81; i++) {
		sudoku_board[i] = sudoku_allow_mask | sudoku_solve_mask | sudoku_digit_mask;
	}
}

// check the contents of all boxes
int sudoku_boxcheck(int sudoku_board[81])
{
	int i,j,d,sum,last,last2,last3;

	for (i=0;i<9;i++) {
		for (d=0;d<9;d++) {
			sum=0;
			last=-1;
			last2=-1;
			last3=-1;
			for (j=0;j<9;j++) {
				if (sudoku_board[sudoku_box_index[i][j]]&sudoku_allow_bits[d]) {
					sum++;
					last3=last2;
					last2=last;
					last=sudoku_box_index[i][j];
				}
				else
					sum += ((sudoku_board[sudoku_box_index[i][j]] & sudoku_solve_mask)==(d << 4)) ? 1: 0;
			}
			// if #possible is 0 then fail
			if (sum==0) return 0;
			// if #possible is 1 then fix it in solution
			if ((sum==1)&&(last>=0)) {
				if (!sudoku_setallowed(sudoku_board,last,d)) return 0;
			}
			// if #possible is 2 then if both on same row, can't be elsewhere on row
			if ((sum==2)&&(last>=0)&&(last2>=0)&&(sudoku_getrow(last)==sudoku_getrow(last2))) {
				for (j=0;j<9;j++) {
					int c=sudoku_row_index[sudoku_getrow(last)][j];
					if ((c!=last)&&(c!=last2)) {
						if (sudoku_board[c]&sudoku_allow_bits[d]) {
							sudoku_board[c] &= ~sudoku_allow_bits[d];
							if ((sudoku_board[c]&sudoku_allow_mask)==0) return 0;
						}
					}
				}
			}
			// if #possible is 2 then if both on same col, can't be elsewhere on col
			if ((sum==2)&&(last>=0)&&(last2>=0)&&(sudoku_getcol(last)==sudoku_getcol(last2))) {
				for (j=0;j<9;j++) {
					int c=sudoku_col_index[sudoku_getcol(last)][j];
					if ((c!=last)&&(c!=last2)) {
						if (sudoku_board[c]&sudoku_allow_bits[d]) {
							sudoku_board[c] &= ~sudoku_allow_bits[d];
							if ((sudoku_board[c]&sudoku_allow_mask)==0) return 0;
						}
					}
				}
			}
			// if #possible is 3 then if all on same row, can't be elsewhere on row
			if ((sum==3)&&(last>=0)&&(last2>=0)&&(last3>=0)&&(sudoku_getrow(last)==sudoku_getrow(last2))&&(sudoku_getrow(last)==sudoku_getrow(last3))) {
				for (j=0;j<9;j++) {
					int c=sudoku_row_index[sudoku_getrow(last)][j];
					if ((c!=last)&&(c!=last2)&&(c!=last3)) {
						if (sudoku_board[c]&sudoku_allow_bits[d]) {
							sudoku_board[c] &= ~sudoku_allow_bits[d];
							if ((sudoku_board[c]&sudoku_allow_mask)==0) return 0;
						}
					}
				}
			}
			// if #possible is 3 then if all on same col, can't be elsewhere on col
			if ((sum==3)&&(last>=0)&&(last2>=0)&&(last3>=0)&&(sudoku_getcol(last)==sudoku_getcol(last2))&&(sudoku_getcol(last)==sudoku_getcol(last3))) {
				for (j=0;j<9;j++) {
					int c=sudoku_col_index[sudoku_getcol(last)][j];
					if ((c!=last)&&(c!=last2)&&(c!=last3)) {
						if (sudoku_board[c]&sudoku_allow_bits[d]) {
							sudoku_board[c] &= ~sudoku_allow_bits[d];
							if ((sudoku_board[c]&sudoku_allow_mask)==0) return 0;
						}
					}
				}
			}
		}
	}
	return 1;
}

// check the contents of all rows
int sudoku_rowcheck(int sudoku_board[81])
{
	int i,j,d,sum,last;

	for (i=0;i<9;i++) {
		for (d=0;d<9;d++) {
			sum=0;
			last=-1;
			for (j=0;j<9;j++) {
				if (sudoku_board[sudoku_row_index[i][j]]&sudoku_allow_bits[d]) {
					sum++;
					last=j;
				}
				else
					sum += ((sudoku_board[sudoku_row_index[i][j]] & sudoku_solve_mask)==(d << 4)) ? 1: 0;
			}
			if (sum==0) return 0;
			if ((sum==1)&&(last>=0)) {
				if (!sudoku_setallowed(sudoku_board,sudoku_row_index[i][last],d)) return 0;
			}
		}
	}
	return 1;
}

// check the contents of all columns
int sudoku_colcheck(int sudoku_board[81])
{
	int i,j,d,sum,last;

	for (i=0;i<9;i++) {
		for (d=0;d<9;d++) {
			sum=0;
			last=-1;
			for (j=0;j<9;j++) {
				if (sudoku_board[sudoku_col_index[i][j]]&sudoku_allow_bits[d]) {
					sum++;
					last=j;
				}
				else
					sum += ((sudoku_board[sudoku_col_index[i][j]] & sudoku_solve_mask)==(d << 4)) ? 1: 0;
			}
			if (sum==0) return 0;
			if ((sum==1)&&(last>=0)) {
				if (!sudoku_setallowed(sudoku_board,sudoku_col_index[i][last],d)) return 0;
			}
		}
	}
	return 1;
}

int sudoku_setallowed(int sudoku_board[81], int cc, int num) {
	int i,j,d;
	int row = sudoku_getrow(cc);
	int col = sudoku_getcol(cc);
	int box=sudoku_getbox(cc);

	// clear alternatives in this cell
	sudoku_board[cc] &= ~sudoku_allow_mask;
	// and set value as solution
	sudoku_board[cc] = (sudoku_board[cc] & ~sudoku_solve_mask) | (num << 4);

	// clear this digit on same row
	for (j=0;j<9;j++) {
		if (sudoku_board[sudoku_row_index[row][j]]&sudoku_allow_bits[num]) {
			sudoku_board[sudoku_row_index[row][j]] &= ~sudoku_allow_bits[num];
			if ((sudoku_board[sudoku_row_index[row][j]]&sudoku_allow_mask)==0) return 0;
		}
	}

	// clear this digit on same column
	for (j=0;j<9;j++) {
		if (sudoku_board[sudoku_col_index[col][j]]&sudoku_allow_bits[num]) {
			sudoku_board[sudoku_col_index[col][j]] &= ~sudoku_allow_bits[num];
			if ((sudoku_board[sudoku_col_index[col][j]]&sudoku_allow_mask)==0) return(0);
		}
	}

	// clear this digit in same box
	for (j=0;j<9;j++) {
		if (sudoku_board[sudoku_box_index[box][j]]&sudoku_allow_bits[num]) {
			sudoku_board[sudoku_box_index[box][j]] &= ~sudoku_allow_bits[num];
			if ((sudoku_board[sudoku_box_index[box][j]]&sudoku_allow_mask)==0) return(0);
		}
	}

	// process all singletons created by setting
	for (i=0;i<81;i++)
		for (d=0;d<9;d++)
			if ((sudoku_board[i]&sudoku_allow_mask)==sudoku_allow_bits[d])
				if (!sudoku_setallowed(sudoku_board,i,d)) return 0;

	// check each digit still available in each box, row and column
	if (!sudoku_boxcheck(sudoku_board)||!sudoku_rowcheck(sudoku_board)||!sudoku_colcheck(sudoku_board))
		return(0);

	// process all singletons created by setting (double check)
	for (i=0;i<81;i++)
		for (d=0;d<9;d++)
			if ((sudoku_board[i]&sudoku_allow_mask)==sudoku_allow_bits[d])
				if (!sudoku_setallowed(sudoku_board,i,d)) return(0);

	return(1);
}

// check if sudoku_board is a solution
int sudoku_chk_solved(int sudoku_board[81])
{
	int i;
	int nchoices=0;

	// count # choices left
	for (i=0;i<81;i++)
		if ((sudoku_board[i]&sudoku_allow_mask)!=0)
				nchoices++;

	// sudoku_solved?
	if (nchoices==0) sudoku_solved=1;

	// OK
	return sudoku_solved;
}

// set a digit into a sudoku_board cell
void sudoku_setdigit(int cc, int num)
{
	sudoku_board[cc] = (sudoku_board[cc] & ~sudoku_digit_mask) | num;
}

// sudoku_attempt to solve from this situation
void sudoku_attempt(int psudoku_board[81], int level)
{
	int sudoku_board[81];
	int i,j,k;
	int s,e;

	// check for runaway
	if (level > sudoku_maxlevel) {
		sudoku_maxlevel=level;
		// alert("Level="+level);
	}
	if (level > 25) return;		// probably gone wrong

	// pick starting square at random
	i = s = rand() % 81;
	do {
	  if ((psudoku_board[i]&sudoku_allow_mask)!=0) {
		// pick starting digit at random
		j = e = rand() % 9 + 1;
		do {
		  if (psudoku_board[i]&sudoku_allow_bits[j]) {
			// push current solution
			for (k=0;k<81;k++) sudoku_board[k]=psudoku_board[k];
			// try out digit
			if (sudoku_setallowed(sudoku_board,i,j)) {
				sudoku_board[i] = (sudoku_board[i] & ~sudoku_digit_mask) | j;
				if (sudoku_chk_solved(sudoku_board)) {
					for (k=0;k<81;k++) psudoku_board[k]=sudoku_board[k];
					return;
				}
				// recurse
				sudoku_attempt(sudoku_board,level+1);
				if (sudoku_chk_solved(sudoku_board)) {
					for (k=0;k<81;k++) psudoku_board[k]=sudoku_board[k];
					return;
				}
				// no good
				sudoku_board[i] |= sudoku_digit_mask;
				if (level > 2) return;	// don't both searching anymore
			}
		  }
		  j=(j+1)%9;
		} while (j!=e);
	  }
	  i=(i+1)%81;
	} while (i!=s);
}

void sudoku_docreate(void) {
	int sudoku_maxlevel = 0;

	int i, d;

	do {
		// clear the sudoku_board
		sudoku_doclear();
		sudoku_solved = 0;
		sudoku_maxlevel = 0;

		// assign one of each digit at random
		for (d = 0; d < 9; d++) {
			i = rand() % 81;
			if (sudoku_board[i] & sudoku_allow_bits[d]) {
				sudoku_setallowed(sudoku_board, i, d);
				sudoku_board[i] = (sudoku_board[i] & ~sudoku_digit_mask) | d;
			}
		}

		// sudoku_attempt to solve from here
		sudoku_attempt(sudoku_board,0);

		// display result
		for (i=0;i<81;i++) {
			if ((0<=(sudoku_board[i]&sudoku_digit_mask))&&((sudoku_board[i]&sudoku_digit_mask) < 9)) sudoku_board[i] |= sudoku_lock_flag;
			sudoku_setdigit(i,sudoku_board[i]&sudoku_digit_mask);
		}
	} while (!sudoku_solved);
}

static void sudoku_draw_once(void) {
	int row, col, t_row, t_col, digit;
    //char s[30];

    /* Clear the window */
    GrClearWindow(sudoku_wid, GR_FALSE);

    /* Draw the counter */
    //sprintf(s, "Counter: %d", sudoku_counter);
    /*GrText(sudoku_wid,
           sudoku_gc,
           1, 103,  // x, y /
           s,
           -1,
           GR_TFASCII);

		GrText(sudoku_wid, sudoku_gc, 50, 50, s, -1, GR_TFASCII);*/

		for (row = 0; row < 9; row++) {
			for (col = 0; col< 9; col++) {
				t_row = COL_H * row + 4;
				t_col = 10 + (COL_W + 4) * col;

				if (col > 2) t_col+= 3;
				if (col > 5) t_col+= 3;

				if (row > 2) t_row+= 3;
				if (row > 5) t_row+= 3;							

				digit = ((sudoku_board[row * 9 + col] & sudoku_digit_mask) + 1) & 0xF;

				if (digit > 0) digit++;

				if (row == sudoku_cur_row && col == sudoku_cur_col) {
					GrSetGCBackground(sudoku_gc, GR_RGB(0,0,0));
					GrSetGCForeground(sudoku_gc, GR_RGB(255,255,255));
				}
				
				GrBitmap (sudoku_wid, sudoku_gc, t_col, t_row, COL_W, COL_H,
	          matrix_code_font[digit]);

				if (row == sudoku_cur_row && col == sudoku_cur_col) {
					GrSetGCBackground(sudoku_gc, GR_RGB(255,255,255));
					GrSetGCForeground(sudoku_gc, GR_RGB(0,0,0));
				}
			}
		}

		
		/* draw the grid */
		GrLine(sudoku_wid, sudoku_gc, 11, 36, 101, 36);
		GrLine(sudoku_wid, sudoku_gc, 11, 68, 101, 68);
		GrLine(sudoku_wid, sudoku_gc, 39, 5, 39, 99);
		GrLine(sudoku_wid, sudoku_gc, 72, 5, 72, 99);
}

static void sudoku_draw()
{
	int row, col, t_row, t_col, digit;

	for (row = 0; row < 9; row++) {
		for (col = 0; col < 9; col++) {
			if ((sudoku_board[row * 9 + col] & sudoku_dirty_flag) > 0) {
				t_row = COL_H * row + 4;
				t_col = 10 + (COL_W + 4) * col;

				if (col > 2) t_col+= 3;
				if (col > 5) t_col+= 3;

				if (row > 2) t_row+= 3;
				if (row > 5) t_row+= 3;							

				digit = ((sudoku_board[row * 9 + col] & sudoku_digit_mask) + 1) & 0xF;

				if (digit > 0) digit++;

				if (row == sudoku_cur_row && col == sudoku_cur_col) {
					GrSetGCBackground(sudoku_gc, GR_RGB(0,0,0));
					GrSetGCForeground(sudoku_gc, GR_RGB(255,255,255));
				}
				
				GrBitmap (sudoku_wid, sudoku_gc, t_col, t_row, COL_W, COL_H,
	          matrix_code_font[digit]);

				if (row == sudoku_cur_row && col == sudoku_cur_col) {
					GrSetGCBackground(sudoku_gc, GR_RGB(255,255,255));
					GrSetGCForeground(sudoku_gc, GR_RGB(0,0,0));
				}
			}

			sudoku_board[row * 9 + col] &= ~sudoku_dirty_flag;
		}
	}
}

static void sudoku_loop()
{
    /* Modify the state of your application: */
    sudoku_counter++;

    /* Redraw the main window: */
    sudoku_draw(); 
}

static int sudoku_handle_event(GR_EVENT *event)
{
    int ret = 0;
    static int paused = 0;

    switch (event->type) {
    case GR_EVENT_TYPE_TIMER:
        if (!paused)
            sudoku_loop();
        break;

    case GR_EVENT_TYPE_KEY_DOWN:
        switch (event->keystroke.ch) {
        case IPOD_WHEEL_COUNTERCLOCKWISE: /* Wheel left */
					sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] |= sudoku_dirty_flag;
					sudoku_cur_row--;
					if (sudoku_cur_row == -1) sudoku_cur_row = 8;
					sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] |= sudoku_dirty_flag;					
					break;
        case IPOD_WHEEL_CLOCKWISE: /* Wheel right */
					sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] |= sudoku_dirty_flag;
					sudoku_cur_row++;
					if (sudoku_cur_row == 9) sudoku_cur_row = 0;
					sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] |= sudoku_dirty_flag;
            //ret |= KEY_CLICK; /* click on this event */
            break;
        case IPOD_SWITCH_HOLD: /* hold switch on */
            paused = 1; /* pause the loop */
            break;
        case IPOD_BUTTON_MENU: /* Menu button */
            pz_close_window(sudoku_wid);
            GrDestroyTimer(sudoku_timer_id);
            GrDestroyGC(sudoku_gc);
            break;
        case IPOD_BUTTON_ACTION: /* Action button */
						if (!(sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] & sudoku_lock_flag)) {
							int cur_val = sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] & sudoku_digit_mask;
							int new_val = 0;

							if (cur_val == 15 || cur_val == 8) {
								new_val = 0;
							} else {
								new_val = cur_val + 1;
							}

							sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] &= (~sudoku_digit_mask);
							sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] |= new_val;
							sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] |= sudoku_dirty_flag;
						}
						break;
        case IPOD_BUTTON_PLAY: /* Play/pause button */
						break;
        case IPOD_BUTTON_REWIND: /* Rewind button */
					sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] |= sudoku_dirty_flag;
					sudoku_cur_col--;
					if (sudoku_cur_col == -1) sudoku_cur_col = 8;
					sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] |= sudoku_dirty_flag;
					break;
        case IPOD_BUTTON_FORWARD: /* Fast forward button */
					sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] |= sudoku_dirty_flag;
					sudoku_cur_col++;
					if (sudoku_cur_col == 9) sudoku_cur_col = 0;
					sudoku_board[sudoku_cur_row * 9 + sudoku_cur_col] |= sudoku_dirty_flag;
					break;
        default:
            ret |= KEY_UNUSED; /* allow a default key action */
            break;
        }
        break;
    case GR_EVENT_TYPE_KEY_UP:
        switch (event->keystroke.ch) {
        case IPOD_SWITCH_HOLD: /* un-held */
            paused = 0;
            break;
        default:
            ret |= KEY_UNUSED;
            break;
        }
    default:
        ret |= EVENT_UNUSED; /* we didn't ask for this event, go away */
    }
    return ret;
}


static void sudoku_do_draw()
{
    pz_draw_header("Sudoku");
    sudoku_draw(); /* the call to your draw function */
}

void new_sudoku_window(void) {
	srand(time(NULL));
	sudoku_cur_col = 4;
	sudoku_cur_row = 4;

	sudoku_gc = pz_get_gc(1);   /* Get a copy of the root graphics context */

    /*Set foreground color */
    GrSetGCUseBackground(sudoku_gc, GR_TRUE);
    GrSetGCForeground(sudoku_gc, GR_RGB(0,0,0));

    /* Open the window: */
    sudoku_wid = pz_new_window (0, HEADER_TOPLINE + 1, /* (x, y) */
                                  screen_info.cols,  
                                  screen_info.rows - (HEADER_TOPLINE + 1), /* Height of screen - header  */
                                  sudoku_do_draw, /* function pointer, called on an EXPOSURE event */
                                  sudoku_handle_event); /* function pointer, called on all other selected events */

    /* Select the types of events you need for your window: */
    GrSelectEvents (sudoku_wid, GR_EVENT_MASK_EXPOSURE | 
                      GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_KEY_UP
                    | GR_EVENT_MASK_TIMER);

    /* Map the window to the screen: */
    GrMapWindow (sudoku_wid);

    /* Create the timer used for animating your application: */
    sudoku_timer_id = GrCreateTimer (sudoku_wid,
                                       150); /* Timer interval, millisecs */

		sudoku_docreate();
		sudoku_draw_once();
}

/*int main(int argc, char **argv) {
	srand(time(NULL));

	sudoku_docreate();

	FILE *fp = fopen("sudoku_board.txt", "w");

	if (!fp) {
		printf("Unable to open sudoku_board.txt\n");
		exit(0);
	}

	int row, col;

	for (row = 0; row < 9; row++) {
		for (col = 0; col < 9; col++) {
			fprintf(fp, "%d ", (sudoku_board[row*9+col] & sudoku_digit_mask));
		}

		fprintf(fp, "\n");
	}

	fclose(fp);

	return 0;
}*/
