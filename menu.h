#ifndef MENU_H
#define MENU_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#define SCREEN_MENU 0
#define SCREEN_SAVE 1
#define SCREEN_LOAD 2
#define SCREEN_OPTIONS 3
#define SCREEN_SCORES 4
#define SCREEN_PLAYER 5
#define SCREEN_PUZZLE 6
#define SCREEN_GAME 7
#define SCREEN_QUIT 8
#define MAX_NAME 50
#define MAX_SCORES 3
#define PAGE_INPUT 0
#define PAGE_LIST 1
typedef struct{SDL_Rect r; char text[40]; int hover; int oldHover; SDL_Texture *normal; SDL_Texture *active;} MButton;
typedef struct{char name[MAX_NAME]; int score;} Score;
typedef struct{TTF_Font *font,*big; Mix_Music *music; Mix_Music *victory; Mix_Chunk *click; Mix_Chunk *hoverSound; int volume,full,mode,multi,avatar,input,page,zone; char name[MAX_NAME]; Score scores[MAX_SCORES]; MButton b[16]; SDL_Texture *mainBg; SDL_Texture *logo; SDL_Texture *saveBg; SDL_Texture *savePrompt; SDL_Texture *saveOrLoad; SDL_Texture *playerBg; SDL_Texture *playerBg2; SDL_Texture *player1; SDL_Texture *player2; SDL_Texture *hsBg1; SDL_Texture *hsBg2; SDL_Texture *hsInput;} Menu;
void initMenu(Menu *m, SDL_Renderer *r);
void handleMenu(Menu *m, SDL_Event *e, int *screen, SDL_Window *w, SDL_Renderer *r, int finalScore);
void displayMenu(Menu *m, SDL_Renderer *r, int screen);
void freeMenu(Menu *m);
#endif
