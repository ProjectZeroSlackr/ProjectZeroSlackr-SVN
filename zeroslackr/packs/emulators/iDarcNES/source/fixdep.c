/*
 * fixdep.c
 *
 * A quick program to fix the .d files used for dependancies
 */

/* $Id: fixdep.c,v 1.3 2000/04/15 15:29:31 nyef Exp $ */

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char buffer[8192]; /* should be more than enough */
    char *bufptr;

    if (argc == 2) {
	strcpy(buffer, argv[1]);
	bufptr = buffer + strlen(buffer);
    } else {
	bufptr = buffer;
    }
    
    bufptr = bufptr + fread(bufptr, 1, 8191, stdin);
    *bufptr = '\0';
    fputs(buffer, stdout);

    for (bufptr = buffer; *bufptr != '\0'; bufptr++) {
	if (*(bufptr+1) == ':') {
	    *bufptr = 'd';
	    break;
	}
    }
    fputs(buffer, stdout);

    return 0;
}

/*
 * $Log: fixdep.c,v $
 * Revision 1.3  2000/04/15 15:29:31  nyef
 * added an optional directory prefix parameter (workaround for gcc misfeature)
 *
 * Revision 1.2  2000/04/15 02:05:18  nyef
 * fixed typo that broke dependancies on any file with a '0' in the name
 *
 * Revision 1.1  2000/03/20 03:35:39  nyef
 * Initial revision
 *
 */
