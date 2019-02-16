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
int ScreenState=MENU;
int cnt=3;

const int RESTART=0;
const int QUIT=1;
int GameOverSelect=RESTART;

const int BLUE=0;
const int WHITE=1;
const int BLACK=2;
const int RED=3;

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

class item
{
private:
    int category;
    bool create;
    SDL_Rect ItemRec;
    static int ItemVy;
public:
    item()
    {
        category=rand()%4;
        create=false;
        ItemRec={SCREEN_WIDTH,SCREEN_HEIGHT,60,60};
    }
    static void SetItemVy(int vy)
    {
        ItemVy=vy;
    }
    void SetCreate(bool y)
    {
        create=y;
    }
    bool GetCreate() const
    {
        return create;
    }
    void CreateItem()
    {
        ItemRec.x=rand()%540+20;
        ItemRec.y=825;
    }
    void MoveItem()
    {
        ItemRec.y-=ItemVy;
        if(ItemRec.y+ItemRec.h<=0)create=false;
    }
    void loadItem()
    {
        SDL_SetRenderDrawColor(ren,0x00,0x7F,0x7F,0xFF);
        SDL_RenderFillRect(ren,&ItemRec);
    }
    int  BabyGetItem(SDL_Rect player)
    {
        //from left top || from right top
        if((player.x+player.w>=ItemRec.x && player.x+player.w<=ItemRec.x+ItemRec.w && ItemRec.y<=player.y+player.h && ItemRec.y>=player.y)
           ||(player.x<=ItemRec.x+ItemRec.w && player.x>=ItemRec.x && ItemRec.y<=player.y+player.h && ItemRec.y>=player.y))
        {
            create=false;
            return category;
        }
        else return -1;
    }
};
int item::ItemVy=0;

class floor
{
private:
    const static int HoleW=101;
    const static int HoleH=10;
    int state;
    SDL_Rect hole[2];
    static int FloorVy;
public:
    SDL_Rect stair;
    static int GetFloorVy()
    {
        return FloorVy;
    }
    static void SetFloorVy(int v)
    {
        FloorVy=v;
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
    bool GenerateItem(Uint32 t)
    {
        if((t/1000)%8==0&&t/1000!=0)return true;
        return false;
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
int floor::FloorVy=0;

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
    void SetVY(int y)
    {
        vy=y;
    }
    int GetBabyStairNum() const
    {
        return StairNum;
    }
    void loadBaby(int playNum)
    {
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
    void MoveBaby(floor nowstair,floor nextstair)
    {
        babyrec.x+=vx;
        if(babyrec.x<=0)babyrec.x=0;
        if(babyrec.x>=SCREEN_WIDTH-BabyWidth)babyrec.x=SCREEN_WIDTH-BabyWidth;
        for(int i=0;i<2;i++)
        {
            if(babyrec.x>=nowstair.GetHoleLeftX(i)&&babyrec.x+babyrec.w<=nowstair.GetHoleRightX(i))
            {
                vy=-floor::GetFloorVy()*2;
            }
        }
        babyrec.y-=vy;
        if(babyrec.y>=SCREEN_HEIGHT-babyrec.h)babyrec.y=SCREEN_HEIGHT-babyrec.h;
        if(babyrec.y+babyrec.h<=0)dead=true;
        if(babyrec.y<610)
        {
            if(babyrec.y+babyrec.h>=nextstair.stair.y+90)
            {
                //std::cout<<nowstair.stair.y<<std::endl;
                //std::cout<<"top land"<<std::endl;
                babyrec.y=nextstair.stair.y+10;
                vy=floor::GetFloorVy();
                StairNum++;
                StairNum%=9;
                score++;
            }
        }
        else
        {
            if(babyrec.y+babyrec.h>=nowstair.stair.y+190)
            {
                //std::cout<<nowstair.stair.y<<std::endl;
                //std::cout<<"top land"<<std::endl;
                babyrec.y=nowstair.stair.y+110;
                vy=floor::GetFloorVy();
                StairNum++;
                StairNum%=9;
                score++;
            }
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
            loadtextatcenter("TTF/PRISTINA.ttf",35,ScoreColor,"1P's SCORE:",100,40);
            loadtextatcenter("TTF/PRISTINA.ttf",35,ScoreColor,ScoreText.str().c_str(),100,80);
            break;
        case 2:
            loadtextatcenter("TTF/PRISTINA.ttf",35,ScoreColor,"2P's SCORE:",540,40);
            loadtextatcenter("TTF/PRISTINA.ttf",35,ScoreColor,ScoreText.str().c_str(),540,80);
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
                ScreenState=MENU;
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
    window=SDL_CreateWindow("Crawling Baby",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    ren= SDL_CreateRenderer(window, -1, 0);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    SDL_Surface* icon=IMG_Load("image/leftw1.png");
    //std::cout<<icon<<std::endl;
    SDL_SetWindowIcon(window,icon);
    SDL_FreeSurface(icon);
    Uint32 StartTime=0,PlayTime=0;
    SDL_Rect background={0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
    SDL_Color textcolor={0,0,0,255};
    floor STAIR[9];
    item towel[9];
    for(int i=0;i<9;i++)
    {
        STAIR[i].stair={0,100*i,SCREEN_WIDTH,100};
        STAIR[i].SetRandState();
        STAIR[i].SetHoleRec();
    }
    floor::SetFloorVy(5);
    item::SetItemVy(floor::GetFloorVy());
    baby p1(160,7,11,floor::GetFloorVy());
    baby p2(400,7,11,floor::GetFloorVy());
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
                if(e.type==SDL_KEYDOWN&&e.key.repeat==0)
                {
                    ScreenState=COUNTDOWN;
                }
                break;
            case PLAYING:
                p1.handleEvent(e,1);
                p2.handleEvent(e,2);
                break;
            case GAMEOVER:
                GameOverHandleEvent(e);
            }
        }
        SDL_SetRenderDrawColor( ren, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear(ren);
        switch(ScreenState)
        {
        default:
        case MENU:
            loadpngwithrec("image/baby_start.png",background);
            break;
        case COUNTDOWN:
            //after start
            PlayTime=0;
            if(cnt==3)
            {
                for(int i=0;i<9;i++)
                {
                    STAIR[i].stair={0,100*i,SCREEN_WIDTH,100};
                    STAIR[i].SetRandState();
                    STAIR[i].SetHoleRec();
                }
                floor::SetFloorVy(5);
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
            PlayTime=(SDL_GetTicks()-StartTime);
            loadpngwithrec("image/background.png",background);
            for(int i=0;i<9;i++)
            {
                STAIR[i].loadfloor();
                //STAIR[i].DrawHole();
                if(towel[i].GetCreate())towel[i].loadItem();
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
                    towel[i].SetCreate(STAIR[i].GenerateItem(PlayTime));
                    if(towel[i].GetCreate())towel[i].CreateItem();
                }
                if(towel[i].GetCreate())
                {
                    towel[i].MoveItem();
                    if(!p1.GetBabyState())towel[i].BabyGetItem(p1.babyrec);
                    if(!p2.GetBabyState())towel[i].BabyGetItem(p2.babyrec);
                }
            }
            if(!p1.GetBabyState())p1.MoveBaby(STAIR[p1.GetBabyStairNum()],STAIR[(p1.GetBabyStairNum()+1)%9]);
            if(!p2.GetBabyState())p2.MoveBaby(STAIR[p2.GetBabyStairNum()],STAIR[(p2.GetBabyStairNum()+1)%9]);
            if(p1.GetBabyState()&&p2.GetBabyState())ScreenState=GAMEOVER;
            break;
        case GAMEOVER:
            loadpngwithrec("image/gameover.png",background);
            SDL_Rect selecter={17,380,80,80};
            loadtextatcenter("TTF/BROADWAY.TTF",140,textcolor,"~GAME~",SCREEN_WIDTH/2,90);
            loadtextatcenter("TTF/BROADWAY.TTF",140,textcolor,"~OVER~",SCREEN_WIDTH/2,230);
            loadtextatcenter("TTF/BROADWAY.TTF",100,textcolor,"RESTART",SCREEN_WIDTH/2,420);
            loadtextatcenter("TTF/BROADWAY.TTF",100,textcolor,"QUIT",SCREEN_WIDTH/2,560);
            if(GameOverSelect==RESTART)
            {
                selecter.x=17;
                selecter.y=380;
                loadpngwithrec("image/bottle.png",selecter);
            }
            else
            {
                selecter.x=127;
                selecter.y=520;
                loadpngwithrec("image/bottle.png",selecter);
            }
            break;
        }
        std::stringstream timetext;
        timetext.str("");
        timetext<<PlayTime/1000.f;
        //loadtextatcenter("TTF/lazy.ttf",40,textcolor,timetext.str().c_str(),SCREEN_WIDTH/2,700);
        SDL_RenderPresent(ren);
        if(ScreenState==COUNTDOWN)
        {
            SDL_Delay(1000);
            if(cnt<0)
            {
                ScreenState=PLAYING;
                StartTime=SDL_GetTicks();
            }
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

