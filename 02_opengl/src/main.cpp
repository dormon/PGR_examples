#include <iostream>
#include <SDL.h>

using GLCLEARCOLOR = void(*)(float,float,float,float);
using GLCLEAR      = void(*)(uint32_t);

int main(int argc,char*argv[]){
  SDL_Init(SDL_INIT_VIDEO);//init. video

  auto window = SDL_CreateWindow("PGR_examples",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1024,768,SDL_WINDOW_OPENGL);

  uint32_t version = 460;//context version
  uint32_t profile = SDL_GL_CONTEXT_PROFILE_CORE;//context profile
  uint32_t flags   = SDL_GL_CONTEXT_DEBUG_FLAG;//context flags
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version/100 );
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,(version%100)/10);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK ,profile );
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS ,flags );

  auto context = SDL_GL_CreateContext(window);


  GLCLEARCOLOR glClearColor = (GLCLEARCOLOR)SDL_GL_GetProcAddress("glClearColor");
  GLCLEAR      glClear      = (GLCLEAR     )SDL_GL_GetProcAddress("glClear"     );
  auto const GL_COLOR_BUFFER_BIT = 0x00004000;

  bool running = true;
  while(running){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
      if(event.type == SDL_QUIT)
        running = false;
    }
    glClearColor(0,0.5,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
