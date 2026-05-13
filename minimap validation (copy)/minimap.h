#ifndef MINIMAP_H_INCLUDED
#define MINIMAP_H_INCLUDED
#include <SDL2/SDL.h>

typedef struct
{
    SDL_Texture *map;
    SDL_Texture *player;
    SDL_Rect minimapRect;
    SDL_Rect playerRect;
    int mapWidth;
    int mapHeight;
    SDL_Texture *realMap;
} minimap;

typedef struct {
    SDL_Rect rect;
    SDL_Texture *texture;
    int type;
    int direction;
    int speed;
    int counter;
    int active;
} Platform;

typedef struct {
    SDL_Rect rect;
    SDL_Texture *texture;
    int active;
} Obstacle;

typedef struct {
    int active;
    int type;
    int shakeTimer;
    int flashTimer;
    int waveTimer;
    int redTintTimer;
} BGAnimation;

void initMinimap(minimap *m, SDL_Renderer *renderer, SDL_Texture *backgroundMap, int mapWidth, int mapHeight);
void updateMinimap(minimap *m, SDL_Rect playerPos);
void displayMinimap(minimap m, SDL_Renderer *renderer);
void freeMinimap(minimap *m);
void recreateMinimap(minimap *m, SDL_Renderer *renderer, SDL_Texture *newBackground);
void animateMinimap(minimap *m);
SDL_Color getPixel(SDL_Surface *surface, int x, int y);
int collisionPerfectPixel(SDL_Surface *mask, SDL_Rect playerPos);
int collisionPerfectPixelDirection(SDL_Surface *mask, SDL_Rect playerPos, int direction, int movement);
int collisionBB(SDL_Rect player, SDL_Rect obstacle);
int collisionBBPlatforms(SDL_Rect player, Platform *platforms, int numPlatforms);
int collisionBBObstacles(SDL_Rect player, Obstacle *obstacles, int numObstacles);
void initObstacles(Obstacle *o, int level, SDL_Renderer *renderer);
void freeObstacles(Obstacle *o, int numObstacles);
void displayObstacles(SDL_Renderer *renderer, Obstacle *obstacles, int numObstacles);
void initPlatforms(Platform *p, SDL_Renderer *renderer, int level);
void freePlatforms(Platform *p, int numPlatforms);
void updatePlatforms(Platform *p, int numPlatforms);
void handleFragileCollision(Platform *p, int index);
void initBGAnimation(BGAnimation *bgAnim);
void triggerBGAnimation(BGAnimation *bgAnim, int collisionType);
void updateBGAnimation(BGAnimation *bgAnim);
void applyBGAnimation(BGAnimation *bgAnim, SDL_Renderer *renderer, SDL_Texture *bg, int currentLevel);

#endif
