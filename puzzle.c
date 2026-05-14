#include "puzzle.h"

static SDL_Texture* LoadTexture(SDL_Renderer* r, const char* path) {
    SDL_Surface* surf = IMG_Load(path);
    if (!surf) return NULL;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_FreeSurface(surf);
    return tex;
}

static void InitButtons(PuzzleMenu* menu) {
    menu->quiz_btn = (Button){540, 300, 200, 60, BUTTON_QUIZ, false};
    menu->puzzle_btn = (Button){540, 400, 200, 60, BUTTON_PUZZLE, false};
    menu->option_btns[0] = (Button){380, 450, 150, 50, BUTTON_OPTION_A, false};
    menu->option_btns[1] = (Button){565, 450, 150, 50, BUTTON_OPTION_B, false};
    menu->option_btns[2] = (Button){730, 450, 150, 60, BUTTON_OPTION_C, false};
}

int Puzzle_Init(PuzzleMenu* menu, SDL_Renderer* renderer) {
    memset(menu, 0, sizeof(PuzzleMenu));
    
    menu->bg = LoadTexture(renderer, "background4.png");
    menu->qusin = LoadTexture(renderer, "qusin.png"); 
    menu->quiz_btn_normal = LoadTexture(renderer, "quiz.png");
    menu->quiz_btn_hover = LoadTexture(renderer, "quiz2.png");
    menu->puzzle_btn_normal = LoadTexture(renderer, "puzzle.png");
    menu->puzzle_btn_hover = LoadTexture(renderer, "puzzle2.png");
    menu->a_btn_normal = LoadTexture(renderer, "a.png");
    menu->a_btn_hover = LoadTexture(renderer, "a2.png");
    menu->b_btn_normal = LoadTexture(renderer, "b.png");
    menu->b_btn_hover = LoadTexture(renderer, "b2.png");
    menu->c_btn_normal = LoadTexture(renderer, "c.png");
    menu->c_btn_hover = LoadTexture(renderer, "c2.png");
    
    menu->suspense = Mix_LoadMUS("suspense_drum.wav");
    menu->hover_sfx = Mix_LoadWAV("hover.wav");
    
    menu->font = TTF_OpenFont("arial.ttf", 30);
    menu->white = (SDL_Color){255, 255, 255, 255};
    
    menu->state = PUZZLE_MAIN_MENU;
    menu->running = 1;
    InitButtons(menu);
    menu->quiz.music_on = false;
    
    return 0;
}

static void RenderText(SDL_Renderer* r, const char* text, int x, int y, TTF_Font* font, SDL_Color color) {
    if (!font || !text) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(r, surface);
    SDL_Rect rect = {x - surface->w/2, y - surface->h/2, surface->w, surface->h};
    SDL_RenderCopy(r, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

static void RenderButton(SDL_Renderer* r, Button* btn, SDL_Texture* normal, SDL_Texture* hover) {
    SDL_Rect rect = {btn->x, btn->y, btn->w, btn->h};
    if (btn->is_hovered && hover) {
        SDL_RenderCopy(r, hover, NULL, &rect);
    } else if (normal) {
        SDL_RenderCopy(r, normal, NULL, &rect);
    }
}

static void RenderMainMenu(SDL_Renderer* r, PuzzleMenu* m) {
    if (m->bg) SDL_RenderCopy(r, m->bg, NULL, NULL);
    RenderButton(r, &m->quiz_btn, m->quiz_btn_normal, m->quiz_btn_hover);
    RenderButton(r, &m->puzzle_btn, m->puzzle_btn_normal, m->puzzle_btn_hover);
    if (m->font) {
        RenderText(r, " Welcome 1A1 ", 640, 100, m->font, m->white);
    }
}

static void RenderQuizScreen(SDL_Renderer* r, PuzzleMenu* m) {
    if (m->bg) SDL_RenderCopy(r, m->bg, NULL, NULL);
    
    if (!m->quiz.music_on) {
        if (m->suspense) {
            Mix_PlayMusic(m->suspense, -1);
            m->quiz.music_on = true;
            }
    }
      if (m->font) {
        RenderText(r, "QUIZ", 640, 80, m->font, m->white);
        }
     if (m->qusin) {
        SDL_Rect qusin_rect = {450, 150, 350, 350};
        SDL_RenderCopy(r, m->qusin, NULL, &qusin_rect);
        }
    
    RenderButton(r, &m->option_btns[0], m->a_btn_normal, m->a_btn_hover);
    RenderButton(r, &m->option_btns[1], m->b_btn_normal, m->b_btn_hover);
    RenderButton(r, &m->option_btns[2], m->c_btn_normal, m->c_btn_hover);
}

void Puzzle_Render(SDL_Renderer* r, PuzzleMenu* m) {
    if (m->state == PUZZLE_MAIN_MENU) {
        RenderMainMenu(r, m);
    } else {
        RenderQuizScreen(r, m);
    }
}

static void ResetQuiz(QuizGame* q) {
    q->music_on = false;
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
}

void Puzzle_HandleEvent(PuzzleMenu* m, SDL_Event* e, int* run) {
    switch (e->type) {
        case SDL_KEYDOWN:
            if (e->key.keysym.sym == SDLK_ESCAPE) {
                if (m->state == QUIZ_ACTIVE_SCREEN) {
                    m->state = PUZZLE_MAIN_MENU;
                    ResetQuiz(&m->quiz);
                } else {
                    *run = 0;
                }
            }
            break;
        case SDL_MOUSEBUTTONDOWN: {
            int x = e->button.x, y = e->button.y;
            if (m->state == PUZZLE_MAIN_MENU) {
                if (x >= m->quiz_btn.x && x <= m->quiz_btn.x + m->quiz_btn.w && 
                    y >= m->quiz_btn.y && y <= m->quiz_btn.y + m->quiz_btn.h) {
                    m->state = QUIZ_ACTIVE_SCREEN;
                }
            }
            break;
        }
    }
}

void Puzzle_HandleHover(PuzzleMenu* m, int x, int y) {
    m->quiz_btn.is_hovered = false;
    m->puzzle_btn.is_hovered = false;
    for (int i = 0; i < 3; i++) {
        m->option_btns[i].is_hovered = false;
    }
    
    if (m->state == PUZZLE_MAIN_MENU) {
        if (x >= m->quiz_btn.x && x <= m->quiz_btn.x + m->quiz_btn.w && 
            y >= m->quiz_btn.y && y <= m->quiz_btn.y + m->quiz_btn.h) {
            if (!m->quiz_btn.is_hovered && m->hover_sfx) {
                Mix_PlayChannel(-1, m->hover_sfx, 0);
            }
            m->quiz_btn.is_hovered = true;
        }
        if (x >= m->puzzle_btn.x && x <= m->puzzle_btn.x + m->puzzle_btn.w && 
            y >= m->puzzle_btn.y && y <= m->puzzle_btn.y + m->puzzle_btn.h) {
            if (!m->puzzle_btn.is_hovered && m->hover_sfx) {
                Mix_PlayChannel(-1, m->hover_sfx, 0);
            }
            m->puzzle_btn.is_hovered = true;
        }
    } else if (m->state == QUIZ_ACTIVE_SCREEN) {
        for (int i = 0; i < 3; i++) {
            Button* btn = &m->option_btns[i];
            if (x >= btn->x && x <= btn->x + btn->w && 
                y >= btn->y && y <= btn->y + btn->h) {
                if (!btn->is_hovered && m->hover_sfx) {
                    Mix_PlayChannel(-1, m->hover_sfx, 0);
                }
                btn->is_hovered = true;
                break;
            }
        }
    }
}

void Puzzle_Free(PuzzleMenu* m) {
    if (m->bg) SDL_DestroyTexture(m->bg);
    if (m->qusin) SDL_DestroyTexture(m->qusin);
    if (m->quiz_btn_normal) SDL_DestroyTexture(m->quiz_btn_normal);
    if (m->quiz_btn_hover) SDL_DestroyTexture(m->quiz_btn_hover);
    if (m->puzzle_btn_normal) SDL_DestroyTexture(m->puzzle_btn_normal);
    if (m->puzzle_btn_hover) SDL_DestroyTexture(m->puzzle_btn_hover);
    if (m->a_btn_normal) SDL_DestroyTexture(m->a_btn_normal);
    if (m->a_btn_hover) SDL_DestroyTexture(m->a_btn_hover);
    if (m->b_btn_normal) SDL_DestroyTexture(m->b_btn_normal);
    if (m->b_btn_hover) SDL_DestroyTexture(m->b_btn_hover);
    if (m->c_btn_normal) SDL_DestroyTexture(m->c_btn_normal);
    if (m->c_btn_hover) SDL_DestroyTexture(m->c_btn_hover);
    if (m->suspense) Mix_FreeMusic(m->suspense);
    if (m->hover_sfx) Mix_FreeChunk(m->hover_sfx);
    if (m->font) TTF_CloseFont(m->font);
}
