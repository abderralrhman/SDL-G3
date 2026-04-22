#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "background.h"

int main()
{
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Event e;
    Background bg;
    int running = 1;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) return 1;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return 1;
    if (TTF_Init() == -1) return 1;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return 1;

    win = SDL_CreateWindow("background", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);
    if (!win) return 1;

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) return 1;

    initBackground(&bg, ren);

    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) running = 0;

            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = 0;

                if (e.key.keysym.sym == SDLK_F2)
                {
                    bg.singleMode = !bg.singleMode;
                    bg.showGuide = 1;
                    bg.guideMulti = !bg.singleMode;
                    bg.guideMultiTimer = SDL_GetTicks();

                    if (!bg.singleMode)
                    {
                        bg.player2.x = bg.player1.x + 70;
                        bg.player2.y = bg.player1.y;
                        bg.startTime2 = SDL_GetTicks();
                    }
                }

                if (e.key.keysym.sym == SDLK_h) bg.showGuide = 1;
                if (e.key.keysym.sym == SDLK_g) bg.showGuide = 0;
            }
        }

        updateBackground(&bg);
        scrollBackground(&bg);
        animateBackground(&bg);

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        displayBackground(&bg, ren);
        SDL_RenderPresent(ren);
    }

    freeBackground(&bg);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
