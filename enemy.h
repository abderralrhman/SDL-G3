#ifndef ENEMY_H
#define ENEMY_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "background.h"
#define MAX_ENEMY 6
#define ITEM_COIN 1
#define ITEM_MEDKIT 2
typedef struct{
    SDL_Texture *normal;
    SDL_Texture *strong;
    SDL_Texture *coin;
    SDL_Texture *medkit;
    SDL_Rect e[MAX_ENEMY];
    SDL_Rect item[4];
    int itemType[4];
    int itemActive[4];
    int live[MAX_ENEMY];
    int hp[MAX_ENEMY];
    int hpMax[MAX_ENEMY];
    int damage[MAX_ENEMY];
    int dir[MAX_ENEMY];
    int left[MAX_ENEMY];
    int right[MAX_ENEMY];
    int ai[MAX_ENEMY];
    int chasing[MAX_ENEMY];
    int frame[MAX_ENEMY];
    int frameCounter[MAX_ENEMY];
    int score;
    SDL_Rect normalSrc;
    SDL_Rect strongSrc;
} EnemyModule;
void initEnemyModule(EnemyModule *em, SDL_Renderer *r);
void loadEnemyLevel(EnemyModule *em, int level);
void updateEnemyModule(EnemyModule *em, Background *bg);
void displayEnemyModule(EnemyModule *em, SDL_Renderer *r, Background *bg);
void freeEnemyModule(EnemyModule *em);
#endif

