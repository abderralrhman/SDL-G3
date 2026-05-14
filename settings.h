#ifndef SETTINGS_H
#define SETTINGS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    int volume;
    int savedVolume;
    int muted;
    int fullscreen;
    SDL_Texture *background;
    SDL_Texture *plusBtn;
    SDL_Texture *minusBtn;
    SDL_Texture *fullscreenBtn;
    SDL_Texture *windowedBtn;
    SDL_Texture *backBtn;
    SDL_Texture *muteBtn;
    SDL_Texture *plusBtnHover;
    SDL_Texture *minusBtnHover;
    SDL_Texture *fullscreenBtnHover;
    SDL_Texture *windowedBtnHover;
    SDL_Texture *backBtnHover;
    SDL_Texture *muteBtnHover;
    int plusHover;
    int minusHover;
    int fullscreenHover;
    int windowedHover;
    int backHover;
    int muteHover;
    SDL_Rect plusRect;
    SDL_Rect minusRect;
    SDL_Rect fullscreenRect;
    SDL_Rect windowedRect;
    SDL_Rect backRect;
    SDL_Rect muteRect;
    TTF_Font *titleFont;
    TTF_Font *font;
    Mix_Chunk *clickSound;
} SettingsMenu;

void initSettingsMenu(SettingsMenu *s, SDL_Window *w, SDL_Renderer *r);
void handleSettingsMenu(SettingsMenu *s, SDL_Event *e, int *screen, int mainScreen);
void displaySettingsMenu(SettingsMenu *s);
void freeSettingsMenu(SettingsMenu *s);

#endif
