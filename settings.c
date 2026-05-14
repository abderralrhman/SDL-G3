#include "settings.h"
#include "menu.h"
#include <stdio.h>

SDL_Texture *loadSettingsTexture(SDL_Renderer *r, char *path)
{
    SDL_Texture *t;
    t = IMG_LoadTexture(r, path);
    return t;
}

int insideSettings(SDL_Rect a, int x, int y)
{
    return x >= a.x && x <= a.x + a.w && y >= a.y && y <= a.y + a.h;
}

void drawSettingsText(SettingsMenu *s, char *txt, int x, int y, TTF_Font *font)
{
    SDL_Color c = {255, 255, 255, 255};
    SDL_Surface *sf;
    SDL_Texture *t;
    SDL_Rect p;

    if (!font) return;
    sf = TTF_RenderText_Solid(font, txt, c);
    if (!sf) return;

    t = SDL_CreateTextureFromSurface(s->renderer, sf);
    p = (SDL_Rect){x, y, sf->w, sf->h};

    SDL_RenderCopy(s->renderer, t, NULL, &p);
    SDL_FreeSurface(sf);
    SDL_DestroyTexture(t);
}

void initSettingsMenu(SettingsMenu *s, SDL_Window *w, SDL_Renderer *r)
{
    s->window = w;
    s->renderer = r;
    s->volume = 64;
    s->savedVolume = 64;
    s->muted = 0;
    s->fullscreen = 0;

    s->background = loadSettingsTexture(r, "bk.png");

    s->plusBtn = loadSettingsTexture(r, "+.png");
    s->minusBtn = loadSettingsTexture(r, "-.png");
    s->fullscreenBtn = loadSettingsTexture(r, "f.png");
    s->windowedBtn = loadSettingsTexture(r, "p.png");
    s->backBtn = loadSettingsTexture(r, "r.png");
    s->muteBtn = loadSettingsTexture(r, "m.png");

    s->plusBtnHover = loadSettingsTexture(r, "++.png");
    s->minusBtnHover = loadSettingsTexture(r, "--.png");
    s->fullscreenBtnHover = loadSettingsTexture(r, "ff.png");
    s->windowedBtnHover = loadSettingsTexture(r, "pp.png");
    s->backBtnHover = loadSettingsTexture(r, "rr.png");
    s->muteBtnHover = loadSettingsTexture(r, "mm.png");

    if (!s->plusBtnHover) s->plusBtnHover = s->plusBtn;
    if (!s->minusBtnHover) s->minusBtnHover = s->minusBtn;
    if (!s->fullscreenBtnHover) s->fullscreenBtnHover = s->fullscreenBtn;
    if (!s->windowedBtnHover) s->windowedBtnHover = s->windowedBtn;
    if (!s->backBtnHover) s->backBtnHover = s->backBtn;
    if (!s->muteBtnHover) s->muteBtnHover = s->muteBtn;

    s->plusHover = 0;
    s->minusHover = 0;
    s->fullscreenHover = 0;
    s->windowedHover = 0;
    s->backHover = 0;
    s->muteHover = 0;

    s->minusRect = (SDL_Rect){460, 260, 110, 70};
    s->plusRect = (SDL_Rect){710, 260, 110, 70};
    s->windowedRect = (SDL_Rect){460, 390, 110, 70};
    s->fullscreenRect = (SDL_Rect){710, 390, 110, 70};
    s->muteRect = (SDL_Rect){460, 520, 110, 70};
    s->backRect = (SDL_Rect){710, 520, 110, 70};

    s->titleFont = TTF_OpenFont("arial.ttf", 46);
    s->font = TTF_OpenFont("arial.ttf", 28);
    s->clickSound = Mix_LoadWAV("clic.mp3");
    if (!s->clickSound) s->clickSound = Mix_LoadWAV("click.wav");

    Mix_VolumeMusic(s->volume);
}

void handleSettingsMenu(SettingsMenu *s, SDL_Event *e, int *screen, int mainScreen)
{
    int x, y;

    if (e->type == SDL_KEYDOWN)
    {
        if (e->key.keysym.sym == SDLK_ESCAPE)
            *screen = mainScreen;

        if (e->key.keysym.sym == SDLK_PLUS || e->key.keysym.sym == SDLK_KP_PLUS)
        {
            if (s->volume < 128) s->volume += 8;
            s->muted = 0;
            Mix_VolumeMusic(s->volume);
        }

        if (e->key.keysym.sym == SDLK_MINUS || e->key.keysym.sym == SDLK_KP_MINUS)
        {
            if (s->volume > 0) s->volume -= 8;
            Mix_VolumeMusic(s->volume);
        }

        if (e->key.keysym.sym == SDLK_m)
        {
            s->muted = !s->muted;
            if (s->muted) Mix_VolumeMusic(0);
            else Mix_VolumeMusic(s->volume);
        }
    }

    if (e->type == SDL_MOUSEMOTION)
    {
        x = e->motion.x;
        y = e->motion.y;

        s->plusHover = insideSettings(s->plusRect, x, y);
        s->minusHover = insideSettings(s->minusRect, x, y);
        s->fullscreenHover = insideSettings(s->fullscreenRect, x, y);
        s->windowedHover = insideSettings(s->windowedRect, x, y);
        s->backHover = insideSettings(s->backRect, x, y);
        s->muteHover = insideSettings(s->muteRect, x, y);
    }

    if (e->type != SDL_MOUSEBUTTONDOWN) return;

    x = e->button.x;
    y = e->button.y;

    if (s->clickSound) Mix_PlayChannel(-1, s->clickSound, 0);

    if (insideSettings(s->plusRect, x, y) && s->volume < 128)
    {
        s->volume += 8;
        s->muted = 0;
        Mix_VolumeMusic(s->volume);
    }

    if (insideSettings(s->minusRect, x, y) && s->volume > 0)
    {
        s->volume -= 8;
        Mix_VolumeMusic(s->volume);
    }

    if (insideSettings(s->muteRect, x, y))
    {
        s->muted = !s->muted;
        if (s->muted) Mix_VolumeMusic(0);
        else Mix_VolumeMusic(s->volume);
    }

    if (insideSettings(s->fullscreenRect, x, y))
    {
        SDL_SetWindowFullscreen(s->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        s->fullscreen = 1;
    }

    if (insideSettings(s->windowedRect, x, y))
    {
        SDL_SetWindowFullscreen(s->window, 0);
        s->fullscreen = 0;
    }

    if (insideSettings(s->backRect, x, y))
        *screen = mainScreen;
}

void drawSettingsButton(SettingsMenu *s, SDL_Texture *normal, SDL_Texture *hover, SDL_Rect rect, int isHover, char *fallback)
{
    SDL_SetRenderDrawColor(s->renderer, isHover ? 90 : 45, isHover ? 100 : 60, isHover ? 150 : 90, 255);
    SDL_RenderFillRect(s->renderer, &rect);
    SDL_SetRenderDrawColor(s->renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(s->renderer, &rect);

    if (isHover && hover)
        SDL_RenderCopy(s->renderer, hover, NULL, &rect);
    else if (normal)
        SDL_RenderCopy(s->renderer, normal, NULL, &rect);
    else
        drawSettingsText(s, fallback, rect.x + 25, rect.y + 18, s->font);
}

void displaySettingsMenu(SettingsMenu *s)
{
    char txt[50];
    SDL_Rect frame = {340, 80, 600, 560};
    SDL_Rect barBg;
    SDL_Rect barFill;

    SDL_SetRenderDrawColor(s->renderer, 15, 15, 35, 255);
    SDL_RenderClear(s->renderer);

    if (s->background)
        SDL_RenderCopy(s->renderer, s->background, NULL, NULL);

    SDL_SetRenderDrawColor(s->renderer, 30, 30, 70, 220);
    SDL_RenderFillRect(s->renderer, &frame);
    SDL_SetRenderDrawColor(s->renderer, 120, 160, 255, 255);
    SDL_RenderDrawRect(s->renderer, &frame);

    drawSettingsText(s, "Settings", 555, 110, s->titleFont);

    sprintf(txt, "Volume: %d", s->muted ? 0 : s->volume);
    drawSettingsText(s, txt, 555, 200, s->font);

    barBg = (SDL_Rect){440, 230, 400, 20};
    barFill = (SDL_Rect){440, 230, (400 * (s->muted ? 0 : s->volume)) / 128, 20};

    SDL_SetRenderDrawColor(s->renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(s->renderer, &barBg);
    SDL_SetRenderDrawColor(s->renderer, 100, 220, 120, 255);
    SDL_RenderFillRect(s->renderer, &barFill);
    SDL_SetRenderDrawColor(s->renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(s->renderer, &barBg);

    drawSettingsText(s, "Audio", 600, 260, s->font);
    drawSettingsButton(s, s->minusBtn, s->minusBtnHover, s->minusRect, s->minusHover, "-");
    drawSettingsButton(s, s->plusBtn, s->plusBtnHover, s->plusRect, s->plusHover, "+");

    drawSettingsText(s, "Display Mode", 555, 360, s->font);
    drawSettingsButton(s, s->windowedBtn, s->windowedBtnHover, s->windowedRect, s->windowedHover, "W");
    drawSettingsButton(s, s->fullscreenBtn, s->fullscreenBtnHover, s->fullscreenRect, s->fullscreenHover, "F");

    drawSettingsButton(s, s->muteBtn, s->muteBtnHover, s->muteRect, s->muteHover, "M");
    drawSettingsButton(s, s->backBtn, s->backBtnHover, s->backRect, s->backHover, "Back");

    SDL_RenderPresent(s->renderer);
}

void freeSettingsMenu(SettingsMenu *s)
{
    if (s->background) SDL_DestroyTexture(s->background);

    if (s->plusBtn) SDL_DestroyTexture(s->plusBtn);
    if (s->minusBtn) SDL_DestroyTexture(s->minusBtn);
    if (s->fullscreenBtn) SDL_DestroyTexture(s->fullscreenBtn);
    if (s->windowedBtn) SDL_DestroyTexture(s->windowedBtn);
    if (s->backBtn) SDL_DestroyTexture(s->backBtn);
    if (s->muteBtn) SDL_DestroyTexture(s->muteBtn);

    if (s->plusBtnHover && s->plusBtnHover != s->plusBtn) SDL_DestroyTexture(s->plusBtnHover);
    if (s->minusBtnHover && s->minusBtnHover != s->minusBtn) SDL_DestroyTexture(s->minusBtnHover);
    if (s->fullscreenBtnHover && s->fullscreenBtnHover != s->fullscreenBtn) SDL_DestroyTexture(s->fullscreenBtnHover);
    if (s->windowedBtnHover && s->windowedBtnHover != s->windowedBtn) SDL_DestroyTexture(s->windowedBtnHover);
    if (s->backBtnHover && s->backBtnHover != s->backBtn) SDL_DestroyTexture(s->backBtnHover);
    if (s->muteBtnHover && s->muteBtnHover != s->muteBtn) SDL_DestroyTexture(s->muteBtnHover);

    if (s->titleFont) TTF_CloseFont(s->titleFont);
    if (s->font) TTF_CloseFont(s->font);
    if (s->clickSound) Mix_FreeChunk(s->clickSound);
}
