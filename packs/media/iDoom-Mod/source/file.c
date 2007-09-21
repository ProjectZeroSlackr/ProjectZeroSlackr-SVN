#include <unistd.h>
#include "file.h"

int filelength(int fileDesc)
{
	int current = lseek(fileDesc, 0, SEEK_CUR);
	int length  = lseek(fileDesc, 0, SEEK_END);
	lseek(fileDesc, current, SEEK_SET);
	
	return length;
}
