#include "minimap.h"

void initMiniMap(MiniMap *m, SDL_Renderer *r, Background *bg)
{
    m->map1 = IMG_LoadTexture(r, "mini.png");
    if (!m->map1) m->map1 = IMG_LoadTexture(r, "map.png");

    m->map2 = IMG_LoadTexture(r, "mini2.png");
    if (!m->map2) m->map2 = IMG_LoadTexture(r, "map2.png");

    m->point = IMG_LoadTexture(r, "point.png");

    m->rect = (SDL_Rect){WINDOW_W - 210, 20, 190, 120};
    m->pointRect = (SDL_Rect){0, 0, 8, 8};

    m->mapW = bg->level1W;
    m->mapH = bg->level1H;
}

void updateMiniMap(MiniMap *m, Background *bg)
{
    float sx, sy;

    m->mapW = bg->currentLevel == LEVEL1 ? bg->level1W : bg->level2W;
    m->mapH = bg->currentLevel == LEVEL1 ? bg->level1H : bg->level2H;

    sx = (float)m->rect.w / m->mapW;
    sy = (float)m->rect.h / m->mapH;

    m->pointRect.x = m->rect.x + (int)(bg->player1.x * sx);
    m->pointRect.y = m->rect.y + (int)(bg->player1.y * sy);
}

void displayMiniMap(MiniMap *m, SDL_Renderer *r, Background *bg)
{
    SDL_Texture *currentMap;

    SDL_RenderSetViewport(r, NULL);

    currentMap = bg->currentLevel == LEVEL1 ? m->map1 : m->map2;

    if (currentMap)
        SDL_RenderCopy(r, currentMap, NULL, &m->rect);
    else
    {
        SDL_SetRenderDrawColor(r, 20, 20, 20, 180);
        SDL_RenderFillRect(r, &m->rect);
    }

    if (m->point)
        SDL_RenderCopy(r, m->point, NULL, &m->pointRect);
    else
    {
        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        SDL_RenderFillRect(r, &m->pointRect);
    }

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderDrawRect(r, &m->rect);
}

void freeMiniMap(MiniMap *m)
{
    if (m->map1) SDL_DestroyTexture(m->map1);
    if (m->map2) SDL_DestroyTexture(m->map2);
    if (m->point) SDL_DestroyTexture(m->point);
}
