#include "background.h"

void landOnPlatform(SDL_Rect *player,
                    int *vy,
                    int *onGround,
                    SDL_Rect oldPos,
                    SDL_Rect platform)
{
    if (*vy >= 0 &&
        oldPos.y <= platform.y &&
        player->y >= platform.y - 10 &&
        player->x + player->w > platform.x &&
        player->x < platform.x + platform.w)
    {
        player->y = platform.y;
        *vy = 0;
        *onGround = 1;
    }
}
void initPlatform(Platform *p,
                         int x,
                         int y,
                         int w,
                         int h,
                         int level)
{
    p->rect = (SDL_Rect){x, y, w, h};
    p->type = PLATFORM_FIXED;
    p->level = level;
    p->active = 1;
    p->direction = 1;
    p->min = 0;
    p->max = 0;
    p->speed = 0;
    p->breaking = 0;
    p->breakStart = 0;
}

void initBackground(Background *b, SDL_Renderer *r)
{
    SDL_Surface *s;

    s = IMG_Load("bg1.png");
    b->level1 = s ? SDL_CreateTextureFromSurface(r, s) : NULL;
    if (s)
    {
        b->level1W = s->w;
        b->level1H = s->h;
        SDL_FreeSurface(s);
    }
    else
    {
        b->level1W = 4000;
        b->level1H = 1200;
    }

    s = IMG_Load("bg2.png");
    b->level2 = s ? SDL_CreateTextureFromSurface(r, s) : NULL;
    if (s)
    {
        b->level2W = s->w;
        b->level2H = s->h;
        SDL_FreeSurface(s);
    }
    else
    {
        b->level2W = 11471;
        b->level2H = 1200;
    }

    b->fixedTex = NULL;
    b->mobileTex = NULL;
    b->destruct0 = NULL;
    b->destruct1 = NULL;
    b->destruct2 = NULL;

    b->font = TTF_OpenFont("arial.ttf", 24);
    b->music1 = Mix_LoadMUS("LVL1.wav");
    b->music2 = Mix_LoadMUS("LVL2.wav");

    if (b->music1)
        Mix_PlayMusic(b->music1, -1);

    b->currentLevel = LEVEL1;
    b->singleMode = 1;

    b->showGuide = 1;
    b->guideSingle = 1;
    b->guideMulti = 0;

    b->guideSingleTimer = 0;
    b->guideMultiTimer = 0;

    b->startTime1 = SDL_GetTicks();
    b->startTime2 = 0;

    b->elapsed1 = 0;
    b->elapsed2 = 0;

    b->player1 = (SDL_Rect){LVL1_PLAYER_START_X, LVL1_PLAYER_START_Y, 40, 60};
    b->player2 = (SDL_Rect){LVL1_PLAYER_START_X + 100, LVL1_PLAYER_START_Y, 40, 60};

    b->vy1 = 0;
    b->vy2 = 0;

    b->onGround1 = 1;
    b->onGround2 = 1;

    b->floorY = LVL1_GROUND_Y;

    b->spawn1x = LVL1_PLAYER_START_X;
    b->spawn1y = LVL1_PLAYER_START_Y;

    b->spawn2x = LVL1_PLAYER_START_X + 100;
    b->spawn2y = LVL1_PLAYER_START_Y;

    b->cam1 = (SDL_Rect){0, 0, WINDOW_W, WINDOW_H};
    b->cam2 = (SDL_Rect){0, 0, WINDOW_W / 2, WINDOW_H};

    b->zone12 = (SDL_Rect){LVL1_END_X, LVL1_TRANSFER_Y, 80, 120};
    b->zone21 = (SDL_Rect){0, 0, 10, 120};

    initPlatform(&b->p[0], 0, LVL1_GROUND_Y, 3999, 20, LEVEL1);
    initPlatform(&b->p[1], 0, LVL1_FLOOR1_Y, 3999, 20, LEVEL1);
    initPlatform(&b->p[2], 0, LVL1_FLOOR2_Y, 3999, 20, LEVEL1);
    initPlatform(&b->p[3], 0, LVL1_FLOOR3_Y, 3999, 20, LEVEL1);
    initPlatform(&b->p[4], 0, LVL2_GROUND_Y, 11471, 20, LEVEL2);

    b->nbPlatform = 5;

    b->stairs[0] = (SDL_Rect){1430, 581, 52, 299};
    b->stairs[1] = (SDL_Rect){3090, 581, 55, 299};
    b->stairs[2] = (SDL_Rect){1715, 326, 55, 255};
    b->stairs[3] = (SDL_Rect){55, 326, 50, 255};

    b->stairCount = 4;
}

void updateBackground(Background *b)
{
    const Uint8 *k = SDL_GetKeyboardState(NULL);
    int i;
    int sp = 5;
    int w = b->currentLevel == LEVEL1 ? b->level1W : b->level2W;
    SDL_Rect old1 = b->player1;
    SDL_Rect old2 = b->player2;

    if (k[SDL_SCANCODE_RIGHT])
        b->player1.x += sp;

    if (k[SDL_SCANCODE_LEFT])
        b->player1.x -= sp;

    if (!b->singleMode)
    {
        if (k[SDL_SCANCODE_D])
            b->player2.x += sp;

        if (k[SDL_SCANCODE_A])
            b->player2.x -= sp;
    }

    if (b->currentLevel == LEVEL1)
    {
        for (i = 0; i < b->stairCount; i++)
        {
            if (SDL_HasIntersection(&b->player1, &b->stairs[i]))
            {
                if (k[SDL_SCANCODE_UP])
                {
                    b->player1.y -= 6;
                    b->vy1 = 0;
                    b->onGround1 = 1;
                }

                if (k[SDL_SCANCODE_DOWN])
                {
                    b->player1.y += 6;
                    b->vy1 = 0;
                    b->onGround1 = 1;
                }
            }

            if (!b->singleMode && SDL_HasIntersection(&b->player2, &b->stairs[i]))
            {
                if (k[SDL_SCANCODE_W])
                {
                    b->player2.y -= 6;
                    b->vy2 = 0;
                    b->onGround2 = 1;
                }

                if (k[SDL_SCANCODE_S])
                {
                    b->player2.y += 6;
                    b->vy2 = 0;
                    b->onGround2 = 1;
                }
            }
        }
    }

    if (k[SDL_SCANCODE_SPACE] && b->onGround1)
    {
        b->vy1 = -18;
        b->onGround1 = 0;
    }

    if (!b->singleMode && k[SDL_SCANCODE_W] && b->onGround2)
    {
        b->vy2 = -18;
        b->onGround2 = 0;
    }

    b->vy1 += 1;
    if (b->vy1 > 18)
        b->vy1 = 18;

    b->player1.y += b->vy1;
    b->onGround1 = 0;

    if (!b->singleMode)
    {
        b->vy2 += 1;
        if (b->vy2 > 18)
            b->vy2 = 18;

        b->player2.y += b->vy2;
        b->onGround2 = 0;
    }

    for (i = 0; i < b->nbPlatform; i++)
    {
        if (b->p[i].active && b->p[i].level == b->currentLevel)
        {
            landOnPlatform(&b->player1, &b->vy1, &b->onGround1, old1, b->p[i].rect);

            if (!b->singleMode)
                landOnPlatform(&b->player2, &b->vy2, &b->onGround2, old2, b->p[i].rect);
        }
    }

    if (b->player1.x < 0)
        b->player1.x = 0;

    if (b->player1.x + b->player1.w > w)
        b->player1.x = w - b->player1.w;

    if (!b->singleMode)
    {
        if (b->player2.x < 0)
            b->player2.x = 0;

        if (b->player2.x + b->player2.w > w)
            b->player2.x = w - b->player2.w;
    }

    if (b->currentLevel == LEVEL1 &&
        b->player1.x >= LVL1_END_X &&
        b->player1.y <= LVL1_TRANSFER_Y + 80)
    {
        b->currentLevel = LEVEL2;

        b->floorY = LVL2_GROUND_Y;

        b->spawn1x = LVL2_PLAYER_START_X;
        b->spawn1y = LVL2_PLAYER_START_Y;

        b->spawn2x = LVL2_PLAYER_START_X + 100;
        b->spawn2y = LVL2_PLAYER_START_Y;

        b->player1 = (SDL_Rect){LVL2_PLAYER_START_X, LVL2_PLAYER_START_Y, 40, 60};
        b->player2 = (SDL_Rect){LVL2_PLAYER_START_X + 100, LVL2_PLAYER_START_Y, 40, 60};

        b->vy1 = 0;
        b->vy2 = 0;

        b->onGround1 = 1;
        b->onGround2 = 1;

        b->cam1.x = 0;
        b->cam1.y = 0;
        b->cam2 = b->cam1;

        if (b->music2)
        {
            Mix_HaltMusic();
            Mix_PlayMusic(b->music2, -1);
        }
    }
}

void scrollBackground(Background *b)
{
    int w = b->currentLevel == LEVEL1 ? b->level1W : b->level2W;
    int h = b->currentLevel == LEVEL1 ? b->level1H : b->level2H;

    b->cam1.w = WINDOW_W;
    b->cam1.h = WINDOW_H;

    b->cam1.x = b->player1.x + b->player1.w / 2 - WINDOW_W / 2;
    b->cam1.y = b->player1.y + b->player1.h / 2 - WINDOW_H / 2;

    if (b->cam1.x < 0)
        b->cam1.x = 0;

    if (b->cam1.y < 0)
        b->cam1.y = 0;

    if (b->cam1.x + b->cam1.w > w)
        b->cam1.x = w - b->cam1.w;

    if (b->cam1.y + b->cam1.h > h)
        b->cam1.y = h - b->cam1.h;

    if (b->cam1.x < 0)
        b->cam1.x = 0;

    if (b->cam1.y < 0)
        b->cam1.y = 0;
}

void animateBackground(Background *b)
{
    unsigned int now = SDL_GetTicks();
    b->elapsed1 = (now - b->startTime1) / 1000;
}

void displayBackground(Background *b, SDL_Renderer *r)
{
    SDL_RenderCopy(r,
                   b->currentLevel == LEVEL1 ? b->level1 : b->level2,
                   &b->cam1,
                   NULL);
}

void freeBackground(Background *b)
{
    if (b->level1)
        SDL_DestroyTexture(b->level1);

    if (b->level2)
        SDL_DestroyTexture(b->level2);

    if (b->fixedTex)
        SDL_DestroyTexture(b->fixedTex);

    if (b->mobileTex)
        SDL_DestroyTexture(b->mobileTex);

    if (b->destruct0)
        SDL_DestroyTexture(b->destruct0);

    if (b->destruct1)
        SDL_DestroyTexture(b->destruct1);

    if (b->destruct2)
        SDL_DestroyTexture(b->destruct2);

    if (b->font)
        TTF_CloseFont(b->font);

    if (b->music1)
        Mix_FreeMusic(b->music1);

    if (b->music2)
        Mix_FreeMusic(b->music2);
}
