#include "header.h"
#include <stdio.h>

int main(void) {
    App app = {0};
    
    printf("========================================\n");
    printf("     Complete SDL Interface\n");
    printf("     with Mute and Audio Controls\n");
    printf("========================================\n");
    
    // Initialization
    if (!initSDL(&app)) {
        printf("Error: SDL initialization failed\n");
        return 1;
    }
    
    // Resource loading
    if (!loadResources(&app)) {
        printf("Warning: Some resources could not be loaded\n");
    }
    
    printf("\n=== Interface started ===\n");
    printf("Window: %dx%d\n", app.windowWidth, app.windowHeight);
    printf("Initial volume: %d%%\n", app.volume);
    printf("Mode: %s\n", app.fullscreen ? "Fullscreen" : "Windowed");
    printf("Mute: %s\n", app.muted ? "Enabled" : "Disabled");
    printf("\n=== Controls ===\n");
    printf("- Click on +/- for volume\n");
    printf("- Click on M for mute/unmute\n");
    printf("- Click on f/p for display mode\n");
    printf("- Click on R to quit\n");
    printf("- +/- keyboard keys\n");
    printf("- M for mute/unmute\n");
    printf("- Alt+F for fullscreen\n");
    printf("- SPACE: Pause/Resume music\n");
    printf("- ESC or Q: Quit application\n");
    printf("=============================\n");
    
    // Main loop
    while (app.running) {
        handleEvents(&app);
        render(&app);
        SDL_Delay(16);
    }
    
    // Cleanup
    cleanup(&app);
    
    printf("\n=== Application terminated ===\n");
    return 0;
}
