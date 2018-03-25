#include <SDL2/SDL.h> /*All SDL App's need this*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
SDL_Texture* sdlTexture;
SDL_Renderer* sdlRenderer;

int thread_exit=0;

#define REFRESH_EVENT  (SDL_USEREVENT + 1)
int refresh_video(void *opaque)
{
    FILE *fp = NULL;
    SDL_Rect sdlRect;
    int rlen;
    int len = 1920*1080 + ((1920*1080) >> 1);
    unsigned char *buffer = malloc(sizeof(char)*len);

    fp = fopen("film.420p","rb");
    if (fp == NULL)
    {
        printf("cannot open this file\n");
        return -1;
    }

    while (thread_exit==0)
    {
        if ((rlen = fread(buffer, 1, len, fp)) != len)
        {
            rewind(fp);
            fread(buffer, 1, len, fp);
        }

        SDL_UpdateTexture( sdlTexture, NULL, buffer, 1920);

        //FIX: If window is resize
        sdlRect.x = 0;
        sdlRect.y = 0;
        sdlRect.w = 640;
        sdlRect.h = 320;

        //SDL_RenderClear( sdlRenderer );
        SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, &sdlRect);
        SDL_RenderPresent( sdlRenderer );

        Sleep(2*1000);
    }

    fclose(fp);
    free(buffer);

    return 0;
}

int main(int argc, char **args)
{
    SDL_Window* window = NULL;

    if (SDL_Init( SDL_INIT_VIDEO ) < 0)
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow( "APP", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN );
    if (window == NULL)
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return -1;
    }

    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, 1920, 1080);

    SDL_Thread *refresh_thread = SDL_CreateThread(refresh_video,NULL,NULL);
    SDL_Event event;

    while(1){
        SDL_WaitEvent(&event);
        if(event.type==REFRESH_EVENT){

        }else if(event.type == SDL_WINDOWEVENT){
            //If Resize
            //SDL_GetWindowSize(screen,&screen_w,&screen_h);
        }else if(event.type == SDL_QUIT){
            thread_exit = 1;
            break;
        }
    }

    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
