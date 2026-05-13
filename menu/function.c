#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//SDL Initialization 
int initSDL(App* app) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Error SDL_Init: %s\n", SDL_GetError());
        return 0;
    }
    
    if (!IMG_Init(IMG_INIT_PNG)) {
        printf("Error IMG_Init: %s\n", IMG_GetError());
        return 0;
    }
    
    if (TTF_Init() == -1) {
        printf("Error TTF_Init: %s\n", TTF_GetError());
        return 0;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Error Mix_OpenAudio: %s\n", Mix_GetError());
        return 0;
    }
    
    // default values
    app->windowWidth = 1024;
    app->windowHeight = 768;
    app->fullscreen = 0;
    app->volume = 50;
    app->savedVolume = 50;
    app->muted = 0;
    
    // Initialize hover states
    app->plusHover = 0;
    app->minusHover = 0;
    app->fullscreenHover = 0;
    app->windowedHover = 0;
    app->quitHover = 0;
    app->muteHover = 0;
    
    // Initialize pointers to NULL
    app->titleText = NULL;
    app->volumeLabelText = NULL;
    app->modeLabelText = NULL;
    app->quitLabelText = NULL;
    app->muteLabelText = NULL;
    app->titleFont = NULL;
    app->labelFont = NULL;
    app->valueFont = NULL;
    app->quitBtn = NULL;
    app->muteBtn = NULL;
    app->plusBtn = NULL;
    app->minusBtn = NULL;
    app->fullscreenBtn = NULL;
    app->windowedBtn = NULL;
    app->plusBtnHover = NULL;
    app->minusBtnHover = NULL;
    app->fullscreenBtnHover = NULL;
    app->windowedBtnHover = NULL;
    app->quitBtnHover = NULL;
    app->muteBtnHover = NULL;
    app->volumeText = NULL;
    app->modeText = NULL;
    app->background = NULL;
    app->backgroundMusic = NULL;
    app->clickSound = NULL;
    
    // Window creation
    app->window = SDL_CreateWindow("Audio and Screen",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   app->windowWidth, app->windowHeight,
                                   SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!app->window) {
        printf("Error creating window: %s\n", SDL_GetError());
        return 0;
    }
    
    // Renderer creation
    app->renderer = SDL_CreateRenderer(app->window, -1, 
                                       SDL_RENDERER_ACCELERATED | 
                                       SDL_RENDERER_PRESENTVSYNC);
    if (!app->renderer) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        return 0;
    }
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    app->running = 1;
    return 1;
}

// load resources
int loadResources(App* app) {
    // loading normal images
    app->plusBtn = IMG_LoadTexture(app->renderer, "+.png");
    if (!app->plusBtn) {
        printf("Loading error +.png: %s\n", IMG_GetError());     
    }
    
    app->minusBtn = IMG_LoadTexture(app->renderer, "-.png");
    if (!app->minusBtn) {
        printf("Loading error -.png: %s\n", IMG_GetError());
    }
    
    app->fullscreenBtn = IMG_LoadTexture(app->renderer, "f.png");
    if (!app->fullscreenBtn) {
        printf("Loading error f.png: %s\n", IMG_GetError());
    }
    
    app->windowedBtn = IMG_LoadTexture(app->renderer, "p.png");
    if (!app->windowedBtn) {
        printf("Loading error p.png: %s\n", IMG_GetError());
    }
    
    app->quitBtn = IMG_LoadTexture(app->renderer, "r.png");
    if (!app->quitBtn) {
        printf("Loading error r.png: %s\n", IMG_GetError());
    }
    
    app->muteBtn = IMG_LoadTexture(app->renderer, "m.png");
    if (!app->muteBtn) {
        printf("Loading error m.png: %s\n", IMG_GetError());
    }
    
    // Loading HOVER images
    app->plusBtnHover = IMG_LoadTexture(app->renderer, "++.png");
    if (!app->plusBtnHover) {
        printf("Warning: ++.png not found\n");
        app->plusBtnHover = app->plusBtn;
    }
    
    app->minusBtnHover = IMG_LoadTexture(app->renderer, "--.png");
    if (!app->minusBtnHover) {
        printf("Warning: --.png not found\n");
        app->minusBtnHover = app->minusBtn;
    }
    
    app->fullscreenBtnHover = IMG_LoadTexture(app->renderer, "ff.png");
    if (!app->fullscreenBtnHover) {
        printf("Warning: ff.png not found\n");
        app->fullscreenBtnHover = app->fullscreenBtn;
    }
    
    app->windowedBtnHover = IMG_LoadTexture(app->renderer, "pp.png");
    if (!app->windowedBtnHover) {
        printf("Warning: pp.png not found\n");
        app->windowedBtnHover = app->windowedBtn;
    }
    
    app->quitBtnHover = IMG_LoadTexture(app->renderer, "rr.png");
    if (!app->quitBtnHover) {
        printf("Warning: rr.png not found\n");
        app->quitBtnHover = app->quitBtn;
    }
    
    app->muteBtnHover = IMG_LoadTexture(app->renderer, "mm.png");
    if (!app->muteBtnHover) {
        printf("Warning: mm.png not found\n");
        app->muteBtnHover = app->muteBtn;
    }
    
    // Loading background
    app->background = IMG_LoadTexture(app->renderer, "bk.png");
    if (!app->background) {
        printf("Warning: bk.png not loaded\n");
    }
    
    // Loading fonts
    app->titleFont = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf", 48);
    if (!app->titleFont) {
        app->titleFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 48);
        if (!app->titleFont) {
            printf("Warning: Title font not loaded\n");
        }
    }
    
    app->labelFont = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 32);
    if (!app->labelFont) {
        printf("Warning: Label font not loaded\n");
    }
    
    app->valueFont = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 28);
    if (!app->valueFont) {
        printf("Warning: Value font not loaded\n");
    }
    
    // Creating text textures
    if (app->titleFont) {
        SDL_Surface* titleSurface = TTF_RenderUTF8_Blended(app->titleFont,
            "Audio and Screen",
            (SDL_Color){255, 255, 255, 255});
        if (titleSurface) {
            app->titleText = SDL_CreateTextureFromSurface(app->renderer, titleSurface);
            app->titleRect.w = titleSurface->w;
            app->titleRect.h = titleSurface->h;
            SDL_FreeSurface(titleSurface);
        }
    }
    
    if (app->labelFont) {
        SDL_Surface* volumeLabelSurface = TTF_RenderUTF8_Blended(app->labelFont,
            "Audio Control",
            (SDL_Color){150, 220, 255, 255});
        if (volumeLabelSurface) {
            app->volumeLabelText = SDL_CreateTextureFromSurface(app->renderer, volumeLabelSurface);
            app->volumeLabelRect.w = volumeLabelSurface->w;
            app->volumeLabelRect.h = volumeLabelSurface->h;
            SDL_FreeSurface(volumeLabelSurface);
        }
        
        SDL_Surface* modeLabelSurface = TTF_RenderUTF8_Blended(app->labelFont,
            "Display Mode",
            (SDL_Color){255, 180, 100, 255});
        if (modeLabelSurface) {
            app->modeLabelText = SDL_CreateTextureFromSurface(app->renderer, modeLabelSurface);
            app->modeLabelRect.w = modeLabelSurface->w;
            app->modeLabelRect.h = modeLabelSurface->h;
            SDL_FreeSurface(modeLabelSurface);
        }
    }
    
    // Loading audio
    app->clickSound = Mix_LoadWAV("clic.mp3");
    if (!app->clickSound) {
        printf("Warning: Could not load clic.mp3\n");
    }
    
    app->backgroundMusic = Mix_LoadMUS("son.flac");
    if (!app->backgroundMusic) {
        printf("Warning: Could not load son.flac\n");
        app->backgroundMusic = Mix_LoadMUS("son.mp3");
    }
    
    if (app->backgroundMusic) {
        if (Mix_PlayMusic(app->backgroundMusic, -1) == -1) {
            printf("Error playing music: %s\n", Mix_GetError());
        }
    }
    
    Mix_VolumeMusic(app->volume * 128 / 100);
    if (app->clickSound) {
        Mix_VolumeChunk(app->clickSound, app->volume * 128 / 100);
    }
    
    updateLayout(app);
    return 1;
}

// Update layout
void updateLayout(App* app) {
    int centerX = app->windowWidth / 2;
    int buttonHeight = 70;
    int buttonWidth = 110;
    int margin = 50;
    int bottomY = app->windowHeight - buttonHeight - margin;
    
    // === PARALLEL BOTTOM BUTTONS ===
    app->muteRect.w = buttonWidth;
    app->muteRect.h = buttonHeight;
    app->muteRect.x = margin;
    app->muteRect.y = bottomY;
    
    app->quitRect.w = buttonWidth;
    app->quitRect.h = buttonHeight;
    app->quitRect.x = app->windowWidth - buttonWidth - margin;
    app->quitRect.y = bottomY;
    
    // === VOLUME BUTTONS ===
    int spacing = 70;
    int totalWidth = buttonWidth * 2 + spacing;
    int startX = centerX - totalWidth / 2;
    
    app->minusRect.w = buttonWidth;
    app->minusRect.h = buttonHeight;
    app->minusRect.x = startX;
    app->minusRect.y = 160;  // Initial position, adjusted in render
    
    app->plusRect.w = buttonWidth;
    app->plusRect.h = buttonHeight;
    app->plusRect.x = startX + buttonWidth + spacing;
    app->plusRect.y = 160;  // Initial position, adjusted in render
    
    // === DISPLAY MODE BUTTONS ===
    app->windowedRect.w = buttonWidth;
    app->windowedRect.h = buttonHeight;
    app->fullscreenRect.w = buttonWidth;
    app->fullscreenRect.h = buttonHeight;
    
    // Text positions (initial positions, adjusted in render)
    if (app->titleText) {
        app->titleRect.x = centerX - app->titleRect.w / 2;
        app->titleRect.y = 80;  // Initial position
    }
    
    if (app->volumeLabelText) {
        app->volumeLabelRect.x = centerX - app->volumeLabelRect.w / 2;
        app->volumeLabelRect.y = 370;  // Initial position
    }
    
    if (app->modeLabelText) {
        app->modeLabelRect.x = centerX - app->modeLabelRect.w / 2;
        app->modeLabelRect.y = 150;  // Initial position
    }
    
    // Update dynamic displays
    updateVolumeDisplay(app);
    updateModeDisplay(app);
    
    if (app->volumeText) {
        app->volumeDisplayRect.x = centerX - app->volumeDisplayRect.w / 2;
        app->volumeDisplayRect.y = 250;  // Initial position
    }
    
    if (app->modeText) {
        app->modeDisplayRect.x = centerX - app->modeDisplayRect.w / 2;
        app->modeDisplayRect.y = 270;  // Initial position
    }
}

// Event handling
void handleEvents(App* app) {
    SDL_Event ev;
    int x, y;
    
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
            case SDL_QUIT:
                app->running = 0;
                break;
                
            case SDL_WINDOWEVENT:
                if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                    app->windowWidth = ev.window.data1;
                    app->windowHeight = ev.window.data2;
                    updateLayout(app);
                }
                break;
                
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        app->running = 0;
                        break;
                    case SDLK_PLUS:
                    case SDLK_KP_PLUS:
                        changeVolume(app, 10);
                        break;
                    case SDLK_MINUS:
                    case SDLK_KP_MINUS:
                        changeVolume(app, -10);
                        break;
                    case SDLK_f:
                        if (SDL_GetModState() & KMOD_ALT)
                            toggleFullscreen(app);
                        break;
                    case SDLK_SPACE:
                        if (Mix_PausedMusic()) {
                            Mix_ResumeMusic();
                        } else {
                            Mix_PauseMusic();
                        }
                        break;
                    case SDLK_m:
                        toggleMute(app);
                        break;
                    case SDLK_q:
                        app->running = 0;
                        break;
                }
                break;
                
            case SDL_MOUSEMOTION:
                // Hover detection
                x = ev.motion.x;
                y = ev.motion.y;
                
                app->minusHover = (x >= app->minusRect.x && x <= app->minusRect.x + app->minusRect.w &&
                                  y >= app->minusRect.y && y <= app->minusRect.y + app->minusRect.h);
                
                app->plusHover = (x >= app->plusRect.x && x <= app->plusRect.x + app->plusRect.w &&
                                 y >= app->plusRect.y && y <= app->plusRect.y + app->plusRect.h);
                
                app->fullscreenHover = (x >= app->fullscreenRect.x && x <= app->fullscreenRect.x + app->fullscreenRect.w &&
                                       y >= app->fullscreenRect.y && y <= app->fullscreenRect.y + app->fullscreenRect.h);
                
                app->windowedHover = (x >= app->windowedRect.x && x <= app->windowedRect.x + app->windowedRect.w &&
                                     y >= app->windowedRect.y && y <= app->windowedRect.y + app->windowedRect.h);
                
                app->muteHover = (x >= app->muteRect.x && x <= app->muteRect.x + app->muteRect.w &&
                                 y >= app->muteRect.y && y <= app->muteRect.y + app->muteRect.h);
                
                app->quitHover = (x >= app->quitRect.x && x <= app->quitRect.x + app->quitRect.w &&
                                 y >= app->quitRect.y && y <= app->quitRect.y + app->quitRect.h);
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (ev.button.button == SDL_BUTTON_LEFT) {
                    x = ev.button.x;
                    y = ev.button.y;
                    
                    if (app->muteHover) {
                        toggleMute(app);
                    }
                    else if (app->minusHover) {
                        changeVolume(app, -10);
                    }
                    else if (app->plusHover) {
                        changeVolume(app, 10);
                    }
                    else if (app->fullscreenHover) {
                        if (!app->fullscreen)
                            toggleFullscreen(app);
                    }
                    else if (app->windowedHover) {
                        if (app->fullscreen)
                            toggleFullscreen(app);
                    }
                    else if (app->quitHover) {
                        app->running = 0;
                    }
                    
                    if (app->clickSound) {
                        Mix_PlayChannel(-1, app->clickSound, 0);
                    }
                }
                break;
        }
    }
}

// Render
void render(App* app) {
    // Local variables for button dimensions
    int buttonHeight = 70;
    int buttonWidth = 110;
    int margin = 50;
    
    // Background
    SDL_SetRenderDrawColor(app->renderer, 30, 30, 50, 255);
    SDL_RenderClear(app->renderer);
    
    if (app->background) {
        SDL_RenderCopy(app->renderer, app->background, NULL, NULL);
    }
    
    // === UNIFIED FRAME (EVERYTHING INSIDE) ===
    int unifiedFrameWidth = 600;  // Width of unified frame (increased)
    int titleFrameHeight = 120;   // Height for title section
    int displayFrameHeight = 180; // Height for display mode section
    int audioFrameHeight = 240;   // Height for audio control section (increased)
    int totalFrameHeight = titleFrameHeight + displayFrameHeight + audioFrameHeight + 20; // +20 for spacing
    int unifiedFrameX = app->windowWidth / 2 - unifiedFrameWidth / 2;
    int unifiedFrameY = 80;  // Y position of unified frame
    
    // Draw the big unified frame
    SDL_SetRenderDrawColor(app->renderer, 40, 40, 80, 200);
    SDL_Rect unifiedFrameRect = {unifiedFrameX, unifiedFrameY, unifiedFrameWidth, totalFrameHeight};
    SDL_RenderFillRect(app->renderer, &unifiedFrameRect);
    
    // Shine effect (gradient) at top of frame
    SDL_SetRenderDrawColor(app->renderer, 80, 100, 180, 100);
    SDL_Rect shineRect = {unifiedFrameX, unifiedFrameY, unifiedFrameWidth, 30};
    SDL_RenderFillRect(app->renderer, &shineRect);
    
    // Outer border of unified frame
    SDL_SetRenderDrawColor(app->renderer, 100, 150, 255, 255);
    SDL_RenderDrawRect(app->renderer, &unifiedFrameRect);
    
    // Inner border of unified frame
    SDL_Rect unifiedInnerRect = {unifiedFrameX + 2, unifiedFrameY + 2, unifiedFrameWidth - 4, totalFrameHeight - 4};
    SDL_SetRenderDrawColor(app->renderer, 80, 120, 200, 255);
    SDL_RenderDrawRect(app->renderer, &unifiedInnerRect);
    
    // === TITLE SECTION IN UNIFIED FRAME ===
    // Title
    if (app->titleText) {
        SDL_Rect titleInFrame = app->titleRect;
        titleInFrame.x = unifiedFrameX + (unifiedFrameWidth - titleInFrame.w) / 2;
        titleInFrame.y = unifiedFrameY + 30;
        SDL_RenderCopy(app->renderer, app->titleText, NULL, &titleInFrame);
    }
    
    // === SEPARATOR LINE AFTER TITLE ===
    SDL_SetRenderDrawColor(app->renderer, 100, 100, 180, 150);
    SDL_RenderDrawLine(app->renderer, 
        unifiedFrameX + 40, unifiedFrameY + titleFrameHeight - 10,
        unifiedFrameX + unifiedFrameWidth - 40, unifiedFrameY + titleFrameHeight - 10);
    
    // === DISPLAY MODE SECTION IN UNIFIED FRAME ===
    int displaySectionY = unifiedFrameY + titleFrameHeight;
    
    // Display Mode label
    if (app->modeLabelText) {
        SDL_Rect labelInFrame = app->modeLabelRect;
        labelInFrame.x = unifiedFrameX + (unifiedFrameWidth - labelInFrame.w) / 2;
        labelInFrame.y = displaySectionY + 15;
        SDL_RenderCopy(app->renderer, app->modeLabelText, NULL, &labelInFrame);
    }
    
    // Window button IN UNIFIED FRAME (with hover)
    if (app->windowedBtn) {
        // Position within unified frame
        app->windowedRect.x = unifiedFrameX + (unifiedFrameWidth / 2) - 150;
        app->windowedRect.y = displaySectionY + 70;
        
        SDL_SetRenderDrawColor(app->renderer, app->windowedHover ? 220 : 180, 
                               app->windowedHover ? 100 : 80, 
                               app->windowedHover ? 100 : 80, 255);
        SDL_RenderFillRect(app->renderer, &app->windowedRect);
        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(app->renderer, &app->windowedRect);
        
        if (app->windowedHover && app->windowedBtnHover) {
            SDL_RenderCopy(app->renderer, app->windowedBtnHover, NULL, &app->windowedRect);
        } else {
            SDL_RenderCopy(app->renderer, app->windowedBtn, NULL, &app->windowedRect);
        }
    }
    
    // Fullscreen button IN UNIFIED FRAME (with hover)
    if (app->fullscreenBtn) {
        // Position within unified frame
        app->fullscreenRect.x = unifiedFrameX + (unifiedFrameWidth / 2) + 40;
        app->fullscreenRect.y = displaySectionY + 70;
        
        SDL_SetRenderDrawColor(app->renderer, app->fullscreenHover ? 100 : 80, 
                               app->fullscreenHover ? 220 : 180, 
                               app->fullscreenHover ? 100 : 80, 255);
        SDL_RenderFillRect(app->renderer, &app->fullscreenRect);
        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(app->renderer, &app->fullscreenRect);
        
        if (app->fullscreenHover && app->fullscreenBtnHover) {
            SDL_RenderCopy(app->renderer, app->fullscreenBtnHover, NULL, &app->fullscreenRect);
        } else {
            SDL_RenderCopy(app->renderer, app->fullscreenBtn, NULL, &app->fullscreenRect);
        }
    }
    
    // Mode display IN UNIFIED FRAME
    if (app->modeText) {
        app->modeDisplayRect.x = unifiedFrameX + (unifiedFrameWidth - app->modeDisplayRect.w) / 2;
        app->modeDisplayRect.y = displaySectionY + 140;
        SDL_RenderCopy(app->renderer, app->modeText, NULL, &app->modeDisplayRect);
    }
    
    // === SEPARATOR LINE BEFORE AUDIO CONTROL ===
    SDL_SetRenderDrawColor(app->renderer, 100, 100, 180, 150);
    SDL_RenderDrawLine(app->renderer, 
        unifiedFrameX + 40, unifiedFrameY + titleFrameHeight + displayFrameHeight,
        unifiedFrameX + unifiedFrameWidth - 40, unifiedFrameY + titleFrameHeight + displayFrameHeight);
    
    // === AUDIO CONTROL SECTION IN UNIFIED FRAME ===
    int audioSectionY = unifiedFrameY + titleFrameHeight + displayFrameHeight + 10;
    
    // Audio Control label
    if (app->volumeLabelText) {
        SDL_Rect volumeLabelInFrame = app->volumeLabelRect;
        volumeLabelInFrame.x = unifiedFrameX + (unifiedFrameWidth - volumeLabelInFrame.w) / 2;
        volumeLabelInFrame.y = audioSectionY + 10;
        SDL_RenderCopy(app->renderer, app->volumeLabelText, NULL, &volumeLabelInFrame);
    }
    
    // Volume - button IN UNIFIED FRAME (with hover)
    if (app->minusBtn) {
        // Position within unified frame
        app->minusRect.x = unifiedFrameX + (unifiedFrameWidth / 2) - 150;
        app->minusRect.y = audioSectionY + 60;
        
        SDL_SetRenderDrawColor(app->renderer, app->minusHover ? 100 : 80, 
                               app->minusHover ? 100 : 80, 
                               app->minusHover ? 220 : 180, 255);
        SDL_RenderFillRect(app->renderer, &app->minusRect);
        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(app->renderer, &app->minusRect);
        
        if (app->minusHover && app->minusBtnHover) {
            SDL_RenderCopy(app->renderer, app->minusBtnHover, NULL, &app->minusRect);
        } else {
            SDL_RenderCopy(app->renderer, app->minusBtn, NULL, &app->minusRect);
        }
    }
    
    // Volume + button IN UNIFIED FRAME (with hover)
    if (app->plusBtn) {
        // Position within unified frame
        app->plusRect.x = unifiedFrameX + (unifiedFrameWidth / 2) + 40;
        app->plusRect.y = audioSectionY + 60;
        
        SDL_SetRenderDrawColor(app->renderer, app->plusHover ? 100 : 80, 
                               app->plusHover ? 220 : 180, 
                               app->plusHover ? 100 : 80, 255);
        SDL_RenderFillRect(app->renderer, &app->plusRect);
        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(app->renderer, &app->plusRect);
        
        if (app->plusHover && app->plusBtnHover) {
            SDL_RenderCopy(app->renderer, app->plusBtnHover, NULL, &app->plusRect);
        } else {
            SDL_RenderCopy(app->renderer, app->plusBtn, NULL, &app->plusRect);
        }
    }
    
    // Volume display IN UNIFIED FRAME
    if (app->volumeText) {
        app->volumeDisplayRect.x = unifiedFrameX + (unifiedFrameWidth - app->volumeDisplayRect.w) / 2;
        app->volumeDisplayRect.y = audioSectionY + 150;  // CHANGED: from 130 to 150 (20px lower)
        SDL_RenderCopy(app->renderer, app->volumeText, NULL, &app->volumeDisplayRect);
    }
    
    // Volume bar IN UNIFIED FRAME (LOWER)
    int barWidth = 400;
    int barHeight = 25;
    int barX = unifiedFrameX + (unifiedFrameWidth - barWidth) / 2;
    int barY = audioSectionY + 120;  // CHANGED: from 100 to 120 (20px lower)
    
    SDL_SetRenderDrawColor(app->renderer, 50, 50, 80, 255);
    SDL_Rect barBackground = {barX, barY, barWidth, barHeight};
    SDL_RenderFillRect(app->renderer, &barBackground);
    
    int filledWidth = (barWidth * app->volume) / 100;
    if (app->muted) {
        SDL_SetRenderDrawColor(app->renderer, 120, 120, 120, 255);
    } else {
        SDL_SetRenderDrawColor(app->renderer, 
                               app->volume < 30 ? 255 : 80, 
                               app->volume > 70 ? 100 : 180, 
                               120, 255);
    }
    SDL_Rect barFilled = {barX, barY, filledWidth, barHeight};
    SDL_RenderFillRect(app->renderer, &barFilled);
    
    SDL_SetRenderDrawColor(app->renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(app->renderer, &barBackground);
    
    // === PARALLEL BOTTOM BUTTONS (OUTSIDE THE FRAME) ===
    
    // Separator line for bottom buttons
    SDL_SetRenderDrawColor(app->renderer, 100, 100, 150, 100);
    SDL_RenderDrawLine(app->renderer, 
        50, app->windowHeight - 150,
        app->windowWidth - 50, app->windowHeight - 150);
    
    // Mute button (m.png) - BOTTOM LEFT (with hover)
    if (app->muteBtn) {
        // Adjust Y position to be at bottom
        app->muteRect.y = app->windowHeight - buttonHeight - margin;
        
        if (app->muted) {
            SDL_SetRenderDrawColor(app->renderer, 
                                   app->muteHover ? 230 : 200, 
                                   app->muteHover ? 80 : 50, 
                                   app->muteHover ? 80 : 50, 255);
        } else {
            SDL_SetRenderDrawColor(app->renderer, 
                                   app->muteHover ? 180 : 150, 
                                   app->muteHover ? 140 : 100, 
                                   app->muteHover ? 240 : 200, 255);
        }
        SDL_RenderFillRect(app->renderer, &app->muteRect);
        
        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(app->renderer, &app->muteRect);
        
        if (app->muteHover && app->muteBtnHover) {
            SDL_RenderCopy(app->renderer, app->muteBtnHover, NULL, &app->muteRect);
        } else {
            SDL_RenderCopy(app->renderer, app->muteBtn, NULL, &app->muteRect);
        }
    }
    
    // Quit button (r.png) - BOTTOM RIGHT (with hover)
    if (app->quitBtn) {
        // Adjust Y position to be at bottom
        app->quitRect.y = app->windowHeight - buttonHeight - margin;
        
        SDL_SetRenderDrawColor(app->renderer, 
                               app->quitHover ? 230 : 200, 
                               app->quitHover ? 80 : 50, 
                               app->quitHover ? 80 : 50, 255);
        SDL_RenderFillRect(app->renderer, &app->quitRect);
        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(app->renderer, &app->quitRect);
        
        if (app->quitHover && app->quitBtnHover) {
            SDL_RenderCopy(app->renderer, app->quitBtnHover, NULL, &app->quitRect);
        } else {
            SDL_RenderCopy(app->renderer, app->quitBtn, NULL, &app->quitRect);
        }
    }
    
    SDL_RenderPresent(app->renderer);
}

// Change volume
void changeVolume(App* app, int delta) {
    if (app->muted) {
        toggleMute(app);
    }
    
    app->volume += delta;
    if (app->volume < 0) app->volume = 0;
    if (app->volume > 100) app->volume = 100;
    
    Mix_VolumeMusic(app->volume * 128 / 100);
    if (app->clickSound) {
        Mix_VolumeChunk(app->clickSound, app->volume * 128 / 100);
    }
    
    updateVolumeDisplay(app);
}

// Toggle fullscreen/windowed
void toggleFullscreen(App* app) {
    app->fullscreen = !app->fullscreen;
    
    if (app->fullscreen) {
        SDL_SetWindowFullscreen(app->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_DisplayMode mode;
        SDL_GetCurrentDisplayMode(0, &mode);
        app->windowWidth = mode.w;
        app->windowHeight = mode.h;
    } else {
        SDL_SetWindowFullscreen(app->window, 0);
        app->windowWidth = 1024;
        app->windowHeight = 768;
        SDL_SetWindowSize(app->window, app->windowWidth, app->windowHeight);
        SDL_SetWindowPosition(app->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    
    updateLayout(app);
    updateModeDisplay(app);
}

// Toggle mute/unmute
void toggleMute(App* app) {
    if (app->muted) {
        app->muted = 0;
        app->volume = app->savedVolume;
        Mix_VolumeMusic(app->volume * 128 / 100);
        if (app->clickSound) {
            Mix_VolumeChunk(app->clickSound, app->volume * 128 / 100);
        }
    } else {
        app->muted = 1;
        app->savedVolume = app->volume;
        Mix_VolumeMusic(0);
        if (app->clickSound) {
            Mix_VolumeChunk(app->clickSound, 0);
        }
    }
    
    updateVolumeDisplay(app);
}

// Update volume display
void updateVolumeDisplay(App* app) {
    if (app->volumeText) {
        SDL_DestroyTexture(app->volumeText);
        app->volumeText = NULL;
    }
    
    char volumeStr[50];
    if (app->muted) {
        snprintf(volumeStr, sizeof(volumeStr), "Volume: %d%% (MUTED)", app->savedVolume);
    } else {
        snprintf(volumeStr, sizeof(volumeStr), "Volume: %d%%", app->volume);
    }
    
    SDL_Color textColor;
    if (app->muted) {
        textColor = (SDL_Color){255, 100, 100, 255};
    } else if (app->volume == 0) {
        textColor = (SDL_Color){255, 100, 100, 255};
    } else if (app->volume < 30) {
        textColor = (SDL_Color){255, 150, 100, 255};
    } else if (app->volume < 70) {
        textColor = (SDL_Color){255, 220, 100, 255};
    } else if (app->volume == 100) {
        textColor = (SDL_Color){100, 255, 100, 255};
    } else {
        textColor = (SDL_Color){200, 230, 255, 255};
    }
    
    if (app->valueFont) {
        SDL_Surface* surface = TTF_RenderUTF8_Blended(app->valueFont, volumeStr, textColor);
        if (surface) {
            app->volumeText = SDL_CreateTextureFromSurface(app->renderer, surface);
            app->volumeDisplayRect.w = surface->w;
            app->volumeDisplayRect.h = surface->h;
            SDL_FreeSurface(surface);
        }
    }
}

// Update mode display
void updateModeDisplay(App* app) {
    if (app->modeText) {
        SDL_DestroyTexture(app->modeText);
        app->modeText = NULL;
    }
    
    const char* modeStr = app->fullscreen ? "Mode: Fullscreen" : "Mode: Windowed";
    SDL_Color textColor = app->fullscreen ? 
        (SDL_Color){255, 200, 100, 255} : (SDL_Color){150, 200, 255, 255};
    
    if (app->valueFont) {
        SDL_Surface* surface = TTF_RenderUTF8_Blended(app->valueFont, modeStr, textColor);
        if (surface) {
            app->modeText = SDL_CreateTextureFromSurface(app->renderer, surface);
            app->modeDisplayRect.w = surface->w;
            app->modeDisplayRect.h = surface->h;
            SDL_FreeSurface(surface);
        }
    }
}

// Cleanup
void cleanup(App* app) {
    // Normal textures
    if (app->background) SDL_DestroyTexture(app->background);
    if (app->plusBtn) SDL_DestroyTexture(app->plusBtn);
    if (app->minusBtn) SDL_DestroyTexture(app->minusBtn);
    if (app->fullscreenBtn) SDL_DestroyTexture(app->fullscreenBtn);
    if (app->windowedBtn) SDL_DestroyTexture(app->windowedBtn);
    if (app->quitBtn) SDL_DestroyTexture(app->quitBtn);
    if (app->muteBtn) SDL_DestroyTexture(app->muteBtn);
    
    // Hover textures
    if (app->plusBtnHover && app->plusBtnHover != app->plusBtn) SDL_DestroyTexture(app->plusBtnHover);
    if (app->minusBtnHover && app->minusBtnHover != app->minusBtn) SDL_DestroyTexture(app->minusBtnHover);
    if (app->fullscreenBtnHover && app->fullscreenBtnHover != app->fullscreenBtn) SDL_DestroyTexture(app->fullscreenBtnHover);
    if (app->windowedBtnHover && app->windowedBtnHover != app->windowedBtn) SDL_DestroyTexture(app->windowedBtnHover);
    if (app->quitBtnHover && app->quitBtnHover != app->quitBtn) SDL_DestroyTexture(app->quitBtnHover);
    if (app->muteBtnHover && app->muteBtnHover != app->muteBtn) SDL_DestroyTexture(app->muteBtnHover);
    
    // Text textures
    if (app->volumeText) SDL_DestroyTexture(app->volumeText);
    if (app->modeText) SDL_DestroyTexture(app->modeText);
    if (app->titleText) SDL_DestroyTexture(app->titleText);
    if (app->volumeLabelText) SDL_DestroyTexture(app->volumeLabelText);
    if (app->modeLabelText) SDL_DestroyTexture(app->modeLabelText);
    if (app->quitLabelText) SDL_DestroyTexture(app->quitLabelText);
    if (app->muteLabelText) SDL_DestroyTexture(app->muteLabelText);
    
    // Fonts
    if (app->titleFont) TTF_CloseFont(app->titleFont);
    if (app->labelFont) TTF_CloseFont(app->labelFont);
    if (app->valueFont) TTF_CloseFont(app->valueFont);
    
    // Audio
    if (app->backgroundMusic) Mix_FreeMusic(app->backgroundMusic);
    if (app->clickSound) Mix_FreeChunk(app->clickSound);
    
    // SDL
    if (app->renderer) SDL_DestroyRenderer(app->renderer);
    if (app->window) SDL_DestroyWindow(app->window);
    
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
