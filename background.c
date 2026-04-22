#include "background.h"

void initBackground(Background *b, SDL_Renderer *r)
{
    SDL_Surface *s;
    int i,d[8][10]={{180,520,230,24,PLATFORM_FIXED,LEVEL1,1,0,0,0},{620,420,150,24,PLATFORM_MOBILE,LEVEL1,1,620,980,2},{960,620,140,24,PLATFORM_DESTRUCTIBLE,LEVEL1,1,0,0,0},{1220,260,220,24,PLATFORM_FIXED,LEVEL1,1,0,0,0},{150,600,240,24,PLATFORM_FIXED,LEVEL2,1,0,0,0},{600,420,140,24,PLATFORM_MOBILE,LEVEL2,1,600,980,2},{980,320,140,24,PLATFORM_DESTRUCTIBLE,LEVEL2,1,0,0,0},{1240,210,220,24,PLATFORM_FIXED,LEVEL2,1,0,0,0}};
    s=IMG_Load("bg1.png"); b->level1=s?SDL_CreateTextureFromSurface(r,s):0; if(s){b->level1W=s->w; b->level1H=s->h; SDL_FreeSurface(s);} else {b->level1W=2048; b->level1H=1536;}
    s=IMG_Load("bg2.png"); b->level2=s?SDL_CreateTextureFromSurface(r,s):0; if(s){b->level2W=s->w; b->level2H=s->h; SDL_FreeSurface(s);} else {b->level2W=2048; b->level2H=1536;}
    s=IMG_Load("fixed.png"); b->fixedTex=s?SDL_CreateTextureFromSurface(r,s):0; if(s) SDL_FreeSurface(s);
    s=IMG_Load("mobile.png"); b->mobileTex=s?SDL_CreateTextureFromSurface(r,s):0; if(s) SDL_FreeSurface(s);
    s=IMG_Load("destruct0.png"); b->destruct0=s?SDL_CreateTextureFromSurface(r,s):0; if(s) SDL_FreeSurface(s);
    s=IMG_Load("destruct1.png"); b->destruct1=s?SDL_CreateTextureFromSurface(r,s):0; if(s) SDL_FreeSurface(s);
    s=IMG_Load("destruct2.png"); b->destruct2=s?SDL_CreateTextureFromSurface(r,s):0; if(s) SDL_FreeSurface(s);
    b->font=TTF_OpenFont("arial.ttf",24);
    b->music1=Mix_LoadMUS("LVL1.wav"); b->music2=Mix_LoadMUS("LVL2.wav"); if(b->music1) Mix_PlayMusic(b->music1,-1);
    b->currentLevel=LEVEL1; b->singleMode=1; b->showGuide=1; b->guideSingle=1; b->guideMulti=0; b->guideSingleTimer=0; b->guideMultiTimer=0;
    b->startTime1=SDL_GetTicks(); b->startTime2=0; b->elapsed1=0; b->elapsed2=0;
    b->player1=(SDL_Rect){120,500,40,60}; b->player2=(SDL_Rect){190,500,40,60};
    b->cam1=(SDL_Rect){0,0,WINDOW_W,WINDOW_H}; b->cam2=(SDL_Rect){0,0,WINDOW_W/2,WINDOW_H};
    b->zone12=(SDL_Rect){1520,300,10,170}; b->zone21=(SDL_Rect){0,520,10,170}; b->nbPlatform=8;
    for(i=0;i<8;i++){ b->p[i].rect=(SDL_Rect){d[i][0],d[i][1],d[i][2],d[i][3]}; b->p[i].type=d[i][4]; b->p[i].level=d[i][5]; b->p[i].active=d[i][6]; b->p[i].direction=1; b->p[i].min=d[i][7]; b->p[i].max=d[i][8]; b->p[i].speed=d[i][9]; b->p[i].breaking=0; b->p[i].breakStart=0; }
}

void updateBackground(Background *b)
{
    const Uint8 *k=SDL_GetKeyboardState(0);
    int i,sp=5,moved=0,w=b->currentLevel==LEVEL1?b->level1W:b->level2W,h=b->currentLevel==LEVEL1?b->level1H:b->level2H;
    unsigned int now=SDL_GetTicks();
    SDL_Rect o1=b->player1,o2=b->player2;
    if(k[SDL_SCANCODE_RIGHT]){b->player1.x+=sp; moved=1;} if(k[SDL_SCANCODE_LEFT]){b->player1.x-=sp; moved=1;} if(k[SDL_SCANCODE_UP]){b->player1.y-=sp; moved=1;} if(k[SDL_SCANCODE_DOWN]){b->player1.y+=sp; moved=1;}
    if(!b->singleMode){ if(k[SDL_SCANCODE_D]) b->player2.x+=sp; if(k[SDL_SCANCODE_A]) b->player2.x-=sp; if(k[SDL_SCANCODE_W]) b->player2.y-=sp; if(k[SDL_SCANCODE_S]) b->player2.y+=sp; }
    if(b->guideSingle&&moved){ b->guideSingle=0; b->guideSingleTimer=now; }

    if(b->player1.x<0) b->player1.x=0; if(b->player1.y<0) b->player1.y=0; if(b->player1.x+b->player1.w>w) b->player1.x=w-b->player1.w; if(b->player1.y+b->player1.h>h) b->player1.y=h-b->player1.h;
    if(b->player2.x<0) b->player2.x=0; if(b->player2.y<0) b->player2.y=0; if(b->player2.x+b->player2.w>w) b->player2.x=w-b->player2.w; if(b->player2.y+b->player2.h>h) b->player2.y=h-b->player2.h;

    for(i=0;i<b->nbPlatform;i++) if(b->p[i].active&&b->p[i].level==b->currentLevel){
        if(b->p[i].type==PLATFORM_DESTRUCTIBLE){
            if(SDL_HasIntersection(&b->player1,&b->p[i].rect)){ if(!b->p[i].breaking){ b->p[i].breaking=1; b->p[i].breakStart=now; } if(now-b->p[i].breakStart<3000) b->player1=o1; }
            if(!b->singleMode&&SDL_HasIntersection(&b->player2,&b->p[i].rect)){ if(!b->p[i].breaking){ b->p[i].breaking=1; b->p[i].breakStart=now; } if(now-b->p[i].breakStart<3000) b->player2=o2; }
        } else {
            if(SDL_HasIntersection(&b->player1,&b->p[i].rect)) b->player1=o1;
            if(!b->singleMode&&SDL_HasIntersection(&b->player2,&b->p[i].rect)) b->player2=o2;
        }
    }

    if(b->currentLevel==LEVEL1){
        if(SDL_HasIntersection(&b->player1,&b->zone12)||(!b->singleMode&&SDL_HasIntersection(&b->player2,&b->zone12))){
            b->currentLevel=LEVEL2; b->player1=(SDL_Rect){420,600,40,60}; b->player2=(SDL_Rect){490,600,40,60};
            if(b->music2){ Mix_HaltMusic(); Mix_PlayMusic(b->music2,-1); }
        }
    } else {
        if(SDL_HasIntersection(&b->player1,&b->zone21)||(!b->singleMode&&SDL_HasIntersection(&b->player2,&b->zone21))){
            b->currentLevel=LEVEL1; b->player1=(SDL_Rect){b->level1W-220,160,40,60}; b->player2=(SDL_Rect){b->level1W-150,160,40,60};
            if(b->music1){ Mix_HaltMusic(); Mix_PlayMusic(b->music1,-1); }
        }
    }
}

void scrollBackground(Background *b)
{
    int w=b->currentLevel==LEVEL1?b->level1W:b->level2W,h=b->currentLevel==LEVEL1?b->level1H:b->level2H;
    if(b->singleMode){
        b->cam1.w=WINDOW_W; b->cam1.h=WINDOW_H; b->cam1.x=b->player1.x+b->player1.w/2-WINDOW_W/2; b->cam1.y=b->player1.y+b->player1.h/2-WINDOW_H/2;
        if(b->cam1.x<0) b->cam1.x=0; if(b->cam1.y<0) b->cam1.y=0; if(b->cam1.x+b->cam1.w>w) b->cam1.x=w-b->cam1.w; if(b->cam1.y+b->cam1.h>h) b->cam1.y=h-b->cam1.h;
    } else {
        b->cam1.w=WINDOW_W/2; b->cam1.h=WINDOW_H; b->cam2.w=WINDOW_W/2; b->cam2.h=WINDOW_H;
        b->cam1.x=b->player1.x+b->player1.w/2-b->cam1.w/2; b->cam1.y=b->player1.y+b->player1.h/2-b->cam1.h/2; b->cam2.x=b->player2.x+b->player2.w/2-b->cam2.w/2; b->cam2.y=b->player2.y+b->player2.h/2-b->cam2.h/2;
        if(b->cam1.x<0) b->cam1.x=0; if(b->cam1.y<0) b->cam1.y=0; if(b->cam1.x+b->cam1.w>w) b->cam1.x=w-b->cam1.w; if(b->cam1.y+b->cam1.h>h) b->cam1.y=h-b->cam1.h;
        if(b->cam2.x<0) b->cam2.x=0; if(b->cam2.y<0) b->cam2.y=0; if(b->cam2.x+b->cam2.w>w) b->cam2.x=w-b->cam2.w; if(b->cam2.y+b->cam2.h>h) b->cam2.y=h-b->cam2.h;
    }
}

void animateBackground(Background *b)
{
    int i; unsigned int now=SDL_GetTicks();
    b->elapsed1=(now-b->startTime1)/1000; if(!b->singleMode&&b->startTime2) b->elapsed2=(now-b->startTime2)/1000;
    if(b->guideMulti&&now-b->guideMultiTimer>5000) b->guideMulti=0;
    for(i=0;i<b->nbPlatform;i++) if(b->p[i].active&&b->p[i].level==b->currentLevel){
        if(b->p[i].type==PLATFORM_MOBILE){ b->p[i].rect.x+=b->p[i].direction*b->p[i].speed; if(b->p[i].rect.x<=b->p[i].min||b->p[i].rect.x>=b->p[i].max) b->p[i].direction*=-1; }
        if(b->p[i].type==PLATFORM_DESTRUCTIBLE&&b->p[i].breaking&&now-b->p[i].breakStart>=3000) b->p[i].active=0;
    }
}

void displayBackground(Background *b, SDL_Renderer *r)
{
    SDL_Color white={255,255,255,255},yellow={255,255,0,255};
    SDL_Rect vp,dst,zone,tp,pd,panel;
    SDL_Surface *sf;
    SDL_Texture *tt,*tx;
    char txt[64];
    int i,j,m,s;

    for(j=0;j<(b->singleMode?1:2);j++){
        vp=(SDL_Rect){j?WINDOW_W/2:0,0,b->singleMode?WINDOW_W:WINDOW_W/2,WINDOW_H}; SDL_RenderSetViewport(r,&vp);
        SDL_RenderCopy(r,b->currentLevel==LEVEL1?b->level1:b->level2,j?&b->cam2:&b->cam1,0);

        for(i=0;i<b->nbPlatform;i++) if(b->p[i].active&&b->p[i].level==b->currentLevel){
            dst=(SDL_Rect){b->p[i].rect.x-(j?b->cam2.x:b->cam1.x),b->p[i].rect.y-(j?b->cam2.y:b->cam1.y),b->p[i].rect.w,b->p[i].rect.h};
            tx=b->p[i].type==PLATFORM_FIXED?b->fixedTex:b->p[i].type==PLATFORM_MOBILE?b->mobileTex:(!b->p[i].breaking?b->destruct0:(SDL_GetTicks()-b->p[i].breakStart<1500?b->destruct1:b->destruct2));
            if(tx) SDL_RenderCopy(r,tx,0,&dst); else { SDL_SetRenderDrawColor(r,b->p[i].type==PLATFORM_FIXED?50:b->p[i].type==PLATFORM_MOBILE?50:255,b->p[i].type==PLATFORM_FIXED?200:b->p[i].type==PLATFORM_MOBILE?120:140,b->p[i].type==PLATFORM_DESTRUCTIBLE?0:255,255); SDL_RenderFillRect(r,&dst); }
        }

        zone=b->currentLevel==LEVEL1?b->zone12:b->zone21; zone.x-=(j?b->cam2.x:b->cam1.x); zone.y-=(j?b->cam2.y:b->cam1.y);
        SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND); if(b->currentLevel==LEVEL1) SDL_SetRenderDrawColor(r,0,255,0,120); else SDL_SetRenderDrawColor(r,255,255,0,120);
        SDL_RenderFillRect(r,&zone); if(b->currentLevel==LEVEL1) SDL_SetRenderDrawColor(r,0,255,0,255); else SDL_SetRenderDrawColor(r,255,255,0,255); SDL_RenderDrawRect(r,&zone);

        sf=TTF_RenderText_Solid(b->font,b->currentLevel==LEVEL1?"2":"1",white); tt=SDL_CreateTextureFromSurface(r,sf);
        tp=(SDL_Rect){zone.x+20,zone.y+20,sf->w,sf->h}; SDL_RenderCopy(r,tt,0,&tp); SDL_FreeSurface(sf); SDL_DestroyTexture(tt);

        pd=(SDL_Rect){b->player1.x-(j?b->cam2.x:b->cam1.x),b->player1.y-(j?b->cam2.y:b->cam1.y),40,60}; SDL_SetRenderDrawColor(r,20,20,20,255); SDL_RenderFillRect(r,&pd);
        if(!b->singleMode){ pd=(SDL_Rect){b->player2.x-(j?b->cam2.x:b->cam1.x),b->player2.y-(j?b->cam2.y:b->cam1.y),40,60}; SDL_SetRenderDrawColor(r,180,60,60,255); SDL_RenderFillRect(r,&pd); }
    }

    SDL_RenderSetViewport(r,0); if(!b->singleMode){ SDL_SetRenderDrawColor(r,255,255,255,255); SDL_RenderDrawLine(r,WINDOW_W/2,0,WINDOW_W/2,WINDOW_H); }

    if(b->singleMode){
        m=b->elapsed1/60; s=b->elapsed1%60; sprintf(txt,"Time %02d:%02d - Arrows",m,s);
        sf=TTF_RenderText_Solid(b->font,txt,white); tt=SDL_CreateTextureFromSurface(r,sf); tp=(SDL_Rect){WINDOW_W/2-140,10,sf->w,sf->h}; SDL_RenderCopy(r,tt,0,&tp); SDL_FreeSurface(sf); SDL_DestroyTexture(tt);
    } else {
        m=b->elapsed1/60; s=b->elapsed1%60; sprintf(txt,"P1 %02d:%02d - Arrows",m,s);
        sf=TTF_RenderText_Solid(b->font,txt,white); tt=SDL_CreateTextureFromSurface(r,sf); tp=(SDL_Rect){20,10,sf->w,sf->h}; SDL_RenderCopy(r,tt,0,&tp); SDL_FreeSurface(sf); SDL_DestroyTexture(tt);
        m=b->elapsed2/60; s=b->elapsed2%60; sprintf(txt,"P2 %02d:%02d - W A S D",m,s);
        sf=TTF_RenderText_Solid(b->font,txt,white); tt=SDL_CreateTextureFromSurface(r,sf); tp=(SDL_Rect){WINDOW_W-260,10,sf->w,sf->h}; SDL_RenderCopy(r,tt,0,&tp); SDL_FreeSurface(sf); SDL_DestroyTexture(tt);
    }

    if(b->showGuide&&b->guideSingle&&b->singleMode){
        panel=(SDL_Rect){180,140,920,230}; SDL_SetRenderDrawColor(r,0,0,0,210); SDL_RenderFillRect(r,&panel);
        sf=TTF_RenderText_Solid(b->font,"Single Player Guide",yellow); tt=SDL_CreateTextureFromSurface(r,sf); tp=(SDL_Rect){WINDOW_W/2-sf->w/2,175,sf->w,sf->h}; SDL_RenderCopy(r,tt,0,&tp); SDL_FreeSurface(sf); SDL_DestroyTexture(tt);
        sf=TTF_RenderText_Solid(b->font,"Move with Arrow Keys",white); tt=SDL_CreateTextureFromSurface(r,sf); tp=(SDL_Rect){WINDOW_W/2-sf->w/2,230,sf->w,sf->h}; SDL_RenderCopy(r,tt,0,&tp); SDL_FreeSurface(sf); SDL_DestroyTexture(tt);
        sf=TTF_RenderText_Solid(b->font,"Go to the green box to enter Level 2",white); tt=SDL_CreateTextureFromSurface(r,sf); tp=(SDL_Rect){WINDOW_W/2-sf->w/2,285,sf->w,sf->h}; SDL_RenderCopy(r,tt,0,&tp); SDL_FreeSurface(sf); SDL_DestroyTexture(tt);
    }

    if(b->showGuide&&b->guideMulti){
        panel=(SDL_Rect){180,140,920,230}; SDL_SetRenderDrawColor(r,0,0,0,210); SDL_RenderFillRect(r,&panel);
        sf=TTF_RenderText_Solid(b->font,"Multiplayer Guide",yellow); tt=SDL_CreateTextureFromSurface(r,sf); tp=(SDL_Rect){WINDOW_W/2-sf->w/2,175,sf->w,sf->h}; SDL_RenderCopy(r,tt,0,&tp); SDL_FreeSurface(sf); SDL_DestroyTexture(tt);
        sf=TTF_RenderText_Solid(b->font,"Player 1: Arrow Keys",white); tt=SDL_CreateTextureFromSurface(r,sf); tp=(SDL_Rect){WINDOW_W/2-sf->w/2,230,sf->w,sf->h}; SDL_RenderCopy(r,tt,0,&tp); SDL_FreeSurface(sf); SDL_DestroyTexture(tt);
        sf=TTF_RenderText_Solid(b->font,"Player 2: W A S D",white); tt=SDL_CreateTextureFromSurface(r,sf); tp=(SDL_Rect){WINDOW_W/2-sf->w/2,285,sf->w,sf->h}; SDL_RenderCopy(r,tt,0,&tp); SDL_FreeSurface(sf); SDL_DestroyTexture(tt);
    }
}

void freeBackground(Background *b)
{
    if(b->level1) SDL_DestroyTexture(b->level1); if(b->level2) SDL_DestroyTexture(b->level2);
    if(b->fixedTex) SDL_DestroyTexture(b->fixedTex); if(b->mobileTex) SDL_DestroyTexture(b->mobileTex);
    if(b->destruct0) SDL_DestroyTexture(b->destruct0); if(b->destruct1) SDL_DestroyTexture(b->destruct1); if(b->destruct2) SDL_DestroyTexture(b->destruct2);
    if(b->font) TTF_CloseFont(b->font); if(b->music1) Mix_FreeMusic(b->music1); if(b->music2) Mix_FreeMusic(b->music2);
}
