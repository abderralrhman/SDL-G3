#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "background.h"
#include "menu.h"
#include "player.h"
#include "enemy.h"
#include "minimap.h"
#include "settings.h"
#include "puzzle.h"

int hitEnemy(EnemyModule *enemy, SDL_Rect p)
{
    int i;

    for (i = 0; i < MAX_ENEMY; i++)
    {
        if (enemy->live[i] && SDL_HasIntersection(&p, &enemy->e[i]))
            return 1;
    }

    return 0;
}

void drawHud(SDL_Renderer *r, TTF_Font *font, int hp, int score, int time)
{
    SDL_Color c = {255, 255, 255, 255};
    SDL_Surface *s;
    SDL_Texture *t;
    SDL_Rect p;
    char txt[100];

    if (!font)
        return;

    sprintf(txt, "HP: %d   SCORE: %d   TIME: %02d:%02d",
            hp, score, time / 60, time % 60);

    s = TTF_RenderText_Solid(font, txt, c);
    if (!s)
        return;

    t = SDL_CreateTextureFromSurface(r, s);
    p = (SDL_Rect){20, 50, s->w, s->h};

    SDL_RenderCopy(r, t, NULL, &p);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
}

void drawDebugText(SDL_Renderer *r, TTF_Font *font, Background *bg, int mouseX, int mouseY)
{
    SDL_Color c = {255, 255, 0, 255};
    SDL_Surface *s;
    SDL_Texture *t;
    SDL_Rect pos;
    char txt[256];
    int worldX;
    int worldY;

    if (!font)
        return;

    worldX = bg->cam1.x + mouseX;
    worldY = bg->cam1.y + mouseY;

    sprintf(txt,
            "LEVEL:%d | SCREEN x=%d y=%d | WORLD x=%d y=%d | PLAYER x=%d y=%d",
            bg->currentLevel,
            mouseX,
            mouseY,
            worldX,
            worldY,
            bg->player1.x,
            bg->player1.y);

    s = TTF_RenderText_Solid(font, txt, c);
    if (!s)
        return;

    t = SDL_CreateTextureFromSurface(r, s);

    pos.x = 20;
    pos.y = WINDOW_H - 40;
    pos.w = s->w;
    pos.h = s->h;

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 180);

    {
        SDL_Rect bgBox = {10, WINDOW_H - 50, s->w + 20, s->h + 10};
        SDL_RenderFillRect(r, &bgBox);
    }

    SDL_RenderCopy(r, t, NULL, &pos);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
}

void switchToLevel1(Background *bg, EnemyModule *enemy)
{
    bg->currentLevel = LEVEL1;

    bg->player1.x = LVL1_PLAYER_START_X;
    bg->player1.y = LVL1_PLAYER_START_Y;

    bg->player2.x = LVL1_PLAYER_START_X + 100;
    bg->player2.y = LVL1_PLAYER_START_Y;

    bg->cam1.x = 0;
    bg->cam1.y = 0;
    bg->cam2 = bg->cam1;

    bg->vy1 = 0;
    bg->vy2 = 0;

    bg->onGround1 = 1;
    bg->onGround2 = 1;

    bg->floorY = LVL1_GROUND_Y;

    if (bg->music1)
    {
        Mix_HaltMusic();
        Mix_PlayMusic(bg->music1, -1);
    }

    loadEnemyLevel(enemy, LEVEL1);
}

void switchToLevel2(Background *bg, EnemyModule *enemy)
{
    bg->currentLevel = LEVEL2;

    bg->player1.x = LVL2_PLAYER_START_X;
    bg->player1.y = LVL2_PLAYER_START_Y;

    bg->player2.x = LVL2_PLAYER_START_X + 100;
    bg->player2.y = LVL2_PLAYER_START_Y;

    bg->cam1.x = 0;
    bg->cam1.y = LVL2_PLAYER_START_Y - 360;

    if (bg->cam1.y < 0)
        bg->cam1.y = 0;

    bg->cam2 = bg->cam1;

    bg->vy1 = 0;
    bg->vy2 = 0;

    bg->onGround1 = 0;
    bg->onGround2 = 0;

    bg->floorY = LVL2_GROUND_Y;

    if (bg->music2)
    {
        Mix_HaltMusic();
        Mix_PlayMusic(bg->music2, -1);
    }

    loadEnemyLevel(enemy, LEVEL2);
}

int main()
{
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Event e;

    Background bg;
    Menu menu;
    PlayerModule player;
    EnemyModule enemy;
    MiniMap mini;
    SettingsMenu settings;
    PuzzleMenu puzzle;

    SDL_Rect view1;
    SDL_Rect view2;

    PlayerControls controls;

    int running = 1;
    int screen = SCREEN_MENU;
    int lastLevel = LEVEL1;
    int finalScore = 0;
    int playerHp = 4;
    int damageTimer = 0;

    int debugMouseX = 0;
    int debugMouseY = 0;
    int showDebug = 1;

    controls.left = SDL_SCANCODE_LEFT;
    controls.right = SDL_SCANCODE_RIGHT;
    controls.jump = SDL_SCANCODE_SPACE;
    controls.punch = SDL_SCANCODE_Z;
    controls.shoot = SDL_SCANCODE_X;
    controls.leftKey = SDLK_LEFT;
    controls.rightKey = SDLK_RIGHT;
    controls.jumpKey = SDLK_SPACE;
    controls.punchKey = SDLK_z;
    controls.shootKey = SDLK_x;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        return 1;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        return 1;

    if (TTF_Init() == -1)
        return 1;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        return 1;

    win = SDL_CreateWindow("Integration Game",
                           SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED,
                           WINDOW_W,
                           WINDOW_H,
                           0);

    if (!win)
        return 1;

    ren = SDL_CreateRenderer(win,
                             -1,
                             SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!ren)
        return 1;

    SDL_StartTextInput();

    initBackground(&bg, ren);
    initMenu(&menu, ren);
    initSettingsMenu(&settings, win, ren);
    initPlayerModule(&player, ren);
    initEnemyModule(&enemy, ren);
    initMiniMap(&mini, ren, &bg);
    Puzzle_Init(&puzzle, ren);

    while (running)
    {
        finalScore = enemy.score + 1000 - (int)bg.elapsed1;

        if (finalScore < 0)
            finalScore = enemy.score;

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = 0;

            if (screen == SCREEN_PUZZLE)
            {
                if (e.type == SDL_MOUSEMOTION)
                    Puzzle_HandleHover(&puzzle, e.motion.x, e.motion.y);
                else
                {
                    int puzzleRun = 1;
                    Puzzle_HandleEvent(&puzzle, &e, &puzzleRun);

                    if (!puzzleRun)
                        screen = SCREEN_MENU;
                }
            }
            else if (screen == SCREEN_OPTIONS)
            {
                handleSettingsMenu(&settings, &e, &screen, SCREEN_MENU);
            }
            else if (screen != SCREEN_GAME)
            {
                handleMenu(&menu, &e, &screen, win, ren, finalScore);

                if (screen == SCREEN_QUIT)
                    running = 0;

                if (screen == SCREEN_GAME)
                {
                    bg.singleMode = !menu.multi;

                    playerHp = 4;
                    damageTimer = 0;

                    player.hp = 4;
                    player.dead = 0;
                    player.state = P_IDLE;
                    player.frame = 0;
                    player.real.active = 1;
                    player.real.health = 4;
                }
            }
            else
            {
                if (e.type == SDL_MOUSEMOTION)
                {
                    debugMouseX = e.motion.x;
                    debugMouseY = e.motion.y;
                }

                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    debugMouseX = e.button.x;
                    debugMouseY = e.button.y;
                }

                handlePlayerEvent(&player.real, &e, controls);

                if (e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_ESCAPE)
                        screen = SCREEN_MENU;

                    if (e.key.keysym.sym == SDLK_F5)
                    {
                        menu.page = PAGE_INPUT;
                        menu.name[0] = '\0';
                        screen = SCREEN_SCORES;
                    }

                    if (e.key.keysym.sym == SDLK_F1)
                        showDebug = !showDebug;

                    if (e.key.keysym.sym == SDLK_1)
                    {
                        switchToLevel1(&bg, &enemy);
                        lastLevel = LEVEL1;
                    }

                    if (e.key.keysym.sym == SDLK_2)
                    {
                        switchToLevel2(&bg, &enemy);
                        lastLevel = LEVEL2;
                    }

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

                    if (e.key.keysym.sym == SDLK_h)
                        bg.showGuide = 1;

                    if (e.key.keysym.sym == SDLK_g)
                        bg.showGuide = 0;
                }
            }
        }

        if (screen == SCREEN_GAME)
        {
            const Uint8 *k = SDL_GetKeyboardState(NULL);
            int i;

            updateBackground(&bg);
            scrollBackground(&bg);
            animateBackground(&bg);

            if (!player.dead)
            {
                if (k[SDL_SCANCODE_LEFT])
                {
                    player.dir = 0;
                    setPlayerAction(&player, P_WALK);
                }
                else if (k[SDL_SCANCODE_RIGHT])
                {
                    player.dir = 1;
                    setPlayerAction(&player, P_WALK);
                }
                else
                {
                    if (player.real.state != PLAYER_PUNCH && player.real.state != PLAYER_SHOOT)
                        setPlayerAction(&player, P_IDLE);
                }

                if ((k[SDL_SCANCODE_LEFT] || k[SDL_SCANCODE_RIGHT]) && k[SDL_SCANCODE_LSHIFT])
                    setPlayerAction(&player, P_RUN);
            }

            player.real.onGround = bg.onGround1;
            player.pos = bg.player1;

            updatePlayerModule(&player);
            updateEnemyModule(&enemy, &bg);

            for (i = 0; i < MAX_ENEMY; i++)
            {
                if (enemy.live[i] && playerBulletHitRect(&player.real, enemy.e[i]))
                {
                    enemy.hp[i] -= 50;

                    if (enemy.hp[i] <= 0)
                    {
                        enemy.live[i] = 0;
                        enemy.score += enemy.hpMax[i] > 100 ? 50 : 20;
                    }
                }
            }

            if (damageTimer > 0)
                damageTimer--;

            if (damageTimer == 0 && hitEnemy(&enemy, bg.player1))
            {
                setPlayerDamage(&player);

                playerHp = player.hp;
                damageTimer = 60;

                bg.player1.x -= 80;

                if (playerHp <= 0)
                {
                    menu.page = PAGE_INPUT;
                    menu.name[0] = '\0';
                    screen = SCREEN_SCORES;
                }
            }

            if (bg.currentLevel != lastLevel)
            {
                loadEnemyLevel(&enemy, bg.currentLevel);
                lastLevel = bg.currentLevel;
            }

            updateMiniMap(&mini, &bg);

            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
            SDL_RenderClear(ren);

            displayBackground(&bg, ren);

            view1 = (SDL_Rect){0, 0, bg.singleMode ? WINDOW_W : WINDOW_W / 2, WINDOW_H};

            displayPlayerModule(&player, ren, bg.player1, bg.cam1, view1);

            if (!bg.singleMode)
            {
                view2 = (SDL_Rect){WINDOW_W / 2, 0, WINDOW_W / 2, WINDOW_H};
                displayPlayerModule(&player, ren, bg.player2, bg.cam2, view2);
            }

            displayEnemyModule(&enemy, ren, &bg);
            displayMiniMap(&mini, ren, &bg);
            drawHud(ren, bg.font, playerHp, enemy.score, bg.elapsed1);

            if (showDebug)
                drawDebugText(ren, bg.font, &bg, debugMouseX, debugMouseY);

            SDL_RenderPresent(ren);
        }
        else
        {
            if (screen == SCREEN_OPTIONS)
            {
                displaySettingsMenu(&settings);
            }
            else if (screen == SCREEN_PUZZLE)
            {
                SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
                SDL_RenderClear(ren);

                Puzzle_Render(ren, &puzzle);

                SDL_RenderPresent(ren);
            }
            else
            {
                displayMenu(&menu, ren, screen);
            }
        }

        SDL_Delay(16);
    }

    Puzzle_Free(&puzzle);
    freeSettingsMenu(&settings);
    freeMiniMap(&mini);
    freeEnemyModule(&enemy);
    freePlayerModule(&player);
    freeMenu(&menu);
    freeBackground(&bg);

    SDL_StopTextInput();

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
