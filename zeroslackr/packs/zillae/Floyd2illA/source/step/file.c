#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef IPOD
#include <linux/vt.h>
#endif
#include <dirent.h>
#include <string.h>

#include "../pz.h"
#include "tstep.h"

extern tstep_global *tstep_g;

int countCommas(const char *s)
{
	int i=0,l=strlen(s),c=0;
	for(i=0;i<l;i++)
		if(s[i]==',')
			c++;
	return c;
}

char * cutLSpaces(const char * s)
{
	//cuts the leading spaces
	int i=0;
	while(s[i]==' ')
		i++;
	return (char *) &s[i];
}

inline unsigned char getStepByte(char c)
{
	switch(c)
	{
		case '0':
			return 0;
			break;
		case 'a':
		case 'A':
			return bUP_ARROW|bDOWN_ARROW;
			break;
		case 'b':
		case 'B':
			return bLEFT_ARROW|bRIGHT_ARROW;
			break;
		case '1':
			return bLEFT_ARROW|bDOWN_ARROW;
			break;
		case '2':
			return bDOWN_ARROW;
			break;
		case '3':
			return bDOWN_ARROW|bRIGHT_ARROW;
			break;
		case '4':
			return bLEFT_ARROW;
			break;
		case '6':
			return bRIGHT_ARROW;
			break;
		case '7':
			return bLEFT_ARROW|bUP_ARROW;
			break;
		case '8':
			return bUP_ARROW;
			break;
		case '9':
			return bUP_ARROW|bRIGHT_ARROW;
			break;
		
	}
	return 0;
}

int getTag(char* data, char **tag, char ** tdata, const char d)
{
//chops data string into tag and tdata
//can only use once. use getTagSafe if handling other things
// d is the delineater character
	int l=0,len=strlen(data);
	if(!data)
		return 0;
	while(data[l]!=d)
	{
		l++;
		if(l>=len)
			return 0;
	}
	data[l]='\0';
	*tag=(char *)data;
	l++;
	*tdata=(char *)&data[l];
	return 1;
}

int getTagSafe(char* data, char* tag, char* tdata)
{
	//saves the data buffer
	int l=0,len=strlen(data),j=0;
	if(!data)
		return 0;
	if(!tag)
		return 0;
	if(!tdata)
		return 0;
	while(data[l]!=':')
	{
		tag[l]=data[l];
		l++;
		if(l>=len)
			return 0;
	}
	tag[l]='\0';
	for(l++;l<len;l++)
	{
		if(data[l]!=';')
		{
			tdata[j]=data[l];
			j++;
		}
		else
			return 0;
	}
	if(j==0)
		return 0;
	if(tdata[j-1]!='\0')
		tdata[j]='\0';
	return 1;
}
int importSteps(const char *s)
{
	int scount=0, fcount=0,i=0,l=strlen(s);
	int beatcount=0; //beat count
	int obeatcount=0;
	int obeat=1; //off beat (8th notes default)
	char tStep;
	//count steps rough estimate, will be off slightly with <> steps (occur rarely though)
	//also count freeze steps
	for(i=0;i<l;i++)
	{
		if(s[i]=='!')
		{
			fcount++;
			i++; //skip the freeze step
		}
		else if(s[i]=='<'||s[i]=='>'||s[i]=='\''||s[i]=='`'||
			s[i]=='('||s[i]==')'||s[i]=='0'||s[i]=='['||s[i]==']'||
			s[i]=='{'||s[i]=='}')
		{		
		}
		else
			scount++;
	}
    tstep_g->steps = (stepType *) malloc (sizeof(stepType)*scount);
    tstep_g->stepFreezes = (stepType *) malloc (sizeof(stepType)*fcount);
	tstep_g->nFreezeArrows=fcount;
	scount=0;
	fcount=0;
	for(i=0;i<l;i++)
	{
		switch(s[i])
		{
			case '!':
				i++;
				tStep=0;
				if(s[i]=='<')
				{
					if(i==l-1)
						return 0;
					i++;
					while(s[i]!='>')
					{
						tStep|=getStepByte(s[i]);
						i++;
						if(i==l)
							return 0;
					}
				}
				else
					tStep=getStepByte(s[i]);
				tstep_g->stepFreezes[fcount].stepByte=tStep;
				tstep_g->stepFreezes[fcount].beat=(beatcount * pNum)/2+(obeatcount*pNum)/(obeat*2);				
				fcount++;
				break;
			case '<':
				tStep=0;
				if(i==l-1)
					return 0;
				i++;
				while(s[i]!='>')
				{
					tStep|=getStepByte(s[i]);
					i++;
					if(i==l)
						return 0;
				}
				tstep_g->steps[scount].stepByte=tStep;
				tstep_g->steps[scount].beat=(beatcount * pNum)/2+(obeatcount*pNum)/(obeat*2);				
				scount++;
				obeatcount++;
				if(obeatcount>=obeat)
				{
					beatcount++;
					obeatcount=0;
				}
				break;
			case '(':
				obeat = 2;
				break;
			case '[':
				obeat = 3;
				break;
			case '{':
				obeat = 8;
				break;
			case '`':
				obeat = 24;
				break;
			case ')':
			case ']':
			case '}':
			case '\'':
				if(obeatcount!=0)
				{
					//should be zero, shouldn't it?
					printf("Error Parsing File (Steps)\n");
				}
				obeat = 1;
				break;
			case ' ': //ignore spaces
				;
			default:
				tStep=getStepByte(s[i]);
				if(tStep!=0)
				{
					tstep_g->steps[scount].stepByte=tStep;
					tstep_g->steps[scount].beat=(beatcount * pNum)/2+(obeatcount*pNum)/(obeat*2);				
					scount++;
				}
				obeatcount++;
				if(obeatcount>=obeat)
				{
					beatcount++;
					obeatcount=0;
				}
		}
	}
	tstep_g->nSteps=scount;
	tstep_g->nFreezeArrows=fcount;
	for(i=0;i<scount;i++)
		tstep_g->steps[i].stepByte |= ((tstep_g->steps[i].stepByte)<<4); //is this absolutely necessary? yes, yes it is
//    steps = (stepType *) malloc (sizeof(stepType)*scount);
//    stepFreezes = (stepType *) malloc (sizeof(stepType)*fcount);
//	free(steps);
//	free(stepFreezes);
	printf("Step Count %d %d\n",scount,(int)strlen(s));
	printf("Freeze Count %d %d\n",fcount,(int)strlen(s));
	return 1;
}
int handleTagInfo(char *tagd, char *data, songInfo *song)
{
// could merge this with handleTag, but, there would
// be a lot of ifs
	char *tag = cutLSpaces(tagd);
	//int i;
	if(strcmp(tag,"#TITLE")==0)
	{
		song->title = (char*) malloc (strlen(data)+1);				
		strcpy(song->title,data);
		song->title[strlen(data)]='\0';
	}
	else if(strcmp(tag,"#ARTIST")==0)
	{
		song->artist = (char*) malloc (strlen(data)+1);				
		strcpy(song->artist,data); 
		song->artist[strlen(data)]='\0';
	}
	else if(strcmp(tag,"#GAP")==0) ;
	else if(strcmp(tag,"#BPM")==0) ;
	else if(strcmp(tag,"#DISPLAYTITLE")==0) ;
	else if(strcmp(tag,"#DISPLAYARTIST")==0) ;
	else if(strcmp(tag,"#DISPLAYBPM")==0) ;
	else if(strcmp(tag,"#FILE")==0) ;
	else if(strcmp(tag,"#MD5")==0) ;
	else if(strcmp(tag,"#CHANGEBPM")==0) ;
	else if(strcmp(tag,"#STATUS")==0) ;
	else if(strcmp(tag,"#GENRE")==0) ;
	else if(strcmp(tag,"#CDTITLE")==0) ;
	else if(strcmp(tag,"#SAMPLESTART")==0) ;
	else if(strcmp(tag,"#SAMPLELENGTH")==0) ;
	else if(strcmp(tag,"#RANDSEED")==0) ;
	else if(strcmp(tag,"#RANDSTART")==0) ;
	else if(strcmp(tag,"#RANDFOLDER")==0) ;
	else if(strcmp(tag,"#RANDLIST")==0) ;
	else if(strcmp(tag,"#SINGLE")==0)
	{
		char *difficulty, *feet, *s;
		getTag(data,&difficulty,&feet,':');
		getTag(feet,&feet,&s,':');
		printf("%s %s %s\n",difficulty, feet, s);
		if(strcmp(difficulty,"BEGINNER")==0)
			song->difficulty[tsBEGINNER]=atoi(feet);
		else if(strcmp(difficulty,"BASIC")==0)
			song->difficulty[tsLIGHT]=atoi(feet);		
		else if(strcmp(difficulty,"ANOTHER")==0)
			song->difficulty[tsSTANDARD]=atoi(feet);		
		else if(strcmp(difficulty,"MANIAC")==0)
			song->difficulty[tsHEAVY]=atoi(feet);		
		else if(strcmp(difficulty,"SMANIAC")==0)
			song->difficulty[tsCHALLENGE]=atoi(feet);
	}
	else if(strcmp(tag,"#DOUBLE")==0) ;
	else if(strcmp(tag,"#COUPLE")==0) ;
	else if(strcmp(tag,"#SOLO")==0) ;
	else if(strcmp(tag,"#BACKGROUND")==0) ;
	else
	{
		printf("Unknown tag: %s\n",tag);
		return 0;
	}
	return 1;
}

int handleTag(char *tagd, char *data)
{
//step format can be found at: http://dwi.ddruk.com/readme.php
//boring plumbing...
//extremely inefficient, but efficiency is not so important
//can replace later with a faster string compare
	char *tag = cutLSpaces(tagd);
	char *point1;
	char *point2;
	char *point3;	
	int i;
	if(strcmp(tag,"#TITLE")==0)
	{
		tstep_g->songTitle = (char*) malloc (strlen(data)+1);				
		strcpy(tstep_g->songTitle,data);
	}
	else if(strcmp(tag,"#ARTIST")==0)
	{
		tstep_g->artist = (char*) malloc (strlen(data)+1);				
		strcpy(tstep_g->artist,data); 
	}
	else if(strcmp(tag,"#GAP")==0)
	{
		tstep_g->gap=atoi(data);
	}
	else if(strcmp(tag,"#BPM")==0)
	{
		tstep_g->startBPM=(int)(atof(data)*pNum);	
	}
	else if(strcmp(tag,"#DISPLAYTITLE")==0) ;
	else if(strcmp(tag,"#DISPLAYARTIST")==0) ;
	else if(strcmp(tag,"#DISPLAYBPM")==0) ;
	else if(strcmp(tag,"#FILE")==0) ;
	else if(strcmp(tag,"#MD5")==0) ;
	else if(strcmp(tag,"#FREEZE")==0)
	{
		tstep_g->nSongFreeze = countCommas(data)+1;
		tstep_g->songFreezes = (songFreeze *) malloc (sizeof(songFreeze)* tstep_g->nSongFreeze);
		point3 = data;
		for (i=0;i<tstep_g->nSongFreeze-1;i++)
		{
			getTag(point3,&point1,&point3,',');
			getTag(point1,&point1,&point2,'=');
			tstep_g->songFreezes[i].beat=atoi(point1);
			tstep_g->songFreezes[i].lengthms=atoi(point2);
		}
		getTag(point3,&point1,&point2,'=');
		tstep_g->songFreezes[i].beat=atoi(point1);
		tstep_g->songFreezes[i].lengthms=atoi(point2);
	}
	else if(strcmp(tag,"#CHANGEBPM")==0)
	{
		tstep_g->nBeatChange = countCommas(data)+1;
		tstep_g->beatChanges = (beatChange *) malloc (sizeof(beatChange)*tstep_g->nBeatChange);
		point3 = data;
		for (i=0;i<tstep_g->nBeatChange-1;i++)
		{
			getTag(point3,&point1,&point3,',');
			getTag(point1,&point1,&point2,'=');
			tstep_g->beatChanges[i].beat=atoi(point1);
			tstep_g->beatChanges[i].newBPM=(int)(atof(point2)*pNum);
		}
		getTag(point3,&point1,&point2,'=');
		tstep_g->beatChanges[i].beat=atoi(point1);
		tstep_g->beatChanges[i].newBPM=(int)(atof(point2)*pNum);
	}
	else if(strcmp(tag,"#STATUS")==0) ;
	else if(strcmp(tag,"#GENRE")==0) ;
	else if(strcmp(tag,"#CDTITLE")==0) ;
	else if(strcmp(tag,"#SAMPLESTART")==0) ;
	else if(strcmp(tag,"#SAMPLELENGTH")==0) ;
	else if(strcmp(tag,"#RANDSEED")==0) ;
	else if(strcmp(tag,"#RANDSTART")==0) ;
	else if(strcmp(tag,"#RANDFOLDER")==0) ;
	else if(strcmp(tag,"#RANDLIST")==0) ;
	else if(strcmp(tag,"#SINGLE")==0)
	{
		char *difficulty, *feet, *s;
		getTag(data,&difficulty,&feet,':');
		getTag(feet,&feet,&s,':');
		printf("%s %s %s\n",difficulty, feet, s);
		if(strcmp(difficulty,"MANIAC")==0)
			importSteps(s);
	}
	else if(strcmp(tag,"#DOUBLE")==0) ;
	else if(strcmp(tag,"#COUPLE")==0) ;
	else if(strcmp(tag,"#SOLO")==0) ;
	else if(strcmp(tag,"#BACKGROUND")==0) ;
	else
	{
		printf("Unknown tag: %s\n",tag);
		return 0;
	}
	return 1;
}
static unsigned int isDWI(char *filename)
{
	char *ext;
	ext = strrchr(filename, '.');
	if(!ext)
		return 0;
	return (strcmp(ext, ".dwi") == 0)||(strcmp(ext, ".DWI") == 0);
}

void loadSong(char* filename, songInfo* song)
{
	FILE * pFile;
	long lSize;
	char * buffer;
	int l=0;
	int j=0;
	int comment = 0;
	char buff[16000];
	char *buff2;
	char *buff3;
	pFile = fopen ( filename , "rb" );
  if (pFile==NULL)
  { 
  printf("Bad News, cannot read file\n");
  exit (1);
  }

  // obtain file size.
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file.
  buffer = (char*) malloc (lSize);
  if (buffer == NULL) exit (2);

  // copy the file into the buffer.
  fread (buffer,1,lSize,pFile);

  /*** the whole file is loaded in the buffer. ***/

	for(l=0;l<lSize;l++)
	{
		if(!buffer[l])
			break;
		if(buffer[l]!='\n'&&buffer[l]!='\r')
		{
			if (comment) ;
			else if(buffer[l]!=';')
			{
				if(buffer[l]=='/'&&buffer[l+1]=='/')
				{
					comment=1;
				}
				else
				{
					buff[j]=buffer[l];
					j++;
				}
			}
			else
			{
				buff[j]='\0';
				j=0;
				if(getTag(buff,&buff2,&buff3,':'))
				{
					if(song==NULL)
						handleTag(buff2,buff3);
					else
						handleTagInfo(buff2,buff3,song);
				}
				else
					printf("Error parsing tags\n");
			}					
		}
		else
			comment=0;
	}
	if(j>2) //possibly a left over tag
	{
		buff[j]='\0';
		printf("junk left over: %s\n",buff);	
	}
  // terminate
  fclose (pFile);
  free (buffer);
}

void findSongs()
{
//	char *path=SONG_PATH,*newPath = (char *)malloc(101);
	DIR *dir, *dir2, *dir3;
	struct stat stat_result,stat_result2;
	struct dirent *subdir, *sub2dir,*sub3dir;
	int albumSongs[100];
	char albumPaths[100][255];
	int i,j,k,counter,numAlbums;
	chdir(SONG_PATH);
	dir = opendir("./");
	while ((subdir = readdir(dir)))
	{
		if (strncmp(subdir->d_name, ".", strlen(subdir->d_name)) != 0 &&
			strncmp(subdir->d_name, "..", strlen(subdir->d_name)) != 0) {
			stat(subdir->d_name, &stat_result);
			if (S_ISDIR(stat_result.st_mode)) 
			{
				chdir(subdir->d_name);
				dir2 = opendir("./");
				counter=0;
				while((sub2dir = readdir(dir2)))
				{
					if (strncmp(sub2dir->d_name, ".", strlen(sub2dir->d_name)) != 0 &&
						strncmp(sub2dir->d_name, "..", strlen(sub2dir->d_name)) != 0) {
						stat(sub2dir->d_name, &stat_result2);
						if (S_ISDIR(stat_result2.st_mode)) 
						{
							chdir(sub2dir->d_name);
							dir3 = opendir("./");
							while((sub3dir = readdir(dir3)))
							{
								if(isDWI(sub3dir->d_name))
								{
									counter++;
									printf("found DWI: %s\\%s\n",sub2dir->d_name,sub3dir->d_name);
								}
							}
							chdir("..");
						}
						else
						{
							if(isDWI(sub2dir->d_name))
							{
								counter++;
								printf("found DWI: %s\n",sub2dir->d_name);
							}
						}
					}
				}
				if(counter>0)
				{
					albumSongs[numAlbums]=counter;
					strcpy(albumPaths[numAlbums],subdir->d_name);
					albumPaths[numAlbums][strlen(subdir->d_name)];
					numAlbums++;
				}
				chdir("..");
			}
		}
	}
	tstep_g->albums = (albumInfo **)malloc(sizeof(albumInfo *)*numAlbums);	
	tstep_g->numAlbums=numAlbums;
	chdir(SONG_PATH);
	for(i=0;i<numAlbums;i++)
	{
		tstep_g->albums[i]=(albumInfo *)malloc(sizeof(albumInfo));
		tstep_g->albums[i]->path=(char *)malloc(sizeof(char)*(strlen(albumPaths[i])+1));
		strcpy(tstep_g->albums[i]->path,albumPaths[i]);
		tstep_g->albums[i]->path[strlen(albumPaths[i])]='\0';
		printf("Album Songs: %d\n", albumSongs[i]);
		tstep_g->albums[i]->songs = (songInfo **)malloc(sizeof(songInfo *)*albumSongs[i]);
		chdir(tstep_g->albums[i]->path);
		j=0;
		dir = opendir("./");
		while((subdir = readdir(dir)))
		{
			if (strncmp(subdir->d_name, ".", strlen(subdir->d_name)) != 0 &&
				strncmp(subdir->d_name, "..", strlen(subdir->d_name)) != 0)
			{
				stat(subdir->d_name, &stat_result);
				if (S_ISDIR(stat_result.st_mode))
				{
					chdir(subdir->d_name);
					dir2 = opendir("./");
					while((sub2dir = readdir(dir2)))
					{
						if(isDWI(sub2dir->d_name)){
						printf("found DWI 2: %s\\%s\n",subdir->d_name,sub2dir->d_name);
						tstep_g->albums[i]->songs[j] = (songInfo *)malloc(sizeof(songInfo));
						for(k=0;k<NUM_DIFFICULTIES;k++)
							tstep_g->albums[i]->songs[j]->difficulty[k]=-1;
						loadSong(sub2dir->d_name,tstep_g->albums[i]->songs[j]);
						j++;}
					}
					chdir("..");
				}
				if(isDWI(subdir->d_name))
				{
					tstep_g->albums[i]->songs[j] = (songInfo *)malloc(sizeof(songInfo));
					for(k=0;k<NUM_DIFFICULTIES;k++)
						tstep_g->albums[i]->songs[j]->difficulty[k]=-1;
					loadSong(subdir->d_name,tstep_g->albums[i]->songs[j]);
					j++;
				}
			}
		}
		tstep_g->albums[i]->numSongs = j;
		chdir("..");
	}
	chdir(SONG_PATH);
	printf("Yay\n");
}

void writeAlbumList(char *filename)
{
	FILE *wfile;
	int i,j,k;
	if ((wfile = fopen(filename,"w"))) 
	{
		fprintf(wfile,"StepTest Album List\n");
		for(i=0;i<tstep_g->numAlbums;i++)
		{
			fprintf(wfile,"#ALBUM:%s;\n",tstep_g->albums[i]->path);
			for(j=0;j<tstep_g->albums[i]->numSongs;j++)
			{
				fprintf(wfile,"#SONG:%s",tstep_g->albums[i]->songs[j]->title);
				for(k=0;k<NUM_DIFFICULTIES;k++)
					fprintf(wfile,":%d",tstep_g->albums[i]->songs[j]->difficulty[k]);
				fprintf(wfile,";\n");
			}			
		}
		fclose(wfile);
	}
}
