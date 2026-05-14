#include "enemy.h"
#include <stdlib.h>

static SDL_Texture *loadEnemyTexture(SDL_Renderer *r, const char *a, const char *b)
{
    SDL_Texture *t;
    t = IMG_LoadTexture(r, a);
    if (!t) t = IMG_LoadTexture(r, b);
    return t;
}

static int inter(SDL_Rect a, SDL_Rect b)
{
    return SDL_HasIntersection(&a, &b);
}

void initEnemyModule(EnemyModule *em, SDL_Renderer *r)
{
    int i;
    int w;
    int h;
    em->normal = loadEnemyTexture(r, "enemy.png", "enemy.png");
    em->strong = loadEnemyTexture(r, "enemystrong.png", "enemystrong.png");
    em->coin = loadEnemyTexture(r, "coin.png", "coin.png");
    em->medkit = loadEnemyTexture(r, "medkit.png", "medkit.png");
    em->score = 0;
    em->normalSrc = (SDL_Rect){0, 0, 170, 256};
    em->strongSrc = (SDL_Rect){0, 0, 170, 256};
    if (em->normal && SDL_QueryTexture(em->normal, NULL, NULL, &w, &h) == 0)
        em->normalSrc = (SDL_Rect){0, 0, w / 6, h / 6};
    if (em->strong && SDL_QueryTexture(em->strong, NULL, NULL, &w, &h) == 0)
        em->strongSrc = (SDL_Rect){0, 0, w / 6, h / 6};
    for (i = 0; i < MAX_ENEMY; i++)
    {
        em->live[i] = 0;
        em->hp[i] = 100;
        em->hpMax[i] = 100;
        em->damage[i] = 5;
        em->dir[i] = 1;
        em->ai[i] = 0;
        em->chasing[i] = 0;
        em->frame[i] = 0;
        em->frameCounter[i] = 0;
    }
    loadEnemyLevel(em, LEVEL1);
}

void loadEnemyLevel(EnemyModule *em, int level)
{
    int i;
    int x1[6] = {180, 200, 600, 110, 850, 1030};
    int y1[6] = {795, 496,  795,  241,  496, -5 };
    int l1[6] = {120, 200, 400, 110, 650, 880};
    int r1[6] = {490, 650, 780, 330, 1120, 1180};
    int x2[6] = {170, 420, 760, 1080, 1320, 1600};
    int y2[6] = {550, 420, 330, 500, 260, 390};
    int l2[6] = {90, 300, 620, 920, 1180, 1450};
    int r2[6] = {360, 680, 980, 1260, 1500, 1780};
    for (i = 0; i < MAX_ENEMY; i++)
    {
        if (level == LEVEL2)
        {
            em->e[i] = (SDL_Rect){x2[i], y2[i], 60, 85};
            em->left[i] = l2[i];
            em->right[i] = r2[i];
        }
        else
        {
            em->e[i] = (SDL_Rect){x1[i], y1[i], 60, 85};
            em->left[i] = l1[i];
            em->right[i] = r1[i];
        }
        em->live[i] = 1;
        em->hpMax[i] = (i == 2 || i == 3) ? 150 : 100;
        em->hp[i] = em->hpMax[i];
        em->damage[i] = em->hpMax[i] > 100 ? 8 : 5;
        em->dir[i] = 1;
        em->ai[i] = i >= 3;
        em->chasing[i] = 0;
        em->frame[i] = 0;
        em->frameCounter[i] = 0;
    }
    em->item[0] = (SDL_Rect){300, 480, 35, 35};
    em->item[1] = (SDL_Rect){560, 330, 35, 35};
    em->item[2] = (SDL_Rect){820, 520, 35, 35};
    em->item[3] = (SDL_Rect){1000, 220, 45, 45};
    em->itemType[0] = ITEM_COIN;
    em->itemType[1] = ITEM_COIN;
    em->itemType[2] = ITEM_COIN;
    em->itemType[3] = ITEM_MEDKIT;
    for (i = 0; i < 4; i++) em->itemActive[i] = 1;
}

void updateEnemyModule(EnemyModule *em, Background *bg)
{
    int i;
    int dx;
    SDL_Rect p;
    p = bg->player1;
    for (i = 0; i < MAX_ENEMY; i++)
    {
        if (!em->live[i]) continue;
        dx = p.x - em->e[i].x;
        if (em->ai[i] && abs(dx) < 330)
        {
            em->chasing[i] = 1;
            if (dx < 0) em->dir[i] = -1;
            else em->dir[i] = 1;
            em->e[i].x += em->dir[i] * (em->hpMax[i] > 100 ? 3 : 2);
        }
        else
        {
            em->chasing[i] = 0;
            em->e[i].x += em->dir[i] * (em->hpMax[i] > 100 ? 3 : 2);
            if (em->e[i].x < em->left[i])
            {
                em->e[i].x = em->left[i];
                em->dir[i] = 1;
            }
            if (em->e[i].x > em->right[i])
            {
                em->e[i].x = em->right[i];
                em->dir[i] = -1;
            }
        }
        em->frameCounter[i]++;
        if (em->frameCounter[i] >= 7)
        {
            em->frameCounter[i] = 0;
            em->frame[i]++;
            if (em->frame[i] >= 6) em->frame[i] = 0;
        }
        if (inter(bg->player1, em->e[i]))
        {
            bg->player1.x -= em->dir[i] * 22;
            em->hp[i] -= 1;
            if (em->hp[i] <= 0)
            {
                em->live[i] = 0;
                em->score += em->hpMax[i] > 100 ? 50 : 20;
            }
        }
        if (!bg->singleMode && inter(bg->player2, em->e[i]))
            bg->player2.x -= em->dir[i] * 22;
    }
    for (i = 0; i < 4; i++)
    {
        if (!em->itemActive[i]) continue;
        if (inter(bg->player1, em->item[i]))
        {
            em->itemActive[i] = 0;
            if (em->itemType[i] == ITEM_COIN) em->score += 10;
        }
    }
}

void displayEnemyModule(EnemyModule *em, SDL_Renderer *r, Background *bg)
{
    int i;
    SDL_Rect view;
    SDL_Rect cam;
    SDL_Rect dst;
    SDL_Rect src;
    SDL_Rect bar;
    SDL_RendererFlip flip;
    view = (SDL_Rect){0, 0, bg->singleMode ? WINDOW_W : WINDOW_W / 2, WINDOW_H};
    cam = bg->cam1;
    SDL_RenderSetViewport(r, &view);
    for (i = 0; i < 4; i++)
    {
        if (!em->itemActive[i]) continue;
        dst = (SDL_Rect){em->item[i].x - cam.x, em->item[i].y - cam.y, em->item[i].w, em->item[i].h};
        if (em->itemType[i] == ITEM_COIN && em->coin) SDL_RenderCopy(r, em->coin, NULL, &dst);
        else if (em->itemType[i] == ITEM_MEDKIT && em->medkit) SDL_RenderCopy(r, em->medkit, NULL, &dst);
    }
    for (i = 0; i < MAX_ENEMY; i++)
    {
        if (!em->live[i]) continue;
        dst = (SDL_Rect){em->e[i].x - cam.x, em->e[i].y - cam.y, em->e[i].w, em->e[i].h};
        flip = em->dir[i] < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        if (em->hpMax[i] > 100 && em->strong)
        {
            src = em->strongSrc;
            src.x = em->frame[i] * src.w;
            src.y = em->chasing[i] ? src.h : 0;
            SDL_RenderCopyEx(r, em->strong, &src, &dst, 0, NULL, flip);
        }
        else if (em->normal)
        {
            src = em->normalSrc;
            src.x = em->frame[i] * src.w;
            src.y = em->chasing[i] ? src.h : 0;
            SDL_RenderCopyEx(r, em->normal, &src, &dst, 0, NULL, flip);
        }
        else
        {
            SDL_SetRenderDrawColor(r, 180, 30, 30, 255);
            SDL_RenderFillRect(r, &dst);
        }
        bar = (SDL_Rect){dst.x, dst.y - 7, dst.w * em->hp[i] / em->hpMax[i], 5};
        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        SDL_RenderFillRect(r, &bar);
    }
    SDL_RenderSetViewport(r, NULL);
}

void freeEnemyModule(EnemyModule *em)
{
    if (em->normal) SDL_DestroyTexture(em->normal);
    if (em->strong) SDL_DestroyTexture(em->strong);
    if (em->coin) SDL_DestroyTexture(em->coin);
    if (em->medkit) SDL_DestroyTexture(em->medkit);
}

