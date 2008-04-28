/*
 * Copyright (C) 2005 Shawn Bruce
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
 *
 * HISTORY:
 *
 * 1.00 RC3 Released (11:37 PM 10/21/2005)
 * - forgot to include unistd.h
 * - changed dealer_cards[16]/player_cards[16] to dealer_cards[12]/player_cards[12]
 *
 * 1.00 RC2 Released (9:25 PM 10/21/2005)
 * - changed sleep to usleep
 *
 * 1.00 RC1 Released (8:56 PM 10/21/2005)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "pz.h"


//Save file
#ifdef IPOD
// KERIPO MOD
	//#define SAVEFILE "/home/.blackjack"
//#else
	//#define SAVEFILE ".blackjack"
#define SAVEFILE "/opt/Zillae/ZacZilla/Data/blackjack.txt"
#endif

//Defines
#define VERSION			"1.00 RC3"
#define BET_INCREMENT	10
#define POT_DEFAULT		100

//Enum
enum { NONE=0 };
enum { PLAYER=1, DEALER=-1, TIE=2 };
enum { BET=1, GAMEOVER=2, DEALER_HIT=4, DEALER_STAY=5 };


//Drawing
void new_blackjack_window (void);
static void draw_header();
static void draw_msg();
static void draw_window();

//Pre-defines
static int handle_event(GR_EVENT *event);
static void reset();
static void deal_card(int who );
static void change_turn();
static void dealer_turn();
static void player_turn();
static void inc_bid();
static void dec_bid();
static void play_stay();
static void hit_me();
static void check_win();
static void draw_help();
static char* cardLetter(int card);
static void readPot();
static void writePot();

//Windows
static GR_WINDOW_ID blackjack_wid;
static GR_GC_ID blackjack_gc;
static GR_WINDOW_INFO wi;

//Variables
static int gameRunning		= 0;

static int dealer_hand		= 0;
static int dealer_cards[12];
static int dealer_ncards	= 0;

static int dealer_stay		= 0;

static int player_hand		= 0;
static int player_cards[12];
static int player_ncards	= 0;

static int player_stay		= 0;

static long int player_pot	= POT_DEFAULT;
static long int player_bet 	= 0;

static int winner			= NONE;
static int msg				= NONE;

static int turn				= PLAYER;

/*0=Two, 1=Three, ..., 12=Ace*/
static int CARDS[]			= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*0=Two, 1=Three, ..., 12=Ace High, 13=Ace Low*/
static int CARDVALS[]		= {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 1};




//Create the blackjack window
void new_blackjack_window(void)
{

	srand((unsigned int)time(NULL) / 2);  //Make a seed for our random # generator

    blackjack_gc = pz_get_gc(1);       /* Get the graphics context */

    /* Open the window: */
    blackjack_wid = pz_new_window (0,
								   21,
								   screen_info.cols,
								   screen_info.rows - (HEADER_TOPLINE+1),
								   draw_header,
								   handle_event);

	GrGetWindowInfo(blackjack_wid, &wi); /* Get screen info */

    /* Select the types of events you need for your window: */
    GrSelectEvents (blackjack_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN|GR_EVENT_MASK_KEY_UP);

    /* Display the window: */
    GrMapWindow (blackjack_wid);

	readPot();
	reset();
}

//Handles input from ipod
static int handle_event(GR_EVENT *event)
{
    switch (event->type)
    {
		case GR_EVENT_TYPE_KEY_UP:
			switch (event->keystroke.ch)
			{

				case 'h': /* Hold switch */
					draw_window();
				break;
			}
		break;

		case GR_EVENT_TYPE_KEY_DOWN:
			switch (event->keystroke.ch)
			{

				case 'h': /* Hold switch */
					draw_help();
				break;

				case 'm': /* Menu button */
					writePot();
					reset();
					pz_close_window (blackjack_wid);

				break;

				case 'l': /* Wheel left */
					dec_bid();
				break;

				case 'r': /* Wheel right */
					inc_bid();
				break;

				case 'w': /* Prev track */
					dec_bid();
				break;

				case 'f': /* Next track */
					inc_bid();
				break;

				case 'd': /* Play */
					play_stay();
				break;

				case '\r': /* Action */
					hit_me();
				break;
			}
		break;
    }
	return 0;
}

//Raises bid amount
static void inc_bid()
{
	if(gameRunning == 0)
	{
		if(player_pot >= BET_INCREMENT)
		{
			player_bet += BET_INCREMENT;
			player_pot -= BET_INCREMENT;

			draw_window();
		}
	}
}

//Lowers bid amount
static void dec_bid()
{
	if(gameRunning == 0)
	{
		if(player_bet >= BET_INCREMENT)
		{
			player_bet -= BET_INCREMENT;
			player_pot += BET_INCREMENT;

			draw_window();
		}
	}
}

//Starts game / stays
static void play_stay()
{
	if(gameRunning == 0 && player_bet >= BET_INCREMENT)
	{
		deal_card(DEALER);
		//Display First Card To All Here
		deal_card(DEALER);

		deal_card(PLAYER);
		//Display First Card To All Here
		deal_card(PLAYER);

		gameRunning = 1;
		msg = NONE;
		change_turn(PLAYER);

		draw_window();
	}else{
		//Stay
		if(gameRunning == 1 && turn == PLAYER)
		{
			player_stay = 1;

			draw_window();

			change_turn(DEALER);
		}
	}
}

//Deal card to player
static void hit_me()
{
	if(gameRunning == 1 && turn == PLAYER)
	{
		deal_card(PLAYER);

		draw_window();

		change_turn(PLAYER);
	}
}

//Reset to new game defaults
static void reset()
{
	memset(CARDS, 0, sizeof(CARDS));
	memset(dealer_cards, 0, sizeof(dealer_cards));
	memset(player_cards, 0, sizeof(player_cards));

	dealer_hand			= 0;
	dealer_stay			= 0;
	dealer_ncards		= 0;

	player_hand			= 0;
	player_stay			= 0;
	player_ncards		= 0;

	turn = PLAYER;

	player_bet = 0;
	gameRunning = 0;

	winner = NONE;
	msg	= BET;

	draw_window();
}

//Generate and deal a random card
static void deal_card(int who /*DEALER, PLAYER*/)
{
	/*
	1.) make random # for card
	2.) check if dealer/player already have that card, if not continue
	3.) add card to dealer/player list of cards
	4.) return
	*/

	int complete = 0;
	int card;

	while(complete == 0)
	{
		card = rand() % (12+1);

		if(CARDS[card] < 4)
		{
			CARDS[card]++;
			complete = 1;
		}
	}

	if(who == DEALER)
	{
		if(card == 12 && (dealer_hand + CARDVALS[12]) > 21)
		{
			dealer_hand+= CARDVALS[13]; //Low Ace
		}else{
			dealer_hand+= CARDVALS[card]; //Any Card || High Ace
		}

		dealer_cards[dealer_ncards] = card;
		dealer_ncards++;
	}else{
		if(card == 12 && (player_hand + CARDVALS[12]) > 21)
		{
			player_hand+= CARDVALS[13]; //Low Ace
		}else{
			player_hand+= CARDVALS[card]; //Any Card || High Ace
		}

		player_cards[player_ncards] = card;
		player_ncards++;
	}

}

//Allow Dealer/Player to act on their turn
static void change_turn(int to)
{
	check_win();

	if(gameRunning == 1)
	{
		if(to == DEALER)
		{
			turn = DEALER;
			dealer_turn();
		}else{
			turn = PLAYER;
			player_turn();
		}
	}
}

//Dealers turn, keep hitting until 17+, once 17+ stay
static void dealer_turn()
{
	if(gameRunning == 1)
	{
		if(dealer_stay == 0)
		{
			if(dealer_hand >= 17)
			{
				dealer_stay = 1;
				msg = DEALER_STAY;
			}else{
				msg = DEALER_HIT;
				deal_card(DEALER);
			}
		}

		draw_window();
		usleep(1000000);

		change_turn(DEALER);
	}
}

//Players turn, skip if staying
static void player_turn()
{
	if(gameRunning == 1 && player_stay == 1)
	{
		change_turn(DEALER);
	}
}

//Check if theres a winner
static void check_win()
{
	winner = NONE;

	//Player out weighs dealer and is not over 21 || dealer is over 21
	if( (player_stay == 1 && dealer_stay == 1 && player_hand <= 21 && player_hand > dealer_hand) || (dealer_hand > 21) )
	{
		msg = NONE;
		winner = PLAYER;

		player_pot += player_bet * 2;

	}

	//Dealer out weighs player and is not over 21 || player is over 21
	if( (player_stay == 1 && dealer_stay == 1 && dealer_hand <= 21 && dealer_hand > player_hand) || (player_hand > 21) )
	{
		msg = NONE;
		winner = DEALER;

		player_pot -= player_bet;
	}

	//Tie Game
	if(player_stay == 1 && dealer_stay == 1 && player_hand == dealer_hand)
	{
		msg = NONE;
		winner = TIE;

		player_pot += player_bet;
	}

	//Winner was found.. Stop the game!
	if(winner != NONE)
	{
		player_bet = 0;

		draw_window();
		usleep(5000000);

		if(player_pot <= 0)
		{
			player_pot = POT_DEFAULT;
			msg = GAMEOVER;

			draw_window();
			usleep(2000000);
		}

		reset();
	}
}


//Reads pot from file in SAVEFILE
static void readPot()
{
	FILE *input;
	if ((input = fopen(SAVEFILE, "r")) == NULL)
	{
		perror(SAVEFILE);
		return;
	}
	fscanf(input, "%li", &player_pot);
	fclose(input);

	if(player_pot <= 0)
	{
		player_pot = POT_DEFAULT;
	}
}

//Writes pot to file in SAVEFILE
static void writePot()
{
	FILE *output;
	if ((output = fopen(SAVEFILE, "w")) == NULL)
	{
		perror(SAVEFILE);
		return;
	}
	fprintf(output, "%li", player_pot);
	fclose(output);
}

//Converts card index to letter
static char* cardLetter(int card)
{
	static char buf[3];

	if(card <= 8)
	{
		sprintf(buf, "%i", card + 2);
	}else{
		switch(card)
		{
			case 9:
				strcpy(buf, "J");
			break;

			case 10:
				strcpy(buf, "Q");
			break;

			case 11:
				strcpy(buf, "K");
			break;

			case 12:
				strcpy(buf, "A");
			break;
		}
	}
	return buf;
}

//Decides on what header to use
static void draw_header()
{
	pz_draw_header ("BlackJack");
}

static void draw_msg()
{
	char buf[128];
	int width, height, depth;

	strcpy(buf, "");

	if(winner != NONE)
	{
		switch(winner)
		{
			case PLAYER:
				strcpy(buf, "You Win!");
			break;

			case DEALER:
				strcpy(buf, "Dealer Wins!");
			break;

			case TIE:
				strcpy(buf, "Tie Game!");
			break;
		}
	}

	if(msg != NONE)
	{
		switch(msg)
		{
			case BET:
				strcpy(buf, "Place Your Bet");
			break;

			case GAMEOVER:
				strcpy(buf, "Game Over");
			break;

			case DEALER_HIT:
				strcpy(buf, "Dealer Drew Card");
			break;

			case DEALER_STAY:
				strcpy(buf, "Dealer Chose To Stay");
			break;
		}
	}

	GrGetGCTextSize (blackjack_gc, buf, -1, GR_TFASCII, &width, &height, &depth);
	GrText(blackjack_wid, blackjack_gc,
	(screen_info.cols / 2) - (width / 2), 15, buf,  -1, GR_TFASCII);
}

//Draws the main GUI
static void draw_window()
{
	/**0**************************/
	/**1*XXX    BlackJack    XXX**/
	/**2*******XXXXX Wins!********/
	/**3**************************/
	/**4**************************/
	/**5**************************/
	/**6**************************/
	/**7**************************/
	/**8***Dealer*****Your Hand***/
	/**9***#######****#######*****/
	/*10*****(##)*******(##)******/
	/*11**************************/
	/*12*Bid      STAY       Pot**/
	/*13*110               11100**/
	/*14**************************/

	char buf[128];
	int i, width, height, depth;

	GrSetGCUseBackground(blackjack_gc, GR_TRUE);
	GrSetGCBackground(blackjack_gc, GR_RGB(255,255,255));

	GrSelectEvents(blackjack_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN|
				   GR_EVENT_MASK_KEY_UP|GR_EVENT_MASK_TIMER);

	GrSetGCForeground(blackjack_gc, GR_RGB(255,255,255));

	GrFillRect(blackjack_wid, blackjack_gc, 0, 0,
			   screen_info.cols, screen_info.rows - HEADER_TOPLINE);

	GrSetGCForeground(blackjack_gc, GR_RGB(0,0,0));

	/*1*/		draw_header();

	/*2*/		draw_msg();


	/*8-9-10*/	if(gameRunning == 1)
				{
					/*8-1*/	GrGetGCTextSize (blackjack_gc, "Dealer", -1, GR_TFASCII, &width, &height, &depth);
							GrText(blackjack_wid, blackjack_gc,
								  (screen_info.cols / 3.5) - (width / 2),
								   45, "Dealer",  -1, GR_TFASCII);

					/*9-1*/ strcpy(buf, " ");

							for(i=0;i<dealer_ncards;i++)
							{
								if(winner == NONE)
								{
									sprintf(buf, "%s%s ", buf, (i == 1) ? cardLetter(dealer_cards[i]) : "X");
								}else{
									sprintf(buf, "%s%s ", buf, cardLetter(dealer_cards[i]));
								}
							}

							GrGetGCTextSize (blackjack_gc, buf, -1, GR_TFASCII, &width, &height, &depth);
							GrText(blackjack_wid, blackjack_gc,
								  (screen_info.cols / 3.5) - (width / 2),
								   58, buf,  -1, GR_TFASCII);

					/*10-1*/if(winner != NONE)
							{
								sprintf(buf, "(%i)", dealer_hand);
								GrGetGCTextSize (blackjack_gc, buf, -1, GR_TFASCII, &width, &height, &depth);
								GrText(blackjack_wid, blackjack_gc,
									  (screen_info.cols / 3.5) - (width / 2),
									   70, buf,  -1, GR_TFASCII);
							}


					/*8-2*/	GrGetGCTextSize (blackjack_gc, "You Hand", -1, GR_TFASCII, &width, &height, &depth);
							GrText(blackjack_wid, blackjack_gc,
								  (screen_info.cols / 1.5) - (width / 2),
								   45, "Your Hand",  -1, GR_TFASCII);

					/*9-2*/ strcpy(buf, " ");

							for(i=0;i<player_ncards;i++)
							{
								sprintf(buf, "%s%s ", buf, cardLetter(player_cards[i]));
							}

							GrGetGCTextSize (blackjack_gc, buf, -1, GR_TFASCII, &width, &height, &depth);
							GrText(blackjack_wid, blackjack_gc,
								  (screen_info.cols / 1.5) - (width / 2),
								   58, buf,  -1, GR_TFASCII);

					/*10-2*/sprintf(buf, "(%i)", player_hand);
							GrGetGCTextSize (blackjack_gc, buf, -1, GR_TFASCII, &width, &height, &depth);
								GrText(blackjack_wid, blackjack_gc,
								  	  (screen_info.cols / 1.5) - (width / 2),
								   	   70, buf,  -1, GR_TFASCII);
				}


	/*12-1*/	GrText(blackjack_wid, blackjack_gc, 1,
				   screen_info.rows - 37, "Bet",  -1, GR_TFASCII);

	/*12-2*/	if(player_stay == 1 && winner == NONE)
				{
					GrGetGCTextSize (blackjack_gc, "STAY", -1, GR_TFASCII, &width, &height, &depth);
					GrText(blackjack_wid, blackjack_gc,
						  (screen_info.cols / 2) - (width / 2),
						   screen_info.rows - 37, "STAY",  -1, GR_TFASCII);
				}

	/*12-3*/	GrText(blackjack_wid, blackjack_gc,
					   screen_info.cols - 18,
					   screen_info.rows - 37, "Pot",  -1, GR_TFASCII);


	/*13-1*/		sprintf(buf, "%ld", player_bet);
				GrText(blackjack_wid, blackjack_gc, 1,
				screen_info.rows - 25, buf,  -1, GR_TFASCII);

	/*13-2*/		sprintf(buf, "%ld", player_pot);
				GrGetGCTextSize (blackjack_gc, buf, -1, GR_TFASCII, &width, &height, &depth);
				GrText(blackjack_wid, blackjack_gc,
					   screen_info.cols - (width + 2),
					   screen_info.rows - 25, buf,  -1, GR_TFASCII);
					   

}

static void draw_help()
{
	//Hold:			Help Menu
	//Play/Pause:	Start/Stay
	//Action:		Hit
	//Wheel:		Dec/Inc Bid
	//Prev/Next:	Dec/Inc Bid
	//Version:		X.XX

	int i, width, height, depth;
	char *help[] =
	{
	"Hold:", "Help",
	"Play/Pause:", "Start/Stay",
	"Action:", "Hit",
	"Wheel:", "Dec/Inc Bid",
	"Prev/Next:", "Dec/Inc Bid",
	"", "",
	"Version:", VERSION,
	0
	};

	GrSetGCUseBackground(blackjack_gc, GR_TRUE);
	GrSetGCBackground(blackjack_gc, GR_RGB(255,255,255));

	GrSelectEvents(blackjack_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN|
				   GR_EVENT_MASK_KEY_UP|GR_EVENT_MASK_TIMER);

	GrSetGCForeground(blackjack_gc, GR_RGB(255,255,255));

	GrFillRect(blackjack_wid, blackjack_gc, 0, 0,
			   screen_info.cols, screen_info.rows - HEADER_TOPLINE);


	GrSetGCForeground(blackjack_gc, GR_RGB(0,0,0));

	for(i=0; help[i] != 0; i++)
	{
		GrGetGCTextSize (blackjack_gc, help[i], -1, GR_TFASCII, &width, &height, &depth);

		if(i % 2 == 0)
		{
			GrText(blackjack_wid, blackjack_gc, 5, (i * 7) + 20, help[i], -1, GR_TFASCII);
		}else{
			GrText(blackjack_wid, blackjack_gc,
				   screen_info.cols - (width + 5),
				  ((i - 1) * 7) + 20, help[i],  -1, GR_TFASCII);
		}
	}
}
