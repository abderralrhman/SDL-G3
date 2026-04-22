#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define WINDOW_W 1280
#define WINDOW_H 720
#define LEVEL1 1
#define LEVEL2 2
#define PLATFORM_FIXED 0
#define PLATFORM_MOBILE 1
#define PLATFORM_DESTRUCTIBLE 2
#define MAX_PLATFORMS 8

typedef struct
{
    SDL_Rect rect;
    int type;
    int level;
    int active;
    int direction;
    int min;
    int max;
    int speed;
    int breaking;
    unsigned int breakStart;
} Platform;

typedef struct
{
    SDL_Texture *level1;
    SDL_Texture *level2;
    SDL_Texture *fixedTex;
    SDL_Texture *mobileTex;
    SDL_Texture *destruct0;
    SDL_Texture *destruct1;
    SDL_Texture *destruct2;
    int level1W;
    int level1H;
    int level2W;
    int level2H;
    int currentLevel;
    int singleMode;
    int showGuide;
    int guideSingle;
    int guideMulti;
    unsigned int guideSingleTimer;
    unsigned int guideMultiTimer;
    unsigned int startTime1;
    unsigned int startTime2;
    unsigned int elapsed1;
    unsigned int elapsed2;
    TTF_Font *font;
    Mix_Music *music1;
    Mix_Music *music2;
    Platform p[MAX_PLATFORMS];
    int nbPlatform;
    SDL_Rect cam1;
    SDL_Rect cam2;
    SDL_Rect player1;
    SDL_Rect player2;
    SDL_Rect zone12;
    SDL_Rect zone21;
} Background;

void initBackground(Background *b, SDL_Renderer *r);
void updateBackground(Background *b);
void scrollBackground(Background *b);
void animateBackground(Background *b);
void displayBackground(Background *b, SDL_Renderer *r);
void freeBackground(Background *b);

#endif
