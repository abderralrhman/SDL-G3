#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#define WALK_FRAMES 4
#define RUN_FRAMES 5
#define JUMP_FRAMES 6
#define PUNCH_FRAMES 2
#define SHOOT_FRAMES 5
#define MAX_BULLETS 12

#define DOUBLE_TAP_TIME 250

typedef enum
{
    PLAYER_IDLE,
    PLAYER_WALK,
    PLAYER_RUN,
    PLAYER_JUMP,
    PLAYER_PUNCH,
    PLAYER_SHOOT
} PlayerState;

typedef struct
{
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Scancode jump;
    SDL_Scancode punch;
    SDL_Scancode shoot;
    SDL_Keycode leftKey;
    SDL_Keycode rightKey;
    SDL_Keycode jumpKey;
    SDL_Keycode punchKey;
    SDL_Keycode shootKey;
} PlayerControls;

typedef struct
{
    SDL_Texture *idle;
    SDL_Texture *walk[WALK_FRAMES];
    SDL_Texture *run[RUN_FRAMES];
    SDL_Texture *jump[JUMP_FRAMES];
    SDL_Texture *punch[PUNCH_FRAMES];
    SDL_Texture *shoot[SHOOT_FRAMES];
    Mix_Chunk *shootSound;
    SDL_Texture *bulletTexture;
    SDL_Rect bullet[MAX_BULLETS];
    int bulletActive[MAX_BULLETS];
    int bulletDir[MAX_BULLETS];
    SDL_Rect rect;
    PlayerState state;
    int frame;
    int facingRight;
    int speedWalk;
    int speedRun;
    int velocityY;
    int onGround;
    int isRunning;
    int runDirection;
    int health;
    int score;
    int active;
    int punchHitDone;
    Uint32 lastFrameTime;
    Uint32 frameDelay;
    Uint32 lastRightTap;
    Uint32 lastLeftTap;
} Player;

int loadPlayer(Player *p, SDL_Renderer *renderer, int x, int y);
void handlePlayerEvent(Player *p, SDL_Event *event, PlayerControls controls);
void updatePlayer(Player *p, const Uint8 *keys, PlayerControls controls);
void updatePlayerVisual(Player *p, const Uint8 *keys, PlayerControls controls, SDL_Rect worldRect, int onGround);
void updatePlayerBullets(Player *p);
void renderPlayerBulletsWorld(Player *p, SDL_Renderer *renderer, SDL_Rect cam, SDL_Rect view);
int playerBulletHitRect(Player *p, SDL_Rect target);
void renderPlayer(Player *p, SDL_Renderer *renderer);
void renderPlayerWorld(Player *p, SDL_Renderer *renderer, SDL_Rect worldRect, SDL_Rect cam, SDL_Rect view);
void damagePlayer(Player *p);
void destroyPlayer(Player *p);


#define P_IDLE 0
#define P_WALK 1
#define P_RUN 2
#define P_JUMP 3
#define P_PUNCH 4
#define P_SHOOT 5

typedef struct PlayerModule {
    Player real;
    int hp;
    int dead;
    int state;
    int frame;
    int dir;
    SDL_Rect pos;
} PlayerModule;

void initPlayerModule(PlayerModule *p, SDL_Renderer *ren);
void setPlayerAction(PlayerModule *p, int action);
void updatePlayerModule(PlayerModule *p);
void setPlayerDamage(PlayerModule *p);
void displayPlayerModule(PlayerModule *p, SDL_Renderer *ren, SDL_Rect playerCam, SDL_Rect cam, SDL_Rect view);
void freePlayerModule(PlayerModule *p);
#endif

