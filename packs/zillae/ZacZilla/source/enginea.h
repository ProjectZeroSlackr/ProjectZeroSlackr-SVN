#ifndef __ENGINEA_H
#define __ENGINEA_H
long fisin(int val);
long ficos(int val);
void draw_background(TWidget *wid, ttk_surface srf);

int find_wall_distanceH(char *map, int x, int y, int yA,int playerX, int playerY,int region,int angle);

int find_wall_distanceV(char *map, int x, int y, int xA, int playerX, int playerY,int region, int angle);

int modify_angle(int angle, int modif);

void draw_walls(TWidget *wid, ttk_surface srf, char *map, int x, int y, int playerX, int playerY, int playerView);

void draw_objects(TWidget *wid, ttk_surface srf);
#endif
