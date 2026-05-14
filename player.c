#include "player.h"
#include <stdio.h>
#include <string.h>

static SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path)
{
    SDL_Texture *texture;
    texture = IMG_LoadTexture(renderer, path);
    if (!texture)
    {
        printf("Failed to load image: %s\n", path);
        printf("IMG Error: %s\n", IMG_GetError());
    }
    return texture;
}

static int maxFrame(PlayerState state)
{
    if (state == PLAYER_WALK) return WALK_FRAMES;
    if (state == PLAYER_RUN) return RUN_FRAMES;
    if (state == PLAYER_JUMP) return JUMP_FRAMES;
    if (state == PLAYER_PUNCH) return PUNCH_FRAMES;
    if (state == PLAYER_SHOOT) return SHOOT_FRAMES;
    return 1;
}

int loadPlayer(Player *p, SDL_Renderer *renderer, int x, int y)
{
    memset(p, 0, sizeof(Player));
    p->idle = loadTexture(renderer, "idle.png");
    p->walk[0] = loadTexture(renderer, "walk1.png");
    p->walk[1] = loadTexture(renderer, "walk2.png");
    p->walk[2] = loadTexture(renderer, "walk3.png");
    p->walk[3] = loadTexture(renderer, "walk4.png");
    p->run[0] = loadTexture(renderer, "run1.png");
    p->run[1] = loadTexture(renderer, "run2.png");
    p->run[2] = loadTexture(renderer, "run3.png");
    p->run[3] = loadTexture(renderer, "run4.png");
    p->run[4] = loadTexture(renderer, "run5.png");
    p->jump[0] = loadTexture(renderer, "jump1.png");
    p->jump[1] = loadTexture(renderer, "jump2.png");
    p->jump[2] = loadTexture(renderer, "jump3.png");
    p->jump[3] = loadTexture(renderer, "jump4.png");
    p->jump[4] = loadTexture(renderer, "jump5.png");
    p->jump[5] = loadTexture(renderer, "jump6.png");
    p->punch[0] = loadTexture(renderer, "punch1.png");
    p->punch[1] = loadTexture(renderer, "punch2.png");
    p->shoot[0] = loadTexture(renderer, "shoot1.png");
    p->shoot[1] = loadTexture(renderer, "shoot2.png");
    p->shoot[2] = loadTexture(renderer, "shoot3.png");
    p->shoot[3] = loadTexture(renderer, "shoot4.png");
    p->shoot[4] = loadTexture(renderer, "shoot5.png");
    p->shootSound = Mix_LoadWAV("shoot.wav");
    if (!p->shootSound) printf("Failed to load sound: shoot.wav\n");
    p->bulletTexture = loadTexture(renderer, "bullet.png");
    p->rect.x = x;
    p->rect.y = y;
    p->rect.w = 110;
    p->rect.h = 130;
    p->state = PLAYER_IDLE;
    p->frame = 0;
    p->facingRight = 1;
    p->speedWalk = 3;
    p->speedRun = 6;
    p->onGround = 1;
    p->health = 3;
    p->active = 1;
    p->lastFrameTime = SDL_GetTicks();
    p->frameDelay = 120;
    return p->idle != NULL;
}


static void shootPlayerBullet(Player *p)
{
    int i;
    for (i = 0; i < MAX_BULLETS; i++)
    {
        if (!p->bulletActive[i])
        {
            p->bulletActive[i] = 1;
            p->bulletDir[i] = p->facingRight ? 1 : -1;
            p->bullet[i].w = 28;
            p->bullet[i].h = 14;
            if (p->facingRight)
                p->bullet[i].x = p->rect.x + p->rect.w - 10;
            else
                p->bullet[i].x = p->rect.x - 18;
            p->bullet[i].y = p->rect.y + 55;
            return;
        }
    }
}

void handlePlayerEvent(Player *p, SDL_Event *event, PlayerControls controls)
{
    Uint32 now;
    if (!p->active) return;
    if (event->type == SDL_KEYDOWN && event->key.repeat == 0)
    {
        now = SDL_GetTicks();
        if (event->key.keysym.sym == controls.rightKey)
        {
            if (now - p->lastRightTap <= DOUBLE_TAP_TIME)
            {
                p->isRunning = 1;
                p->runDirection = 1;
            }
            p->lastRightTap = now;
        }
        else if (event->key.keysym.sym == controls.leftKey)
        {
            if (now - p->lastLeftTap <= DOUBLE_TAP_TIME)
            {
                p->isRunning = 1;
                p->runDirection = -1;
            }
            p->lastLeftTap = now;
        }
        else if (event->key.keysym.sym == controls.punchKey)
        {
            if (p->state != PLAYER_JUMP)
            {
                p->state = PLAYER_PUNCH;
                p->frame = 0;
                p->lastFrameTime = now;
            }
        }
        else if (event->key.keysym.sym == controls.shootKey)
        {
            if (p->state != PLAYER_JUMP)
            {
                p->state = PLAYER_SHOOT;
                p->frame = 0;
                p->lastFrameTime = now;
                shootPlayerBullet(p);
                if (p->shootSound) Mix_PlayChannel(-1, p->shootSound, 0);
            }
        }
    }
    if (event->type == SDL_KEYUP)
    {
        if (event->key.keysym.sym == controls.rightKey && p->runDirection == 1)
        {
            p->isRunning = 0;
            p->runDirection = 0;
        }
        if (event->key.keysym.sym == controls.leftKey && p->runDirection == -1)
        {
            p->isRunning = 0;
            p->runDirection = 0;
        }
    }
}

void updatePlayer(Player *p, const Uint8 *keys, PlayerControls controls)
{
    int moving;
    Uint32 now;
    if (!p->active) return;
    now = SDL_GetTicks();
    moving = 0;
    if (keys[controls.right])
    {
        p->facingRight = 1;
        moving = 1;
        p->rect.x += p->isRunning ? p->speedRun : p->speedWalk;
    }
    if (keys[controls.left])
    {
        p->facingRight = 0;
        moving = 1;
        p->rect.x -= p->isRunning ? p->speedRun : p->speedWalk;
    }
    if (!keys[controls.right] && p->runDirection == 1)
    {
        p->isRunning = 0;
        p->runDirection = 0;
    }
    if (!keys[controls.left] && p->runDirection == -1)
    {
        p->isRunning = 0;
        p->runDirection = 0;
    }
    if (p->state == PLAYER_PUNCH || p->state == PLAYER_SHOOT)
    {
        if (now - p->lastFrameTime >= 110)
        {
            p->frame++;
            p->lastFrameTime = now;
            if (p->frame >= maxFrame(p->state))
            {
                p->frame = 0;
                p->state = PLAYER_IDLE;
            }
        }
        return;
    }
    if (moving) p->state = p->isRunning ? PLAYER_RUN : PLAYER_WALK;
    else p->state = PLAYER_IDLE;
    if (now - p->lastFrameTime >= p->frameDelay)
    {
        p->lastFrameTime = now;
        p->frame++;
        if (p->frame >= maxFrame(p->state)) p->frame = 0;
    }
}

void updatePlayerVisual(Player *p, const Uint8 *keys, PlayerControls controls, SDL_Rect worldRect, int onGround)
{
    int moving;
    Uint32 now;
    if (!p->active) return;
    now = SDL_GetTicks();
    moving = 0;
    p->rect = (SDL_Rect){worldRect.x - 35, worldRect.y - 70, 110, 130};
    p->onGround = onGround;
    if (keys[controls.right])
    {
        p->facingRight = 1;
        moving = 1;
    }
    if (keys[controls.left])
    {
        p->facingRight = 0;
        moving = 1;
    }
    if (!keys[controls.right] && p->runDirection == 1)
    {
        p->isRunning = 0;
        p->runDirection = 0;
    }
    if (!keys[controls.left] && p->runDirection == -1)
    {
        p->isRunning = 0;
        p->runDirection = 0;
    }
    if (p->state == PLAYER_PUNCH || p->state == PLAYER_SHOOT)
    {
        if (now - p->lastFrameTime >= 110)
        {
            p->frame++;
            p->lastFrameTime = now;
            if (p->frame >= maxFrame(p->state))
            {
                p->frame = 0;
                p->state = PLAYER_IDLE;
            }
        }
        return;
    }
    if (!onGround) p->state = PLAYER_JUMP;
    else if (moving) p->state = p->isRunning ? PLAYER_RUN : PLAYER_WALK;
    else p->state = PLAYER_IDLE;
    if (now - p->lastFrameTime >= p->frameDelay)
    {
        p->lastFrameTime = now;
        p->frame++;
        if (p->frame >= maxFrame(p->state)) p->frame = 0;
    }
}

static SDL_Texture *currentTexture(Player *p)
{
    if (p->state == PLAYER_WALK && p->walk[p->frame]) return p->walk[p->frame];
    if (p->state == PLAYER_RUN && p->run[p->frame]) return p->run[p->frame];
    if (p->state == PLAYER_JUMP && p->jump[p->frame]) return p->jump[p->frame];
    if (p->state == PLAYER_PUNCH && p->punch[p->frame]) return p->punch[p->frame];
    if (p->state == PLAYER_SHOOT && p->shoot[p->frame]) return p->shoot[p->frame];
    return p->idle;
}

void renderPlayer(Player *p, SDL_Renderer *renderer)
{
    SDL_RendererFlip flip;
    SDL_Texture *texture;
    if (!p->active) return;
    texture = currentTexture(p);
    flip = p->facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    if (texture) SDL_RenderCopyEx(renderer, texture, NULL, &p->rect, 0, NULL, flip);
}

void renderPlayerWorld(Player *p, SDL_Renderer *renderer, SDL_Rect worldRect, SDL_Rect cam, SDL_Rect view)
{
    SDL_Rect oldRect;
    if (!p->active) return;
    oldRect = p->rect;
    SDL_RenderSetViewport(renderer, &view);
    p->rect.x = worldRect.x - cam.x - 35;
    p->rect.y = worldRect.y - cam.y - 70;
    p->rect.w = 110;
    p->rect.h = 130;
    renderPlayer(p, renderer);
    p->rect = oldRect;
    SDL_RenderSetViewport(renderer, NULL);
}


void updatePlayerBullets(Player *p)
{
    int i;
    for (i = 0; i < MAX_BULLETS; i++)
    {
        if (p->bulletActive[i])
        {
            p->bullet[i].x += p->bulletDir[i] * 14;
            if (p->bullet[i].x < -100 || p->bullet[i].x > 5000)
                p->bulletActive[i] = 0;
        }
    }
}

void renderPlayerBulletsWorld(Player *p, SDL_Renderer *renderer, SDL_Rect cam, SDL_Rect view)
{
    int i;
    SDL_Rect dst;
    SDL_RenderSetViewport(renderer, &view);
    for (i = 0; i < MAX_BULLETS; i++)
    {
        if (p->bulletActive[i])
        {
            dst.x = p->bullet[i].x - cam.x;
            dst.y = p->bullet[i].y - cam.y;
            dst.w = p->bullet[i].w;
            dst.h = p->bullet[i].h;
            if (p->bulletTexture)
                SDL_RenderCopyEx(renderer, p->bulletTexture, NULL, &dst, 0, NULL, p->bulletDir[i] == 1 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
            else
            {
                SDL_SetRenderDrawColor(renderer, 255, 220, 0, 255);
                SDL_RenderFillRect(renderer, &dst);
            }
        }
    }
    SDL_RenderSetViewport(renderer, NULL);
}

int playerBulletHitRect(Player *p, SDL_Rect target)
{
    int i;
    for (i = 0; i < MAX_BULLETS; i++)
    {
        if (p->bulletActive[i] && SDL_HasIntersection(&p->bullet[i], &target))
        {
            p->bulletActive[i] = 0;
            return 1;
        }
    }
    return 0;
}

void damagePlayer(Player *p)
{
    if (!p->active) return;
    p->health--;
    if (p->health < 0) p->health = 0;
}

void destroyPlayer(Player *p)
{
    int i;
    if (p->idle) SDL_DestroyTexture(p->idle);
    for (i = 0; i < WALK_FRAMES; i++) if (p->walk[i]) SDL_DestroyTexture(p->walk[i]);
    for (i = 0; i < RUN_FRAMES; i++) if (p->run[i]) SDL_DestroyTexture(p->run[i]);
    for (i = 0; i < JUMP_FRAMES; i++) if (p->jump[i]) SDL_DestroyTexture(p->jump[i]);
    for (i = 0; i < PUNCH_FRAMES; i++) if (p->punch[i]) SDL_DestroyTexture(p->punch[i]);
    for (i = 0; i < SHOOT_FRAMES; i++) if (p->shoot[i]) SDL_DestroyTexture(p->shoot[i]);
    if (p->shootSound) Mix_FreeChunk(p->shootSound);
    if (p->bulletTexture) SDL_DestroyTexture(p->bulletTexture);
}
void initPlayerModule(PlayerModule *p, SDL_Renderer *ren)
{
    loadPlayer(&p->real, ren, 100, 370);

    p->hp = 4;
    p->dead = 0;
    p->state = P_IDLE;
    p->frame = 0;
    p->dir = 1;
    p->pos = p->real.rect;
}

void setPlayerAction(PlayerModule *p, int action)
{
    if (p->dead)
        return;

    p->state = action;

    if (action == P_IDLE)
    {
        if (p->real.state != PLAYER_PUNCH && p->real.state != PLAYER_SHOOT)
            p->real.state = PLAYER_IDLE;
    }
    else if (action == P_WALK)
    {
        if (p->real.state != PLAYER_PUNCH && p->real.state != PLAYER_SHOOT)
            p->real.state = PLAYER_WALK;
    }
    else if (action == P_RUN)
    {
        if (p->real.state != PLAYER_PUNCH && p->real.state != PLAYER_SHOOT)
            p->real.state = PLAYER_RUN;
    }

    p->real.facingRight = p->dir;
}

void updatePlayerModule(PlayerModule *p)
{
    const Uint8 *keys;
    PlayerControls controls;

    if (p->dead)
        return;

    controls.left = SDL_SCANCODE_LEFT;
    controls.right = SDL_SCANCODE_RIGHT;
    controls.jump = SDL_SCANCODE_SPACE;
    controls.punch = SDL_SCANCODE_Z;
    controls.shoot = SDL_SCANCODE_X;

    controls.leftKey = SDLK_LEFT;
    controls.rightKey = SDLK_RIGHT;
    controls.jumpKey = SDLK_SPACE;
    controls.punchKey = SDLK_z;
    controls.shootKey = SDLK_x;

    keys = SDL_GetKeyboardState(NULL);

    updatePlayerVisual(&p->real, keys, controls, p->pos, p->real.onGround);
    updatePlayerBullets(&p->real);

    p->frame = p->real.frame;
}

void setPlayerDamage(PlayerModule *p)
{
    if (p->dead)
        return;

    p->hp--;
    p->real.health = p->hp;

    if (p->hp <= 0)
    {
        p->hp = 0;
        p->dead = 1;
        p->real.active = 0;
    }
}

void displayPlayerModule(PlayerModule *p, SDL_Renderer *ren, SDL_Rect playerCam, SDL_Rect cam, SDL_Rect view)
{
    p->pos = playerCam;

    renderPlayerWorld(&p->real, ren, playerCam, cam, view);
    renderPlayerBulletsWorld(&p->real, ren, cam, view);
}

void freePlayerModule(PlayerModule *p)
{
    destroyPlayer(&p->real);
}
