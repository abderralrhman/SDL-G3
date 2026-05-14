#ifndef MINIMAP_H
#define MINIMAP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "background.h"

typedef struct
{
    SDL_Texture *map1;
    SDL_Texture *map2;
    SDL_Texture *point;
    SDL_Rect rect;
    SDL_Rect pointRect;
    int mapW;
    int mapH;
} MiniMap;

void initMiniMap(MiniMap *m, SDL_Renderer *r, Background *bg);
void updateMiniMap(MiniMap *m, Background *bg);
void displayMiniMap(MiniMap *m, SDL_Renderer *r, Background *bg);
void freeMiniMap(MiniMap *m);

#endif
