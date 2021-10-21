#include <iostream>
#include <SDL.h>

int main(int argc,char*argv[]){
  SDL_CreateWindow("PGR_examples",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1024,768,SDL_WINDOW_OPENGL);

  bool running = true;
  while(running){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
      if(event.type == SDL_QUIT)
        running = false;
    }
  }

  return 0;
}
