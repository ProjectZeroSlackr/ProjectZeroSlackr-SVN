/*iDeal or No Deal
 *
 * Copyright (C) 2006 Terry Stenvold
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include "pz.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>



static TWindow *window;

static TWidget *wid;
static ttk_surface image[11], intro[4];
static char file[10];
static ttk_color bg, fg, tx, gn;
static int w, h, bpp, i, x, y, selc = 1, csnum = 6, pause = 0;
static int er = 0, er1 = 0, er2 = 0, er3 = 0, raner =
    0, bcase[27], briefnum = -5, bpicked[26], tak, sbar, zero, in=0, in2=0, in3=29;
static double dell, toff, total, deal, mult, hdeal;
static double nums[27] =
    { 
0.01, 1, 5, 10, 25, 50, 75, 100, 200, 300, 400, 500, 750, 1000,
5000, 10000, 25000, 50000, 75000, 100000, 200000, 300000, 400000, 500000, 750000,
1000000, 0.0000 
};

static char *cnums[] =
    { 
".01", "1", "5", "10", "25", "50", "75", "100", "200", "300", "400",
"500", "750", "1K", "5K", "10K", "25K", "50K", "75K", "100K", "200K", "300K", "400K",
"500K", "750K", "1Mil" 
};

void intro_deal()
{
   in++;
   if(in>=4){in=0;}
   if(w-(in2*2)==(wid->x + (wid->w - 100) / 2)){in3++; in=0;}
   else in2++;
   if(in3>=70){briefnum=0; in2=0; in=0; in3=29;}
   wid->dirty++;
}

void calc_deal()
{
    toff = 0;
    total = 3418416.01;

    for (i = 1; i <= 25; i++)
      {
	  if (bpicked[i] != 26)
	    {
		toff += nums[bcase[bpicked[i]]];
	    }
      }

    //printf(" %.0f ",toff); 

    srand(time(NULL));

    total = total - toff;

    if (briefnum == 6)
      {
	  i = random() % 3 + 1;
	  switch (i)
	    {
	    case 1:
		mult = 0.05;
	    case 2:
		mult = 0.08;
	    case 3:
		mult = 0.04;
	    }
	  dell = ((total / 21) * (mult));
      }

    else if (briefnum == 11)
      {
	  i = random() % 3 + 1;
	  switch (i)
	    {
	    case 1:
		mult = 0.1;
	    case 2:
		mult = 0.12;
	    case 3:
		mult = 0.15;
	    }
	  dell = ((total / 16) * (mult));
      }

    else if (briefnum == 15)
      {
	  i = random() % 3 + 1;
	  switch (i)
	    {
	    case 1:
		mult = 0.25;
	    case 2:
		mult = 0.32;
	    case 3:
		mult = 0.28;
	    }
	  dell = ((total / 12) * (mult));
      }

    else if (briefnum == 18)
      {
	  i = random() % 3 + 1;
	  switch (i)
	    {
	    case 1:
		mult = 0.35;
	    case 2:
		mult = 0.33;
	    case 3:
		mult = 0.38;
	    }
	  dell = ((total / 9) * (mult));
      }

    else if (briefnum == 20)
      {
	  i = random() % 3 + 1;
	  switch (i)
	    {
	    case 1:
		mult = 0.45;
	    case 2:
		mult = 0.43;
	    case 3:
		mult = 0.48;
	    }
	  dell = ((total / 7) * (mult));
      }

    else if (briefnum == 21)
      {
	  i = random() % 3 + 1;
	  switch (i)
	    {
	    case 1:
		mult = 0.52;
	    case 2:
		mult = 0.57;
	    case 3:
		mult = 0.59;
	    }
	  dell = ((total / 6) * (mult));
      }

    else if (briefnum == 22)
      {
	  i = random() % 3 + 1;
	  switch (i)
	    {
	    case 1:
		mult = 0.65;
	    case 2:
		mult = 0.68;
	    case 3:
		mult = 0.73;
	    }
	  dell = ((total / 5) * (mult));
      }

    else if (briefnum == 23)
      {
	  i = random() % 3 + 1;
	  switch (i)
	    {
	    case 1:
		mult = 0.90;
	    case 2:
		mult = 0.85;
	    case 3:
		mult = 0.91;
	    }
	  dell = ((total / 4) * (mult));
      }

    else if (briefnum == 24)
      {
	  i = random() % 3 + 1;
	  switch (i)
	    {
	    case 1:
		mult = 0.98;
	    case 2:
		mult = 0.99;
	    case 3:
		mult = 1.1;
	    }
	  dell = ((total / 3) * (mult));
      }

    else if (briefnum == 25)
      {
	  i = random() % 3 + 1;
	  switch (i)
	    {
	    case 1:
		mult = 1;
	    case 2:
		mult = 1.15;
	    case 3:
		mult = 1.1;
	    }
	  dell = ((total / 2) * (mult));
      }


    if (dell >= hdeal)
      {
	  hdeal = dell;
      }

}

void check_box()
{
    for (i = 0; i <= 25; i++)
      {
	  if (selc - 1 == bpicked[i])
	    {
		er = 1;
		break;
	    }
	  else
	      er = 0;
      }

}

void check_ran()
{

    for (x = 0; x <= 25; x++)
      {
	  if (bcase[i] == bcase[x] && i != x)
	    {
		raner = 1;
		break;
	    }
	  else
	      raner = 0;
      }
}

void money_deal()
{
    srand(time(NULL));

	zero = random() % 26;	   
	bcase[zero] = 0;

    for (i = 0; i <= 25; i++)
      {
	  if(i==zero){i++;}
	  bcase[i] = random() % 25+1;
	  check_ran();
	  if (raner == 1)
	    {
		i--;
		raner = 0;
	    }
      }
    bcase[26] = 26;
//for (i = 0; i <= 25; i++){printf(" %d: %d ", i, bcase[i]);}
}

void reset_deal()
{

    for (i = 0; i <= 26; i++)
      {
	  bpicked[i] = 26;
	  bcase[i] = 26;
      }

    briefnum = 0;

    dell = 0;
    deal = 0;
    hdeal = 0;

    selc = 1;

    money_deal();

    pause = 0;

    csnum = 5;

}

void reset_deal2()
{

    for (i = 0; i <= 26; i++)
      {
	  bpicked[i] = 26;
	  bcase[i] = 26;
      }

    briefnum = -5;

    dell = 0;
    deal = 0;
    hdeal = 0;

    selc = 1;

    money_deal();

    pause = 0;

    csnum = 5;

}
void cleanup_deal()
{
    reset_deal();
}

void draw_deal(TWidget * wid, ttk_surface srf)
{

    bg = ttk_makecol(WHITE);	//249,232,0 gold
    fg = ttk_makecol(BLACK);
    gn = ttk_makecol(249, 232, 0);
    tx = fg;
    ttk_fillrect(srf, 0, 0, wid->win->w, wid->win->h, fg);

if (briefnum==(-5)){

	if(in3>=30)
	{ttk_blit_image(intro[in], srf,wid->x + (wid->w - 100) / 2, wid->y + (wid->h - 100) / 2);}

	else
	{ttk_blit_image(intro[in], srf,w-(in2*2), wid->y + (wid->h - 100) / 2);}
}


else if (w<=180&&w>=165)
{
    for (i = 0; i <= 6; i++)
      {

	  for (x = 0; x <= 25; x++)
	    {
		if (bpicked[x] + 1 == (i))
		  {
		      er = 1;
		  }
		if (bpicked[x] + 1 == (i + 7))
		  {
		      er1 = 1;
		  }
		if (bpicked[x] + 1 == (i + 14))
		  {
		      er2 = 1;
		  }
		if (bpicked[x] + 1 == (i + 20))
		  {
		      er3 = 1;
		  }
	    }


	  if (er3 == 0 && i >= 1)
	    {
		ttk_blit_image(image[1], srf, 39 + (i * 14), 2);
		sprintf(file, "%d", i + 20);
		pz_vector_string_center(srf, file, 45 + (i * 14), 8, 4.5,
					7, 1, fg);
	    }

	  if (er == 0 && i >= 1)
	    {
		ttk_blit_image(image[1], srf, 39 + (i * 14), 62);
		sprintf(file, "%d", i);
		pz_vector_string_center(srf, file, 45 + (i * 14), 68, 4.5,
					7, 1, fg);
	    }

	  if (er2 == 0)
	    {
		ttk_blit_image(image[1], srf, 39 + (i * 14), 22);
		sprintf(file, "%d", i + 14);
		pz_vector_string_center(srf, file, 45 + (i * 14), 28, 4.5,
					7, 1, fg);
	    }

	  if (er1 == 0)
	    {
		ttk_blit_image(image[1], srf, 39 + (i * 14), 42);
		sprintf(file, "%d", i + 7);
		pz_vector_string_center(srf, file, 45 + (i * 14), 48, 4.5,
					7, 1, fg);
	    }
	  er = 0;
	  er1 = 0;
	  er2 = 0;
	  er3 = 0;
      }

    for (i = 0; i <= 12; i++)
      {

	  for (x = 1; x <= 25; x++)
	    {
		if (bcase[bpicked[x]] == (i))
		  {
		      er = 1;
		  }
		if (bcase[bpicked[x]] == (i + 13))
		  {
		      er1 = 1;
		  }
	   }

	  ttk_blit_image(image[6], srf, 3, 1 + (i * 10));

	  if (er == 0)
	    {
		sprintf(file, "%s", cnums[i]);
		pz_vector_string_center(srf, file, 22, 6 + (i * 10), 4.5,
					6, 1, fg);
	    }

	  ttk_blit_image(image[6], srf, wid->win->w - 37, 1 + (i * 10));

	  if (er1 == 0)
	    {
		sprintf(file, "%s", cnums[i + 13]);
		pz_vector_string_center(srf, file, wid->win->w - 18,
					6 + (i * 10), 4.5, 6, 1, fg);
	    }

	  er = 0;
	  er1 = 0;

      }
    if (pause == 2 && briefnum == 25)
      {
	  sprintf(file, "Your Case \n   $%.0f", nums[bcase[bpicked[0]]]);
	  pz_vector_string(srf, file, 50, 20,6, 6,
				  1, ttk_makecol(WHITE));
	  sprintf(file, " You Won \n\n $%.0f", deal);
	  pz_vector_string_center(srf, file, 115, wid->win->h - 50, 6, 6,
				  1, ttk_makecol(WHITE));
	  sprintf(file, " Could have \n\n won $%.0f", hdeal);
	  pz_vector_string(srf, file, 40, wid->win->h - 20, 6, 5, 1,
			   ttk_makecol(WHITE));
      }
    else if (briefnum == 0)
      {
	  sprintf(file, "Pick Case: %d", selc);
	  pz_vector_string(srf, file, 40, wid->win->h - 25, 5, 6, 1,
			   ttk_makecol(WHITE));
      }
    else if (briefnum >= 1 && pause != 1)
      {
	  sprintf(file, "Pick #%d Case: \n \n     %d", csnum, selc);
	  pz_vector_string(srf, file, 40, wid->win->h - 25, 5, 6, 1,
			   ttk_makecol(WHITE));
      }

    else if (pause == 1 && tak == 1
	     && (briefnum == 6 || briefnum == 11 || briefnum == 15
		 || briefnum == 18 || briefnum == 20 || briefnum == 21
		 || briefnum == 22 || briefnum == 23 || briefnum == 24))
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n $%.0f", dell);
	  pz_vector_string_center(srf, file, 120, wid->win->h - 50, 6, 6,
				  1, ttk_makecol(WHITE));
	  sprintf(file, "Press Forward");
	  pz_vector_string(srf, file, 40, wid->win->h - 30, 6, 5, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[4], srf, wid->win->w - 80, 112);
      }
    else if (pause == 1
	     && (briefnum == 6 || briefnum == 11 || briefnum == 15
		 || briefnum == 18 || briefnum == 20 || briefnum == 21
		 || briefnum == 22 || briefnum == 23 || briefnum == 24))
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n $%.0f", dell);
	  pz_vector_string_center(srf, file, 120, wid->win->h - 50, 6, 6,
				  1, ttk_makecol(WHITE));
	  sprintf(file, "Deal No Deal");
	  pz_vector_string(srf, file, 40, wid->win->h - 30, 6, 5, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[5], srf, 50, 112);
	  ttk_blit_image(image[4], srf, wid->win->w - 65, 112);
      }
    else if (pause == 1 && briefnum == 25 && tak == 1)
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n $%.0f", dell);
	  pz_vector_string_center(srf, file, 120, wid->win->h - 50, 6, 6,
				  1, ttk_makecol(WHITE));
	  sprintf(file, "Press Forward");
	  pz_vector_string(srf, file, 40, wid->win->h - 30, 6, 5, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[4], srf, wid->win->w - 80, 112);
      }
    else if (pause == 1 && briefnum == 25)
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n $%.0f", dell);
	  pz_vector_string_center(srf, file, 120, wid->win->h - 50, 6, 6,
				  1, ttk_makecol(WHITE));
	  sprintf(file, "Deal    Case");
	  pz_vector_string(srf, file, 40, wid->win->h - 30, 6, 5, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[5], srf, 50, 112);
	  ttk_blit_image(image[4], srf, wid->win->w - 65, 112);
      }

}

else if (w<=400&&w>=250)
{
    for (i = 0; i <= 6; i++)
      {

	  for (x = 0; x <= 25; x++)
	    {
		if (bpicked[x] + 1 == (i))
		  {
		      er = 1;
		  }
		if (bpicked[x] + 1 == (i + 7))
		  {
		      er1 = 1;
		  }
		if (bpicked[x] + 1 == (i + 14))
		  {
		      er2 = 1;
		  }
		if (bpicked[x] + 1 == (i + 20))
		  {
		      er3 = 1;
		  }
	    }


	  if (er3 == 0 && i >= 1)
	    {
		ttk_blit_image(image[2], srf, 60 + (i * 27), 2);
		sprintf(file, "%d", i + 20);
		pz_vector_string_center(srf, file, 72 + (i * 27), 13, 6,
					7, 1, fg);
	    }

	  if (er == 0 && i >= 1)
	    {
		ttk_blit_image(image[2], srf, 60 + (i * 27), 79);
		sprintf(file, "%d", i);
		pz_vector_string_center(srf, file, 72 + (i * 27), 90, 6,
					7, 1, fg);
	    }

	  if (er2 == 0)
	    {
		ttk_blit_image(image[2], srf, 60 + (i * 27), 27);
		sprintf(file, "%d", i + 14);
		pz_vector_string_center(srf, file, 72 + (i * 27), 37, 6,
					7, 1, fg);
	    }

	  if (er1 == 0)
	    {
		ttk_blit_image(image[2], srf, 60 + (i * 27), 52);
		sprintf(file, "%d", i + 7);
		pz_vector_string_center(srf, file, 72 + (i * 27), 63, 6,
					7, 1, fg);
	    }
	  er = 0;
	  er1 = 0;
	  er2 = 0;
	  er3 = 0;
      }

    for (i = 0; i <= 12; i++)
      {

	  for (x = 1; x <= 25; x++)
	    {
		if (bcase[bpicked[x]] == (i))
		  {
		      er = 1;
		  }
		if (bcase[bpicked[x]] == (i + 13))
		  {
		      er1 = 1;
		  }
	   }

	  ttk_blit_image(image[7], srf, 3, 1 + (i * 18));

	  if (er == 0)
	    {
		sprintf(file, "%s", cnums[i]);
		pz_vector_string_center(srf, file, 30, 10 + (i * 18), 6,
					5, 1, fg);
	    }

	  ttk_blit_image(image[7], srf, wid->win->w - 55, 1 + (i * 18));

	  if (er1 == 0)
	    {
		sprintf(file, "%s", cnums[i + 13]);
		pz_vector_string_center(srf, file, wid->win->w - 24,
					10 + (i * 18), 6, 5, 1, fg);
	    }

	  er = 0;
	  er1 = 0;

      }
    if (pause == 2 && briefnum == 25)
      {
	  sprintf(file, "Your Case $%.0f", nums[bcase[bpicked[0]]]);
	  pz_vector_string(srf, file, 70, wid->win->h - 110,8, 8,
				  1, ttk_makecol(WHITE));
	  sprintf(file, " You Won \n\n  $%.0f", deal);
	  pz_vector_string_center(srf, file, 200, wid->win->h - 80, 8, 8,
				  1, ttk_makecol(WHITE));
	  sprintf(file, " Could have \n\n won $%.0f", hdeal);
	  pz_vector_string(srf, file, 80, wid->win->h - 40, 8, 8, 1,
			   ttk_makecol(WHITE));
      }
    else if (briefnum == 0)
      {
	  sprintf(file, "Pick Case: %d", selc);
	  pz_vector_string(srf, file, 75, wid->win->h - 40, 8, 8, 1,
			   ttk_makecol(WHITE));
      }
    else if (briefnum >= 1 && pause != 1)
      {
	  sprintf(file, "Pick #%d Case: \n \n     %d", csnum, selc);
	  pz_vector_string(srf, file, 85, wid->win->h - 40, 8, 8, 1,
			   ttk_makecol(WHITE));
      }

    else if (pause == 1 && tak == 1
	     && (briefnum == 6 || briefnum == 11 || briefnum == 15
		 || briefnum == 18 || briefnum == 20 || briefnum == 21
		 || briefnum == 22 || briefnum == 23 || briefnum == 24))
      {
	 calc_deal();
	  sprintf(file, "Deal is  \n \n$%.0f", dell);
	  pz_vector_string_center(srf, file, 210, wid->win->h - 90, 8, 8,
				  1, ttk_makecol(WHITE));
	   sprintf(file, "Press Forward");
	  pz_vector_string(srf, file, 80, wid->win->h - 50, 8, 8, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[4], srf, wid->win->w - 170, wid->win->h - 30);
      }
    else if (pause == 1
	     && (briefnum == 6 || briefnum == 11 || briefnum == 15
		 || briefnum == 18 || briefnum == 20 || briefnum == 21
		 || briefnum == 22 || briefnum == 23 || briefnum == 24))
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n \n$%.0f", dell);
	  pz_vector_string_center(srf, file, 210, wid->win->h - 90, 8, 8,
				  1, ttk_makecol(WHITE));
	  sprintf(file, "Deal  or  No Deal");
	  pz_vector_string(srf, file, 70, wid->win->h - 50, 8, 8, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[5], srf, 85, wid->win->h - 30);
	  ttk_blit_image(image[4], srf, wid->win->w - 95, wid->win->h - 30);
      }
    else if (pause == 1 && briefnum == 25 && tak == 1)
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n \n$%.0f", dell);
	  pz_vector_string_center(srf, file, 210, wid->win->h - 90, 8, 8,
				  1, ttk_makecol(WHITE));
	   sprintf(file, "Press Forward");
	  pz_vector_string(srf, file, 80, wid->win->h - 50, 8, 8, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[4], srf, wid->win->w - 170, wid->win->h - 30);
      }
    else if (pause == 1 && briefnum == 25)
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n \n$%.0f", dell);
	  pz_vector_string_center(srf, file, 210, wid->win->h - 90, 8, 8,
				  1, ttk_makecol(WHITE));
	  sprintf(file, "Deal        Case");
	  pz_vector_string(srf, file, 70, wid->win->h - 50, 8, 8, 1,
			   ttk_makecol(WHITE));
	   ttk_blit_image(image[5], srf, 85, wid->win->h - 30);
	  ttk_blit_image(image[4], srf, wid->win->w - 95, wid->win->h - 30);
      }

}

else if (w<=250&&w>=220)
{
    for (i = 0; i <= 6; i++)
      {

	  for (x = 0; x <= 25; x++)
	    {
		if (bpicked[x] + 1 == (i))
		  {
		      er = 1;
		  }
		if (bpicked[x] + 1 == (i + 7))
		  {
		      er1 = 1;
		  }
		if (bpicked[x] + 1 == (i + 14))
		  {
		      er2 = 1;
		  }
		if (bpicked[x] + 1 == (i + 20))
		  {
		      er3 = 1;
		  }
	    }


	  if (er3 == 0 && i >= 1)
	    {
		ttk_blit_image(image[3], srf, 44 + (i * 19), 2);
		sprintf(file, "%d", i + 20);
		pz_vector_string(srf, file, 46 + (i * 19), 8, 6,
					5, 1, fg);
	    }

	  if (er == 0 && i >= 1)
	    {
		ttk_blit_image(image[3], srf, 44 + (i * 19), 79);
		sprintf(file, "%d", i);
		pz_vector_string(srf, file, 46 + (i * 19), 85, 6,
					5, 1, fg);
	    }

	  if (er2 == 0)
	    {
		ttk_blit_image(image[3], srf, 44 + (i * 19), 27);
		sprintf(file, "%d", i + 14);
		pz_vector_string(srf, file, 46 + (i * 19), 32, 6,
					5, 1, fg);
	    }

	  if (er1 == 0)
	    {
		ttk_blit_image(image[3], srf, 44 + (i * 19), 52);
		sprintf(file, "%d", i + 7);
		pz_vector_string(srf, file, 46 + (i * 19), 58, 6,
					5, 1, fg);
	    }
	  er = 0;
	  er1 = 0;
	  er2 = 0;
	  er3 = 0;
      }

    for (i = 0; i <= 12; i++)
      {

	  for (x = 1; x <= 25; x++)
	    {
		if (bcase[bpicked[x]] == (i))
		  {
		      er = 1;
		  }
		if (bcase[bpicked[x]] == (i + 13))
		  {
		      er1 = 1;
		  }
	   }

	  ttk_blit_image(image[8], srf, 3, 1 + (i * 13));

	  if (er == 0)
	    {
		sprintf(file, "%s", cnums[i]);
		pz_vector_string_center(srf, file, 24, 8 + (i * 13), 4.5,
					5, 1, fg);
	    }

	  ttk_blit_image(image[8], srf, wid->win->w - 40, 1 + (i * 13));

	  if (er1 == 0)
	    {
		sprintf(file, "%s", cnums[i + 13]);
		pz_vector_string_center(srf, file, wid->win->w - 18,
					8 + (i * 13), 4.5, 5, 1, fg);
	    }

	  er = 0;
	  er1 = 0;

      }
    if (pause == 2 && briefnum == 25)
      {
	  sprintf(file, "Your Case \n\n $%.0f", nums[bcase[bpicked[0]]]);
	  pz_vector_string(srf, file, 70, 20,7, 7,
				  1, ttk_makecol(WHITE));
	  sprintf(file, " You Won \n\n $%.0f", deal);
	  pz_vector_string_center(srf, file, 145, wid->win->h - 70, 7, 7,
				  1, ttk_makecol(WHITE));
	  sprintf(file, " Could have \n\n won $%.0f", hdeal);
	  pz_vector_string(srf, file, 60, wid->win->h - 30, 7, 7, 1,
			   ttk_makecol(WHITE));
      }
    else if (briefnum == 0)
      {
	  sprintf(file, "Pick Case: %d", selc);
	  pz_vector_string(srf, file, 50, wid->win->h - 40, 6, 6, 1,
			   ttk_makecol(WHITE));
      }
    else if (briefnum >= 1 && pause != 1)
      {
	  sprintf(file, "Pick #%d Case: \n \n     %d", csnum, selc);
	  pz_vector_string(srf, file, 60, wid->win->h - 40, 6, 6, 1,
			   ttk_makecol(WHITE));
      }

    else if (pause == 1 && tak == 1
	     && (briefnum == 6 || briefnum == 11 || briefnum == 15
		 || briefnum == 18 || briefnum == 20 || briefnum == 21
		 || briefnum == 22 || briefnum == 23 || briefnum == 24))
      {
	 calc_deal();
	  sprintf(file, "Deal is  \n \n $%.0f", dell);
	  pz_vector_string_center(srf, file, 150, wid->win->h - 60, 6, 6,
				  1, ttk_makecol(WHITE));
	   sprintf(file, "Press Forward");
	  pz_vector_string(srf, file, 55, wid->win->h - 30, 6, 6, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[4], srf, wid->win->w - 120, wid->win->h - 20);
      }
    else if (pause == 1
	     && (briefnum == 6 || briefnum == 11 || briefnum == 15
		 || briefnum == 18 || briefnum == 20 || briefnum == 21
		 || briefnum == 22 || briefnum == 23 || briefnum == 24))
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n \n $%.0f", dell);
	  pz_vector_string_center(srf, file, 150, wid->win->h - 60, 6, 6,
				  1, ttk_makecol(WHITE));
	  sprintf(file, "Deal or No Deal");
	  pz_vector_string(srf, file, 50, wid->win->h - 30, 6, 6, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[5], srf, 55, wid->win->h - 20);
	  ttk_blit_image(image[4], srf, wid->win->w - 80, wid->win->h - 20);
      }
    else if (pause == 1 && briefnum == 25 && tak == 1)
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n \n $%.0f", dell);
	  pz_vector_string_center(srf, file, 150, wid->win->h - 60, 6, 6,
				  1, ttk_makecol(WHITE));
	   sprintf(file, "Press Forward");
	  pz_vector_string(srf, file, 55, wid->win->h - 30, 6, 6, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[4], srf, wid->win->w - 120, wid->win->h - 20);
      }
    else if (pause == 1 && briefnum == 25)
      {
	  calc_deal();
	 sprintf(file, "Deal is  \n \n $%.0f", dell);
	  pz_vector_string_center(srf, file, 150, wid->win->h - 60, 6, 6,
				  1, ttk_makecol(WHITE));
	  sprintf(file, "Deal     Case");
	  pz_vector_string(srf, file, 55, wid->win->h - 30, 6, 6, 1,
			   ttk_makecol(WHITE));
	   ttk_blit_image(image[5], srf, 55, wid->win->h - 20);
	  ttk_blit_image(image[4], srf, wid->win->w - 80, wid->win->h - 20);
      }

}

else
{
    for (i = 0; i <= 6; i++)
      {

	  for (x = 0; x <= 25; x++)
	    {
		if (bpicked[x] + 1 == (i))
		  {
		      er = 1;
		  }
		if (bpicked[x] + 1 == (i + 7))
		  {
		      er1 = 1;
		  }
		if (bpicked[x] + 1 == (i + 14))
		  {
		      er2 = 1;
		  }
		if (bpicked[x] + 1 == (i + 20))
		  {
		      er3 = 1;
		  }
	    }


	  if (er3 == 0 && i >= 1)
	    {
		ttk_blit_image(image[10], srf, 30 + (i * 11), 2);
		sprintf(file, "%d", i + 20);
		pz_vector_string(srf, file, 30 + (i * 11), 4, 4,
					4, 1, fg);
	    }

	  if (er == 0 && i >= 1)
	    {
		ttk_blit_image(image[10], srf, 30 + (i * 11), 34);
		sprintf(file, "%d", i);
		pz_vector_string(srf, file, 32 + (i * 11), 37, 3,
					4, 1, fg);
	    }

	  if (er2 == 0)
	    {
		ttk_blit_image(image[10], srf, 30 + (i * 11), 13);
		sprintf(file, "%d", i + 14);
		pz_vector_string(srf, file, 30 + (i * 11), 15, 3,
					4, 1, fg);
	    }

	  if (er1 == 0)
	    {
		ttk_blit_image(image[10], srf, 30 + (i * 11), 24);
		sprintf(file, "%d", i + 7);
		pz_vector_string(srf, file, 31 + (i * 11), 26, 3,
					4, 1, fg);
	    }
	  er = 0;
	  er1 = 0;
	  er2 = 0;
	  er3 = 0;
      }

    for (i = 0; i <= 12; i++)
      {

	  for (x = 1; x <= 25; x++)
	    {
		if (bcase[bpicked[x]] == (i))
		  {
		      er = 1;
		  }
		if (bcase[bpicked[x]] == (i + 13))
		  {
		      er1 = 1;
		  }
	   }

	  ttk_blit_image(image[9], srf, 2, 1 + (i * 8));

	  if (er == 0)
	    {
		sprintf(file, "%s", cnums[i]);
		pz_vector_string_center(srf, file, 18, 5 + (i * 8), 4,
					4, 1, fg);
	    }

	  ttk_blit_image(image[9], srf, wid->win->w - 28, 1 + (i * 8));

	  if (er1 == 0)
	    {
		sprintf(file, "%s", cnums[i + 13]);
		pz_vector_string_center(srf, file, wid->win->w - 14,
					5 + (i * 8), 4, 4, 1, fg);
	    }

	  er = 0;
	  er1 = 0;

      }
    if (pause == 2 && briefnum == 25)
      {
	  sprintf(file, "Your Case \n\n $%.0f", nums[bcase[bpicked[0]]]);
	  pz_vector_string(srf, file, 30, 20,5, 5,
				  1, ttk_makecol(WHITE));
	  sprintf(file, " You Won \n\n $%.0f", deal);
	  pz_vector_string(srf, file, 30, 45 , 5, 5,
				  1, ttk_makecol(WHITE));
	  sprintf(file, " Could have \n\n won $%.0f", hdeal);
	  pz_vector_string(srf, file, 30, 80 , 5, 5, 1,
			   ttk_makecol(WHITE));
      }
    else if (briefnum == 0)
      {
	  sprintf(file, "Pick Case: %d", selc);
	  pz_vector_string(srf, file, 29, 70 , 4, 5, 1,
			   ttk_makecol(WHITE));
      }
    else if (briefnum >= 1 && pause != 1)
      {
	  sprintf(file, "Pick #%d Case: \n \n     %d", csnum, selc);
	  pz_vector_string(srf, file, 29, 70 , 4, 5, 1,
			   ttk_makecol(WHITE));
      }

    else if (pause == 1 && tak == 1
	     && (briefnum == 6 || briefnum == 11 || briefnum == 15
		 || briefnum == 18 || briefnum == 20 || briefnum == 21
		 || briefnum == 22 || briefnum == 23 || briefnum == 24))
      {
	 calc_deal();
	  sprintf(file, "Deal is  \n \n $%.0f", dell);
	  pz_vector_string(srf, file, 50, 50 , 5, 5,
				  1, ttk_makecol(WHITE));
	   sprintf(file, "Press Forward");
	  pz_vector_string(srf, file, 30, 80 , 4, 5, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[4], srf, 55, 90);
      }
    else if (pause == 1
	     && (briefnum == 6 || briefnum == 11 || briefnum == 15
		 || briefnum == 18 || briefnum == 20 || briefnum == 21
		 || briefnum == 22 || briefnum == 23 || briefnum == 24))
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n \n $%.0f", dell);
	  pz_vector_string(srf, file, 50, 50 , 5, 5,
				  1, ttk_makecol(WHITE));
	  sprintf(file, "Deal No Deal");
	  pz_vector_string(srf, file, 35 ,82, 4, 5, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[5], srf, 35, 90);
	  ttk_blit_image(image[4], srf, 85, 90);
      }
    else if (pause == 1 && briefnum == 25 && tak == 1)
      {
	  calc_deal();
	  sprintf(file, "Deal is  \n \n $%.0f", dell);
	  pz_vector_string(srf, file, 50, 50 , 5, 5,
				  1, ttk_makecol(WHITE));
	   sprintf(file, "Press Forward");
	  pz_vector_string(srf, file, 30, 80 , 4, 5, 1,
			   ttk_makecol(WHITE));
	  ttk_blit_image(image[4], srf, 55, 90);
      }
    else if (pause == 1 && briefnum == 25)
      {
	  calc_deal();
	 sprintf(file, "Deal is  \n \n $%.0f", dell);
	  pz_vector_string(srf, file, 50, 50 , 5, 5,
				  1, ttk_makecol(WHITE));
	  sprintf(file, "Deal  Case");
	  pz_vector_string(srf, file, 30, 80, 5, 5, 1,
			   ttk_makecol(WHITE));
	   ttk_blit_image(image[5], srf, 35, 90);
	  ttk_blit_image(image[4], srf, 85, 90);
      }
}

}

static int ideal_scroll(TWidget *this,int dir)
{
	  TTK_SCROLLMOD(dir, 10);
	  if (dir > 0)
	    {
		if (selc <= 26 && selc >= 1 && pause == 0)
		  {	
		   selc++;
		
		  for(i=0;i<=25;i++){
		   while(selc-1==bpicked[i]){
		    selc++; i=0;}}

		   if(selc>26){selc=1; 
		   for(i=0;i<=25;i++){
		   while(selc-1==bpicked[i])
		   {selc++; i=0;}}}
		   
		    wid->dirty++;
		  }
		
	    }

	  else
	    {
		if (selc <= 26 && selc >= 1 && pause == 0)
		  {
		      selc--;

		  
		  for(i=0;i<=25;i++){
		  while(selc-1==bpicked[i]){
		    selc--; i=0;}}

		  if(selc<1){selc=26; 
		   for(i=0;i<=25;i++){
		   while(selc-1==bpicked[i])
		   {selc--; i=0;}}}

		      wid->dirty++;
		     
		  }
		
	    }
return TTK_EV_CLICK;
}

static int ideal_down(TWidget *this,int button)
{
	  switch (button)
	    {
	    case TTK_BUTTON_ACTION:
		check_box();
		 if(briefnum==-5){briefnum=0; in2=0; in=0; in3=29; wid->dirty++; break;}
		 if (er == 0 && pause == 0 && briefnum != -5)
		  {
		      bpicked[briefnum] = selc - 1;
		      briefnum++;
		      csnum--;

		      if (briefnum == 6 || briefnum == 11 || briefnum == 15
			  || briefnum == 18 || briefnum == 20
			  || briefnum == 21 || briefnum == 22
			  || briefnum == 23 || briefnum == 24
			  || briefnum == 25)
			{
			    pause = 1;
			}
		      wid->dirty++;
		  }
		break;

	    case TTK_BUTTON_HOLD:
		reset_deal2();
		cleanup_deal();
		pz_close_window(this->win);
		break;

	    case TTK_BUTTON_MENU:
		reset_deal();
		wid->dirty++;
		break;

	    case TTK_BUTTON_NEXT:
		if (pause == 1 && briefnum == 6)
		  {
		      pause = 0;
		      csnum = 5;
		      wid->dirty++;
		  }
		else if (pause == 1 && briefnum == 11)
		  {
		      pause = 0;
		      csnum = 4;
		      wid->dirty++;
		  }
		else if (pause == 1 && briefnum == 15)
		  {
		      pause = 0;
		      csnum = 3;
		      wid->dirty++;
		  }
		else if (pause == 1 && briefnum == 18)
		  {
		      pause = 0;
		      csnum = 2;
		      wid->dirty++;
		  }
		else if (pause == 1 && briefnum == 20)
		  {
		      pause = 0;
		      csnum = 1;
		      wid->dirty++;
		  }
		else if (pause == 1 && briefnum == 21)
		  {
		      pause = 0;
		      csnum = 1;
		      wid->dirty++;
		  }
		else if (pause == 1 && briefnum == 22)
		  {
		      pause = 0;
		      csnum = 1;
		      wid->dirty++;
		  }
		else if (pause == 1 && briefnum == 23)
		  {
		      pause = 0;
		      csnum = 1;
		      wid->dirty++;
		  }
		else if (pause == 1 && briefnum == 24)
		  {
		      pause = 0;
		      csnum = 1;
		      wid->dirty++;
		  }
		else if (pause == 1 && briefnum == 25)
		  {
		      pause = 2;
		      deal = nums[bcase[bpicked[0]]];
		      wid->dirty++;
		  }
		else if (pause == 1 && briefnum == 25&&tak==1)
		  {
		      pause = 2;
		      wid->dirty++;
		  }
		break;
	    case TTK_BUTTON_PREVIOUS:
		if (pause == 1 && briefnum == 6 && tak != 1)
		  {
		      pause = 0;
		      csnum = 5;
		      deal = dell;
		      wid->dirty++;
		      tak = 1;
		  }
		else if (pause == 1 && briefnum == 11 && tak != 1)
		  {
		      pause = 0;
		      csnum = 4;
		      deal = dell;
		      wid->dirty++;
		      tak = 1;
		  }
		else if (pause == 1 && briefnum == 15 && tak != 1)
		  {
		      pause = 0;
		      csnum = 3;
		      deal = dell;
		      wid->dirty++;
		      tak = 1;
		  }
		else if (pause == 1 && briefnum == 18 && tak != 1)
		  {
		      pause = 0;
		      csnum = 2;
		      deal = dell;
		      wid->dirty++;
		      tak = 1;
		  }
		else if (pause == 1 && briefnum == 20 && tak != 1)
		  {
		      pause = 0;
		      csnum = 1;
		      deal = dell;
		      wid->dirty++;
		      tak = 1;
		  }
		else if (pause == 1 && briefnum == 21 && tak != 1)
		  {
		      pause = 0;
		      csnum = 1;
		      deal = dell;
		      wid->dirty++;
		      tak = 1;
		  }
		else if (pause == 1 && briefnum == 22 && tak != 1)
		  {
		      pause = 0;
		      csnum = 1;
		      deal = dell;
		      wid->dirty++;
		      tak = 1;
		  }
		else if (pause == 1 && briefnum == 23 && tak != 1)
		  {
		      pause = 0;
		      csnum = 1;
		      deal = dell;
		      wid->dirty++;
		      tak = 1;
		  }
		else if (pause == 1 && briefnum == 24 && tak != 1)
		  {
		      pause = 0;
		      csnum = 1;
		      deal = dell;
		      wid->dirty++;
		      tak = 1;
		  }
		else if (pause == 1 && briefnum == 24 && tak != 1)
		  {
		      pause = 0;
		      csnum = 1;
		      deal = dell;
		      wid->dirty++;
		      tak = 1;
		  }
		else if (pause == 1 && briefnum == 25 && tak != 1)
		  {
		      pause = 2;
		      deal = dell;
		      wid->dirty++;
		  }
		break;

	   
	    }
return 0;
}
static int ideal_timer(TWidget *this)
{
      
	   if(briefnum==-5){intro_deal();}
return 0;
	  }



TWindow *new_deal_window()
{
    w = ttk_screen->w;		//gets screen width
    h = ttk_screen->h;		//gets screen hieght
    bpp = ttk_screen->bpp;	//gets colour depth

    money_deal();

 if (h>200){sbar=7;}
 else {sbar=6;}

    for (i = 0; i <= 26; i++)
      {
	  bpicked[i] = 26;
      }
    //printf (" %f ", nums[bcase[bpicked[i]]]);}

    for (i = 0; i <= 10; i++)
      {
	  sprintf(file, "/opt/Zillae/ZacZilla/Data/ideal/%d.gif", i);
	  image[i] = ttk_load_image(file);
      }
    for (i = 0; i <= 3; i++)
      {
	  sprintf(file, "/opt/Zillae/ZacZilla/Data/ideal/0%d.gif", i);
	  intro[i] = ttk_load_image(file);
      }
    window = ttk_new_window("iDeal or No Deal");
    ttk_window_hide_header(window);
    wid = ttk_new_widget(0,0);
    wid->focusable=1;
    wid->draw=draw_deal;
    wid->down=ideal_down;
    wid->scroll=ideal_scroll;
    wid->timer=ideal_timer;
    ttk_add_widget(window, wid);
    ttk_widget_set_timer(wid, 50);
   ttk_show_window(window);

}



