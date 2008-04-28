/*
 * Copyright (C) 2006 Terry Stenvold & Armon Khosravi
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
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pz.h"
#include <ttk.h>

static TWindow *window;
static TWidget *wid;
static ttk_surface iracer_srf, car[12];
static ttk_surface menu_bg, menu_sbg, menu_wheel;

struct MAPSTUFF {
	 char *author;
	 char *mapname;
	 char imgpath[30];
	 short int difficulty;
	 short int xstart;
	 short int ystart;
	 short int cstart;
	 float cc50[3];
	 float cc100[3];
	 float cc150[3];
	 ttk_surface image;
};

struct SETSTUFF {
	int laps;
	int cc;
	int gdif;
};

static char *dif_opts[] = {
	 "Stupid Easy", "Easy",
	 "Sub-Par", "Normal",
	 "Hard", "Really Hard"
};
static char *onoff_opts[] = {
	 "On", "Off"
};
static char *menu_opts[] = {
	 "Start Race ->", "<- Map ->",
	 "<- Settings ->", "<- Exit Game"
};
static const short set_laps[] = { 3, 5, 7 };
static const short set_cc[] = { 50, 100, 150 };

static int bgw, bgh;
static char direc[30];
static short int frame=0, option=0;
static short int iprc=0, mapc=1;
static struct MAPSTUFF map;
static struct SETSTUFF settings;
static short int xoff=3, yoff=0,cpos, count=3,wall=0,wall2=0,lap=1, cheating=0;
static short int z=0, ipods[321],lphold=0, rhigh=0;
static int fina, speed=1;
static float ipod=1, times=0,timed=0;
static ttk_color texts;

static void ipodset()
{
int i=1;

	for(i=1; i<=320; i++){
		ipods[i]=i*ipod;
	}

}

static int right(char *str, char *search)
{
	char *find;
	return ((find = strstr(str, search)) && (strcasecmp(find, search) == 0));
}

static void iracer_scan(const char *dirname)
{
	DIR *dir = opendir(dirname);
	struct dirent *wdir;
	if(!dir) {
		return;
	}
	while ((wdir = readdir(dir))) {
		if (right(wdir->d_name, ".ipr")) {
			iprc++;
		}
	}
	mapc=1;
	closedir(dir);
}

static void write_high()
{
         FILE *fp;
	 char filename[30];
	 int t = 0;
	 
    	sprintf(filename, "%smap%d.ipr",direc,mapc);

	 	 if(!(fp = fopen(filename, "r"))){
       		  	new_message_window("Could not Open Map");
		 }

	 fp = fopen(filename, "w");

	switch(settings.cc){
     	  case 2: map.cc150[settings.laps]=(times/11);  break; //150cc
      	  case 1: map.cc100[settings.laps]=(times/11);  break; //100cc
       	  case 0: map.cc50[settings.laps]=(times/11);   break;  //50cc;
   	}

	fprintf(fp, "%s\n", map.author);
	fprintf(fp, "%s\n", map.mapname);
	fprintf(fp, "%d\n", map.difficulty);
	fprintf(fp, "%d\n", map.xstart);
	fprintf(fp, "%d\n", map.ystart);
	fprintf(fp, "%d\n", map.cstart);
	for(t=0;t<3;t++){fprintf(fp, "%2.1f\n", map.cc50[t]);}
	for(t=0;t<3;t++){fprintf(fp, "%2.1f\n", map.cc100[t]);}
	for(t=0;t<3;t++){fprintf(fp, "%2.1f\n", map.cc150[t]);}
	
	fclose(fp);
	
	new_message_window("Saved New Time.");
	
	frame=0;
	   
}


//---------------------------LOADING OF CARS, MAPS, AND (saving)SETTINGS------------------------

static void find_dir()
{	

	FILE *fp;

	// KERIPO MOD
	//sprintf(direc, "/hp/aj/iracer_maps/");
	sprintf(direc, "/opt/Zillae/ZacZilla/Data/iracer_maps/");

	if(!(fp = fopen(direc, "r"))) {
    		sprintf(direc, "/mnt/iracer_maps/");
	

		 if(!(fp = fopen(direc, "r"))){
			sprintf(direc, "./iracer_maps/");

		 	if(!(fp = fopen(direc, "r"))){
       		  		new_message_window("Could not Open Map");
			}
	  
	   	}

	}


}



static void iracer_scale_images()
{

	int x; 

	switch (ttk_screen->w){ 

	 case 220: 	
			ipod=0.6875;
			break;
	
	 case 176: 	
			ipod=0.55;
			break;

	 case 160: 	
			ipod=0.5;
			break;

	case 138: 	
			ipod=0.43125;
			break;
	}

			for(x=0;x<12;x++){ car[x] = ttk_scale_surface(car[x],ipod);};
			menu_bg = ttk_scale_surface(menu_bg,ipod);
	  		menu_sbg = ttk_scale_surface(menu_sbg,ipod);
	  		menu_wheel = ttk_scale_surface(menu_wheel,ipod);

	ttk_surface_get_dimen (menu_bg, &bgw, &bgh);
	iracer_srf = ttk_new_surface (bgw, bgh, ttk_screen->bpp);


}



static void load_map()
{
   	 FILE *fp;
	 char buffer[120],filename[30];
	 int x = 0,t = 0 ;
	 
	 if(mapc == 0) { mapc=1; }	 
	 else if(mapc == 5) { mapc=4;}	

		sprintf(filename, "%smap%d.ipr",direc,mapc);

	 	 if(!(fp = fopen(filename, "r"))){
       		  	new_message_window("Could not Open map");
		 }

	for(t=0;t<3;t++){ //flush times incase == 0 for no high time.
		map.cc50[t] = 0;
		map.cc100[t] = 0;
		map.cc150[t] = 0;
	}

	 fp = fopen(filename, "r");

	while(!feof(fp)) {	//Loading *.ipr into pointers

		 fgets(buffer, 80, fp);
		 buffer[strlen(buffer)-1] = '\0';


	 switch(x) {
         case 0: map.author = strdup(buffer); break;
   	 case 1: map.mapname = strdup(buffer); break;
   	 case 2: map.difficulty = atoi(buffer); break;
   	 case 3: map.xstart = atoi(buffer); break;
	 case 4: map.ystart = atoi(buffer); break;
	 case 5: map.cstart = atoi(buffer); break;
	 case 6: map.cc50[0] = atof(buffer); break;
	 case 7: map.cc50[1] = atof(buffer); break;
	 case 8: map.cc50[2] = atof(buffer); break;
	 case 9: map.cc100[0] = atof(buffer); break;
	 case 10: map.cc100[1] = atof(buffer); break;
	 case 11: map.cc100[2] = atof(buffer); break;
	 case 12: map.cc150[0] = atof(buffer); break;
	 case 13: map.cc150[1] = atof(buffer); break;
	 case 14: map.cc150[2] = atof(buffer); break;
   	 }
   	 x++;
   	 if(x == 15) break;
	}

	switch(settings.cc){
     	  case 2: timed=map.cc150[settings.laps]; break; //150cc
      	  case 1: timed=map.cc100[settings.laps]; break; //100cc
       	  case 0: timed=map.cc50[settings.laps]; break;  //50cc;
   	}

    	sprintf(map.imgpath, "%smap%d.gif",direc,mapc);


	 	 if(!(fp = fopen(map.imgpath, "r"))){
       		  	new_message_window("Could not Open map image");
		 }
	 
	map.image = ttk_load_image(map.imgpath);

	switch (ttk_screen->w){ 

	 case 220: 	ipod=0.6875;
			break;
	
	 case 176: 	ipod=0.55;
			break;
	 case 160: 	
			ipod=0.5;
			break;
	case 138: 	
			ipod=0.43125;
			break;
	}

	map.image = ttk_scale_surface(map.image,ipod);

	fclose(fp);
		
	cpos=map.cstart; //give track direction 
		  
}


static void load_car()
{
        FILE *fp; 
	int x;
	char filename[15];
	char file[30];
	 
    		sprintf(filename, "%sc1/",direc);

	 	 if(!(fp = fopen(filename, "r"))){
       		  	new_message_window("Could not Open");
		 }

	 fp = fopen(filename, "r");

	 for(x=0;x<12;x++){
   	 	sprintf(file, "%s%d.gif",filename,x);
    		car[x] = ttk_load_image(file);
	 }
	    

}

static void load_images()
{
	 menu_bg = ttk_load_image ("/opt/Zillae/ZacZilla/Data/bg.gif");
	 menu_sbg = ttk_load_image ("/opt/Zillae/ZacZilla/Data/sbg.gif");
	 menu_wheel = ttk_load_image ("/opt/Zillae/ZacZilla/Data/wheel.gif");
	 	 
}


// KERIPO MOD
// Conflicts with main settings functions
//static int load_settings()
static int ipr_load_settings()
{
	 FILE *fp;
	 char filename[30];
	 
    	sprintf(filename, "%ssettings.conf",direc);

	 	 if(!(fp = fopen(filename, "r"))){
       		  	new_message_window("Could not Open settings");
		 }

	 fp = fopen(filename, "r");

	 fscanf(fp, "%d", &settings.laps);
	 fscanf(fp, "%d", &settings.cc);
	 fscanf(fp, "%d", &settings.gdif);

   fclose(fp);
   
   return 0;
}


// KERIPO MOD
// Conflicts with main settings
//static void save_settings() 
static void ipr_save_settings() 
{
	 FILE *fp;
	 char filename[30];
	 
   	sprintf(filename, "%ssettings.conf",direc);

	 	 if(!(fp = fopen(filename, "r"))){
       		  	new_message_window("Could not Open Conf");
		 }

   	fp = fopen(filename, "w");
   
  	 fprintf(fp, "%d\n%d\n%d\n", settings.laps, settings.cc, settings.gdif);
  	 fclose(fp);
   
  	 new_message_window("Settings saved.");

  	 frame=0;	
  	 option=0;

   
}

static void iracer_startup()
{
	  // KERIPO MOD
	  //load_settings();
	  ipr_load_settings();
	  load_car(); 
	  load_map();
	  load_images();
	  iracer_scale_images();

	 FILE *fp;
	 
	if(!(fp = fopen(direc, "r"))){
	new_message_window("Couldn't Find iracer Folder"); 
	}
   	 else {
		iracer_scan(direc);
	 }

}


//-------------------------------MOVEMENT OF VECHILE--------------------------------

static int chk_col(int d, int e)	
{
	ttk_color c;
	int d2=d, e2=e;
	int r,g,b,t, evals=0;

for (t=-1;t<=1;t++){
  
	d=d2+t; 
	e=e2+t;

	d+=(map.xstart+(xoff*speed));
	e+=(map.ystart+(yoff*speed));

	c = ttk_getpixel(iracer_srf,ipods[d],ipods[e]);
	ttk_unmakecol(c,&r,&g,&b);

  if(r==0&&g==0&&b==0){evals=1;}
}

 if (evals==1){return 1;}
 else {return 0;}
}

static void move_car() //COLLISIONS
{
	int bol=0, bol2=0;

  	if(cpos>0&&cpos<6){
		if(ipods[(map.xstart+(xoff*speed))]>ipods[map.xstart]-speed+1 && 
		 ipods[(map.xstart+(xoff*speed))]<ipods[map.xstart]+ speed+1 && 
		 ipods[(map.ystart+(yoff*speed))]>ipods[map.ystart-35]&& lphold == 0)
		 {lap=(lap+1)-cheating; cheating=0; lphold++;}
	}
	 else if(cpos>6&&cpos<11){
		if(ipods[(map.xstart+(xoff*speed))]>ipods[map.xstart]- speed+1 && 
		 ipods[(map.xstart+(xoff*speed))]<ipods[map.xstart]+ speed+1 && 
		 ipods[(map.ystart+(yoff*speed))]>ipods[map.ystart-35]&& lphold == 0)
		 {cheating=1; lphold++;}
	 }

	if(lphold==10){lphold=0;}
	 else if(lphold>=1){lphold++;}

  	if(lap>set_laps[settings.laps]){
	 fina=1; 
	 lap=set_laps[settings.laps]; 
 	}

  	switch(cpos){
    		case 0: 
			bol=chk_col(7,0);
			bol2=chk_col(12,0);
      			if(bol==1||bol2==1){yoff+=3; }
			else{yoff-=2;}
      			break;

	  	case 1: 
			bol=chk_col(12,0);
			bol2=chk_col(18,4);
      			if(bol==1||bol2==1){xoff-=2; yoff+=3; }
			else{xoff+=1; yoff-=2;}
      			break;
      
		case 2: 
			bol=chk_col(18,2);
			bol2=chk_col(20,6);
      			if(bol==1||bol2==1){xoff-=3; yoff+=2;}
			else{xoff+=2; yoff--;}
      			break;
      
		case 3:
			bol=chk_col(20,7);
			bol2=chk_col(20,12);
      			if(bol==1||bol2==1){xoff-=3;}
			else{xoff+=2;}
     			break;
      
		case 4:
			bol=chk_col(20,11);
			bol2=chk_col(20,18);
     			if(bol==1||bol2==1){yoff-=2; xoff-=3;}
			else{yoff++; xoff+=2;}
      			break;
      
		case 5:
			bol=chk_col(19,17);
			bol2=chk_col(14,20);
      			if(bol==1||bol2==1){yoff-=3; xoff-=2;}
			else{yoff+=2; xoff++;}
      			break;
      
		case 6:
			bol=chk_col(12,20);
			bol2=chk_col(7,20);
     			if(bol==1||bol2==1){yoff-=3;}
			else{yoff+=2;}
     			break;
      
		case 7:
			bol=chk_col(8,20);
			bol2=chk_col(2,14);
      			if(bol==1||bol2==1){xoff+=2; yoff-=3;}
			else{xoff--; yoff+=2; }
      			break;
      
		case 8:
			bol=chk_col(5,19);
			bol2=chk_col(0,11);
      			if(bol==1||bol2==1){xoff+=3; yoff-=2;}
			else{xoff-=2; yoff++;}
     			break;
      
		case 9:
			bol=chk_col(0,12);
			bol2=chk_col(0,7);
      			if(bol==1||bol2==1){xoff+=3;}
			else{xoff-=2;}
     			break;
      
		case 10:
			bol=chk_col(0,9);
			bol2=chk_col(3,2);
      			if(bol==1||bol2==1){xoff+=3; yoff+=2;}
			else{xoff-=2; yoff--;}
      			break;
      
		 case 11:
			bol=chk_col(2,7);
			bol2=chk_col(7,0);
      			if(bol==1||bol2==1){xoff+=2; yoff+=3;}
			else{xoff--; yoff-=2;}
      			break;
	 }

	if(bol==1||bol2==1){wall++;}
}


static void car_speed() //speeds may need to be adjusted
{
   if(count<-1){
    	 switch(settings.cc){
     	  case 2: speed=5;  break;	//move 150cc
      	  case 1: speed=4;  break;	//move 100cc
       	  case 0: speed=3;  break;	   //move 50cc;
	 }
     	if(wall2==0){move_car();}
   }

	 else{
	   if(z>=10){count--; z=0;}	
	   z++;	
	 }
}


//---------------------------------------------Drawn to Screen------------------------------------

static void draw_race()
{
	 char file[5];
	 char files[20];
	 int s1,s2;

	 ttk_blit_image (map.image,iracer_srf,0,0); //Map 
	
	 ttk_blit_image (car[cpos],iracer_srf,ipods[(map.xstart+(xoff*speed))],ipods[(map.ystart+(yoff*speed))]); //Car
	 	
	 if(count>0){ //3,2,1 
		 sprintf(file,"%d",count);	
		 pz_vector_string_center(iracer_srf, file, ipods[160], ipods[100], ipods[((z<<2)+z)], ipods[((z<<2)+z)],1, ttk_makecol(BLACK));
		if(z==5){ttk_click_ex(100, 150);}
	 }  
	 else if(count>-1){ //GO
		 sprintf(file,"GO");	
		 pz_vector_string_center(iracer_srf, file, ipods[160], ipods[100], ipods[((z<<2)+z)], ipods[((z<<2)+z)],1, ttk_makecol(BLACK)); wall2=0;
		lphold=1;
		if(z==5){ttk_click_ex(50,250);}
	 }

	if(fina==1&&(timed-(times/11))>0&&settings.gdif==0||(fina==1&&settings.gdif==0&&timed==0)){
	  	sprintf(files,"FINISHED");	
		pz_vector_string_center(iracer_srf, files, ipods[160], ipods[100], ipods[20], 
		ipods[(20*z)],1, ttk_makecol(BLACK));
		write_high();
	}
	else if(fina==1&&settings.gdif==0){
	  	sprintf(files,"FAILED");	
		pz_vector_string_center(iracer_srf, files, ipods[160], ipods[100], ipods[20], 
		ipods[(20*z)],1, ttk_makecol(BLACK));
	}
	else if(fina==1){
	  	sprintf(files,"FINISHED");	
		pz_vector_string_center(iracer_srf, files, ipods[160], ipods[100], ipods[20], 
		ipods[(20*z)],1, ttk_makecol(BLACK));
	}

	 ttk_fillrect (iracer_srf, 0, ipods[220], ipods[320], ipods[240], ttk_makecol(BLACK));

 
	 sprintf(files,"Lap %d of %d",lap,set_laps[settings.laps]);	
	 pz_vector_string(iracer_srf, files, ipods[5], ipods[225] , ipods[8],ipods[14], 1, ttk_makecol(WHITE));


	if(settings.gdif==1||(settings.gdif==0&&timed==0)){
			s1 = times/11;
			s2 = (times/11)*10;
			s2 = s2 % 10;
	 sprintf(files,"Time %2d.%1d secs", s1,s2);	
	 pz_vector_string(iracer_srf, files, ipods[150], ipods[225] , ipods[8] ,ipods[14] , 1, ttk_makecol(WHITE));
	}
	else if(settings.gdif==0){
		if(timed-(times/11)<=0)
			sprintf(files,"Time 0.0 secs");
		else{	
			s1 = timed- (times/11);
			s2 = (timed-(times/11))*10;
			s2 = s2 % 10;

		sprintf(files,"Time %2d.%1d secs",s1,s2);}	
	 pz_vector_string(iracer_srf, files, ipods[150], ipods[225] , ipods[8] ,ipods[14] , 1, ttk_makecol(WHITE));
	}
	  
}


static void main_menu() 
{

	switch(ttk_screen->bpp){
		case 2: texts = ttk_makecol(WHITE); break;
		case 16: texts = ttk_makecol(151, 124, 33); break;
	}
		
	int width;
   	width = ttk_text_width (ttk_menufont, menu_opts[option]);
	 
	ttk_blit_image (menu_bg, iracer_srf, 0, 0);
	ttk_blit_image (menu_wheel, iracer_srf, (bgw/2)-((ipods[191])/2), ipods[44]);
	ttk_text (iracer_srf, ttk_menufont, (bgw/2)-(width/2), ipods[95], texts,menu_opts[option]);
}


static void view_map()
{
  	ttk_surface scaled;
	char file[40];
	int t=-1, s1, s2;
	
	switch(ttk_screen->bpp){
		case 2: texts = ttk_makecol(WHITE); break;
		case 16: texts = ttk_makecol(DKGREY); break;
	}
	 
   ttk_fillrect(iracer_srf,0,0,bgw,bgh,ttk_makecol(GREY));

   ttk_fillrect(iracer_srf,ipods[5],ipods[50],(ipods[320]+ipods[10])*.5,ipods[320]*.5,ttk_makecol(BLACK)); //wholly hackish
  
   ttk_blit_image (scaled = ttk_scale_surface(map.image,.5),iracer_srf,ipods[5],ipods[50]); //Map Scaled View

	 sprintf(file,"MAPS");	
	 pz_vector_string_center(iracer_srf, file, ipods[160], ipods[25] , ipods[20] ,ipods[20] , 1, ttk_makecol(WHITE));

	 sprintf(file,"%s",map.mapname);
	 pz_vector_string(iracer_srf, file, ipods[167] , ipods[50], ipods[6] ,ipods[18] , 1, texts); 
	
	 sprintf(file,"By: %s",map.author);	
	 pz_vector_string(iracer_srf, file, ipods[5] , ipods[175], ipods[8] ,ipods[16] , 1, texts); 
	
	 sprintf(file,"Difficulty: %s",dif_opts[map.difficulty]); 	
	 pz_vector_string(iracer_srf, file, ipods[5] , ipods[210], ipods[8] ,ipods[16] , 1, texts); 

	 if(rhigh>=0&&rhigh<30){

			sprintf(file,"50CC"); 	
	 		pz_vector_string(iracer_srf, file, ipods[175] , ipods[100+(t*20)], 
			ipods[8] ,ipods[16] , 1, ttk_makecol(WHITE)); 

		for(t=0;t<3;t++){

			s1 = map.cc50[t];
			s2 = map.cc50[t]*10;
			s2 = s2 % 10;

			sprintf(file,"laps %d: %2d.%1d",set_laps[t],  s1,s2); 	
	 		pz_vector_string(iracer_srf, file, ipods[175] , ipods[110+(t*20)], 
			ipods[8] ,ipods[16] , 1, ttk_makecol(WHITE)); 
		}
	 }
	 else if(rhigh>=30&&rhigh<59){

			sprintf(file,"100CC"); 	
	 		pz_vector_string(iracer_srf, file, ipods[175] , ipods[100+(t*20)], 
			ipods[8] ,ipods[16] , 1, ttk_makecol(WHITE)); 

		for(t=0;t<3;t++){

			s1 = map.cc100[t];
			s2 = map.cc100[t]*10;
			s2 = s2 % 10;

			sprintf(file,"laps %d: %2d.%1d",set_laps[t],  s1,s2); 	
	 		pz_vector_string(iracer_srf, file, ipods[175] , ipods[110+(t*20)], 
			ipods[8] ,ipods[16] , 1, ttk_makecol(WHITE)); 
		}
	 }
	 else{

			sprintf(file,"150CC"); 	
	 		pz_vector_string(iracer_srf, file, ipods[175] , ipods[100+(t*20)], 
			ipods[8] ,ipods[16] , 1, ttk_makecol(WHITE)); 

		for(t=0;t<3;t++){

			s1 = map.cc150[t];
			s2 = map.cc150[t]*10;
			s2 = s2 % 10;

			sprintf(file,"laps %d: %2d.%1d",set_laps[t], s1,s2); 	
	 		pz_vector_string(iracer_srf, file, ipods[175] , ipods[110+(t*20)], 
			ipods[8] ,ipods[16] , 1, ttk_makecol(WHITE)); 
		}
	 }
	

	 ttk_free_surface(scaled);
}


static void settings_menu() 
{
	 char str[35];
	 char buf[15];
	 int height;
	
	switch(ttk_screen->bpp){
		case 2: texts = ttk_makecol(WHITE); break;
		case 16: texts = ttk_makecol(151, 124, 33); break;
	}
	 
	 height = ttk_text_height(ttk_menufont);
	 
	 ttk_blit_image (menu_sbg, iracer_srf, 0, 0);
	 
	 strcpy(str, "Laps: ");
	 sprintf(buf, "%d", set_laps[settings.laps]);
	 strcat(str, buf);
	 ttk_text (iracer_srf, ttk_menufont, 5, bgh/4, (option==0)?texts:ttk_makecol(BLACK), str);
	 
	 strcpy(str, "Speed: ");
	 sprintf(buf, "%d CC", set_cc[settings.cc]);
	 strcat(str, buf);
	 ttk_text (iracer_srf, ttk_menufont, 5, bgh/4+(height+2), (option==1)?texts:ttk_makecol(BLACK), str);
	 
	 strcpy(str, "Time Limit: ");
	 sprintf(buf, "%s", onoff_opts[settings.gdif]);
	 strcat(str, buf);
	 ttk_text (iracer_srf, ttk_menufont, 5, bgh/4+(2*height+4), (option==2)?texts:ttk_makecol(BLACK), str);
	 
	 ttk_text (iracer_srf, ttk_menufont, 5, bgh/4+(3*height+6), (option==3)?texts:ttk_makecol(BLACK), "Save Settings");
}


static void draw_iracer (TWidget *wid, ttk_surface srf)
{
	 ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(BLACK));//bg black.
	 ttk_fillrect (iracer_srf, 0, 0, bgw, bgh, ttk_makecol(BLACK));

	 switch(frame) {
    	 case 0: main_menu(); break;
   	 case 1: draw_race(); break;
   	 case 2: view_map(); break;
   	 case 3: settings_menu(); break;
	}
	ttk_blit_image (iracer_srf, srf, (ttk_screen->w/2)-(bgw/2), (ttk_screen->h/2)-(bgh/2));
}


static void reset_race()
{
	
	xoff=3; yoff=0;
	option=0;
	count=3,wall=0, wall2=0,lap=1;
	cpos=3;
	times=0;
	timed=0;
	fina=0;
	z=0;
	speed=1;
	
	load_map();


}

//---------------------------------Input Events-------------------------------------------------

static int scroll_iracer (TWidget *this,int dir)
{
     TTK_SCROLLMOD( dir, 8);
      if( dir > 0){
      	if(frame==0 || frame ==3) {
      		if(option < 3) option++;
      	}
      	else if(frame==1&&count<-1&&fina!=1){
	 	      cpos++; wall2=0;
		      if(cpos>11){cpos=0;}
	      }
	      else if(frame==2){
	      	mapc++; load_map(); return 0; 
	      }
       return 0;}
      else {
      	if(frame==0 || frame==3) {
      		if(option > 0) option--;
      	}
      	else if(frame==1&&count<-1&&fina!=1){
	 	      cpos--;	wall2=0;
		      if(cpos<0){cpos=11;}
	      }
	      else if(frame==2){
	      	mapc--; load_map(); return 0; 
	      }
      return 0;}
      return 0;
}
static int down_iracer (TWidget *this,int button)
{
  
      switch (button) {
      case TTK_BUTTON_ACTION:
      	 if(frame == 0) {
      	 	 switch(option) {
      	 	 	 case 0: frame=1; reset_race(); break;  
      	 	 	 case 1: frame=2; reset_race(); break;
      	 	 	 case 2: frame=3; break;
      	 	 	 case 3: reset_race(); frame=0; pz_close_window(this->win); break;
      	 	 }
      	 }
      	 else if(frame == 1) {	
		if(fina==1){frame=0; reset_race();}
      	 }
      	 else if(frame == 2) {
      	 	 frame=1;
      	 }
      	 else if(frame == 3) {
      	   switch(option) {
      	   	 case 0: break;
      	   	 case 1: break;
      	   	 case 2: break;
			 // KERIPO MOD
			 //case 3: save_settings(); break;
      	   	 case 3: ipr_save_settings(); break;
      	   }
      	 }
this	 ->dirty++;
         break;

      case TTK_BUTTON_MENU:
      	 switch(frame) {
      	 	 case 0: break;
      	 	 case 1: reset_race(); frame=0; break;
      	 	 case 2: frame=0; break;
      	 	 case 3: frame=0; break;
      	 }
	 this->dirty++;
         break;

      case TTK_BUTTON_PLAY:
         break;

      case TTK_BUTTON_NEXT:
      	 if(frame == 0) {
      	 	 switch(option) {
      	 	 	 case 0: break;
      	 	 	 case 1: break;
      	 	 	 case 2: break;
      	 	 	 case 3: break;
      	 	 }
      	 }
      	 else if(frame == 3) {
      	 	 switch(option) {
      	 	 	 case 0: if(settings.laps < 2) settings.laps++; break;
      	 	 	 case 1: if(settings.cc < 2) settings.cc++; break;
      	 	 	 case 2: if(settings.gdif < 1) settings.gdif++; break;
      	 	 	 case 3: break;
      	   }
      	 }
         break;

      case TTK_BUTTON_PREVIOUS:
      	 if(frame == 0) {
      	 	 switch(option) {
      	 	 	 case 0: break;
      	 	 	 case 1: break;
      	 	 	 case 2: break;
      	 	 	 case 3: break;
      	 	 }
      	 }
      	 else if(frame == 3) {
      	 	 switch(option) {
      	 	 	 case 0: if(settings.laps > 0) settings.laps--; break;
      	 	 	 case 1: if(settings.cc > 0) settings.cc--; break;
      	 	 	 case 2: if(settings.gdif > 0) settings.gdif--; break;
      	 	 	 case 3: break;
      	   }
      	 }
      	 break;
      	 
      case TTK_BUTTON_HOLD:   
         break;
         
   
      }
     return 0;
     }

static int timer_iracer(TWidget *this)
{
	   if(frame==2){
		rhigh++;
	    	if(rhigh>90){rhigh=0;}}
	    if(fina!=1){
	     if(frame==1&&wall==0){car_speed();}
	     else if(wall>=8){wall=0;}
	     else {wall++;}
	    if(wall==1){wall2=1;}
	    if(count<-1){times++;}
	    }
   return 0;
     this->dirty++;
   

   }
 



TWindow *new_iracer_window()
{

	  find_dir();
	  iracer_startup();
	  ipodset();
	 
	  
    window = ttk_new_window ("iPod Racer");
    ttk_window_hide_header(window);
    wid = ttk_new_widget(0,0);
    wid->focusable=1;
    wid->draw=draw_iracer;
     wid->scroll=scroll_iracer;
     wid->down=down_iracer;
     wid->timer=timer_iracer;
    ttk_add_widget(window, wid);
    ttk_widget_set_timer(wid, 91);
   ttk_show_window(window);
}


