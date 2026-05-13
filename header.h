#ifndef HEADER_H
#define HEADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

typedef struct {
    // SDL
    SDL_Window* window;
    SDL_Renderer* renderer;
    int running;
    int windowWidth;
    int windowHeight;
    int fullscreen;
    
    // Audio
    int volume;
    int savedVolume;
    int muted;
    Mix_Music* backgroundMusic;
    Mix_Chunk* clickSound;
    
    // Normal textures
    SDL_Texture* background;
    SDL_Texture* plusBtn;
    SDL_Texture* minusBtn;
    SDL_Texture* fullscreenBtn;
    SDL_Texture* windowedBtn;
    SDL_Texture* quitBtn;
    SDL_Texture* muteBtn;
    
    // Hover textures
    SDL_Texture* plusBtnHover;
    SDL_Texture* minusBtnHover;
    SDL_Texture* fullscreenBtnHover;
    SDL_Texture* windowedBtnHover;
    SDL_Texture* quitBtnHover;
    SDL_Texture* muteBtnHover;
    
    // Hover states
    int plusHover;
    int minusHover;
    int fullscreenHover;
    int windowedHover;
    int quitHover;
    int muteHover;
    
    // Texts
    SDL_Texture* titleText;
    SDL_Texture* volumeLabelText;
    SDL_Texture* modeLabelText;
    SDL_Texture* quitLabelText;
    SDL_Texture* muteLabelText;
    SDL_Texture* volumeText;
    SDL_Texture* modeText;
    
    // Rectangles
    SDL_Rect titleRect;
    SDL_Rect volumeLabelRect;
    SDL_Rect modeLabelRect;
    SDL_Rect quitLabelRect;
    SDL_Rect muteLabelRect;
    SDL_Rect plusRect;
    SDL_Rect minusRect;
    SDL_Rect fullscreenRect;
    SDL_Rect windowedRect;
    SDL_Rect quitRect;
    SDL_Rect muteRect;
    SDL_Rect volumeDisplayRect;
    SDL_Rect modeDisplayRect;
    
    // Fonts
    TTF_Font* titleFont;
    TTF_Font* labelFont;
    TTF_Font* valueFont;
} App;

// Functions
int initSDL(App* app);
int loadResources(App* app);
void updateLayout(App* app);
void handleEvents(App* app);
void render(App* app);
void changeVolume(App* app, int delta);
void toggleFullscreen(App* app);
void toggleMute(App* app);
void updateVolumeDisplay(App* app);
void updateModeDisplay(App* app);
void cleanup(App* app);

#endif

/**
 * @file header.h
 * @author Ali
 * @date 2026-05-05
 * @brief Fichier principal d'en-tête du projet
 * 
 * Ce fichier contient toutes les structures et prototypes
 * pour le jeu de plateforme et le menu volume.
 */

/**
 * @struct App
 * @brief Structure principale pour l'application (menu volume)
 * 
 * Gère la fenêtre, le renderer, l'audio, les textures des boutons
 */
/**
 * @struct minimap
 * @brief Structure pour la mini-carte
 * 
 * Affiche une vue réduite du niveau avec la position du joueur
 */
/**
 * @struct Platform
 * @brief Structure pour les plateformes
 * 
 * @param type 0 = fixe, 1 = fragile, 2 = mobile
 */
/**
 * @struct Obstacle
 * @brief Structure pour les obstacles
 * 
 * Bloque le passage du joueur
 */
/**
 * @struct BGAnimation
 * @brief Structure pour les animations du fond
 * 
 * Gère les effets de shake, flash, wave et teinte rouge
 */
