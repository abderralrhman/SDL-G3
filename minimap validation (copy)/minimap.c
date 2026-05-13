#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "minimap.h"

void initMinimap(minimap *m, SDL_Renderer *renderer, SDL_Texture *backgroundMap, int mapWidth, int mapHeight)
{
    int originalW, originalH;
    SDL_Texture *oldTarget;
    SDL_Rect srcRect, destRect;

    m->minimapRect.w = 180;
    m->minimapRect.h = 120;
    m->minimapRect.x = 700;
    m->minimapRect.y = 20;
    m->playerRect.w = 8;
    m->playerRect.h = 8;
    m->mapWidth = mapWidth;
    m->mapHeight = mapHeight;
    m->realMap = backgroundMap;
    m->map = NULL;

    if (backgroundMap)
    {
        SDL_QueryTexture(backgroundMap, NULL, NULL, &originalW, &originalH);
        m->map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_TARGET,
                                   m->minimapRect.w, m->minimapRect.h);
        if (m->map)
        {
            oldTarget = SDL_GetRenderTarget(renderer);
            SDL_SetRenderTarget(renderer, m->map);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            srcRect.x = 0; srcRect.y = 0;
            srcRect.w = originalW; srcRect.h = originalH;
            destRect.x = 0; destRect.y = 0;
            destRect.w = m->minimapRect.w; destRect.h = m->minimapRect.h;
            SDL_RenderCopy(renderer, backgroundMap, &srcRect, &destRect);
            SDL_SetRenderTarget(renderer, oldTarget);
        }
    }

    m->player = IMG_LoadTexture(renderer, "point.png");
    if (m->player)
        SDL_SetTextureBlendMode(m->player, SDL_BLENDMODE_BLEND);
}

void updateMinimap(minimap *m, SDL_Rect playerPos)
{
    float scaleX = (float)m->minimapRect.w / m->mapWidth;
    float scaleY = (float)m->minimapRect.h / m->mapHeight;

    m->playerRect.x = m->minimapRect.x + (int)(playerPos.x * scaleX);
    m->playerRect.y = m->minimapRect.y + (int)(playerPos.y * scaleY);

    if (m->playerRect.x < m->minimapRect.x)
        m->playerRect.x = m->minimapRect.x;
    if (m->playerRect.y < m->minimapRect.y)
        m->playerRect.y = m->minimapRect.y;
    if (m->playerRect.x + m->playerRect.w > m->minimapRect.x + m->minimapRect.w)
        m->playerRect.x = m->minimapRect.x + m->minimapRect.w - m->playerRect.w;
    if (m->playerRect.y + m->playerRect.h > m->minimapRect.y + m->minimapRect.h)
        m->playerRect.y = m->minimapRect.y + m->minimapRect.h - m->playerRect.h;
}

void displayMinimap(minimap m, SDL_Renderer *renderer)
{
    if (m.map)
        SDL_RenderCopy(renderer, m.map, NULL, &m.minimapRect);
    if (m.player)
        SDL_RenderCopy(renderer, m.player, NULL, &m.playerRect);
}

void freeMinimap(minimap *m)
{
    if (m->map && m->map != m->realMap)
        SDL_DestroyTexture(m->map);
    if (m->player)
        SDL_DestroyTexture(m->player);
}

void recreateMinimap(minimap *m, SDL_Renderer *renderer, SDL_Texture *newBackground)
{
    int originalW, originalH;
    SDL_Texture *oldTarget;
    SDL_Rect srcRect, destRect;

    if (m->map && m->map != m->realMap)
        SDL_DestroyTexture(m->map);

    m->realMap = newBackground;
    m->map = NULL;

    if (newBackground)
    {
        SDL_QueryTexture(newBackground, NULL, NULL, &originalW, &originalH);
        m->map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_TARGET,
                                   m->minimapRect.w, m->minimapRect.h);
        if (m->map)
        {
            oldTarget = SDL_GetRenderTarget(renderer);
            SDL_SetRenderTarget(renderer, m->map);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            srcRect.x = 0; srcRect.y = 0;
            srcRect.w = originalW; srcRect.h = originalH;
            destRect.x = 0; destRect.y = 0;
            destRect.w = m->minimapRect.w; destRect.h = m->minimapRect.h;
            SDL_RenderCopy(renderer, newBackground, &srcRect, &destRect);
            SDL_SetRenderTarget(renderer, oldTarget);
        }
    }
}

void animateMinimap(minimap *m)
{
    float scale;
    int centerX, centerY, newW, newH;

    centerX = m->minimapRect.x + m->minimapRect.w / 2;
    centerY = m->minimapRect.y + m->minimapRect.h / 2;

    if (m->minimapRect.w >= 185)
        scale = 0.997f;
    else if (m->minimapRect.w <= 175)
        scale = 1.003f;
    else
        scale = (m->minimapRect.w > 180) ? 0.997f : 1.003f;

    newW = (int)(m->minimapRect.w * scale);
    newH = (int)(m->minimapRect.h * scale);
    m->minimapRect.x = centerX - newW / 2;
    m->minimapRect.y = centerY - newH / 2;
    m->minimapRect.w = newW;
    m->minimapRect.h = newH;
}

SDL_Color getPixel(SDL_Surface *surface, int x, int y)
{
    SDL_Color color = {0, 0, 0, 0};
    Uint32 col = 0;
    char *position;

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= surface->w) x = surface->w - 1;
    if (y >= surface->h) y = surface->h - 1;

    position = (char *)surface->pixels;
    position += (surface->pitch * y);
    position += (surface->format->BytesPerPixel * x);
    memcpy(&col, position, surface->format->BytesPerPixel);
    SDL_GetRGB(col, surface->format, &color.r, &color.g, &color.b);
    return color;
}

int collisionPerfectPixel(SDL_Surface *mask, SDL_Rect playerPos)
{
    int startX, startY, endX, endY, x, y;
    SDL_Color col;

    if (!mask) return 0;

    startX = playerPos.x;
    startY = playerPos.y;
    endX   = playerPos.x + playerPos.w;
    endY   = playerPos.y + playerPos.h;

    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (endX > mask->w) endX = mask->w;
    if (endY > mask->h) endY = mask->h;

    for (x = startX; x < endX; x++)
    {
        for (y = startY; y < endY; y++)
        {
            col = getPixel(mask, x, y);
            if (col.r == 0 && col.g == 0 && col.b == 0)
                return 1;
        }
    }
    return 0;
}

int collisionPerfectPixelDirection(SDL_Surface *mask, SDL_Rect playerPos, int direction, int movement)
{
    SDL_Rect newPos;
    int x, y;
    SDL_Color col;

    if (!mask) return 0;

    newPos = playerPos;

    if (direction == 0)      newPos.x -= movement;
    else if (direction == 1) newPos.x += movement;
    else if (direction == 2) newPos.y -= movement;
    else if (direction == 3) newPos.y += movement;
    else return 0;

    if (newPos.x < 0) return 1;
    if (newPos.x + newPos.w > mask->w) return 1;
    if (newPos.y < 0) return 1;
    if (newPos.y + newPos.h > mask->h) return 1;

    for (x = newPos.x; x < newPos.x + newPos.w && x < mask->w; x++)
    {
        for (y = newPos.y; y < newPos.y + newPos.h && y < mask->h; y++)
        {
            if (x >= 0 && y >= 0)
            {
                col = getPixel(mask, x, y);
                if (col.r == 0 && col.g == 0 && col.b == 0)
                    return 1;
            }
        }
    }
    return 0;
}

int collisionBB(SDL_Rect player, SDL_Rect obstacle)
{
    if (obstacle.y > player.y + player.h) return 0;
    if (obstacle.x > player.x + player.w) return 0;
    if (obstacle.y + obstacle.h < player.y) return 0;
    if (obstacle.x + obstacle.w < player.x) return 0;
    return 1;
}

int collisionBBPlatforms(SDL_Rect player, Platform *platforms, int numPlatforms)
{
    int i;
    for (i = 0; i < numPlatforms; i++)
    {
        if (platforms[i].active && collisionBB(player, platforms[i].rect))
            return 1;
    }
    return 0;
}

int collisionBBObstacles(SDL_Rect player, Obstacle *obstacles, int numObstacles)
{
    int i;
    for (i = 0; i < numObstacles; i++)
    {
        if (obstacles[i].active && collisionBB(player, obstacles[i].rect))
            return 1;
    }
    return 0;
}

void initObstacles(Obstacle *o, int level, SDL_Renderer *renderer)
{
    int i;
    SDL_Texture *obstacleTexture = IMG_LoadTexture(renderer, "obstacle.png");

    for (i = 0; i < 30; i++)
    {
        o[i].rect.x = 0; o[i].rect.y = 0;
        o[i].rect.w = 0; o[i].rect.h = 0;
        o[i].texture = obstacleTexture;
        o[i].active = 0;
    }

    if (level == 1)
    {
        o[3].rect.x = 550; o[3].rect.y = 250; o[3].rect.w = 40; o[3].rect.h = 40; o[3].active = 1;
        o[4].rect.x = 200; o[4].rect.y = 500; o[4].rect.w = 60; o[4].rect.h = 40; o[4].active = 1;
        o[7].rect.x = 50;  o[7].rect.y = 300; o[7].rect.w = 50; o[7].rect.h = 50; o[7].active = 1;
        o[8].rect.x = 800; o[8].rect.y = 450; o[8].rect.w = 50; o[8].rect.h = 50; o[8].active = 1;
        o[9].rect.x = 350; o[9].rect.y = 100; o[9].rect.w = 40; o[9].rect.h = 60; o[9].active = 1;
    }
    else
    {
        o[3].rect.x  = 450; o[3].rect.y  = 150; o[3].rect.w  = 50;  o[3].rect.h  = 50;  o[3].active  = 1;
        o[4].rect.x  = 550; o[4].rect.y  = 250; o[4].rect.w  = 50;  o[4].rect.h  = 50;  o[4].active  = 1;
        o[11].rect.x = 50;  o[11].rect.y = 100; o[11].rect.w = 40;  o[11].rect.h = 80;  o[11].active = 1;
        o[12].rect.x = 400; o[12].rect.y = 500; o[12].rect.w = 100; o[12].rect.h = 30;  o[12].active = 1;
        o[13].rect.x = 700; o[13].rect.y = 450; o[13].rect.w = 60;  o[13].rect.h = 60;  o[13].active = 1;
        o[14].rect.x = 200; o[14].rect.y = 250; o[14].rect.w = 40;  o[14].rect.h = 40;  o[14].active = 1;
    }
}

void freeObstacles(Obstacle *o, int numObstacles)
{
    if (numObstacles > 0 && o[0].texture)
        SDL_DestroyTexture(o[0].texture);
}

void displayObstacles(SDL_Renderer *renderer, Obstacle *obstacles, int numObstacles)
{
    int i;
    for (i = 0; i < numObstacles; i++)
    {
        if (obstacles[i].active)
        {
            if (obstacles[i].texture)
                SDL_RenderCopy(renderer, obstacles[i].texture, NULL, &obstacles[i].rect);
            else
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &obstacles[i].rect);
            }
        }
    }
}

void initPlatforms(Platform *p, SDL_Renderer *renderer, int level)
{
    int i;
    for (i = 0; i < 20; i++)
    {
        p[i].rect.x = 0; p[i].rect.y = 0;
        p[i].rect.w = 0; p[i].rect.h = 0;
        p[i].texture   = NULL;
        p[i].type      = 0;
        p[i].direction = 0;
        p[i].speed     = 0;
        p[i].counter   = 0;
        p[i].active    = 0;
    }

    if (level == 1)
    {
        p[0].rect.x = 100; p[0].rect.y = 450; p[0].rect.w = 80; p[0].rect.h = 40;
        p[0].texture = IMG_LoadTexture(renderer, "plateforme.png");
        p[0].type = 0; p[0].active = 1;

        p[1].rect.x = 300; p[1].rect.y = 400; p[1].rect.w = 80; p[1].rect.h = 40;
        p[1].texture = IMG_LoadTexture(renderer, "plateforme_fragile.png");
        p[1].type = 1; p[1].counter = 0; p[1].active = 1;

        p[2].rect.x = 520; p[2].rect.y = 350; p[2].rect.w = 80; p[2].rect.h = 40;
        p[2].texture = IMG_LoadTexture(renderer, "plateforme_mobile.png");
        p[2].type = 2; p[2].direction = 1; p[2].speed = 3; p[2].active = 1;

        p[3].rect.x = 700; p[3].rect.y = 500; p[3].rect.w = 80; p[3].rect.h = 40;
        p[3].texture = IMG_LoadTexture(renderer, "plateforme.png");
        p[3].type = 0; p[3].active = 1;

        p[4].rect.x = 600; p[4].rect.y = 200; p[4].rect.w = 80; p[4].rect.h = 40;
        p[4].texture = IMG_LoadTexture(renderer, "plateforme.png");
        p[4].type = 0; p[4].active = 1;
    }
    else
    {
        p[0].rect.x = 80;  p[0].rect.y = 480; p[0].rect.w = 80; p[0].rect.h = 40;
        p[0].texture = IMG_LoadTexture(renderer, "plateforme.png");
        p[0].type = 0; p[0].active = 1;

        p[1].rect.x = 250; p[1].rect.y = 420; p[1].rect.w = 80; p[1].rect.h = 40;
        p[1].texture = IMG_LoadTexture(renderer, "plateforme_fragile.png");
        p[1].type = 1; p[1].counter = 0; p[1].active = 1;

        p[2].rect.x = 450; p[2].rect.y = 380; p[2].rect.w = 80; p[2].rect.h = 40;
        p[2].texture = IMG_LoadTexture(renderer, "plateforme_mobile.png");
        p[2].type = 2; p[2].direction = 1; p[2].speed = 4; p[2].active = 1;

        p[3].rect.x = 650; p[3].rect.y = 320; p[3].rect.w = 80; p[3].rect.h = 40;
        p[3].texture = IMG_LoadTexture(renderer, "plateforme.png");
        p[3].type = 0; p[3].active = 1;

        p[4].rect.x = 800; p[4].rect.y = 250; p[4].rect.w = 80; p[4].rect.h = 40;
        p[4].texture = IMG_LoadTexture(renderer, "plateforme_fragile.png");
        p[4].type = 1; p[4].counter = 0; p[4].active = 1;

        p[5].rect.x = 350; p[5].rect.y = 200; p[5].rect.w = 80; p[5].rect.h = 40;
        p[5].texture = IMG_LoadTexture(renderer, "plateforme_mobile.png");
        p[5].type = 2; p[5].direction = -1; p[5].speed = 3; p[5].active = 1;

        p[6].rect.x = 550; p[6].rect.y = 150; p[6].rect.w = 80; p[6].rect.h = 40;
        p[6].texture = IMG_LoadTexture(renderer, "plateforme.png");
        p[6].type = 0; p[6].active = 1;
    }
}

void freePlatforms(Platform *p, int numPlatforms)
{
    int i;
    for (i = 0; i < numPlatforms; i++)
    {
        if (p[i].texture)
            SDL_DestroyTexture(p[i].texture);
    }
}

void updatePlatforms(Platform *p, int numPlatforms)
{
    int i;
    for (i = 0; i < numPlatforms; i++)
    {
        if (!p[i].active) continue;

        if (p[i].type == 2)
        {
            p[i].rect.x += p[i].speed * p[i].direction;
            if (p[i].rect.x > 750) { p[i].rect.x = 750; p[i].direction = -1; }
            if (p[i].rect.x < 50)  { p[i].rect.x = 50;  p[i].direction =  1; }
        }

        if (p[i].type == 1 && p[i].counter > 0)
        {
            p[i].counter++;
            if (p[i].counter > 60)
                p[i].active = 0;
        }
    }
}

void handleFragileCollision(Platform *p, int index)
{
    if (p[index].type == 1 && p[index].active && p[index].counter == 0)
        p[index].counter = 1;
}

void initBGAnimation(BGAnimation *bgAnim)
{
    bgAnim->active      = 0;
    bgAnim->type        = 0;
    bgAnim->shakeTimer  = 0;
    bgAnim->flashTimer  = 0;
    bgAnim->waveTimer   = 0;
    bgAnim->redTintTimer = 0;
}

void triggerBGAnimation(BGAnimation *bgAnim, int collisionType)
{
    bgAnim->active       = 1;
    bgAnim->type         = collisionType;
    bgAnim->shakeTimer   = 15;
    bgAnim->flashTimer   = 5;
    bgAnim->waveTimer    = 20;
    bgAnim->redTintTimer = 10;
}

void updateBGAnimation(BGAnimation *bgAnim)
{
    if (bgAnim->shakeTimer   > 0) bgAnim->shakeTimer--;
    if (bgAnim->flashTimer   > 0) bgAnim->flashTimer--;
    if (bgAnim->waveTimer    > 0) bgAnim->waveTimer--;
    if (bgAnim->redTintTimer > 0) bgAnim->redTintTimer--;

    if (bgAnim->shakeTimer == 0 && bgAnim->flashTimer == 0 &&
        bgAnim->waveTimer  == 0 && bgAnim->redTintTimer == 0)
        bgAnim->active = 0;
}

void applyBGAnimation(BGAnimation *bgAnim, SDL_Renderer *renderer, SDL_Texture *bg, int currentLevel)
{
    int intensity, offsetX, offsetY, alpha;
    SDL_Rect dest;

    if (!bgAnim->active) return;
    (void)currentLevel;

    if (bgAnim->shakeTimer > 0)
    {
        intensity = bgAnim->shakeTimer / 3 + 2;
        offsetX   = (rand() % (intensity * 2)) - intensity;
        offsetY   = (rand() % (intensity * 2)) - intensity;
        dest.x = offsetX; dest.y = offsetY;
        dest.w = 900;     dest.h = 600;
        SDL_RenderCopy(renderer, bg, NULL, &dest);
    }

    if (bgAnim->flashTimer > 0)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        alpha = bgAnim->flashTimer * 30;
        if (alpha > 200) alpha = 200;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
        SDL_RenderFillRect(renderer, NULL);
    }

    if (bgAnim->redTintTimer > 0 && bgAnim->type == 2)
    {
        intensity = 255 - (bgAnim->redTintTimer * 15);
        if (intensity < 100) intensity = 100;
        SDL_SetTextureColorMod(bg, 255, intensity, intensity);
    }
    else
        SDL_SetTextureColorMod(bg, 255, 255, 255);
}
