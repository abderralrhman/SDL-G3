#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#define MAX_NAME 50
#define MAX_SCORES 3
#define PAGE_INPUT 0
#define PAGE_SCORES 1

typedef struct
{
    char name[MAX_NAME];
    int score;
} Score;

typedef struct
{
    SDL_Texture *bg;
    SDL_Texture *zone;
    SDL_Texture *save;
    SDL_Texture *exit;
    SDL_Texture *back;
    TTF_Font *font;
    TTF_Font *titleFont;
    Mix_Chunk *click;
    Mix_Music *music;
    SDL_Rect zoneRect;
    SDL_Rect saveRect;
    SDL_Rect exitRect;
    SDL_Rect backRect;
} ScoreMenu;

void loadScores(Score scores[]);
void saveScore(char name[], int score, Score scores[]);
void sortScores(Score scores[]);
void drawText(SDL_Renderer *r, TTF_Font *font, char text[], int x, int y, SDL_Color color);
int buttonInside(SDL_Rect rect, int x, int y);
void runMenuScores(SDL_Renderer *r, int score);

#endif
