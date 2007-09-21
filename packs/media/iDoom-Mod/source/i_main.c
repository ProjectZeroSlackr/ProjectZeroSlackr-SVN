// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_main.c,v 1.4 1997/02/03 22:45:10 b1 Exp $";



#include "doomdef.h"

#include "m_argv.h"
#include "d_main.h"

int
main
( int		argc,
//  char**	argv ) 
  char*	argv[] ) 
{ 
 
	// KERIPO MOD
	printf("Loading up iDoom-Mod\n\n");
	// Pass on wad locations
	char arg_share_wad[64]; // Ultimate Doom
	char arg_doom_wad[64]; // Shareware
	char arg_doom2_wad[64]; // Doom II
	sprintf(arg_share_wad, "%s", argv[1]);
	sprintf(arg_doom_wad, "%s", argv[2]);
	sprintf(arg_doom2_wad, "%s", argv[3]);
	printf("doom1.wad: %s\n", argv[1]);
	printf("doom.wad: %s\n", argv[2]);
	printf("doom2.wad: %s\n", argv[3]);
	
    myargc = argc; 
    myargv = argv; 

	
	//D_DoomMain (); 
    D_DoomMain (arg_share_wad, arg_doom_wad, arg_doom2_wad); 

    return 0;
} 
