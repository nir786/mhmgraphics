
#include "stb_image.h"

#include "Components.hpp"
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
// assimps
//
//
//
#include <assimp/vector3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

/// glm
///
///
///
///
///
///
///
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// glfw
// clang-format on

#include "Camera.hpp"
#include "MeshData.hpp"
#include "Object.hpp"
#include "Shader.hpp"
#include <cstring>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
////
///
#include "Input.hpp"
#include "texture.hpp"
#include <random>

#define vertShaderSource "/home/nir/cpp_graphics/simpleShade.vert"
#define fragShaderSource "/home/nir/cpp_graphics/simpleShade.frag"

float _width = 1920.0f, _height = 1080.0f;

void renderScene_2() {}

void moveUpAndDown(Object &obj, float start_pos) {

  auto &transform = obj.getComponent<Components::Transform>();
  transform.Position.y = start_pos + 10.0f * sin(glfwGetTime() * 2.0f);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  _width = width;
  _height = height;
}

GLFWwindow *initGLFW() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(500, 500, "testing", NULL, NULL);
  if (window == NULL) {
    std::cerr << "GLFW window wasnt initialized" << std::endl;
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    exit(1);
  }

  glEnable(GL_DEPTH_TEST);

  int nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes
            << std::endl;
  return window;
}
// clang-format off
// clang-format on

struct GPULight {
  glm::vec4 position;
  glm::vec4 direction;
  glm::vec4 color;
};
std::vector<GPULight> Construct_LightBuffer(std::vector<Object *> &lights) {
  std::vector<GPULight> data;
  for (auto &light : lights) {

    auto &lightdata = light->getComponent<Components::Light>();
    auto &transform = light->getComponent<Components::Transform>();
    data.push_back(GPULight{
        glm::vec4(transform.Position.x, transform.Position.y,
                  transform.Position.z, lightdata.cutoff),

        glm::vec4(lightdata.direction.x, lightdata.direction.y,
                  lightdata.direction.z, static_cast<float>(lightdata.Type)),

        glm::vec4(lightdata.Color.x, lightdata.Color.y, lightdata.Color.z,
                  lightdata.Intensity)});
    //    std::cout << "light data . type  = " <<
    //    static_cast<float>(lightdata.Type)
    //             << std::endl;
    // xyz  |dir xyz | rgb |
  }
  return data;
}

void drawLight(Object &obj, Shader &csh, glm::mat4 &projection, glm::mat4 &view,
               int count) {
  std::string aAlpha = "aAlpha";
  std::string aTexture1 = "Texture1";
  std::string aTexture2 = "Texture2";
  std::string aBlend = "blend";
  std::string aTransform = "transform";

  std::string aView = "view";
  std::string aProjection = "projection";
  std::string aModel = "model";
  std::string aLightObject = "isLight";
  std::string aLightSource = "LightSource";
  std::string aViewPos = "ViewPos";
  std::string aObjectPos = "ObjectPos";

  std::string aAmbient = "material.ambient";
  std::string aDiffuse = "material.diffuse";
  std::string aSpecular = "material.specular";
  std::string aShine = "material.shininess";

  std::string aLightLoc = "lightLoc";

  csh.use();

  glBindVertexArray(obj.getComponent<Components::Mesh>().mesh.VAO);
  auto &ttransform = obj.getComponent<Components::Transform>();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,
                obj.getComponent<Components::Tex>().tex.textureID);
  glm::mat4 mmodel(1.0f);
  mmodel = glm::translate(mmodel, ttransform.Position);
  mmodel *= glm::mat4_cast(glm::quat(glm::radians(ttransform.Rotation)));
  mmodel = glm::scale(mmodel, ttransform.Scale);

  //  csh.setVec3(aLightLoc, static_light);
  glm::vec3 aambient = glm::vec3(0.05f, 0.08f, 0.12f);
  glm::vec3 adiffuse = glm::vec3(0.18f, 0.42f, 0.72f);
  glm::vec3 aspecular = glm::vec3(0.85f, 0.90f, 1.00f);
  float sshininess = 40.0f;
  csh.setVec3(aAmbient, aambient);
  csh.setVec3(aDiffuse, adiffuse);
  csh.setVec3(aSpecular, aspecular);
  csh.setFloat(aShine.c_str(), sshininess);
  csh.setMat4(aProjection, projection);
  csh.setMat4(aView, view);
  csh.setMat4(aModel, mmodel);
  csh.setInt("lightCount", count);
  csh.setInt("TEXTURE0", 0);
  csh.setInt("ignore", 1);

  glDrawArrays(GL_TRIANGLES, 0,
               obj.getComponent<Components::Mesh>().mesh.verts_normals.size() /
                   8);
}

std::unique_ptr<Object> preperCube(float _x, float _y, float _z,
                                   textureManager &manager, MeshData &cube_m) {
  std::mt19937 rng{std::random_device{}()};
  std::vector<const char *> colors{BF_WHITE, BF_MAGENTA, BF_GRAY, BF_BLACK,
                                   BF_RED};
  std::uniform_real_distribution<float> angle(0.0f, 360.0f);
  std::uniform_real_distribution<float> scale(1.0f, 10.0f);
  std::uniform_int_distribution<size_t> dist(0, colors.size() - 1);

  static unsigned int id;
  std::string name = "cube_" + std::to_string(id);
  std::unique_ptr<Object> cube = std::make_unique<Object>(name);
  cube->addComponenet<Components::Mesh>(cube_m);
  glm::vec3 x = {_x, _y, _z};
  glm::vec3 y = {angle(rng), angle(rng), angle(rng)};
  std::cout << y.x << " " << y.y << " " << y.z << std::endl;
  glm::vec3 z = {scale(rng), scale(rng), scale(rng)};
  cube->addComponenet<Components::Transform>(x, y, z);
  cube->addComponenet<Components::Tex>(manager.getTexture(colors[dist(rng)]));
  id++;
  return std::move(cube);
}
void drawCube(Object &obj, Shader &csh, glm::mat4 &view, glm::mat4 &projection,
              int size) {

  std::string aAlpha = "aAlpha";
  std::string aTexture1 = "Texture1";
  std::string aTexture2 = "Texture2";
  std::string aBlend = "blend";
  std::string aTransform = "transform";

  std::string aView = "view";
  std::string aProjection = "projection";
  std::string aModel = "model";
  std::string aLightObject = "isLight";
  std::string aLightSource = "LightSource";
  std::string aViewPos = "ViewPos";
  std::string aObjectPos = "ObjectPos";

  std::string aAmbient = "material.ambient";
  std::string aDiffuse = "material.diffuse";
  std::string aSpecular = "material.specular";
  std::string aShine = "material.shininess";

  std::string aLightLoc = "lightLoc";

  csh.use();
  auto &transform = obj.getComponent<Components::Transform>();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,
                obj.getComponent<Components::Tex>().tex.textureID);
  glm::mat4 model(1.0f);
  model = glm::translate(model, transform.Position);
  model *= glm::mat4_cast(glm::quat(glm::radians(transform.Rotation)));
  model = glm::scale(model, transform.Scale);
  glBindVertexArray(obj.getComponent<Components::Mesh>().mesh.VAO);

  //  csh.setVec3(aLightLoc, static_light);
  glm::vec3 ambient = glm::vec3(0.15f);
  glm::vec3 diffuse = glm::vec3(0.65f);
  glm::vec3 specular = glm::vec3(0.12f);
  float shininess = 30.0f;
  csh.setVec3(aAmbient, ambient);
  csh.setVec3(aDiffuse, diffuse);
  csh.setVec3(aSpecular, specular);
  csh.setFloat(aShine.c_str(), shininess);
  csh.setMat4(aProjection, projection);
  csh.setMat4(aView, view);
  csh.setMat4(aModel, model);
  csh.setInt("lightCount", size);
  csh.setInt("TEXTURE0", 0);

  csh.setInt("ignore", 0);
  glDrawArrays(GL_TRIANGLES, 0,
               obj.getComponent<Components::Mesh>().mesh.verts_normals.size() /
                   8);
}
void renderScene_1(Camera &c, glm::vec3 &F, glm::vec3 &U, glm::vec3 &P,
                   Shader &csh, Object &plane, int size, Object &cube_1,
                   Object &cube_2, Object &cube_3, Object &cube_4,
                   Object &cube_5, Object &LightSource_1, Object &LightSource_2,
                   Shader &skyboxSh, Object &skybox) {
  std::string aAlpha = "aAlpha";
  std::string aTexture1 = "Texture1";
  std::string aTexture2 = "Texture2";
  std::string aBlend = "blend";
  std::string aTransform = "transform";

  std::string aView = "view";
  std::string aProjection = "projection";
  std::string aModel = "model";
  std::string aLightObject = "isLight";
  std::string aLightSource = "LightSource";
  std::string aViewPos = "ViewPos";
  std::string aObjectPos = "ObjectPos";

  std::string aAmbient = "material.ambient";
  std::string aDiffuse = "material.diffuse";
  std::string aSpecular = "material.specular";
  std::string aShine = "material.shininess";

  std::string aLightLoc = "lightLoc";

  //////////////////////////////////////////////////////////////////////////
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  view = c.GetViewMatrix(F, U, P);
  projection =
      glm::perspective(glm::radians(120.0f), _width / _height, 0.1f, 150.0f);
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///
  ///
  ///
  ///
  ///
  ///
  ///
  csh.use();

  auto &transform = plane.getComponent<Components::Transform>();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,
                plane.getComponent<Components::Tex>().tex.textureID);
  glm::mat4 model(1.0f);
  model = glm::translate(model, transform.Position);
  model *= glm::mat4_cast(glm::quat(glm::radians(transform.Rotation)));
  model = glm::scale(model, transform.Scale);
  glBindVertexArray(plane.getComponent<Components::Mesh>().mesh.VAO);

  //  csh.setVec3(aLightLoc, static_light);
  glm::vec3 ambient = glm::vec3(0.15f);
  glm::vec3 diffuse = glm::vec3(0.65f);
  glm::vec3 specular = glm::vec3(0.12f);
  float shininess = 30.0f;
  csh.setVec3(aAmbient, ambient);
  csh.setVec3(aDiffuse, diffuse);
  csh.setVec3(aSpecular, specular);
  csh.setFloat(aShine.c_str(), shininess);
  csh.setMat4(aProjection, projection);
  csh.setMat4(aView, view);
  csh.setMat4(aModel, model);
  csh.setInt("lightCount", size);
  csh.setInt("TEXTURE0", 0);

  csh.setInt("ignore", 0);
  glDrawArrays(
      GL_TRIANGLES, 0,
      plane.getComponent<Components::Mesh>().mesh.verts_normals.size() / 8);

  drawCube(cube_1, csh, view, projection, size);
  drawCube(cube_2, csh, view, projection, size);
  drawCube(cube_3, csh, view, projection, size);
  drawCube(cube_4, csh, view, projection, size);
  drawCube(cube_5, csh, view, projection, size);

  //////////////////////////////////////////////////////////////////////////////////////
  csh.use();

  glBindVertexArray(LightSource_1.getComponent<Components::Mesh>().mesh.VAO);
  auto &ttransform = LightSource_1.getComponent<Components::Transform>();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,
                LightSource_1.getComponent<Components::Tex>().tex.textureID);
  glm::mat4 mmodel(1.0f);
  mmodel = glm::translate(mmodel, ttransform.Position);
  mmodel *= glm::mat4_cast(glm::quat(glm::radians(ttransform.Rotation)));
  mmodel = glm::scale(mmodel, ttransform.Scale);

  //  csh.setVec3(aLightLoc, static_light);
  glm::vec3 aambient = glm::vec3(0.05f, 0.08f, 0.12f);
  glm::vec3 adiffuse = glm::vec3(0.18f, 0.42f, 0.72f);
  glm::vec3 aspecular = glm::vec3(0.85f, 0.90f, 1.00f);
  float sshininess = 40.0f;
  csh.setVec3(aAmbient, aambient);
  csh.setVec3(aDiffuse, adiffuse);
  csh.setVec3(aSpecular, aspecular);
  csh.setFloat(aShine.c_str(), sshininess);
  csh.setMat4(aProjection, projection);
  csh.setMat4(aView, view);
  csh.setMat4(aModel, mmodel);
  csh.setInt("lightCount", size);

  csh.setInt("TEXTURE0", 0);
  csh.setInt("ignore", 1);

  glDrawArrays(
      GL_TRIANGLES, 0,
      LightSource_1.getComponent<Components::Mesh>().mesh.verts_normals.size() /
          8);

  ///

  drawLight(LightSource_2, csh, projection, view, size);
  /*
    glDepthMask(GL_FALSE);

    skyboxSh.use();

    glm::mat4 SkyView = glm::mat4(glm::mat3(view));

    auto &cubemap = skybox.getComponent<Components::CubeMap>();
    glBindVertexArray(cubemap.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.TextureID);

    skyboxSh.setInt("skybox", 0);
    skyboxSh.setMat4(aProjection, projection);
    skyboxSh.setMat4(aView, SkyView);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
    /////////////////////////////////////////////////////////////////////////////////////////////////
    */
}

int main() {

  GLFWwindow *window = initGLFW();
  textureManager manager;
  manager.LoadDefault(BG_TEX_MAGENTA, BF_MAGENTA);
  manager.LoadDefault(BG_TEX_GRAY, BF_GRAY);

  ////////////////////////////////////////test mesh system/////////////////
  Object plane{"plane"};
  MeshData plane_m = Importer::LoadPrimitive(Importer::Cube);
  plane.addComponenet<Components::Mesh>(plane_m);
  glm::vec3 x = {0.0f, 0.0f, 0.0f};
  glm::vec3 y = {0.0f, 0.0f, 0.0f};
  glm::vec3 z = {50.0f, 1.0f, 50.0f};
  plane.addComponenet<Components::Transform>(x, y, z);
  plane.addComponenet<Components::Tex>(manager.getTexture(BF_GRAY));
  /////////////////////////cube factory////////////////////////////////////////

  std::unique_ptr<Object> cube_1 =
      preperCube(-15.0f, 10.0f, -15.0f, manager, plane_m);
  std::unique_ptr<Object> cube_2 =
      preperCube(15.0f, 13.0f, -15.0f, manager, plane_m);
  std::unique_ptr<Object> cube_3 =
      preperCube(-15.0f, 15.0f, 15.0f, manager, plane_m);
  std::unique_ptr<Object> cube_4 =
      preperCube(15.0f, 10.0f, 15.0f, manager, plane_m);
  std::unique_ptr<Object> cube_5 =
      preperCube(0.0f, 24.0f, 0.0f, manager, plane_m);
  //////////////////////////////////////////////////////////////////////////
  Object LightSource_1 = Object("lightsource sphere");
  MeshData LightSource_M = Importer::LoadPrimitive(Importer::Cube);
  LightSource_1.addComponenet<Components::Mesh>(LightSource_M);

  glm::vec3 Light_P_1 = {0.0f, 10.0f, 0.0f};
  glm::vec3 Light_R_1 = {0.0f, 0.0f, 0.0f};
  glm::vec3 Light_S_1 = {1.0f, 1.0f, 1.0f};
  LightSource_1.addComponenet<Components::Transform>(Light_P_1, Light_R_1,
                                                     Light_S_1);

  LightSource_1.addComponenet<Components::Light>(
      true, Components::SpotLight, 5.50f, glm::vec3(1.0f, 1.0f, 1.0f),
      glm::vec3(0.0f, -1.0f, 0.0f), glm::cos(glm::radians(50.5f)));
  std::cout << glm::cos(glm::radians(50.5f)) << "< theta" << std::endl;
  LightSource_1.addComponenet<Components::Tex>(manager.getTexture(BF_WHITE));
  /////////////////////////////////////////////////////////////////////////////////////////
  Object LightSource_2 = Object("lightsource sphere");
  MeshData LightSource_2M = Importer::LoadPrimitive(Importer::Cube);
  LightSource_2.addComponenet<Components::Mesh>(LightSource_M);

  glm::vec3 Light_P_2 = {5.0f, 20.0f, 0.0f};
  glm::vec3 Light_R_2 = {0.0f, 0.0f, 0.0f};
  glm::vec3 Light_S_2 = {1.0f, 1.0f, 1.0f};
  LightSource_2.addComponenet<Components::Transform>(Light_P_2, Light_R_2,
                                                     Light_S_2);

  LightSource_2.addComponenet<Components::Light>(
      true, Components::Point, 10.00f, glm::vec3(1.0f, 0.1f, 0.5f),
      glm::vec3(0.0f, 0.0f, 0.0f), glm::cos(glm::radians(12.5f)));
  LightSource_2.addComponenet<Components::Tex>(manager.getTexture(BF_WHITE));
  ////////////////////////////////////////////////////////////////////////////////
  Object skybox = Object("Skybox");
  std::vector<std::string> faces = {
      "sky_12_cubemap_2k/px.png", "sky_12_cubemap_2k/nx.png",
      "sky_12_cubemap_2k/py.png", "sky_12_cubemap_2k/ny.png",
      "sky_12_cubemap_2k/pz.png", "sky_12_cubemap_2k/nz.png"};

  skybox.addComponenet<Components::CubeMap>(faces);
  std::cout << "cubemap id -> "
            << skybox.getComponent<Components::CubeMap>().TextureID
            << std::endl;
  /////////////////////////////////////////////////////////////////////////////////////////
  Shader sh = Shader(fragShaderSource, vertShaderSource);
  Shader csh = Shader("/home/nir/cpp_graphics/shadercar.frag",
                      "/home/nir/cpp_graphics/shadercar.vert");
  Shader phongshaderbase = Shader("/home/nir/cpp_graphics/shaderphong.frag",
                                  "/home/nir/cpp_graphics/shaderphong.vert");
  Shader hdrShader("hdrShader.frag", "hdrShader.vert");
  Shader skyboxSh = Shader("skybox.frag", "skybox.vert");

  std::string aAlpha = "aAlpha";
  std::string aTexture1 = "Texture1";
  std::string aTexture2 = "Texture2";
  std::string aBlend = "blend";
  std::string aTransform = "transform";

  std::string aView = "view";
  std::string aProjection = "projection";
  std::string aModel = "model";
  std::string aLightObject = "isLight";
  std::string aLightSource = "LightSource";
  std::string aViewPos = "ViewPos";
  std::string aObjectPos = "ObjectPos";

  std::string aAmbient = "material.ambient";
  std::string aDiffuse = "material.diffuse";
  std::string aSpecular = "material.specular";
  std::string aShine = "material.shininess";

  std::string aLightLoc = "lightLoc";

  ///////////////////////////////////////////////////////////////////////////////////////////////
  GLuint LightmapSSBO;

  std::vector<Object *> objectbuff;
  objectbuff.push_back(&LightSource_1);
  objectbuff.push_back(&LightSource_2);

  std::vector<GPULight> lightmapsdata = Construct_LightBuffer(objectbuff);
  glGenBuffers(1, &LightmapSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightmapSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               lightmapsdata.size() * sizeof(GPULight), lightmapsdata.data(),
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, LightmapSSBO);
  // clang-format off
  /////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////HDR INITILIZATION//////////////////////////////////////////////////////
  // clang-format on

  GLuint HDRcolorBufferTexture;
  glGenTextures(1, &HDRcolorBufferTexture);
  glBindTexture(GL_TEXTURE_2D, HDRcolorBufferTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA,
               GL_FLOAT, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //////////////////
  GLuint depthRBO;
  glGenRenderbuffers(1, &depthRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _width, _height);

  GLuint hdrFBO;
  glGenFramebuffers(1, &hdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         HDRcolorBufferTexture, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, depthRBO);

  GLuint hdrVAO;
  glGenVertexArrays(1, &hdrVAO);
  ////////////////////////shadowmapping/////////////////////////////////////////////////////
  GLuint depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);

  GLuint DepthMapTexture;
  glGenTextures(1, &DepthMapTexture);
  glBindTexture(GL_TEXTURE_2D, DepthMapTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         DepthMapTexture, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  Camera c;

  glm::vec3 F{0.0f, 0.0f, -1.0f};
  glm::vec3 U{0.0f, 1.0f, 0.0f};
  glm::vec3 R{1.0f, 0.0f, 0.0f};
  glm::vec3 P{0.0f, 18.0f, 48.0f};
  c.SetPosition(P);
  int state = 0;
  glm::vec3 static_light{0.0f, 10.0f, 10.0f};

  float exposure = 1.0f;
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  InputManager input = InputManager(window);

  while (!glfwWindowShouldClose(window)) {
    lightmapsdata = Construct_LightBuffer(objectbuff);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightmapSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                    lightmapsdata.size() * sizeof(GPULight),
                    lightmapsdata.data());
    ////////////lightmap- pass////////////////////////////////////////////
    input.movement_early(c, P, F, R, U);
    // moveUpAndDown(LightSource_1, 10.0f);
    moveUpAndDown(LightSource_2, 10.0f);
    input.exposure_m(exposure);
    ///////////movement- ////////////////////////////////////////////////
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    //////////////////////////HDRPASS//////////////////////////////////////////

    glViewport(0, 0, _width, _height);
    glBindTexture(GL_TEXTURE_2D, HDRcolorBufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glViewport(0, 0, _width, _height);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //////////////////////////////////////////////////////////////////////////
    renderScene_1(c, F, U, P, csh, plane, objectbuff.size(), *cube_1, *cube_2,
                  *cube_3, *cube_4, *cube_5, LightSource_1, LightSource_2,
                  skyboxSh, skybox);
    /*
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = c.GetViewMatrix(F, U, P);
    projection =
        glm::perspective(glm::radians(120.0f), _width / _height, 0.1f, 150.0f);
    ////////////////////////////////////////////////////////////////////////////////////////////
    ///
    ///
    ///
    ///
    ///
    ///
    ///
    csh.use();

    auto &transform = plane.getComponent<Components::Transform>();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  plane.getComponent<Components::Tex>().tex.textureID);
    glm::mat4 model(1.0f);
    model = glm::translate(model, transform.Position);
    model *= glm::mat4_cast(glm::quat(glm::radians(transform.Rotation)));
    model = glm::scale(model, transform.Scale);
    glBindVertexArray(plane.getComponent<Components::Mesh>().mesh.VAO);

    //  csh.setVec3(aLightLoc, static_light);
    glm::vec3 ambient = glm::vec3(0.15f);
    glm::vec3 diffuse = glm::vec3(0.65f);
    glm::vec3 specular = glm::vec3(0.12f);
    float shininess = 30.0f;
    csh.setVec3(aAmbient, ambient);
    csh.setVec3(aDiffuse, diffuse);
    csh.setVec3(aSpecular, specular);
    csh.setFloat(aShine.c_str(), shininess);
    csh.setMat4(aProjection, projection);
    csh.setMat4(aView, view);
    csh.setMat4(aModel, model);
    csh.setInt("lightCount", objectbuff.size());
    csh.setInt("TEXTURE0", 0);

    csh.setInt("ignore", 0);
    glDrawArrays(GL_TRIANGLES, 0, plane_m.verts_normals.size() / 8);

    drawCube(*cube_1, csh, view, projection, objectbuff.size());
    drawCube(*cube_2, csh, view, projection, objectbuff.size());
    drawCube(*cube_3, csh, view, projection, objectbuff.size());
    drawCube(*cube_4, csh, view, projection, objectbuff.size());
    drawCube(*cube_5, csh, view, projection, objectbuff.size());

    //////////////////////////////////////////////////////////////////////////////////////
    csh.use();

    glBindVertexArray(LightSource_M.VAO);
    auto &ttransform = LightSource_1.getComponent<Components::Transform>();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  LightSource_1.getComponent<Components::Tex>().tex.textureID);
    glm::mat4 mmodel(1.0f);
    mmodel = glm::translate(mmodel, ttransform.Position);
    mmodel *= glm::mat4_cast(glm::quat(glm::radians(ttransform.Rotation)));
    mmodel = glm::scale(mmodel, ttransform.Scale);

    //  csh.setVec3(aLightLoc, static_light);
    glm::vec3 aambient = glm::vec3(0.05f, 0.08f, 0.12f);
    glm::vec3 adiffuse = glm::vec3(0.18f, 0.42f, 0.72f);
    glm::vec3 aspecular = glm::vec3(0.85f, 0.90f, 1.00f);
    float sshininess = 40.0f;
    csh.setVec3(aAmbient, aambient);
    csh.setVec3(aDiffuse, adiffuse);
    csh.setVec3(aSpecular, aspecular);
    csh.setFloat(aShine.c_str(), sshininess);
    csh.setMat4(aProjection, projection);
    csh.setMat4(aView, view);
    csh.setMat4(aModel, mmodel);
    csh.setInt("lightCount", objectbuff.size());

    csh.setInt("TEXTURE0", 0);
    csh.setInt("ignore", 1);

    glDrawArrays(GL_TRIANGLES, 0, LightSource_M.verts_normals.size() / 8);

    ///

    drawLight(LightSource_2, csh, projection, view, objectbuff.size());

    glDepthMask(GL_FALSE);

    skyboxSh.use();

    glm::mat4 SkyView = glm::mat4(glm::mat3(view));

    auto &cubemap = skybox.getComponent<Components::CubeMap>();
    glBindVertexArray(cubemap.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.TextureID);

    skyboxSh.setInt("skybox", 0);
    skyboxSh.setMat4(aProjection, projection);
    skyboxSh.setMat4(aView, SkyView);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
    */
    /////////////////////////////////////////////////////////////////////////////////////////////////

    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _width, _height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(hdrVAO);
    hdrShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, HDRcolorBufferTexture);
    hdrShader.setInt("hdrBuffer", 0);
    hdrShader.setFloat("exposure", exposure);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    ///
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
