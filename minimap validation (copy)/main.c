#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "minimap.h"

int main(int argc, char *argv[])
{
    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Event     event;

    SDL_Texture  *level1Background;
    SDL_Texture  *level2Background;
    SDL_Texture  *playerTexture;
    SDL_Texture  *currentBackground;
    SDL_Surface  *level1Mask;
    SDL_Surface  *level2Mask;

    SDL_Rect  playerPos;
    SDL_Rect  nextPos;
    SDL_Rect  dest;
    minimap   miniMap;
    Platform  platforms[20];
    Obstacle  obstacles[30];
    BGAnimation bgAnim;

    int currentLevel;
    int gameRunning;
    int direction;
    int numPlatforms;
    int numObstacles;
    int movementSpeed;
    int screenWidth;
    int screenHeight;
    int i;
    int outOfBounds;
    int fragileBroken;
    int platformCollision;
    int obstacleCollision;
    int alpha;
    float scale;

    (void)argc;
    (void)argv;

    level1Background = NULL;
    level2Background = NULL;
    playerTexture    = NULL;
    level1Mask       = NULL;
    level2Mask       = NULL;
    currentLevel     = 1;
    gameRunning      = 1;
    direction        = -1;
    numPlatforms     = 7;
    numObstacles     = 15;
    movementSpeed    = 5;
    screenWidth      = 900;
    screenHeight     = 600;

    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return 1;

    if ((IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG)) == 0)
    {
        SDL_Quit();
        return 1;
    }

    window = SDL_CreateWindow("Game - Minimap with Images",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              screenWidth, screenHeight, 0);
    if (!window)
    {
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
        renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    level1Background = IMG_LoadTexture(renderer, "map.png");
    level2Background = IMG_LoadTexture(renderer, "map2.png");
    playerTexture    = IMG_LoadTexture(renderer, "perso.png");
    level1Mask       = IMG_Load("map_collision.png");
    level2Mask       = IMG_Load("mask.bmp");

    initPlatforms(platforms, renderer, currentLevel);
    initObstacles(obstacles, currentLevel, renderer);
    initBGAnimation(&bgAnim);

    playerPos.x = 50;
    playerPos.y = 50;
    playerPos.w = 48;
    playerPos.h = 48;

    initMinimap(&miniMap, renderer, level1Background, screenWidth, screenHeight);

    while (gameRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameRunning = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        gameRunning = 0;
                        break;
                    case SDLK_RIGHT:
                        direction = 1;
                        break;
                    case SDLK_LEFT:
                        direction = 0;
                        break;
                    case SDLK_UP:
                        direction = 2;
                        break;
                    case SDLK_DOWN:
                        direction = 3;
                        break;
                    case SDLK_SPACE:
                        if (currentLevel == 1)
                        {
                            currentLevel = 2;
                            recreateMinimap(&miniMap, renderer, level2Background);
                        }
                        else
                        {
                            currentLevel = 1;
                            recreateMinimap(&miniMap, renderer, level1Background);
                        }
                        freePlatforms(platforms, numPlatforms);
                        initPlatforms(platforms, renderer, currentLevel);
                        freeObstacles(obstacles, numObstacles);
                        initObstacles(obstacles, currentLevel, renderer);
                        playerPos.x = 50;
                        playerPos.y = 50;
                        break;
                    default:
                        break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_RIGHT:
                    case SDLK_LEFT:
                    case SDLK_UP:
                    case SDLK_DOWN:
                        direction = -1;
                        break;
                    default:
                        break;
                }
            }
        }

        updatePlatforms(platforms, numPlatforms);

        if (direction != -1)
        {
            nextPos = playerPos;

            if (direction == 1)      nextPos.x += movementSpeed;
            else if (direction == 0) nextPos.x -= movementSpeed;
            else if (direction == 2) nextPos.y -= movementSpeed;
            else if (direction == 3) nextPos.y += movementSpeed;

            outOfBounds = 0;
            if (nextPos.x < 0)                        outOfBounds = 1;
            if (nextPos.x + nextPos.w > screenWidth)  outOfBounds = 1;
            if (nextPos.y < 0)                        outOfBounds = 1;
            if (nextPos.y + nextPos.h > screenHeight) outOfBounds = 1;

            if (!outOfBounds)
            {
                fragileBroken = 0;
                for (i = 0; i < numPlatforms; i++)
                {
                    if (platforms[i].active && platforms[i].type == 1
                        && collisionBB(nextPos, platforms[i].rect))
                    {
                        platforms[i].active = 0;
                        triggerBGAnimation(&bgAnim, 3);
                        fragileBroken = 1;
                        break;
                    }
                }

                if (fragileBroken)
                {
                    playerPos = nextPos;
                }
                else
                {
                    platformCollision = collisionBBPlatforms(nextPos, platforms, numPlatforms);
                    obstacleCollision = collisionBBObstacles(nextPos, obstacles, numObstacles);

                    if (!platformCollision && !obstacleCollision)
                    {
                        playerPos = nextPos;
                    }
                    else
                    {
                        if (platformCollision) triggerBGAnimation(&bgAnim, 3);
                        if (obstacleCollision)  triggerBGAnimation(&bgAnim, 4);
                    }
                }
            }
            else
            {
                triggerBGAnimation(&bgAnim, 1);
            }
        }

        updateBGAnimation(&bgAnim);
        updateMinimap(&miniMap, playerPos);
        animateMinimap(&miniMap);

        SDL_RenderClear(renderer);

        currentBackground = (currentLevel == 1) ? level1Background : level2Background;

        if (currentBackground)
        {
            if (bgAnim.waveTimer > 0)
            {
                scale  = 1.0f + (bgAnim.waveTimer * 0.003f);
                dest.x = (int)((screenWidth  - screenWidth  * scale) / 2);
                dest.y = (int)((screenHeight - screenHeight * scale) / 2);
                dest.w = (int)(screenWidth  * scale);
                dest.h = (int)(screenHeight * scale);
                SDL_RenderCopy(renderer, currentBackground, NULL, &dest);
            }
            else if (bgAnim.shakeTimer > 0)
            {
                dest.x = (rand() % 7) - 3;
                dest.y = (rand() % 7) - 3;
                dest.w = screenWidth;
                dest.h = screenHeight;
                SDL_RenderCopy(renderer, currentBackground, NULL, &dest);
            }
            else
            {
                SDL_RenderCopy(renderer, currentBackground, NULL, NULL);
            }
        }

        if (bgAnim.flashTimer > 0)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            alpha = bgAnim.flashTimer * 30;
            if (alpha > 200) alpha = 200;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
            SDL_RenderFillRect(renderer, NULL);
        }

        displayObstacles(renderer, obstacles, numObstacles);

        for (i = 0; i < numPlatforms; i++)
        {
            if (platforms[i].active && platforms[i].texture)
                SDL_RenderCopy(renderer, platforms[i].texture, NULL, &platforms[i].rect);
        }

        if (playerTexture)
        {
            if (bgAnim.redTintTimer > 0 && bgAnim.type == 2)
            {
                if ((bgAnim.redTintTimer / 2) % 2 == 0)
                    SDL_SetTextureColorMod(playerTexture, 255, 100, 100);
                else
                    SDL_SetTextureColorMod(playerTexture, 255, 255, 255);
            }
            else
            {
                SDL_SetTextureColorMod(playerTexture, 255, 255, 255);
            }
            SDL_RenderCopy(renderer, playerTexture, NULL, &playerPos);
        }

        displayMinimap(miniMap, renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    freeMinimap(&miniMap);
    if (level1Mask)       SDL_FreeSurface(level1Mask);
    if (level2Mask)       SDL_FreeSurface(level2Mask);
    if (level1Background) SDL_DestroyTexture(level1Background);
    if (level2Background) SDL_DestroyTexture(level2Background);
    if (playerTexture)    SDL_DestroyTexture(playerTexture);
    freePlatforms(platforms, numPlatforms);
    freeObstacles(obstacles, numObstacles);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
