#include<iostream>
#include<vector>

#include<SDL.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_access.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<geGL/geGL.h>
#include<geGL/StaticCalls.h>

#include<bunny.hpp>

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
  
  uniform mat4 proj = mat4(1);
  uniform mat4 view = mat4(1);

  out vec3 vNormal;
  void main(){
    gl_Position = proj*view*vec4(position,1);
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

  auto projId = glGetUniformLocation(prg,"proj");
  auto viewId = glGetUniformLocation(prg,"view");
  auto proj = glm::perspective(glm::half_pi<float>(),1024.f/768.f,0.01f,1000.f);

  float angleX=0.f;
  float angleY=0.f;



  glEnable(GL_DEPTH_TEST);

  bool running = true;
  while(running){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
      if(event.type == SDL_QUIT)
        running = false;
      if(event.type == SDL_KEYDOWN){
        float sensitivity = 0.1f;
        if(event.key.keysym.sym == SDLK_a)          angleY += sensitivity;
        if(event.key.keysym.sym == SDLK_d)          angleY -= sensitivity;
        if(event.key.keysym.sym == SDLK_w)          angleX += sensitivity;
        if(event.key.keysym.sym == SDLK_s)          angleX -= sensitivity;
      }
    }


    auto view = 
      glm::translate(glm::mat4(1.f),glm::vec3(0.f,0.f,-10.f))*
      glm::rotate(glm::mat4(1.f),angleY,glm::vec3(0.f,1.f,0.f))*
      glm::rotate(glm::mat4(1.f),angleX,glm::vec3(1.f,0.f,0.f));

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

    glProgramUniformMatrix4fv(prg,projId,1,GL_FALSE,glm::value_ptr(proj));
    glProgramUniformMatrix4fv(prg,viewId,1,GL_FALSE,glm::value_ptr(view));
    glDrawElements(GL_TRIANGLES,sizeof(bunnyIndices)/sizeof(VertexIndex),GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
