#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 800;
SDL_Window *window= NULL;
SDL_Renderer *ren = NULL;
SDL_Texture *tex=NULL;
TTF_Font *font=NULL;

void loadpng(std::string path,int x,int y,int w,int h)
{
    SDL_Surface *png = IMG_Load(path.c_str());
    tex = SDL_CreateTextureFromSurface(ren, png);
    SDL_FreeSurface(png);
    SDL_Rect Viewport;
    Viewport.x=x;
    Viewport.y=y;
    Viewport.w=w;
    Viewport.h=h;
    SDL_RenderCopy(ren,tex,NULL,&Viewport);
    SDL_DestroyTexture(tex);
    tex=NULL;
}

void loadtext(std::string fontpath,int textsize,SDL_Color textcolor,std::string content,int x,int y)
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

int main(int argc,char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    window=SDL_CreateWindow("ttf",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    ren= SDL_CreateRenderer(window, -1, 0);
    if(!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG))std::cout<<"img error"<<std::endl;
    TTF_Init();

    SDL_Color textcolor={0,0,128,255};

    SDL_Rect background={0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
    bool quit=false;
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
                    }
                    SDL_SetRenderDrawColor(ren,255,255,255,255);
                    SDL_RenderFillRect(ren,&background);
                    loadtext("PRISTINA.TTF",36,textcolor,"true type font testing",SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
                    SDL_RenderPresent(ren);
                }
            }
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
