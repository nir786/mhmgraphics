
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
#include <vector>
////
///
#include "Input.hpp"
#include "texture.hpp"

#define vertShaderSource "/home/nir/cpp_graphics/simpleShade.vert"
#define fragShaderSource "/home/nir/cpp_graphics/simpleShade.frag"

float _width = 500.0f, _height = 500.0f;
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
  glm::vec4 color;
};
std::vector<GPULight> Construct_LightBuffer(std::vector<Object *> &lights) {
  std::vector<GPULight> data;
  for (auto &light : lights) {

    auto &lightdata = light->getComponent<Components::Light>();
    auto &transform = light->getComponent<Components::Transform>();
    data.push_back(
        GPULight{glm::vec4(transform.Position.x, transform.Position.y,
                           transform.Position.z, 0.0f),
                 glm::vec4(lightdata.Color.x, lightdata.Color.y,
                           lightdata.Color.z, lightdata.Intensity)});
    // xyz | rgb |
  }
  return data;
}

void drawLight(Object &obj, Shader &csh, glm::mat4 &projection,
               glm::mat4 &view) {
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
  csh.setFloat(aShine, sshininess);
  csh.setMat4(aProjection, projection);
  csh.setMat4(aView, view);
  csh.setMat4(aModel, mmodel);
  csh.setInt("lightCount", 1);
  csh.setInt("TEXTURE0", 0);
  csh.setInt("ignore", 1);

  glDrawArrays(GL_TRIANGLES, 0,
               obj.getComponent<Components::Mesh>().mesh.verts_normals.size() /
                   8);
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
  glm::vec3 y = {90.0f, 0.0f, 0.0f};
  glm::vec3 z = {50.0f, 50.0f, 1.0f};
  plane.addComponenet<Components::Transform>(x, y, z);
  plane.addComponenet<Components::Tex>(manager.getTexture(BF_GRAY));

  //////////////////////////////////////////////////////////////////////////
  Object LightSource_1 = Object("lightsource sphere");
  MeshData LightSource_M = Importer::LoadPrimitive(Importer::Cube);
  LightSource_1.addComponenet<Components::Mesh>(LightSource_M);

  glm::vec3 Light_P_1 = {0.0f, 10.0f, 0.0f};
  glm::vec3 Light_R_1 = {0.0f, 0.0f, 0.0f};
  glm::vec3 Light_S_1 = {1.0f, 1.0f, 1.0f};
  LightSource_1.addComponenet<Components::Transform>(Light_P_1, Light_R_1,
                                                     Light_S_1);

  LightSource_1.addComponenet<Components::Light>(true, Components::Point, 1.00f,
                                                 glm::vec3(1.0f, 0.1f, 0.5f));
  LightSource_1.addComponenet<Components::Tex>(manager.getTexture(BF_WHITE));
  /////////////////////////////////////////////////////////////////////////////////////////
  Object LightSource_2 = Object("lightsource sphere");
  MeshData LightSource_2M = Importer::LoadPrimitive(Importer::Cube);
  LightSource_2.addComponenet<Components::Mesh>(LightSource_M);

  glm::vec3 Light_P_2 = {5.0f, 10.0f, 0.0f};
  glm::vec3 Light_R_2 = {0.0f, 0.0f, 0.0f};
  glm::vec3 Light_S_2 = {1.0f, 1.0f, 1.0f};
  LightSource_2.addComponenet<Components::Transform>(Light_P_2, Light_R_2,
                                                     Light_S_2);

  LightSource_2.addComponenet<Components::Light>(true, Components::Point, 1.00f,
                                                 glm::vec3(1.0f, 0.1f, 0.5f));
  LightSource_2.addComponenet<Components::Tex>(manager.getTexture(BF_WHITE));
  ////////////////////////////////////////////////////////////////////////////////
  Object skybox = Object("Skybox");
  std::vector<std::string> faces = {
      "sky_12_cubemap_2k/px.png", "sky_12_cubemap_2k/nx.png",
      "sky_12_cubemap_2k/py.png", "sky_12_cubemap_2k/ny.png",
      "sky_12_cubemap_2k/pz.png", "sky_12_cubemap_2k/nz.png"};

  skybox.addComponenet<Components::CubeMap>(faces);
  /////////////////////////////////////////////////////////////////////////////////////////
  Shader sh = Shader(fragShaderSource, vertShaderSource);
  Shader csh = Shader("/home/nir/cpp_graphics/shadercar.frag",
                      "/home/nir/cpp_graphics/shadercar.vert");
  Shader phongshaderbase = Shader("/home/nir/cpp_graphics/shaderphong.frag",
                                  "/home/nir/cpp_graphics/shaderphong.vert");
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

  /////////////////////////////////////////////////////////////////////////////////////////////

  Camera c;

  glm::vec3 F{0.0f, 0.0f, -1.0f};
  glm::vec3 U{0.0f, 1.0f, 0.0f};
  glm::vec3 R{1.0f, 0.0f, 0.0f};
  glm::vec3 P{0.0f, 0.0f, 3.0f};
  c.SetPosition(P);
  int state = 0;
  glm::vec3 static_light{0.0f, 10.0f, 10.0f};

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
    ///////////movement- ////////////////////////////////////////////////
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    csh.setFloat(aShine, shininess);
    csh.setMat4(aProjection, projection);
    csh.setMat4(aView, view);
    csh.setMat4(aModel, model);
    csh.setInt("lightCount", 2);
    csh.setInt("TEXTURE0", 0);

    csh.setInt("ignore", 0);
    glDrawArrays(GL_TRIANGLES, 0, plane_m.verts_normals.size() / 8);

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
    csh.setFloat(aShine, sshininess);
    csh.setMat4(aProjection, projection);
    csh.setMat4(aView, view);
    csh.setMat4(aModel, mmodel);
    csh.setInt("lightCount", 2);
    csh.setInt("TEXTURE0", 0);
    csh.setInt("ignore", 1);

    glDrawArrays(GL_TRIANGLES, 0, LightSource_M.verts_normals.size() / 8);

    ///

    drawLight(LightSource_2, csh, projection, view);

    glDepthMask(GL_FALSE);

    skyboxSh.use();

    glm::mat4 SkyView = glm::mat4(glm::mat3(view));

    auto &cubemap = skybox.getComponent<Components::CubeMap>();
    glBindVertexArray(cubemap.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.TextureID);

    skyboxSh.setInt("skybox", cubemap.TextureID);
    skyboxSh.setMat4(aProjection, projection);
    skyboxSh.setMat4(aView, SkyView);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
    /////////////////////////////////////////////////////////////////////////////////////////////////
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
