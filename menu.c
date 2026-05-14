#include "menu.h"
#include <stdio.h>
#include <string.h>

SDL_Texture *loadMenuTexture(SDL_Renderer *r,const char *p)
{
    return IMG_LoadTexture(r,p);
}

void drawMenuText(SDL_Renderer *r,TTF_Font *f,const char *s,int x,int y,SDL_Color c)
{
    SDL_Surface *sf;
    SDL_Texture *t;
    SDL_Rect p;
    if(!f||!s)return;
    sf=TTF_RenderUTF8_Blended(f,s,c);
    if(!sf)return;
    t=SDL_CreateTextureFromSurface(r,sf);
    if(!t){SDL_FreeSurface(sf);return;}
    p=(SDL_Rect){x,y,sf->w,sf->h};
    SDL_RenderCopy(r,t,0,&p);
    SDL_FreeSurface(sf);
    SDL_DestroyTexture(t);
}

int inRect(SDL_Rect a,int x,int y)
{
    return x>=a.x&&x<=a.x+a.w&&y>=a.y&&y<=a.y+a.h;
}

void setBtn(MButton *b,int x,int y,int w,int h,const char *t)
{
    b->r=(SDL_Rect){x,y,w,h};
    strncpy(b->text,t,39);
    b->text[39]='\0';
    b->hover=0;
    b->oldHover=0;
    b->normal=0;
    b->active=0;
}

void sortScores(Score s[])
{
    int i,j;
    Score t;
    for(i=0;i<MAX_SCORES;i++)
        for(j=i+1;j<MAX_SCORES;j++)
            if(s[j].score>s[i].score){t=s[i];s[i]=s[j];s[j]=t;}
}

void loadScores(Score s[])
{
    FILE *f;
    int i;
    for(i=0;i<MAX_SCORES;i++){strcpy(s[i].name,"---");s[i].score=0;}
    f=fopen("scores.txt","r");
    if(!f)return;
    i=0;
    while(i<MAX_SCORES&&fscanf(f,"%49s %d",s[i].name,&s[i].score)==2)i++;
    fclose(f);
    sortScores(s);
}

void saveScore(char *name,int score,Score s[])
{
    FILE *f;
    int i,min=0;
    if(!name||strlen(name)==0)name="Player";
    for(i=1;i<MAX_SCORES;i++)if(s[i].score<s[min].score)min=i;
    strncpy(s[min].name,name,MAX_NAME-1);
    s[min].name[MAX_NAME-1]='\0';
    s[min].score=score;
    sortScores(s);
    f=fopen("scores.txt","w");
    if(!f)return;
    for(i=0;i<MAX_SCORES;i++)fprintf(f,"%s %d\n",s[i].name,s[i].score);
    fclose(f);
}

void initMenu(Menu *m,SDL_Renderer *r)
{
    int i;
    char path[128];
    memset(m,0,sizeof(Menu));
    m->font=TTF_OpenFont("assets/highscore/MONO.ttf",28);
    if(!m->font)m->font=TTF_OpenFont("arial.ttf",28);
    m->big=TTF_OpenFont("assets/highscore/MONO.ttf",54);
    if(!m->big)m->big=TTF_OpenFont("arial.ttf",54);
    m->mainBg=loadMenuTexture(r,"assets/main/background.png");
    m->logo=loadMenuTexture(r,"assets/main/logo.png");
    m->saveBg=loadMenuTexture(r,"assets/menu_save/background.png");
    m->savePrompt=loadMenuTexture(r,"assets/menu_save/save_prompt.png");
    m->saveOrLoad=loadMenuTexture(r,"assets/menu_save/save_or_load.png");
    m->playerBg=loadMenuTexture(r,"assets/menu_player/background.png");
    m->playerBg2=loadMenuTexture(r,"assets/menu_player/background1.png");
    m->player1=loadMenuTexture(r,"assets/menu_player/player1.png");
    m->player2=loadMenuTexture(r,"assets/menu_player/player2.png");
    m->hsBg1=loadMenuTexture(r,"assets/highscore/MEN1.jpg");
    m->hsBg2=loadMenuTexture(r,"assets/highscore/MEN2.jpg");
    m->hsInput=loadMenuTexture(r,"assets/highscore/exm.png");
    for(i=0;i<5;i++){
        setBtn(&m->b[i],50,200+i*80,300,60,"");
        sprintf(path,"assets/main/B%d.png",i+1);
        m->b[i].normal=loadMenuTexture(r,path);
        sprintf(path,"assets/main/B%d.png",i+6);
        m->b[i].active=loadMenuTexture(r,path);
    }
    setBtn(&m->b[5],360,500,220,90,"Yes");
    m->b[5].normal=loadMenuTexture(r,"assets/menu_save/yes.png");
    setBtn(&m->b[6],700,500,220,90,"No");
    m->b[6].normal=loadMenuTexture(r,"assets/menu_save/no.png");
    setBtn(&m->b[7],410,290,460,90,"Load Game");
    m->b[7].normal=loadMenuTexture(r,"assets/menu_save/loadgame.png");
    setBtn(&m->b[8],410,430,460,90,"New Game");
    m->b[8].normal=loadMenuTexture(r,"assets/menu_save/savegame.png");
    setBtn(&m->b[9],240,260,230,70,"Single player");
    setBtn(&m->b[10],810,260,230,70,"Multi players");
    setBtn(&m->b[11],1040,590,150,60,"Return");
    m->b[11].normal=loadMenuTexture(r,"assets/highscore/return.png");
    setBtn(&m->b[12],950,600,280,90,"Validate");
    m->b[12].normal=loadMenuTexture(r,"assets/highscore/CONFIRM1.png");
    setBtn(&m->b[13],580,600,220,80,"Exit");
    m->b[13].normal=loadMenuTexture(r,"assets/highscore/exit.png");
    setBtn(&m->b[14],810,600,220,80,"Return");
    m->b[14].normal=loadMenuTexture(r,"assets/highscore/return.png");
    m->music=Mix_LoadMUS("assets/sfx/music.mp3");
    if(!m->music)m->music=Mix_LoadMUS("menu.wav");
    m->victory=Mix_LoadMUS("assets/sfx/music.mp3");
    m->click=Mix_LoadWAV("assets/sfx/click.wav");
    if(!m->click)m->click=Mix_LoadWAV("click.wav");
    m->hoverSound=Mix_LoadWAV("assets/sfx/hover.wav");
    if(!m->hoverSound)m->hoverSound=m->click;
    m->volume=64;
    m->page=PAGE_LIST;
    loadScores(m->scores);
    if(m->music)Mix_PlayMusic(m->music,-1);
}

void drawButton(SDL_Renderer *r,Menu *m,MButton *b)
{
    SDL_Color c={255,255,255,255};
    SDL_Texture *img=b->hover&&b->active?b->active:b->normal;
    if(img){SDL_RenderCopy(r,img,0,&b->r);return;}
    SDL_SetRenderDrawColor(r,b->hover?120:50,b->hover?120:70,b->hover?150:90,255);
    SDL_RenderFillRect(r,&b->r);
    SDL_SetRenderDrawColor(r,255,255,255,255);
    SDL_RenderDrawRect(r,&b->r);
    drawMenuText(r,m->font,b->text,b->r.x+18,b->r.y+18,c);
}

void displayMenu(Menu *m,SDL_Renderer *r,int screen)
{
    SDL_Color w={255,255,255,255};
    SDL_Color g={255,215,0,255};
    SDL_Rect p;
    char line[120];
    int i;
    SDL_SetRenderDrawColor(r,15,15,35,255);
    SDL_RenderClear(r);
    if(screen==SCREEN_MENU){
        if(m->mainBg)SDL_RenderCopy(r,m->mainBg,0,0);
        if(m->logo){p=(SDL_Rect){1000,120,200,200};SDL_RenderCopy(r,m->logo,0,&p);}
        drawMenuText(r,m->big,"THE BOOGEYMAN",390,35,w);
        for(i=0;i<5;i++)drawButton(r,m,&m->b[i]);
    }else if(screen==SCREEN_SAVE){
        if(m->saveBg)SDL_RenderCopy(r,m->saveBg,0,0);
        if(m->savePrompt){p=(SDL_Rect){360,110,560,160};SDL_RenderCopy(r,m->savePrompt,0,&p);}else drawMenuText(r,m->big,"Do you want to save your game?",300,160,w);
        drawButton(r,m,&m->b[5]);
        drawButton(r,m,&m->b[6]);
    }else if(screen==SCREEN_LOAD){
        if(m->saveBg)SDL_RenderCopy(r,m->saveBg,0,0);
        if(m->saveOrLoad){p=(SDL_Rect){300,100,680,120};SDL_RenderCopy(r,m->saveOrLoad,0,&p);}else drawMenuText(r,m->big,"Save or Load",460,110,w);
        drawButton(r,m,&m->b[7]);
        drawButton(r,m,&m->b[8]);
    }else if(screen==SCREEN_PLAYER){
        if(m->playerBg)SDL_RenderCopy(r,m->playerBg,0,0);
        drawButton(r,m,&m->b[9]);
        drawButton(r,m,&m->b[10]);
        drawButton(r,m,&m->b[11]);
        if(m->player1){p=(SDL_Rect){310,330,110,110};SDL_RenderCopy(r,m->player1,0,&p);}
        if(m->player2){p=(SDL_Rect){880,330,110,110};SDL_RenderCopy(r,m->player2,0,&p);}
    }else if(screen==SCREEN_SCORES){
        if(m->page==PAGE_INPUT){
            if(m->hsBg1)SDL_RenderCopy(r,m->hsBg1,0,0);
            if(m->hsInput){p=(SDL_Rect){340,310,600,180};SDL_RenderCopy(r,m->hsInput,0,&p);}
            drawMenuText(r,m->font,m->name,400,370,w);
            drawButton(r,m,&m->b[12]);
        }else{
            if(m->hsBg2)SDL_RenderCopy(r,m->hsBg2,0,0);
            drawMenuText(r,m->big,"PLAYERS HISTORY",390,100,g);
            for(i=0;i<MAX_SCORES;i++){sprintf(line,"%d. %s - %d",i+1,m->scores[i].name,m->scores[i].score);drawMenuText(r,m->font,line,470,230+i*70,w);}
            drawButton(r,m,&m->b[13]);
            drawButton(r,m,&m->b[14]);
            drawMenuText(r,m->font,"Press E for Puzzle",510,520,w);
        }
    }
    SDL_RenderPresent(r);
}

void hoverAll(Menu *m,int first,int last,int x,int y)
{
    int i;
    for(i=0;i<16;i++)m->b[i].hover=0;
    for(i=first;i<=last;i++){
        m->b[i].hover=inRect(m->b[i].r,x,y);
        if(m->b[i].hover&&!m->b[i].oldHover&&m->hoverSound)Mix_PlayChannel(-1,m->hoverSound,0);
        m->b[i].oldHover=m->b[i].hover;
    }
}

void validateScore(Menu *m,int finalScore)
{
    saveScore(m->name,finalScore,m->scores);
    loadScores(m->scores);
    m->page=PAGE_LIST;
    if(m->victory){Mix_HaltMusic();Mix_PlayMusic(m->victory,0);}
}

void handleMenu(Menu *m,SDL_Event *e,int *screen,SDL_Window *w,SDL_Renderer *r,int finalScore)
{
    int mx,my;
    (void)w;
    (void)r;
    if(e->type==SDL_TEXTINPUT&&*screen==SCREEN_SCORES&&m->page==PAGE_INPUT&&strlen(m->name)<MAX_NAME-2){strcat(m->name,e->text.text);return;}
    if(e->type==SDL_KEYDOWN){
        if(e->key.keysym.sym==SDLK_ESCAPE){if(*screen==SCREEN_MENU)*screen=SCREEN_QUIT;else *screen=SCREEN_MENU;return;}
        if(*screen==SCREEN_MENU){
            if(e->key.keysym.sym==SDLK_j){*screen=SCREEN_SAVE;return;}
            if(e->key.keysym.sym==SDLK_o){*screen=SCREEN_OPTIONS;return;}
            if(e->key.keysym.sym==SDLK_m){m->page=PAGE_INPUT;m->name[0]='\0';*screen=SCREEN_SCORES;return;}
            if(e->key.keysym.sym==SDLK_e){m->page=PAGE_LIST;*screen=SCREEN_SCORES;return;}
        }
        if(*screen==SCREEN_SAVE){
            if(e->key.keysym.sym==SDLK_y){*screen=SCREEN_LOAD;return;}
            if(e->key.keysym.sym==SDLK_n){*screen=SCREEN_PLAYER;return;}
        }
        if(*screen==SCREEN_LOAD&&e->key.keysym.sym==SDLK_n){*screen=SCREEN_PLAYER;return;}
        if(*screen==SCREEN_PLAYER&&e->key.keysym.sym==SDLK_RETURN){*screen=SCREEN_GAME;return;}
        if(*screen==SCREEN_SCORES){
            if(m->page==PAGE_INPUT&&e->key.keysym.sym==SDLK_BACKSPACE&&strlen(m->name)>0)m->name[strlen(m->name)-1]='\0';
            else if(m->page==PAGE_INPUT&&e->key.keysym.sym==SDLK_RETURN)validateScore(m,finalScore);
            else if(m->page==PAGE_LIST&&e->key.keysym.sym==SDLK_e)*screen=SCREEN_PUZZLE;
            return;
        }
    }
    if(e->type==SDL_MOUSEMOTION){
        mx=e->motion.x;my=e->motion.y;
        if(*screen==SCREEN_MENU)hoverAll(m,0,4,mx,my);
        else if(*screen==SCREEN_SAVE)hoverAll(m,5,6,mx,my);
        else if(*screen==SCREEN_LOAD)hoverAll(m,7,8,mx,my);
        else if(*screen==SCREEN_PLAYER)hoverAll(m,9,11,mx,my);
        else if(*screen==SCREEN_SCORES&&m->page==PAGE_INPUT)hoverAll(m,12,12,mx,my);
        else if(*screen==SCREEN_SCORES)hoverAll(m,13,14,mx,my);
        return;
    }
    if(e->type!=SDL_MOUSEBUTTONDOWN)return;
    mx=e->button.x;my=e->button.y;
    if(m->click)Mix_PlayChannel(-1,m->click,0);
    if(*screen==SCREEN_MENU){
        if(inRect(m->b[0].r,mx,my))*screen=SCREEN_SAVE;
        else if(inRect(m->b[1].r,mx,my))*screen=SCREEN_OPTIONS;
        else if(inRect(m->b[2].r,mx,my)){m->page=PAGE_INPUT;m->name[0]='\0';*screen=SCREEN_SCORES;}
        else if(inRect(m->b[3].r,mx,my)){m->page=PAGE_LIST;*screen=SCREEN_SCORES;}
        else if(inRect(m->b[4].r,mx,my))*screen=SCREEN_QUIT;
    }else if(*screen==SCREEN_SAVE){
        if(inRect(m->b[5].r,mx,my))*screen=SCREEN_LOAD;
        else if(inRect(m->b[6].r,mx,my))*screen=SCREEN_PLAYER;
    }else if(*screen==SCREEN_LOAD){
        if(inRect(m->b[7].r,mx,my))*screen=SCREEN_GAME;
        else if(inRect(m->b[8].r,mx,my))*screen=SCREEN_PLAYER;
    }else if(*screen==SCREEN_PLAYER){
        if(inRect(m->b[9].r,mx,my)){m->multi=0;*screen=SCREEN_GAME;}
        else if(inRect(m->b[10].r,mx,my)){m->multi=1;*screen=SCREEN_GAME;}
        else if(inRect(m->b[11].r,mx,my))*screen=SCREEN_MENU;
    }else if(*screen==SCREEN_SCORES){
        if(m->page==PAGE_INPUT&&inRect(m->b[12].r,mx,my))validateScore(m,finalScore);
        else if(m->page==PAGE_LIST&&inRect(m->b[13].r,mx,my))*screen=SCREEN_QUIT;
        else if(m->page==PAGE_LIST&&inRect(m->b[14].r,mx,my))*screen=SCREEN_MENU;
    }
}

void freeTex(SDL_Texture *t){if(t)SDL_DestroyTexture(t);}

void freeMenu(Menu *m)
{
    int i;
    for(i=0;i<16;i++){freeTex(m->b[i].normal);if(m->b[i].active&&m->b[i].active!=m->b[i].normal)freeTex(m->b[i].active);}
    freeTex(m->mainBg);freeTex(m->logo);freeTex(m->saveBg);freeTex(m->savePrompt);freeTex(m->saveOrLoad);freeTex(m->playerBg);freeTex(m->playerBg2);freeTex(m->player1);freeTex(m->player2);freeTex(m->hsBg1);freeTex(m->hsBg2);freeTex(m->hsInput);
    if(m->font)TTF_CloseFont(m->font);
    if(m->big)TTF_CloseFont(m->big);
    if(m->music)Mix_FreeMusic(m->music);
    if(m->victory)Mix_FreeMusic(m->victory);
    if(m->click)Mix_FreeChunk(m->click);
    if(m->hoverSound&&m->hoverSound!=m->click)Mix_FreeChunk(m->hoverSound);
}
