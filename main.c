#include "header.h"
#include "minimap.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GAME_BASE_WIDTH 1024
#define GAME_BASE_HEIGHT 768

typedef struct {
    SDL_Rect rect;
    float scaleX;
    float scaleY;
} Viewport;

void calculateViewport(Viewport* viewport, int windowWidth, int windowHeight) {
    float windowAspect = (float)windowWidth / windowHeight;
    float gameAspect = (float)GAME_BASE_WIDTH / GAME_BASE_HEIGHT;
    
    if (windowAspect > gameAspect) {
        viewport->rect.h = windowHeight;
        viewport->rect.w = (int)(windowHeight * gameAspect);
        viewport->rect.x = (windowWidth - viewport->rect.w) / 2;
        viewport->rect.y = 0;
    } else {
        viewport->rect.w = windowWidth;
        viewport->rect.h = (int)(windowWidth / gameAspect);
        viewport->rect.x = 0;
        viewport->rect.y = (windowHeight - viewport->rect.h) / 2;
    }
    
    viewport->scaleX = (float)viewport->rect.w / GAME_BASE_WIDTH;
    viewport->scaleY = (float)viewport->rect.h / GAME_BASE_HEIGHT;
}

SDL_Rect scaleRect(SDL_Rect original, Viewport viewport) {
    SDL_Rect scaled;
    scaled.x = viewport.rect.x + (int)(original.x * viewport.scaleX);
    scaled.y = viewport.rect.y + (int)(original.y * viewport.scaleY);
    scaled.w = (int)(original.w * viewport.scaleX);
    scaled.h = (int)(original.h * viewport.scaleY);
    return scaled;
}

int main(void) {
    App app = {0};
    int showMenu = 0;
    int gameRunning = 1;
    SDL_Event event;
    Viewport gameViewport = {0};
    
    int windowWidth = GAME_BASE_WIDTH;
    int windowHeight = GAME_BASE_HEIGHT;

    printf("========================================\n");
    printf("     GAME WITH VOLUME SUB-MENU\n");
    printf("     Press V to open the menu\n");
    printf("     Press ESC to quit\n");
    printf("========================================\n");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Error SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        printf("Error IMG_Init: %s\n", IMG_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("Error TTF_Init: %s\n", TTF_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Error Mix_OpenAudio: %s\n", Mix_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Game with Volume Menu",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          GAME_BASE_WIDTH, GAME_BASE_HEIGHT, 
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Error creating window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        return 1;
    }

    app.window = window;
    app.renderer = renderer;
    app.running = 1;
    app.windowWidth = GAME_BASE_WIDTH;
    app.windowHeight = GAME_BASE_HEIGHT;
    app.fullscreen = 0;
    app.volume = 50;
    app.savedVolume = 50;
    app.muted = 0;

    app.plusHover = 0;
    app.minusHover = 0;
    app.fullscreenHover = 0;
    app.windowedHover = 0;
    app.quitHover = 0;
    app.muteHover = 0;

    app.plusBtn        = IMG_LoadTexture(renderer, "+.png");
    app.minusBtn       = IMG_LoadTexture(renderer, "-.png");
    app.fullscreenBtn  = IMG_LoadTexture(renderer, "f.png");
    app.windowedBtn    = IMG_LoadTexture(renderer, "p.png");
    app.quitBtn        = IMG_LoadTexture(renderer, "r.png");
    app.muteBtn        = IMG_LoadTexture(renderer, "m.png");

    app.plusBtnHover       = IMG_LoadTexture(renderer, "++.png");
    app.minusBtnHover      = IMG_LoadTexture(renderer, "--.png");
    app.fullscreenBtnHover = IMG_LoadTexture(renderer, "ff.png");
    app.windowedBtnHover   = IMG_LoadTexture(renderer, "pp.png");
    app.quitBtnHover       = IMG_LoadTexture(renderer, "rr.png");
    app.muteBtnHover       = IMG_LoadTexture(renderer, "mm.png");

    if (!app.plusBtnHover)       app.plusBtnHover       = app.plusBtn;
    if (!app.minusBtnHover)      app.minusBtnHover      = app.minusBtn;
    if (!app.fullscreenBtnHover) app.fullscreenBtnHover = app.fullscreenBtn;
    if (!app.windowedBtnHover)   app.windowedBtnHover   = app.windowedBtn;
    if (!app.quitBtnHover)       app.quitBtnHover       = app.quitBtn;
    if (!app.muteBtnHover)       app.muteBtnHover       = app.muteBtn;

    app.background = IMG_LoadTexture(renderer, "bk.png");

    app.backgroundMusic = Mix_LoadMUS("son.flac");
    if (!app.backgroundMusic)
        app.backgroundMusic = Mix_LoadMUS("son.mp3");
    if (app.backgroundMusic) {
        Mix_PlayMusic(app.backgroundMusic, -1);
        Mix_VolumeMusic(app.volume * 128 / 100);
    }

    app.clickSound = Mix_LoadWAV("clic.mp3");

    SDL_Rect playerPos;
    SDL_Texture *level1Background, *level2Background, *playerTexture;
    minimap miniMap;
    Platform platforms[20];
    Obstacle obstacles[30];
    BGAnimation bgAnim;

    int currentLevel  = 1;
    int direction     = -1;
    int numPlatforms  = 7;
    int numObstacles  = 15;
    int movementSpeed = 5;

    srand(time(NULL));

    level1Background = IMG_LoadTexture(renderer, "map.png");
    level2Background = IMG_LoadTexture(renderer, "map2.png");
    playerTexture    = IMG_LoadTexture(renderer, "perso.png");

    initPlatforms(platforms, renderer, currentLevel);
    initObstacles(obstacles, currentLevel, renderer);
    initBGAnimation(&bgAnim);

    playerPos.x = 50;
    playerPos.y = 50;
    playerPos.w = 48;
    playerPos.h = 48;

    initMinimap(&miniMap, renderer, level1Background, GAME_BASE_WIDTH, GAME_BASE_HEIGHT);
    
    calculateViewport(&gameViewport, app.windowWidth, app.windowHeight);

    while (gameRunning) {

        int centerX         = app.windowWidth / 2;
        int unifiedFrameY   = 80;
        int unifiedFrameWidth = 600;
        int unifiedFrameX   = centerX - unifiedFrameWidth / 2;

        SDL_Rect minusRect      = {centerX - 150,               unifiedFrameY + 160, 110, 70};
        SDL_Rect plusRect       = {centerX + 40,                unifiedFrameY + 160, 110, 70};
        SDL_Rect windowedRect   = {centerX - 150,               unifiedFrameY + 350, 110, 70};
        SDL_Rect fullscreenRect = {centerX + 40,                unifiedFrameY + 350, 110, 70};
        SDL_Rect muteRect       = {50,                          app.windowHeight - 70 - 50, 110, 70};
        SDL_Rect quitRect       = {app.windowWidth - 110 - 50,  app.windowHeight - 70 - 50, 110, 70};

        int mx, my;
        SDL_GetMouseState(&mx, &my);
        if (showMenu) {
            app.minusHover      = (mx >= minusRect.x      && mx <= minusRect.x + minusRect.w           && my >= minusRect.y      && my <= minusRect.y + minusRect.h);
            app.plusHover       = (mx >= plusRect.x       && mx <= plusRect.x + plusRect.w             && my >= plusRect.y       && my <= plusRect.y + plusRect.h);
            app.windowedHover   = (mx >= windowedRect.x   && mx <= windowedRect.x + windowedRect.w     && my >= windowedRect.y   && my <= windowedRect.y + windowedRect.h);
            app.fullscreenHover = (mx >= fullscreenRect.x && mx <= fullscreenRect.x + fullscreenRect.w && my >= fullscreenRect.y && my <= fullscreenRect.y + fullscreenRect.h);
            app.muteHover       = (mx >= muteRect.x       && mx <= muteRect.x + muteRect.w             && my >= muteRect.y       && my <= muteRect.y + muteRect.h);
            app.quitHover       = (mx >= quitRect.x       && mx <= quitRect.x + quitRect.w             && my >= quitRect.y       && my <= quitRect.y + quitRect.h);
        } else {
            app.minusHover = app.plusHover = app.windowedHover =
            app.fullscreenHover = app.muteHover = app.quitHover = 0;
        }

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                gameRunning = 0;
            }

            else if (event.type == SDL_WINDOWEVENT &&
                     event.window.event == SDL_WINDOWEVENT_RESIZED) {
                app.windowWidth  = event.window.data1;
                app.windowHeight = event.window.data2;
                calculateViewport(&gameViewport, app.windowWidth, app.windowHeight);
            }

            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        if (showMenu)
                            showMenu = 0;
                        else
                            gameRunning = 0;
                        break;
                    case SDLK_v:
                        showMenu = !showMenu;
                        printf("Volume menu: %s\n", showMenu ? "OPEN" : "CLOSED");
                        break;
                    case SDLK_m:
                        if (showMenu) {
                            if (app.muted) {
                                app.muted = 0;
                                app.volume = app.savedVolume;
                            } else {
                                app.muted = 1;
                                app.savedVolume = app.volume;
                                app.volume = 0;
                            }
                            Mix_VolumeMusic(app.volume * 128 / 100);
                        }
                        break;
                    case SDLK_RIGHT:
                        if (!showMenu) direction = 1;
                        break;
                    case SDLK_LEFT:
                        if (!showMenu) direction = 0;
                        break;
                    case SDLK_UP:
                        if (!showMenu) direction = 2;
                        break;
                    case SDLK_DOWN:
                        if (!showMenu) direction = 3;
                        break;
                    case SDLK_SPACE:
                        if (!showMenu && currentLevel == 1) {
                            currentLevel = 2;
                            recreateMinimap(&miniMap, renderer, level2Background);
                            freePlatforms(platforms, numPlatforms);
                            initPlatforms(platforms, renderer, currentLevel);
                            freeObstacles(obstacles, numObstacles);
                            initObstacles(obstacles, currentLevel, renderer);
                            playerPos.x = 50; playerPos.y = 50;
                        } else if (!showMenu && currentLevel == 2) {
                            currentLevel = 1;
                            recreateMinimap(&miniMap, renderer, level1Background);
                            freePlatforms(platforms, numPlatforms);
                            initPlatforms(platforms, renderer, currentLevel);
                            freeObstacles(obstacles, numObstacles);
                            initObstacles(obstacles, currentLevel, renderer);
                            playerPos.x = 50; playerPos.y = 50;
                        }
                        break;
                    default:
                        break;
                }
            }

            else if (event.type == SDL_KEYUP && !showMenu) {
                switch (event.key.keysym.sym) {
                    case SDLK_RIGHT:
                    case SDLK_LEFT:
                    case SDLK_UP:
                    case SDLK_DOWN:
                        direction = -1;
                        break;
                    default:
                        break;
                }
            }

            else if (event.type == SDL_MOUSEBUTTONDOWN &&
                     event.button.button == SDL_BUTTON_LEFT && showMenu) {

                int cx = event.button.x;
                int cy = event.button.y;

                if (cx >= muteRect.x && cx <= muteRect.x + muteRect.w &&
                    cy >= muteRect.y && cy <= muteRect.y + muteRect.h) {
                    if (app.muted) {
                        app.muted  = 0;
                        app.volume = app.savedVolume;
                    } else {
                        app.muted       = 1;
                        app.savedVolume = app.volume;
                        app.volume      = 0;
                    }
                    Mix_VolumeMusic(app.volume * 128 / 100);
                    if (app.clickSound) Mix_PlayChannel(-1, app.clickSound, 0);
                }

                else if (cx >= quitRect.x && cx <= quitRect.x + quitRect.w &&
                         cy >= quitRect.y && cy <= quitRect.y + quitRect.h) {
                    showMenu = 0;
                    if (app.clickSound) Mix_PlayChannel(-1, app.clickSound, 0);
                }

                else if (cx >= minusRect.x && cx <= minusRect.x + minusRect.w &&
                         cy >= minusRect.y && cy <= minusRect.y + minusRect.h) {
                    if (!app.muted) {
                        app.volume -= 10;
                        if (app.volume < 0) app.volume = 0;
                        Mix_VolumeMusic(app.volume * 128 / 100);
                    }
                    if (app.clickSound) Mix_PlayChannel(-1, app.clickSound, 0);
                }

                else if (cx >= plusRect.x && cx <= plusRect.x + plusRect.w &&
                         cy >= plusRect.y && cy <= plusRect.y + plusRect.h) {
                    if (!app.muted) {
                        app.volume += 10;
                        if (app.volume > 100) app.volume = 100;
                        Mix_VolumeMusic(app.volume * 128 / 100);
                    }
                    if (app.clickSound) Mix_PlayChannel(-1, app.clickSound, 0);
                }

                else if (cx >= windowedRect.x && cx <= windowedRect.x + windowedRect.w &&
                         cy >= windowedRect.y && cy <= windowedRect.y + windowedRect.h) {
                    if (app.fullscreen) {
                        app.fullscreen = 0;
                        SDL_SetWindowFullscreen(window, 0);
                        SDL_SetWindowSize(window, GAME_BASE_WIDTH, GAME_BASE_HEIGHT);
                        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                        app.windowWidth = GAME_BASE_WIDTH;
                        app.windowHeight = GAME_BASE_HEIGHT;
                        calculateViewport(&gameViewport, app.windowWidth, app.windowHeight);
                    }
                    if (app.clickSound) Mix_PlayChannel(-1, app.clickSound, 0);
                }

                else if (cx >= fullscreenRect.x && cx <= fullscreenRect.x + fullscreenRect.w &&
                         cy >= fullscreenRect.y && cy <= fullscreenRect.y + fullscreenRect.h) {
                    if (!app.fullscreen) {
                        app.fullscreen = 1;
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        SDL_DisplayMode mode;
                        SDL_GetCurrentDisplayMode(0, &mode);
                        app.windowWidth = mode.w;
                        app.windowHeight = mode.h;
                        calculateViewport(&gameViewport, app.windowWidth, app.windowHeight);
                        printf("Fullscreen mode activated: %dx%d\n", app.windowWidth, app.windowHeight);
                        printf("Viewport: x=%d, y=%d, w=%d, h=%d\n", 
                               gameViewport.rect.x, gameViewport.rect.y, 
                               gameViewport.rect.w, gameViewport.rect.h);
                    }
                    if (app.clickSound) Mix_PlayChannel(-1, app.clickSound, 0);
                }
            }
        }

        if (!showMenu) {
            if (direction != -1) {
                SDL_Rect nextPos = playerPos;
                if      (direction == 1) nextPos.x += movementSpeed;
                else if (direction == 0) nextPos.x -= movementSpeed;
                else if (direction == 2) nextPos.y -= movementSpeed;
                else if (direction == 3) nextPos.y += movementSpeed;

                int platformCollision = collisionBBPlatforms(nextPos, platforms, numPlatforms);
                int obstacleCollision = collisionBBObstacles(nextPos, obstacles, numObstacles);

                if (!platformCollision && !obstacleCollision) {
                    playerPos = nextPos;
                } else {
                    triggerBGAnimation(&bgAnim, 3);
                }
            }

            updatePlatforms(platforms, numPlatforms);
            updateBGAnimation(&bgAnim);
            updateMinimap(&miniMap, playerPos);
            animateMinimap(&miniMap);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (!showMenu) {
            SDL_RenderSetViewport(renderer, &gameViewport.rect);
            
            SDL_Texture* currentBackground = (currentLevel == 1) ? level1Background : level2Background;
            if (currentBackground) {
                SDL_Rect bgDest = {0, 0, gameViewport.rect.w, gameViewport.rect.h};
                
                if (bgAnim.waveTimer > 0) {
                    float scale = 1.0f + (bgAnim.waveTimer * 0.003f);
                    SDL_Rect dest = {
                        (int)((gameViewport.rect.w - gameViewport.rect.w * scale) / 2),
                        (int)((gameViewport.rect.h - gameViewport.rect.h * scale) / 2),
                        (int)(gameViewport.rect.w * scale),
                        (int)(gameViewport.rect.h * scale)
                    };
                    SDL_RenderCopy(renderer, currentBackground, NULL, &dest);
                } else if (bgAnim.shakeTimer > 0) {
                    SDL_Rect dest = {(rand() % 7) - 3, (rand() % 7) - 3, gameViewport.rect.w, gameViewport.rect.h};
                    SDL_RenderCopy(renderer, currentBackground, NULL, &dest);
                } else {
                    SDL_RenderCopy(renderer, currentBackground, NULL, &bgDest);
                }
            }

            for (int i = 0; i < numObstacles; i++) {
                if (obstacles[i].active && obstacles[i].texture) {
                    SDL_Rect scaledRect = scaleRect(obstacles[i].rect, gameViewport);
                    SDL_RenderCopy(renderer, obstacles[i].texture, NULL, &scaledRect);
                }
            }

            for (int i = 0; i < numPlatforms; i++) {
                if (platforms[i].active && platforms[i].texture) {
                    SDL_Rect scaledRect = scaleRect(platforms[i].rect, gameViewport);
                    SDL_RenderCopy(renderer, platforms[i].texture, NULL, &scaledRect);
                }
            }

            if (playerTexture) {
                SDL_Rect scaledPlayer = scaleRect(playerPos, gameViewport);
                SDL_RenderCopy(renderer, playerTexture, NULL, &scaledPlayer);
            }

            SDL_RenderSetViewport(renderer, NULL);
            displayMinimap(miniMap, renderer);
            SDL_RenderSetViewport(renderer, &gameViewport.rect);
        }

        SDL_RenderSetViewport(renderer, NULL);

        if (showMenu) {
            SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
            SDL_RenderClear(renderer);

            if (app.background)
                SDL_RenderCopy(renderer, app.background, NULL, NULL);

            int totalFrameHeight = 500;
            SDL_SetRenderDrawColor(renderer, 40, 40, 80, 200);
            SDL_Rect unifiedFrame = {unifiedFrameX, unifiedFrameY, unifiedFrameWidth, totalFrameHeight};
            SDL_RenderFillRect(renderer, &unifiedFrame);

            SDL_SetRenderDrawColor(renderer, 80, 100, 180, 100);
            SDL_Rect shineRect = {unifiedFrameX, unifiedFrameY, unifiedFrameWidth, 30};
            SDL_RenderFillRect(renderer, &shineRect);

            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
            SDL_RenderDrawRect(renderer, &unifiedFrame);
            SDL_Rect innerBorder = {unifiedFrameX+2, unifiedFrameY+2, unifiedFrameWidth-4, totalFrameHeight-4};
            SDL_SetRenderDrawColor(renderer, 80, 120, 200, 255);
            SDL_RenderDrawRect(renderer, &innerBorder);

            TTF_Font* titleFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 36);
            if (titleFont) {
                SDL_Surface* surf = TTF_RenderUTF8_Blended(titleFont, "VOLUME MENU", (SDL_Color){255,255,255,255});
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                    SDL_Rect dst = {centerX - surf->w/2, unifiedFrameY + 30, surf->w, surf->h};
                    SDL_RenderCopy(renderer, tex, NULL, &dst);
                    SDL_DestroyTexture(tex);
                    SDL_FreeSurface(surf);
                }
                TTF_CloseFont(titleFont);
            }

            SDL_SetRenderDrawColor(renderer, 100, 100, 180, 150);
            SDL_RenderDrawLine(renderer, unifiedFrameX+40, unifiedFrameY+100,
                                         unifiedFrameX+unifiedFrameWidth-40, unifiedFrameY+100);

            TTF_Font* labelFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
            if (labelFont) {
                SDL_Surface* surf = TTF_RenderUTF8_Blended(labelFont, "Audio Control", (SDL_Color){150,220,255,255});
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                    SDL_Rect dst = {centerX - surf->w/2, unifiedFrameY + 108, surf->w, surf->h};
                    SDL_RenderCopy(renderer, tex, NULL, &dst);
                    SDL_DestroyTexture(tex);
                    SDL_FreeSurface(surf);
                }

                SDL_Surface* surf2 = TTF_RenderUTF8_Blended(labelFont, "Screen Mode", (SDL_Color){255,180,100,255});
                if (surf2) {
                    SDL_Texture* tex2 = SDL_CreateTextureFromSurface(renderer, surf2);
                    SDL_Rect dst2 = {centerX - surf2->w/2, unifiedFrameY + 320, surf2->w, surf2->h};
                    SDL_RenderCopy(renderer, tex2, NULL, &dst2);
                    SDL_DestroyTexture(tex2);
                    SDL_FreeSurface(surf2);
                }
                TTF_CloseFont(labelFont);
            }

            SDL_SetRenderDrawColor(renderer,
                app.minusHover ? 100 : 80,
                app.minusHover ? 100 : 80,
                app.minusHover ? 220 : 180, 255);
            SDL_RenderFillRect(renderer, &minusRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &minusRect);
            if (app.minusBtn)
                SDL_RenderCopy(renderer, app.minusHover ? app.minusBtnHover : app.minusBtn, NULL, &minusRect);

            SDL_SetRenderDrawColor(renderer,
                app.plusHover ? 100 : 80,
                app.plusHover ? 220 : 180,
                app.plusHover ? 100 : 80, 255);
            SDL_RenderFillRect(renderer, &plusRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &plusRect);
            if (app.plusBtn)
                SDL_RenderCopy(renderer, app.plusHover ? app.plusBtnHover : app.plusBtn, NULL, &plusRect);

            char volumeStr[50];
            if (app.muted)
                snprintf(volumeStr, sizeof(volumeStr), "Volume: %d%% (MUTED)", app.savedVolume);
            else
                snprintf(volumeStr, sizeof(volumeStr), "Volume: %d%%", app.volume);

            SDL_Color volColor = app.muted
                ? (SDL_Color){255,100,100,255}
                : (SDL_Color){200,230,255,255};

            TTF_Font* valueFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 28);
            if (valueFont) {
                SDL_Surface* surf = TTF_RenderUTF8_Blended(valueFont, volumeStr, volColor);
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                    SDL_Rect dst = {centerX - surf->w/2, unifiedFrameY + 260, surf->w, surf->h};
                    SDL_RenderCopy(renderer, tex, NULL, &dst);
                    SDL_DestroyTexture(tex);
                    SDL_FreeSurface(surf);
                }
                TTF_CloseFont(valueFont);
            }

            int barWidth = 400;
            int barX = unifiedFrameX + (unifiedFrameWidth - barWidth) / 2;
            int barY = unifiedFrameY + 295;
            SDL_SetRenderDrawColor(renderer, 50, 50, 80, 255);
            SDL_Rect barBg = {barX, barY, barWidth, 22};
            SDL_RenderFillRect(renderer, &barBg);
            int filled = (barWidth * app.volume) / 100;
            SDL_SetRenderDrawColor(renderer,
                app.muted ? 120 : (app.volume < 30 ? 255 : 80),
                app.muted ? 120 : 180,
                app.muted ? 120 : 120, 255);
            SDL_Rect barFill = {barX, barY, filled, 22};
            SDL_RenderFillRect(renderer, &barFill);
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(renderer, &barBg);

            SDL_SetRenderDrawColor(renderer, 100, 100, 180, 150);
            SDL_RenderDrawLine(renderer, unifiedFrameX+40, unifiedFrameY+295,
                                         unifiedFrameX+unifiedFrameWidth-40, unifiedFrameY+295);

            SDL_SetRenderDrawColor(renderer,
                app.windowedHover ? 220 : 180,
                app.windowedHover ? 100 : 80,
                app.windowedHover ? 100 : 80, 255);
            SDL_RenderFillRect(renderer, &windowedRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &windowedRect);
            if (app.windowedBtn)
                SDL_RenderCopy(renderer, app.windowedHover ? app.windowedBtnHover : app.windowedBtn, NULL, &windowedRect);

            SDL_SetRenderDrawColor(renderer,
                app.fullscreenHover ? 100 : 80,
                app.fullscreenHover ? 220 : 180,
                app.fullscreenHover ? 100 : 80, 255);
            SDL_RenderFillRect(renderer, &fullscreenRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &fullscreenRect);
            if (app.fullscreenBtn)
                SDL_RenderCopy(renderer, app.fullscreenHover ? app.fullscreenBtnHover : app.fullscreenBtn, NULL, &fullscreenRect);

            const char* modeStr = app.fullscreen ? "Mode: full screen" : "Mode: window";
            SDL_Color modeColor = app.fullscreen
                ? (SDL_Color){255,200,100,255}
                : (SDL_Color){150,200,255,255};
            TTF_Font* modeFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 22);
            if (modeFont) {
                SDL_Surface* surf = TTF_RenderUTF8_Blended(modeFont, modeStr, modeColor);
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                    SDL_Rect dst = {centerX - surf->w/2, unifiedFrameY + 460, surf->w, surf->h};
                    SDL_RenderCopy(renderer, tex, NULL, &dst);
                    SDL_DestroyTexture(tex);
                    SDL_FreeSurface(surf);
                }
                TTF_CloseFont(modeFont);
            }

            SDL_SetRenderDrawColor(renderer, 100, 100, 150, 100);
            SDL_RenderDrawLine(renderer, 50, app.windowHeight - 150,
                                          app.windowWidth - 50, app.windowHeight - 150);

            SDL_SetRenderDrawColor(renderer,
                app.muted ? (app.muteHover ? 230 : 200) : (app.muteHover ? 180 : 150),
                app.muted ? (app.muteHover ?  80 :  50) : (app.muteHover ? 140 : 100),
                app.muted ? (app.muteHover ?  80 :  50) : (app.muteHover ? 240 : 200), 255);
            SDL_RenderFillRect(renderer, &muteRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &muteRect);
            if (app.muteBtn)
                SDL_RenderCopy(renderer, app.muteHover ? app.muteBtnHover : app.muteBtn, NULL, &muteRect);

            SDL_SetRenderDrawColor(renderer,
                app.quitHover ? 230 : 200,
                app.quitHover ?  80 :  50,
                app.quitHover ?  80 :  50, 255);
            SDL_RenderFillRect(renderer, &quitRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &quitRect);
            if (app.quitBtn)
                SDL_RenderCopy(renderer, app.quitHover ? app.quitBtnHover : app.quitBtn, NULL, &quitRect);

            TTF_Font* smallFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 18);
            if (smallFont) {
                SDL_Surface* surf = TTF_RenderUTF8_Blended(smallFont, "Press V or click Return to close",
                                                            (SDL_Color){150,150,200,255});
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                    SDL_Rect dst = {centerX - surf->w/2, app.windowHeight - 28, surf->w, surf->h};
                    SDL_RenderCopy(renderer, tex, NULL, &dst);
                    SDL_DestroyTexture(tex);
                    SDL_FreeSurface(surf);
                }
                TTF_CloseFont(smallFont);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);

    }

    if (app.backgroundMusic) Mix_FreeMusic(app.backgroundMusic);
    if (app.clickSound)      Mix_FreeChunk(app.clickSound);

    if (app.plusBtnHover       && app.plusBtnHover       != app.plusBtn)       SDL_DestroyTexture(app.plusBtnHover);
    if (app.minusBtnHover      && app.minusBtnHover      != app.minusBtn)      SDL_DestroyTexture(app.minusBtnHover);
    if (app.fullscreenBtnHover && app.fullscreenBtnHover != app.fullscreenBtn) SDL_DestroyTexture(app.fullscreenBtnHover);
    if (app.windowedBtnHover   && app.windowedBtnHover   != app.windowedBtn)   SDL_DestroyTexture(app.windowedBtnHover);
    if (app.quitBtnHover       && app.quitBtnHover       != app.quitBtn)       SDL_DestroyTexture(app.quitBtnHover);
    if (app.muteBtnHover       && app.muteBtnHover       != app.muteBtn)       SDL_DestroyTexture(app.muteBtnHover);

    if (app.plusBtn)       SDL_DestroyTexture(app.plusBtn);
    if (app.minusBtn)      SDL_DestroyTexture(app.minusBtn);
    if (app.fullscreenBtn) SDL_DestroyTexture(app.fullscreenBtn);
    if (app.windowedBtn)   SDL_DestroyTexture(app.windowedBtn);
    if (app.quitBtn)       SDL_DestroyTexture(app.quitBtn);
    if (app.muteBtn)       SDL_DestroyTexture(app.muteBtn);
    if (app.background)    SDL_DestroyTexture(app.background);

    freeMinimap(&miniMap);
    freePlatforms(platforms, numPlatforms);
    freeObstacles(obstacles, numObstacles);

    if (level1Background) SDL_DestroyTexture(level1Background);
    if (level2Background) SDL_DestroyTexture(level2Background);
    if (playerTexture)    SDL_DestroyTexture(playerTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
