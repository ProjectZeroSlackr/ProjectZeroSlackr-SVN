/* Free for absolutely any use whatsoever. */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "pz.h"
#include <math.h>
#include "enginea.h"


char *map;
int x;
int y;
int playerView;
int playerX;
int playerY;

int draw_mymodule (TWidget *wid, ttk_surface srf){
     ttk_fillrect(srf,wid->x,wid->y,wid->w,wid->h/2,ttk_makecol (1,73,255));
    ttk_fillrect(srf,wid->x,wid->h/2,wid->w,wid->h,ttk_makecol (BLACK));
    int i = 0;
    int angle;
    int width = ((wid->w-wid->x)/60)+1;
    int magicHeigthNumber = (fisin(30)*wid->h) / ficos(30);
    magicHeigthNumber = magicHeigthNumber*64;

    for(angle = 30;angle > -30;angle--){
        int rayAngle = modify_angle(playerView,angle);
        int xA;
        int yA;
//         printf("angle %d\n",rayAngle);

        int region = 0;
        /*find the correct region on the einheitskreis*/
        if(rayAngle >= 0 && rayAngle <=90){
            region = 1;
        }else if(rayAngle >= 90 && rayAngle <=180){
            region = 2;
        }else if(rayAngle >= 180 && rayAngle <=270){
            region = 3;
        }else if(rayAngle >= 270 && rayAngle <=360){
            region = 4;
        }
        /*find xA and yA*/
        switch(rayAngle){
            case 0:
                xA = 64;
                yA = 0;
            break;
            case 90:
                xA = 0;
                yA = -64;
            break;
            case 180:
                xA = -64;
                yA = 0;
            break;
            case 270:
                xA = 0;
                yA = 64;
            break;
            default:
                 xA = (ficos(rayAngle)*64) / fisin(rayAngle);
                 yA = (fisin(rayAngle)*64) / ficos(rayAngle);
        }

        int distH = find_wall_distanceH(map,x,y,yA,playerX,playerY,region,rayAngle);
        int distV = find_wall_distanceV(map,x,y,xA,playerX,playerY,region,rayAngle);
        int dist = 0;

        ttk_color color; //Color of the wall
        if(distH>=distV){
            color = ttk_makecol (108,0,2);
            dist = distV;
        }else{
            color = ttk_makecol (185,0,3);
            dist = distH;
        }

        dist = sqrt(dist);
        dist = (dist * ficos(fabs(angle)))>>14;
        if(dist <= 8000 && dist >0){//draw the wall
            int height = magicHeigthNumber/dist; //8864 = (64*277)/2
            ttk_fillrect(srf, i, wid->h/2-height, i+width, wid->h/2+height, color);

        }
        i+=width;
    }
   // draw_objects(wid,srf);
    return 0;
}

int can_move_to(char *map, int x, int y, int x2, int y2){
    if(x2/64 > x || x2/64 < 0 || y2/64 > y || y2/64 < 0){
        printf("out\n");
        return 0;
    }
    if(map[(y2/64)*x+x2/64] == 'X'){
        printf("whops\n");
        return 0;
    }
    return 1;
}

int down_e3d (TWidget *this,int button){
    int playerYTemp,playerXTemp;
    switch (button) {
        
                case TTK_BUTTON_MENU:
                        playerYTemp = playerY-((10*fisin(playerView))>>14);
                        playerXTemp = playerX+((10*ficos(playerView))>>14);
                        if(can_move_to(map, x,y,playerXTemp,playerYTemp)){
                        playerY = playerYTemp;
                        playerX = playerXTemp;

                        this->dirty = 1;}
                    break;
                case TTK_BUTTON_PLAY:
                        playerYTemp = playerY+((10*fisin(playerView))>>14);
                        playerXTemp = playerX-((10*ficos(playerView))>>14);
                        if(can_move_to(map, x,y,playerXTemp,playerYTemp)){
                            playerY = playerYTemp;
                            playerX = playerXTemp;
                            this->dirty = 1;
                        }
                    break;
                case TTK_BUTTON_HOLD:
                        pz_close_window (this->win);
                        this->dirty = 1;
                    break;
            }
        
       return 0;
       }
 int scroll_e3d(TWidget *this, int dir)
 {
            playerView = modify_angle(playerView, -(dir*5));
            this->dirty = 1;
 
    return 0;
} 

TWindow *new_e3d_window()
{
    TWindow *ret;
TWidget *wid=ttk_new_widget(0,0);
    map = (char *)malloc((strlen("XXXXXXXXXXX0000XXX0XX00000000XX0000XX0XXX00X0XX0XXX0000XX0XXXX0XXXX0XXXX000000XXXXXXXXXXXX")+1)*sizeof(char));
    strcpy(map,"XXXXXXXXXXX0000XXX0XX00000000XX0000XX0XXX00X0XX0XXX0000XX0XXXX0XXXX0XXXX000000XXXXXXXXXXXX");
    x = 10;
    y = 9;
wid->focusable=1;
wid->draw=draw_mymodule;
wid->scroll=scroll_e3d;
wid->down=down_e3d;
    playerView = 90;
    playerX = 2*64+32;
    playerY = 2*64+32;

    ret = ttk_new_window ("engine3d");
    ttk_add_widget (ret, wid);
    ttk_show_window (ret);
}


