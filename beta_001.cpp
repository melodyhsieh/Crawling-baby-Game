#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <sstream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 800;
SDL_Window *window= NULL;
SDL_Renderer *ren = NULL;
SDL_Texture *tex=NULL;
TTF_Font *font=NULL;

bool quit=false;

const int MENU=0;
const int COUNTDOWN=1;
const int PLAYING=2;
const int GAMEOVER=3;
int ScreenState=COUNTDOWN;
int cnt=3;

const int RESTART=0;
const int QUIT=1;
int GameOverSelect=RESTART;

void loadpngwithrec(std::string path,SDL_Rect Viewport)
{
    SDL_Surface *png = IMG_Load(path.c_str());
    tex = SDL_CreateTextureFromSurface(ren, png);
    SDL_FreeSurface(png);
    SDL_RenderCopy(ren,tex,NULL,&Viewport);
    SDL_DestroyTexture(tex);
    tex=NULL;
}

void loadtextatcenter(std::string fontpath,int textsize,SDL_Color textcolor,std::string content,int x,int y)
{
    font=TTF_OpenFont(fontpath.c_str(),textsize);
    SDL_Surface* textsurface=TTF_RenderText_Solid(font,content.c_str(),textcolor);
    tex=SDL_CreateTextureFromSurface(ren,textsurface);
    SDL_FreeSurface(textsurface);
    TTF_CloseFont(font);
    SDL_Rect textrec;
    SDL_QueryTexture(tex,NULL,NULL,&textrec.w,&textrec.h);
    textrec.x=x-textrec.w/2;
    textrec.y=y-textrec.h/2;
    SDL_RenderCopy(ren,tex,NULL,&textrec);
    SDL_DestroyTexture(tex);
    tex=NULL;
}

class floor
{
private:
    const static int HoleW=101;
    const static int HoleH=10;
    int state;
    SDL_Rect hole[2];
    int FloorVy;
public:
    SDL_Rect stair;
    void SetFloorVy(int v)
    {
        FloorVy=v;
    }
    int GetFloorVy() const
    {
        return FloorVy;
    }
    int GetHoleLeftX(int i) const
    {
        return hole[i].x;
    }
    int GetHoleRightX(int i) const
    {
        return hole[i].x+hole[i].w;
    }
    void SetHoleRec()
    {
        switch(state)
        {
        case 0:
            hole[0]={520,stair.y-HoleH+100,HoleW,HoleH};
            hole[1]={640,stair.y-HoleH+100,HoleW,HoleH};
            break;
        case 1:
            hole[0]={20,stair.y-HoleH+100,HoleW,HoleH};
            hole[1]={640,stair.y-HoleH+100,HoleW,HoleH};
            break;
        case 2:
            hole[0]={150,stair.y-HoleH+100,HoleW,HoleH};
            hole[1]={390,stair.y-HoleH+100,HoleW,HoleH};
            break;
        case 3:
            hole[0]={240,stair.y-HoleH+100,HoleW,HoleH};
            hole[1]={490,stair.y-HoleH+100,HoleW,HoleH};
            break;
        case 4:
            hole[0]={50,stair.y-HoleH+100,HoleW,HoleH};
            hole[1]={345,stair.y-HoleH+100,HoleW,HoleH};
            break;
        case 5:
            hole[0]={340,stair.y-HoleH+100,HoleW,HoleH};
            hole[1]={640,stair.y-HoleH+100,HoleW,HoleH};
            break;
        case 6:
            hole[0]={200,stair.y-HoleH+100,HoleW,HoleH};
            hole[1]={640,stair.y-HoleH+100,HoleW,HoleH};
            break;
        case 7:
            hole[0]={270,stair.y-HoleH+100,HoleW,HoleH};
            hole[1]={640,stair.y-HoleH+100,HoleW,HoleH};
            break;
        case 8:
            hole[0]={100,stair.y-HoleH+100,HoleW,HoleH};
            hole[1]={440,stair.y-HoleH+100,HoleW,HoleH};
            break;
        }
    }
    void DrawHole()
    {
        SDL_SetRenderDrawColor( ren, 0x00, 0x00, 0xFF, 0xFF );
        SDL_RenderDrawRect( ren, &hole[0] );
        SDL_RenderDrawRect( ren, &hole[1] );
    }
    void SetRandState()
    {
        state=rand()%9;
    }
    void loadfloor()
    {
        switch(state)
        {
        case 0:
            loadpngwithrec("image/stair0.png",stair);
            break;
        case 1:
            loadpngwithrec("image/stair1.png",stair);
            break;
        case 2:
            loadpngwithrec("image/stair2.png",stair);
            break;
        case 3:
            loadpngwithrec("image/stair3.png",stair);
            break;
        case 4:
            loadpngwithrec("image/stair4.png",stair);
            break;
        case 5:
            loadpngwithrec("image/stair5.png",stair);
            break;
        case 6:
            loadpngwithrec("image/stair6.png",stair);
            break;
        case 7:
            loadpngwithrec("image/stair7.png",stair);
            break;
        case 8:
            loadpngwithrec("image/stair8.png",stair);
            break;
        default:
            loadpngwithrec("image/stair8.png",stair);
            break;
        }
    }
};

class baby
{
private:
    const static int BabyWidth=80;
    const static int BabyHeight=80;
    int vx,vy;
    int Speedx,Speedy;
    int StairNum;
    bool dead;
    int score;
public:
    SDL_Rect babyrec;
    baby(int x,int i,int sx,int sy)
    {
        Speedx=sx;
        vy=sy;
        vx=0;
        StairNum=i;
        babyrec.x=x;
        babyrec.y=(i+1)*100-90;
        babyrec.w=BabyWidth;
        babyrec.h=BabyHeight;
        dead=false;
        score=0;
    }
    void ReBorn(int x,int i,int sx,int sy)
    {
        Speedx=sx;
        vy=sy;
        vx=0;
        StairNum=i;
        babyrec.x=x;
        babyrec.y=(i+1)*100-90;
        babyrec.w=BabyWidth;
        babyrec.h=BabyHeight;
        dead=false;
        score=0;
    }
    void SpeedUp(int dv)
    {
        vx+=dv;
    }
    void EndSpeedUp(int dv)
    {
        vx-=dv;
    }
    int GetBabyStairNum() const
    {
        return StairNum;
    }
    void loadBaby(int playNum)
    {
        //要用成動畫的程式
        switch(playNum)
        {
        case 1:
            SDL_SetRenderDrawColor(ren,0x00,0x00,0x7F,0xFF);
            SDL_RenderFillRect(ren,&babyrec);
            break;
        case 2:
            SDL_SetRenderDrawColor(ren,0x7F,0x00,0x00,0xFF);
            SDL_RenderFillRect(ren,&babyrec);
            break;
        }
    }
    bool GetBabyState() const
    {
        return dead;
    }
    void MoveBaby(floor nowstair)
    {
        babyrec.x+=vx;
        if(babyrec.x<=0)babyrec.x=0;
        if(babyrec.x>=SCREEN_WIDTH-BabyWidth)babyrec.x=SCREEN_WIDTH-BabyWidth;
        for(int i=0;i<2;i++)
        {
            if(babyrec.x>=nowstair.GetHoleLeftX(i)&&babyrec.x+babyrec.w<nowstair.GetHoleRightX(i))
            {
                vy=-10;
            }
        }
        babyrec.y-=vy;
        if(babyrec.y>=SCREEN_HEIGHT-babyrec.h)babyrec.y=SCREEN_HEIGHT-babyrec.h;
        if(babyrec.y+babyrec.w<=0)dead=true;
        if(babyrec.y+babyrec.w>=nowstair.stair.y+190)
        {
            babyrec.y=nowstair.stair.y+110;
            vy=5;
            StairNum++;
            StairNum%=9;
            score++;
        }
    }
    void ShowScore(int playerNum)
    {
        std::stringstream ScoreText;
        ScoreText.str( "" );
        ScoreText<<score;
        SDL_Color ScoreColor={128,0,128,255};
        switch(playerNum)
        {
        case 1:
            loadtextatcenter("TTF/PRISTINA.ttf",35,ScoreColor,"1P's SCORE:",140,40);
            loadtextatcenter("TTF/PRISTINA.ttf",35,ScoreColor,ScoreText.str().c_str(),140,80);
            break;
        case 2:
            loadtextatcenter("TTF/PRISTINA.ttf",35,ScoreColor,"2P's SCORE:",500,40);
            loadtextatcenter("TTF/PRISTINA.ttf",35,ScoreColor,ScoreText.str().c_str(),500,80);
            break;
        }
    }
    void ShowDead(int playerNum)
    {
        SDL_Color MSG_Color={255,0,0,255};
        switch(playerNum)
        {
        case 1:
            loadtextatcenter("TTF/PRISTINA.ttf",45,MSG_Color,"DEAD",140,130);
            break;
        case 2:
            loadtextatcenter("TTF/PRISTINA.ttf",45,MSG_Color,"DEAD",500,130);
            break;
        }
    }
    void handleEvent(SDL_Event& e,int playerNum)
    {
        switch(playerNum)
        {
        case 1:
            //If a key was pressed
            if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
            {
                //Adjust the velocity
                switch( e.key.keysym.sym )
                {
                    case SDLK_a:
                        vx -= Speedx;
                        break;
                    case SDLK_d:
                        vx += Speedx;
                        break;
                }
            }
            //If a key was released
            else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
            {
                //Adjust the velocity
                switch( e.key.keysym.sym )
                {
                    case SDLK_a:
                        vx += Speedx;
                        break;
                    case SDLK_d:
                        vx -= Speedx;
                        break;
                }
            }
            break;
        case 2:
            //If a key was pressed
            if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
            {
                //Adjust the velocity
                switch( e.key.keysym.sym )
                {
                    case SDLK_LEFT:
                        vx -= Speedx;
                        break;
                    case SDLK_RIGHT:
                        vx += Speedx;
                        break;
                }
            }
                //If a key was released
            else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
            {
                //Adjust the velocity
                switch( e.key.keysym.sym )
                {
                    case SDLK_LEFT:
                        vx += Speedx;
                        break;
                    case SDLK_RIGHT:
                        vx -= Speedx;
                        break;
                }
            }
            break;
        }
    }
};

void GameOverHandleEvent(SDL_Event& e)
{
    if(e.type==SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch(e.key.keysym.sym)
        {
        case SDLK_UP:
            GameOverSelect--;
            GameOverSelect+=2;
            GameOverSelect%=2;
            break;
        case SDLK_DOWN:
            GameOverSelect++;
            GameOverSelect%=2;
            break;
        case SDLK_RETURN:
            if(GameOverSelect==RESTART)
            {
                ScreenState=COUNTDOWN;
                cnt=3;
            }
            else
            {
                quit=true;
                break;
            }
            break;
        }
    }
}

int main(int argc,char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    window=SDL_CreateWindow("MAP",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    ren= SDL_CreateRenderer(window, -1, 0);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    SDL_Rect background={0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
    SDL_Color textcolor={0,0,0,255};
    floor STAIR[9];
    for(int i=0;i<9;i++)
    {
        STAIR[i].stair={0,100*i,SCREEN_WIDTH,100};
        STAIR[i].SetRandState();
        STAIR[i].SetHoleRec();
        STAIR[i].SetFloorVy(5);
    }
    baby p1(160,7,10,5);
    baby p2(400,7,10,5);
    SDL_Event e;
	while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
                break;
            }
            switch(ScreenState)
            {
            case MENU:
                break;
            case PLAYING:
                p1.handleEvent(e,1);
                p2.handleEvent(e,2);
                break;
            case GAMEOVER:
                //可以再做修飾
                GameOverHandleEvent(e);
            }
        }
        SDL_SetRenderDrawColor( ren, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear(ren);
        switch(ScreenState)
        {
        default:
        case MENU:
            break;
        case COUNTDOWN:
            //after start
            if(cnt==3)
            {
                for(int i=0;i<9;i++)
                {
                    STAIR[i].stair={0,100*i,SCREEN_WIDTH,100};
                    STAIR[i].SetRandState();
                    STAIR[i].SetHoleRec();
                    STAIR[i].SetFloorVy(5);
                }
                p1.ReBorn(160,7,10,5);
                p2.ReBorn(400,7,10,5);
            }
            loadpngwithrec("image/background.png",background);
            for(int i=0;i<9;i++)
            {
                STAIR[i].loadfloor();
                //STAIR[i].DrawHole();
            }
            p1.loadBaby(1);
            p2.loadBaby(2);
            p1.ShowScore(1);
            p2.ShowScore(2);
            switch(cnt)
            {
            case 3:
                loadtextatcenter("TTF/lazy.ttf",140,textcolor,"3",SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
                cnt--;
                break;
            case 2:
                loadtextatcenter("TTF/lazy.ttf",140,textcolor,"2",SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
                cnt--;
                break;
            case 1:
                loadtextatcenter("TTF/lazy.ttf",140,textcolor,"1",SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
                cnt--;
                break;
            case 0:
                loadtextatcenter("TTF/lazy.ttf",140,textcolor,"START!",SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
                cnt--;
                break;
            }
            break;
        case PLAYING:
            //after start
            loadpngwithrec("image/background.png",background);
            for(int i=0;i<9;i++)
            {
                STAIR[i].loadfloor();
                //STAIR[i].DrawHole();
            }
            if(!p1.GetBabyState())p1.loadBaby(1);
            else p1.ShowDead(1);
            if(!p2.GetBabyState())p2.loadBaby(2);
            else p2.ShowDead(2);
            p1.ShowScore(1);
            p2.ShowScore(2);
            for(int i=0;i<9;i++)
            {
                if(STAIR[i].stair.y>(-1)*STAIR[i].stair.h)
                {
                    STAIR[i].stair.y-=STAIR[i].GetFloorVy();
                    STAIR[i].SetHoleRec();
                }
                else
                {
                    STAIR[i].stair.y=795;
                    STAIR[i].SetRandState();
                    STAIR[i].SetHoleRec();
                }
            }
            if(!p1.GetBabyState())p1.MoveBaby(STAIR[p1.GetBabyStairNum()]);
            if(!p2.GetBabyState())p2.MoveBaby(STAIR[p2.GetBabyStairNum()]);
            if(p1.GetBabyState()&&p2.GetBabyState())ScreenState=GAMEOVER;
            break;
        case GAMEOVER:
            SDL_Rect selecter={20,370,80,80};
            loadtextatcenter("TTF/PRISTINA.ttf",140,textcolor,"~GAME~",SCREEN_WIDTH/2,90);
            loadtextatcenter("TTF/PRISTINA.ttf",140,textcolor,"~OVER~",SCREEN_WIDTH/2,230);
            loadtextatcenter("TTF/PRISTINA.ttf",100,textcolor,"RESTART",SCREEN_WIDTH/2,420);
            loadtextatcenter("TTF/PRISTINA.ttf",100,textcolor,"QUIT",SCREEN_WIDTH/2,560);
            if(GameOverSelect==RESTART)
            {
                selecter.y=370;
                SDL_SetRenderDrawColor(ren,0x00,0xFF,0x00,0xFF);
                SDL_RenderFillRect(ren,&selecter);
            }
            else
            {
                selecter.y=510;
                SDL_SetRenderDrawColor(ren,0x00,0xFF,0x00,0xFF);
                SDL_RenderFillRect(ren,&selecter);
            }
            break;
        }
        SDL_RenderPresent(ren);
        if(ScreenState==COUNTDOWN)
        {
            SDL_Delay(1000);
            if(cnt<0)ScreenState=PLAYING;
        }
    }
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

