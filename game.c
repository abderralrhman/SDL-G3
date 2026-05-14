#include "game.h"
#include <stdio.h>
#include <string.h>

void sortScores(Score scores[])
{
    int i, j;
    Score temp;
    for (i = 0; i < MAX_SCORES; i++)
        for (j = i + 1; j < MAX_SCORES; j++)
            if (scores[j].score > scores[i].score)
            {
                temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
}

void loadScores(Score scores[])
{
    FILE *f;
    int i;
    for (i = 0; i < MAX_SCORES; i++)
    {
        strcpy(scores[i].name, "---");
        scores[i].score = 0;
    }
    f = fopen("scores.txt", "r");
    if (!f) return;
    i = 0;
    while (i < MAX_SCORES && fscanf(f, "%49s %d", scores[i].name, &scores[i].score) == 2)
        i++;
    fclose(f);
    sortScores(scores);
}

void saveScore(char name[], int score, Score scores[])
{
    FILE *f;
    int i, min = 0;
    for (i = 1; i < MAX_SCORES; i++)
        if (scores[i].score < scores[min].score)
            min = i;
    strncpy(scores[min].name, name, MAX_NAME - 1);
    scores[min].name[MAX_NAME - 1] = '\0';
    scores[min].score = score;
    sortScores(scores);
    f = fopen("scores.txt", "w");
    if (!f) return;
    for (i = 0; i < MAX_SCORES; i++)
        fprintf(f, "%s %d\n", scores[i].name, scores[i].score);
    fclose(f);
}

void drawText(SDL_Renderer *r, TTF_Font *font, char text[], int x, int y, SDL_Color color)
{
    SDL_Surface *s;
    SDL_Texture *t;
    SDL_Rect p;
    if (!font) return;
    s = TTF_RenderText_Solid(font, text, color);
    if (!s) return;
    t = SDL_CreateTextureFromSurface(r, s);
    p.x = x;
    p.y = y;
    p.w = s->w;
    p.h = s->h;
    SDL_RenderCopy(r, t, NULL, &p);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
}

int buttonInside(SDL_Rect rect, int x, int y)
{
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

void runMenuScores(SDL_Renderer *r, int score)
{
    ScoreMenu m;
    Score scores[MAX_SCORES];
    SDL_Event e;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gold = {255, 215, 0, 255};
    char name[MAX_NAME] = "";
    char line[100];
    int run = 1, page = PAGE_INPUT, active = 0, cursor = 1, i, mx, my, len;
    unsigned int timer = SDL_GetTicks();

    m.bg = IMG_LoadTexture(r, "score_bg.png");
    m.zone = IMG_LoadTexture(r, "zone.png");
    m.save = IMG_LoadTexture(r, "save.png");
    m.exit = IMG_LoadTexture(r, "exit.png");
    m.back = IMG_LoadTexture(r, "back.png");
    m.font = TTF_OpenFont("arial.ttf", 26);
    m.titleFont = TTF_OpenFont("arial.ttf", 42);
    m.click = Mix_LoadWAV("click.wav");
    m.music = Mix_LoadMUS("victory.mp3");

    m.zoneRect = (SDL_Rect){340, 300, 600, 80};
    m.saveRect = (SDL_Rect){390, 580, 200, 60};
    m.exitRect = (SDL_Rect){690, 580, 200, 60};
    m.backRect = (SDL_Rect){540, 580, 200, 60};

    loadScores(scores);
    SDL_StartTextInput();

    while (run)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) run = 0;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) run = 0;

            if (page == PAGE_INPUT && active && e.type == SDL_TEXTINPUT)
                if (strlen(name) + strlen(e.text.text) < MAX_NAME - 1)
                    strcat(name, e.text.text);

            if (page == PAGE_INPUT && active && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE)
            {
                len = strlen(name);
                if (len > 0) name[len - 1] = '\0';
            }

            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                mx = e.button.x;
                my = e.button.y;

                if (page == PAGE_INPUT)
                {
                    active = buttonInside(m.zoneRect, mx, my);

                    if (buttonInside(m.saveRect, mx, my) && strlen(name) > 0)
                    {
                        saveScore(name, score, scores);
                        page = PAGE_SCORES;
                        if (m.click) Mix_PlayChannel(-1, m.click, 0);
                        if (m.music) Mix_PlayMusic(m.music, 0);
                    }

                    if (buttonInside(m.exitRect, mx, my))
                        run = 0;
                }
                else
                {
                    if (buttonInside(m.backRect, mx, my))
                        run = 0;
                }
            }

            if (page == PAGE_INPUT && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && strlen(name) > 0)
            {
                saveScore(name, score, scores);
                page = PAGE_SCORES;
            }
        }

        if (SDL_GetTicks() - timer > 500)
        {
            cursor = !cursor;
            timer = SDL_GetTicks();
        }

        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderClear(r);

        if (m.bg) SDL_RenderCopy(r, m.bg, NULL, NULL);

        if (page == PAGE_INPUT)
        {
            drawText(r, m.titleFont, "Enter Player Name", 430, 180, gold);

            if (m.zone) SDL_RenderCopy(r, m.zone, NULL, &m.zoneRect);
            else
            {
                SDL_SetRenderDrawColor(r, 40, 40, 40, 255);
                SDL_RenderFillRect(r, &m.zoneRect);
            }

            if (active)
            {
                SDL_SetRenderDrawColor(r, 255, 215, 0, 255);
                SDL_RenderDrawRect(r, &m.zoneRect);
            }

            strcpy(line, name);
            if (active && cursor) strcat(line, "|");
            drawText(r, m.font, line, m.zoneRect.x + 20, m.zoneRect.y + 25, white);

            if (m.save) SDL_RenderCopy(r, m.save, NULL, &m.saveRect);
            else drawText(r, m.font, "SAVE", m.saveRect.x + 60, m.saveRect.y + 15, white);

            if (m.exit) SDL_RenderCopy(r, m.exit, NULL, &m.exitRect);
            else drawText(r, m.font, "EXIT", m.exitRect.x + 60, m.exitRect.y + 15, white);
        }
        else
        {
            drawText(r, m.titleFont, "High Scores", 500, 100, gold);

            for (i = 0; i < MAX_SCORES; i++)
            {
                sprintf(line, "%d. %s - %d", i + 1, scores[i].name, scores[i].score);
                drawText(r, m.font, line, 500, 220 + i * 70, white);
            }

            if (m.back) SDL_RenderCopy(r, m.back, NULL, &m.backRect);
            else drawText(r, m.font, "BACK", m.backRect.x + 60, m.backRect.y + 15, white);
        }

        SDL_RenderPresent(r);
        SDL_Delay(16);
    }

    SDL_StopTextInput();

    if (m.bg) SDL_DestroyTexture(m.bg);
    if (m.zone) SDL_DestroyTexture(m.zone);
    if (m.save) SDL_DestroyTexture(m.save);
    if (m.exit) SDL_DestroyTexture(m.exit);
    if (m.back) SDL_DestroyTexture(m.back);
    if (m.font) TTF_CloseFont(m.font);
    if (m.titleFont) TTF_CloseFont(m.titleFont);
    if (m.click) Mix_FreeChunk(m.click);
    if (m.music) Mix_FreeMusic(m.music);
}
