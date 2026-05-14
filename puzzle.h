#ifndef PUZZLE_H
#define PUZZLE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAX_QUESTION_LEN 256
#define MAX_OPTION_LEN 64

#define QUIZ_NOT_STARTED 0
#define QUIZ_ACTIVE 1
#define QUIZ_CORRECT 2
#define QUIZ_WRONG 3

#define PUZZLE_MAIN_MENU 0
#define QUIZ_ACTIVE_SCREEN 1

#define BUTTON_QUIZ 0
#define BUTTON_PUZZLE 1
#define BUTTON_OPTION_A 2
#define BUTTON_OPTION_B 3
#define BUTTON_OPTION_C 4

typedef struct {
    int x, y, w, h;
    int type;  
    bool is_hovered;
} Button;

typedef struct {
    int score;
    bool music_on;
} QuizGame;

typedef struct {
    int state;
    Button quiz_btn;
    Button puzzle_btn;
    Button option_btns[3];
    QuizGame quiz;
    SDL_Texture* bg;
    SDL_Texture* qusin; 
    SDL_Texture* quiz_btn_normal;
    SDL_Texture* quiz_btn_hover;
    SDL_Texture* puzzle_btn_normal;
    SDL_Texture* puzzle_btn_hover;
    SDL_Texture* a_btn_normal;
    SDL_Texture* a_btn_hover;
    SDL_Texture* b_btn_normal;
    SDL_Texture* b_btn_hover;
    SDL_Texture* c_btn_normal;
    SDL_Texture* c_btn_hover;
    Mix_Music* suspense;
    Mix_Chunk* hover_sfx;
    TTF_Font* font;
    SDL_Color white;
    int running;
} PuzzleMenu;

int Puzzle_Init(PuzzleMenu* menu, SDL_Renderer* renderer);
void Puzzle_Render(SDL_Renderer* renderer, PuzzleMenu* menu);
void Puzzle_HandleEvent(PuzzleMenu* menu, SDL_Event* e, int* run);
void Puzzle_HandleHover(PuzzleMenu* menu, int x, int y);
void Puzzle_Free(PuzzleMenu* menu);

#endif
