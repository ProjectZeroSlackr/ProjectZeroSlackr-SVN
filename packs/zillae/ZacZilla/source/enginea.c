#include "pz.h"
#include <math.h>

/* Precalculated sine and cosine * 16384 (fixed point 18.14) */
static const short sin_table[91] =
{
            0,   285,   571,   857,  1142,  1427,  1712,  1996,  2280,  2563,
	 2845,  3126,  3406,  3685,  3963,  4240,  4516,  4790,  5062,  5334,
	 5603,  5871,  6137,  6401,  6663,  6924,  7182,  7438,  7691,  7943,
	 8191,  8438,  8682,  8923,  9161,  9397,  9630,  9860, 10086, 10310,
	10531, 10748, 10963, 11173, 11381, 11585, 11785, 11982, 12175, 12365,
	12550, 12732, 12910, 13084, 13254, 13420, 13582, 13740, 13894, 14043,
	14188, 14329, 14466, 14598, 14725, 14848, 14967, 15081, 15190, 15295,
	15395, 15491, 15582, 15668, 15749, 15825, 15897, 15964, 16025, 16082,
	16135, 16182, 16224, 16261, 16294, 16321, 16344, 16361, 16374, 16381,
	16384
};

long fisin(int val)
{
	/* Speed improvement through sukzessive lookup */
	if (val<181)
	{
		if (val<91)
		{
			/* phase 0-90 degree */
			return (long)sin_table[val];
		}
		else
		{
			/* phase 91-180 degree */
			return (long)sin_table[180-val];
		}
	}
	else
	{
		if (val<271)
		{
			/* phase 181-270 degree */
			return (-1L)*(long)sin_table[val-180];
		} 
		else
		{
			/* phase 270-359 degree */
			return (-1L)*(long)sin_table[360-val];
		}
	}
	return 0;
}

long ficos(int val)
{
	/* Speed improvement through sukzessive lookup */
	if (val<181)
	{
		if (val<91)
		{
			/* phase 0-90 degree */
			return (long)sin_table[90-val];
		}
		else
		{
			/* phase 91-180 degree */
			return (-1L)*(long)sin_table[val-90];
		}
	}
	else
	{
		if (val<271)
		{
			/* phase 181-270 degree */
			return (-1L)*(long)sin_table[270-val];
		}
		else
		{
			/* phase 270-359 degree */
			return (long)sin_table[val-270];
		}
	}
	return 0;
}

int draw_background(TWidget *wid, ttk_surface srf){
    ttk_fillrect(srf,wid->x,wid->y,wid->w,wid->h/2,ttk_makecol (1,73,255));
    ttk_fillrect(srf,wid->x,wid->h/2,wid->w,wid->h,ttk_makecol (BLACK));
    
}

int find_wall_loop(char *map, int x, int y, int playerX, int playerY, int xA, int yA, int newx, int newy){
    int dist = 650000;
//     printf("test: Ax = %d, Ay = %d, playerX = %d, playerY = %d, xA = %d, yA = %d\n",newx,newy, playerX,playerY,xA,yA);
    int tempx = x*64;
    int tempy = y * 64;

    while(newx < tempx && newx >= 0 && newy < tempy && newy >= 0){
//            printf("search at %d %d (%d %d) there is %c\n",newx/64,newy/64,newx,newy,map[(newy/64)*x+newx/64]);

        if(map[(newy/64)*x+newx/64] == 'X'){
            dist = (newx-playerX)*(newx-playerX) + (newy-playerY)*(newy-playerY);

//               printf("found wall at : %d %d from %d %d dist = %d\n",newx/64,newy/64,playerX/64,playerY/64,dist);

            break;
        }
        newx += xA;
        newy += yA;
    }
    return dist;

}

int find_wall_distanceH(char *map, int x, int y, int yA,int playerX, int playerY,int region,int angle){
    int Ax, Ay, xA;
    if(region == 1 || region == 4){
        Ax = (playerX/64) * 64 + 64;
        xA = 64;
    }else{
        Ax = (playerX/64) * 64 - 1;
        xA = -64;
    }

    if(region == 1 || region == 2){
        yA = -fabs(yA);
    }else{
        yA = fabs(yA);
    }

    switch(angle){
            case 90:
                Ay = 65000;
                xA = 0;
            break;
            case 270:
                Ay = 65000;
                xA = 0;
            break;
            default:
                Ay = playerY + (fisin(angle)*(playerX-Ax)) / ficos(angle);
        }

    return find_wall_loop(map, x, y, playerX,playerY, xA, yA, Ax, Ay);

}


int find_wall_distanceV(char *map, int x, int y, int xA, int playerX, int playerY,int region, int angle){
    int Ax, Ay, yA;
    if(region == 1 || region == 2){
        yA = -64;
        Ay = (playerY/64) * 64 - 1;
    }else{
        yA = 64;
        Ay = (playerY/64) * 64 + 64;
    }

    if(region == 1 || region == 4){
        xA = fabs(xA);
    }else{
        xA = -fabs(xA);
    }

    switch(angle){
            case 0:
                Ax = 65000;
                yA = 0;
            break;
            case 180:
                yA = 0;
                Ax = 65000;
            break;
            default:
                Ax = playerX + (ficos(angle)*(playerY-Ay)) / fisin(angle);
    } 
    return find_wall_loop(map, x, y, playerX,playerY, xA, yA, Ax, Ay);

}

int modify_angle(int angle, int modif){
    int temp = (angle + modif)%360;

    if(temp < 0) {temp = 360 + temp;}
    return temp;
}

void draw_ASCII(char *map, int x, int y, int px, int py){
    int loopx, loopy;
    for(loopy = 0; loopy < y; loopy++){
         for(loopx = 0; loopx < x; loopx++){
            if(loopx == px/64 && loopy == py/64){
                printf("@");
            }else{
                printf("%c",map[loopy*x+loopx]);
            }
         }
        printf("\n");
    }

}

void draw_walls(TWidget *wid, ttk_surface srf, char *map, int x, int y, int playerX, int playerY, int playerView){
//     draw_ASCII(map,x,y,playerX,playerY);
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
}

void draw_objects(TWidget *wid, ttk_surface srf){

} 
