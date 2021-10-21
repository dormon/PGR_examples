#include<iostream>
#include<vector>
#include<SDL.h>

#include<bunny.hpp>

#include<geGL/geGL.h>
#include<geGL/StaticCalls.h>
using namespace ge::gl;

GLuint createShader(GLenum type,std::string const&src){
  auto id = glCreateShader(type);
  char const*srcs[]={src.data()};
  glShaderSource(id,1,srcs,nullptr);
  glCompileShader(id);
  return id;
}

GLuint createProgram(std::vector<GLuint>const&shaders){
  auto id = glCreateProgram();
  for(auto const&shader:shaders)
    glAttachShader(id,shader);
  glLinkProgram(id);
  return id;
}

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

  ge::gl::init();

  GLuint vbo;
  glCreateBuffers(1,&vbo);
  std::vector<float>vertices = {
    0,0,0,1,
    1,0,0,1,
    0,1,0,1,
  };
  glNamedBufferData(vbo,vertices.size()*sizeof(decltype(vertices)::value_type),vertices.data(),GL_DYNAMIC_DRAW);

  GLuint vao;
  glCreateVertexArrays(1,&vao);

  glVertexArrayAttribBinding(vao,0,0);
  glEnableVertexArrayAttrib(vao,0);
  glVertexArrayAttribFormat(vao,
      0,//cislo vertex attributu
      4,//pocet polozek pro cteni (vec2)
      GL_FLOAT,//typ polozek
      GL_FALSE,//normalizace
      0);//relativni offset
  glVertexArrayVertexBuffer(vao,0,
    vbo,
    sizeof(float)*0,//offset
    sizeof(float)*4);//stride

  GLuint vaoBunny;
  GLuint vboBunny;
  GLuint eboBunny;
  glCreateVertexArrays(1,&vaoBunny);
  glCreateBuffers(1,&vboBunny);
  glNamedBufferData(vboBunny,sizeof(bunnyVertices),bunnyVertices,GL_DYNAMIC_DRAW);

  glCreateBuffers(1,&eboBunny);
  glNamedBufferData(eboBunny,sizeof(bunnyIndices),bunnyIndices,GL_DYNAMIC_DRAW);

  glVertexArrayAttribBinding(vaoBunny,0,0);
  glEnableVertexArrayAttrib(vaoBunny,0);
  glVertexArrayAttribFormat(vaoBunny,
      0,//cislo vertex attributu
      3,//pocet polozek pro cteni (vec2)
      GL_FLOAT,//typ polozek
      GL_FALSE,//normalizace
      0);//relativni offset
  glVertexArrayVertexBuffer(vaoBunny,0,
    vboBunny,
    sizeof(float)*0,//offset
    sizeof(float)*6);//stride

  glVertexArrayAttribBinding(vaoBunny,1,1);
  glEnableVertexArrayAttrib(vaoBunny,1);
  glVertexArrayAttribFormat(vaoBunny,
      1,//cislo vertex attributu
      3,//pocet polozek pro cteni (vec2)
      GL_FLOAT,//typ polozek
      GL_FALSE,//normalizace
      0);//relativni offset
  glVertexArrayVertexBuffer(vaoBunny,1,
    vboBunny,
    sizeof(float)*3,//offset
    sizeof(float)*6);//stride

  glVertexArrayElementBuffer(vaoBunny,eboBunny);


  auto vsSrc = R".(
  #version 460
  layout(location=0)in vec3 position;
  layout(location=1)in vec3 normal  ;
  out vec3 vNormal;
  void main(){
    gl_Position = vec4(position,1);
    vNormal = normal;
  }

  ).";

  auto fsSrc = R".(
  #version 460
  layout(location=0)out vec4 fColor;
  in vec3 vNormal;
  void main(){
    fColor = vec4(vNormal,1);
  }
  ).";

  GLuint prg = createProgram({
      createShader(GL_VERTEX_SHADER,vsSrc),
      createShader(GL_FRAGMENT_SHADER,fsSrc)});

  glEnable(GL_DEPTH_TEST);

  bool running = true;
  while(running){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
      if(event.type == SDL_QUIT)
        running = false;
    }
    glClearColor(0,0.5,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //triangle
    glBindVertexArray(vao);
    glUseProgram(prg);
    glDrawArrays(GL_TRIANGLES,0,3);
    glBindVertexArray(0);

    //bunny
    glBindVertexArray(vaoBunny);
    glUseProgram(prg);
    glDrawElements(GL_TRIANGLES,sizeof(bunnyIndices)/sizeof(VertexIndex),GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
