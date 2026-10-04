
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

#define vertShaderSource "/home/nir/cpp_graphics/simpleShade.vert"
#define fragShaderSource "/home/nir/cpp_graphics/simpleShade.frag"

float _width = 500.0f, _height = 500.0f;
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  _width = width;
  _height = height;
}

unsigned int Load_cb_map(std::vector<std::string> faces) {

  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); ++i) {

    unsigned char *data =
        stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    std::cout << width << " w, " << height << " h, " << nrChannels << " nr"
              << std::endl;
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width,
                   height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                   data); // because they're alligned

    } else {
      std::cout << "Failed to load texture from " << faces[i] << std::endl;
      stbi_image_free(data);
      exit(1);
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}

void initSkyBox(GLuint &VAOSkybox) {
  //..................................................................................................

  // clang-format off
  std::vector<float> vertSky = {
            
// positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };
  GLuint  VBOSkybox;

    glGenVertexArrays(1, &VAOSkybox);
    glBindVertexArray(VAOSkybox);
    glGenBuffers(1, &VBOSkybox);

    glBindBuffer(GL_ARRAY_BUFFER, VBOSkybox);
    glBufferData(GL_ARRAY_BUFFER, vertSky.size() * sizeof(float), vertSky.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);

glEnableVertexAttribArray(0);
  // clang-format on
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
  std::vector<float> vertecies = {
      // position                  color                    UV
      // Back face
      -0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 0.0f,        0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 
       0.5f, -0.5f, -0.5f,       0.0f, 1.0f, 0.0f,        1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
       0.5f,  0.5f, -0.5f,       0.0f, 0.0f, 1.0f,        1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
       0.5f,  0.5f, -0.5f,       0.0f, 0.0f, 1.0f,        1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
      -0.5f,  0.5f, -0.5f,       1.0f, 1.0f, 0.0f,        0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
      -0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 0.0f,        0.0f, 0.0f,  0.0f, 0.0f, -1.0f,

      // Front face
      -0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 1.0f,        0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,       0.0f, 1.0f, 1.0f,        1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
       0.5f,  0.5f,  0.5f,       1.0f, 1.0f, 1.0f,        1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
       0.5f,  0.5f,  0.5f,       1.0f, 1.0f, 1.0f,        1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,       1.0f, 0.5f, 0.0f,        0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 1.0f,        0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 

      // Left face
      -0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 0.0f,        1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 0.0f,        1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, 1.0f,        0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, 1.0f,        0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,       1.0f, 1.0f, 0.0f,        0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 0.0f,        1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

      // Right face
       0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 1.0f,        1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
       0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 1.0f,        1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,       1.0f, 1.0f, 1.0f,        0.0f, 1.0f,   1.0f, 0.0f, 0.0f,  
       0.5f, -0.5f, -0.5f,       1.0f, 1.0f, 1.0f,        0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
       0.5f, -0.5f,  0.5f,       1.0f, 0.5f, 0.0f,        0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
       0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 1.0f,        1.0f, 0.0f,   1.0f, 0.0f, 0.0f,

      // Bottom face
      -0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 0.0f,        0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
       0.5f, -0.5f, -0.5f,       0.0f, 1.0f, 0.0f,        1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,       0.0f, 0.0f, 1.0f,        1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,       0.0f, 0.0f, 1.0f,        1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,       1.0f, 1.0f, 0.0f,        0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 0.0f,        0.0f, 1.0f,   0.0f, -1.0f, 0.0f,

      // Top face
      -0.5f,  0.5f, -0.5f,       1.0f, 0.0f, 1.0f,        0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 1.0f,        1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,       1.0f, 1.0f, 1.0f,        1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,       1.0f, 1.0f, 1.0f,        1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,       1.0f, 0.5f, 0.0f,        0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,       1.0f, 0.0f, 1.0f,        0.0f, 1.0f,   0.0f, 1.0f, 0.0f

  };
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

int main() {

  GLFWwindow *window = initGLFW();
  ////////////////////////////////////////test mesh system/////////////////
  std::vector<MeshData> RenderQueue;

  Object ambulance = Object("ambulance");

  textureManager manager("Assets/dragon/fbx/textures");
  manager.loadMore("Assets/Models/FBX format/Textures/");
  manager.loadMore("Assets/bricks/texture");
  for (const auto &[key, value] : manager.map) {
    std::cout << key << std::endl;
  }
  MeshData amb =
      Importer::LoadMesh("Assets/Models/FBX format/ambulance.fbx", manager.map);
  ambulance.addComponenet<Components::Mesh>(amb);
  glm::vec3 position = {5.0f, 0.0f, 5.0f};
  glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
  glm::vec3 scale = {0.01f, 0.01f, 0.01f};
  ambulance.addComponenet<Components::Transform>(position, rotation, scale);

  RenderQueue.push_back(amb);
  //////////////////////////////////////////////////////////////////////////////

  Object PrimitiveCube = Object("cube");
  MeshData Cube = Importer::LoadPrimitive(Importer::Sphere);
  PrimitiveCube.addComponenet<Components::Mesh>(Cube);

  glm::vec3 pposition = {0.0f, 0.0f, 0.0f};
  glm::vec3 rrotation = {0.0f, 90.0f, 0.0f};
  glm::vec3 sscale = {1.0f, 1.0f, 1.0f};
  PrimitiveCube.addComponenet<Components::Transform>(pposition, rrotation,
                                                     sscale);

  ////////////////////////////////////////////////////////////////////////////

  Object dragon = Object("dragon");
  MeshData drag =
      Importer::LoadMesh("Assets/dragon/fbx/Dragon 2.5_fbx.fbx", manager.map);
  dragon.addComponenet<Components::Mesh>(drag);

  glm::vec3 ppposition = {5.0f, 5.0f, 0.0f};
  glm::vec3 rrrotation = {-90.0f, 0.0f, 0.0f};
  glm::vec3 ssscale = {0.2f, 0.2f, 0.2f};
  dragon.addComponenet<Components::Transform>(ppposition, rrrotation, ssscale);

  ///////////////////////////////////////////////////////////////////////
  Object Girl = Object("FuturisticGirl");
  MeshData FuturisticGirl =
      Importer::LoadMesh("Assets/bricks/bricks.obj", manager.map);
  Girl.addComponenet<Components::Mesh>(drag);

  glm::vec3 girlposition = {-30.0f, 5.0f, 0.0f};
  glm::vec3 girlrotation = {0.0f, 0.0f, 0.0f};
  glm::vec3 girlscale = {1.0f, 1.0f, 1.0f};
  Girl.addComponenet<Components::Transform>(girlposition, girlrotation,
                                            girlscale);

  //////////////////////////////////////////////////////////////////////////

  Object LightSource_1 = Object("lightsource sphere");
  MeshData LightSource_M = Importer::LoadPrimitive(Importer::Sphere);
  LightSource_1.addComponenet<Components::Mesh>(Cube);

  glm::vec3 Light_P_1 = {15.0f, 10.0f, 0.0f};
  glm::vec3 Light_R_1 = {0.0f, 0.0f, 0.0f};
  glm::vec3 Light_S_1 = {2.0f, 2.0f, 2.0f};
  LightSource_1.addComponenet<Components::Transform>(Light_P_1, Light_R_1,
                                                     Light_S_1);

  LightSource_1.addComponenet<Components::Light>(true, Components::Point, 1.0f,
                                                 glm::vec3(1.0f, 0.1f, 0.5f));
  /////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////

  Object LightSource_2 = Object("lightsource sphere 2 ");
  MeshData LightSource_M_2 = Importer::LoadPrimitive(Importer::Sphere);
  LightSource_2.addComponenet<Components::Mesh>(Cube);

  glm::vec3 Light_P_2 = {0.0f, 12.0f, 20.0f};
  glm::vec3 Light_R_2 = {0.0f, 0.0f, 0.0f};
  glm::vec3 Light_S_2 = {3.0f, 3.0f, 3.0f};
  LightSource_2.addComponenet<Components::Transform>(Light_P_2, Light_R_2,
                                                     Light_S_2);

  LightSource_2.addComponenet<Components::Light>(true, Components::Point, 1.0f,
                                                 glm::vec3(0.0f, 0.3f, 1.0f));
  /////////////////////////////////////////////////////////////////////////////////////////

  GLuint VBO, VAO;
  // GLuint EBO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  // clang-format off

  // clang-format on
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertecies.size() * sizeof(float),
               vertecies.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  //
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(8 * sizeof(float)));
  glEnableVertexAttribArray(3);

  //
  // ######## fully using the that VAO #########
  //

  GLuint VAOLight;
  glGenVertexArrays(1, &VAOLight);
  glBindVertexArray(VAOLight);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(8 * sizeof(float)));
  glEnableVertexAttribArray(3);

  Shader sh = Shader(fragShaderSource, vertShaderSource);
  Shader csh = Shader("/home/nir/cpp_graphics/shadercar.frag",
                      "/home/nir/cpp_graphics/shadercar.vert");
  Shader phongshaderbase = Shader("/home/nir/cpp_graphics/shaderphong.frag",
                                  "/home/nir/cpp_graphics/shaderphong.vert");

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
  // ###### a single object in our program#######
  //
  //
  //
  //
  //
  //
  //
  //
  //
  // Texture loading ...

  ////.........................................................................
  //..................................................................................................
  GLuint VAOSkybox;
  stbi_set_flip_vertically_on_load(false);
  std::vector<std::string> faces = {
      "sky_12_cubemap_2k/px.png", "sky_12_cubemap_2k/nx.png",
      "sky_12_cubemap_2k/py.png", "sky_12_cubemap_2k/ny.png",
      "sky_12_cubemap_2k/pz.png", "sky_12_cubemap_2k/nz.png"};

  unsigned int cubemapTexture = Load_cb_map(faces);

  Shader skyboxSh = Shader("skybox.frag", "skybox.vert");
  initSkyBox(VAOSkybox);

  // clang-format on

  //******************************SKYBOX*****************************************************
  /////////////////////////////light buffer setup
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

  sh.use();

  sh.setInt(aTexture1.c_str(), 0);
  sh.setInt(aTexture2.c_str(), 1);

  Camera c;

  glm::vec3 F{0.0f, 0.0f, -1.0f};
  glm::vec3 U{0.0f, 1.0f, 0.0f};
  glm::vec3 R{1.0f, 0.0f, 0.0f};
  glm::vec3 P{0.0f, 0.0f, 3.0f};
  c.SetPosition(P);
  int state = 0;
  glm::vec3 lightSourceDir{0.0f, 0.5f, 0.0f};
  float lightx = 0.0f;
  float lighty = 10.0f;
  float lightz = 10.0f;

  float lightxx = 0.5f;
  float lightyy = 15.0f;
  float lightzz = 30.0f;

  glm::vec3 static_light{0.0f, 20.0f, 0.0f};
  glEnable(GL_DEPTH_TEST);
  //  glDepthMask(GL_FALSE);
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

    sh.use();
    float timeValue = glfwGetTime() * 0.001f;
    float alphaValue = sin(timeValue);
    timeValue = 0.005f;

    float orbity = lighty * cos(timeValue) - lightz * sin(timeValue);
    float orbitz = lighty * sin(timeValue) + lightz * cos(timeValue);
    lighty = orbity;
    lightz = orbitz;

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 modelLight = glm::mat4(1.0f);
    modelLight = glm::translate(modelLight, glm::vec3(0, lighty, lightz));

    // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    for (int i = 1; i < 4; i++) {
      view = c.GetViewMatrix(F, U, P);
      glm::vec3 lightProj =
          glm::normalize(lightSourceDir - glm::vec3(0.0f, orbity, orbitz));
      lightProj = -lightProj;

      projection = glm::perspective(glm::radians(120.0f), _width / _height,
                                    0.1f, 150.0f);
      model = glm::rotate(
          model, glm::radians(-55.0f),
          glm::vec3(0.5f + float(i), 1.0f + (float(i) * 500.0f), 0.0f));
      model = glm::translate(model,
                             glm::vec3(0.5f, 1.0f - (float(i) * 1.0f), 0.0f));

      glm::vec3 model_pos_temp{0.5f, 1.0f - float(i), 0.0f};
      sh.setInt(aLightObject.c_str(), 0);
      sh.setMat4(aModel, model);
      sh.setMat4(aView, view);
      sh.setMat4(aProjection, projection);

      sh.setFloat(aAlpha, alphaValue);
      sh.setFloat(aBlend, input.BlendValue);
      sh.setVec3(aLightSource, lightProj);
      glm::vec3 c_pos = c.getPosition();
      sh.setVec3(aViewPos, c_pos);
      sh.setVec3(aObjectPos, model_pos_temp);

      /// Scenelight drawin
      view = c.GetViewMatrix(F, U, P);
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
    }

    view = c.GetViewMatrix(F, U, P);

    projection =
        glm::perspective(glm::radians(120.0f), _width / _height, 0.1f, 150.0f);
    model = glm::rotate(model, glm::radians(-55.0f),
                        glm::vec3(0.5f, 1.0f + (500.0f), 0.0f));
    model = glm::translate(model, glm::vec3(0.5f, 1.0f, 0.0f));

    //
    // trans = glm::translate(trans, glm::vec3(0.1f, -0.1f, 0.0f));
    // trans = glm::rotate(trans, timeValue, glm::vec3(0.0f, 0.0f, 1.0f));
    // sh.setMat4(aTransform, trans);
    sh.setInt(aLightObject.c_str(), 1);
    sh.setMat4(aModel, modelLight);
    sh.setMat4(aView, view);
    sh.setMat4(aProjection, projection);

    sh.setFloat(aAlpha, alphaValue);
    sh.setFloat(aBlend, input.BlendValue);

    glBindVertexArray(VAOLight);
    // glDrawElement(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    ///////////////////////////////////////////////////////////////////
    ///
    ///
    ///
    ///
    csh.use();

    float timeValuee = glfwGetTime() * 0.001f;
    float alphaValuee = sin(timeValue);
    timeValue = 0.005f;

    float orbityy = lighty * cos(timeValue) - lightz * sin(timeValue);
    float orbitzz = lighty * sin(timeValue) + lightz * cos(timeValue);
    lightyy = orbityy;
    lightzz = orbitzz;

    view = c.GetViewMatrix(F, U, P);
    projection =
        glm::perspective(glm::radians(120.0f), _width / _height, 0.1f, 150.0f);
    model = glm::rotate(model, glm::radians(-55.0f),
                        glm::vec3(0.5f, 1.0f + (500.0f), 0.0f));
    model = glm::translate(model, glm::vec3(0.5f, 1.0f, 0.0f));

    csh.setMat4(aModel, model);
    csh.setMat4(aView, view);
    csh.setMat4(aProjection, projection);
    glm::vec3 lightloc = glm::vec3(lightxx, lightyy, lightzz);
    csh.setVec3(aLightLoc, lightloc);
    /////////////////////////////////////////////////////////////////////////////////////////////////
    phongshaderbase.use();

    glBindVertexArray(amb.VAO);
    auto &transform = ambulance.getComponent<Components::Transform>();
    glm::mat4 ambmodel = glm::mat4(1.0f);
    ambmodel = glm::translate(ambmodel, transform.Position);
    ambmodel *= glm::mat4_cast(glm::quat(glm::radians(transform.Rotation)));
    ambmodel = glm::scale(ambmodel, transform.Scale);

    phongshaderbase.setInt("lightCount", 2);
    phongshaderbase.setMat4(aModel, ambmodel);
    phongshaderbase.setMat4(aView, view);
    phongshaderbase.setMat4(aProjection, projection);
    phongshaderbase.setVec3(aLightLoc, static_light);

    for (auto &sub : amb.submeshes) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, sub.tex.BindingID);
      phongshaderbase.setInt("tex", 0);

      phongshaderbase.setVec3(aAmbient, sub.mat.ambient);
      phongshaderbase.setVec3(aDiffuse, sub.mat.diffuse);
      phongshaderbase.setVec3(aSpecular, sub.mat.specular);
      phongshaderbase.setFloat(aShine, sub.mat.shininess);

      glDrawElements(
          GL_TRIANGLES, static_cast<GLsizei>(sub.indexCount), GL_UNSIGNED_INT,
          reinterpret_cast<void *>(sub.indexOffset * sizeof(unsigned int)));
    }

    glBindVertexArray(0);
    ////////////////////////////////////////////////////////////////////////////////////////////

    csh.use();
    glBindVertexArray(Cube.VAO);
    auto &ttransform = PrimitiveCube.getComponent<Components::Transform>();
    glm::mat4 cubemodel = glm::mat4(1.0f);
    cubemodel = glm::translate(cubemodel, ttransform.Position);
    cubemodel *= glm::mat4_cast(glm::quat(glm::radians(ttransform.Rotation)));
    cubemodel = glm::scale(cubemodel, ttransform.Scale);

    csh.setMat4(aModel, cubemodel);
    csh.setMat4(aView, view);
    csh.setMat4(aProjection, projection);
    csh.setVec3(aLightLoc, lightloc);
    glm::vec3 amb = glm::vec3(0.10f, 0.02f, 0.02f);
    glm::vec3 diff = glm::vec3(0.80f, 0.15f, 0.10f);
    glm::vec3 spec = glm::vec3(1.00f, 1.00f, 1.00f);
    csh.setVec3(aAmbient, amb);
    csh.setVec3(aDiffuse, diff);
    csh.setVec3(aSpecular, spec);
    csh.setFloat(aShine, 32.0f);
    glDrawArrays(GL_TRIANGLES, 0, Cube.verts_normals.size() / 6);

    ///////////////////////////////////////////////////////////////////////////////////////////

    glBindVertexArray(0);
    //////////////////////////////////////////////////////////////////////////////////////////////////
    phongshaderbase.use();
    glBindVertexArray(drag.VAO);
    auto &tttransform = dragon.getComponent<Components::Transform>();
    glm::mat4 dragonmodel = glm::mat4(1.0f);
    dragonmodel = glm::translate(dragonmodel, tttransform.Position);
    dragonmodel *=
        glm::mat4_cast(glm::quat(glm::radians(tttransform.Rotation)));
    dragonmodel = glm::scale(dragonmodel, tttransform.Scale);

    phongshaderbase.setMat4(aModel, dragonmodel);
    phongshaderbase.setMat4(aView, view);
    phongshaderbase.setMat4(aProjection, projection);
    phongshaderbase.setVec3(aLightLoc, static_light);

    for (auto &sub : drag.submeshes) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, sub.tex.BindingID);
      phongshaderbase.setInt("tex", 0);
      phongshaderbase.setVec3(aAmbient, sub.mat.ambient);
      phongshaderbase.setVec3(aDiffuse, sub.mat.diffuse);
      phongshaderbase.setVec3(aSpecular, sub.mat.specular);
      phongshaderbase.setFloat(aShine, sub.mat.shininess);

      glDrawElements(
          GL_TRIANGLES, static_cast<GLsizei>(sub.indexCount), GL_UNSIGNED_INT,
          reinterpret_cast<void *>(sub.indexOffset * sizeof(unsigned int)));
    }
    glBindVertexArray(0);
    /////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////////////////
    phongshaderbase.use();
    glBindVertexArray(FuturisticGirl.VAO);
    auto &girltransform = Girl.getComponent<Components::Transform>();
    glm::mat4 girlmodel = glm::mat4(1.0f);
    girlmodel = glm::translate(girlmodel, girltransform.Position);
    girlmodel *=
        glm::mat4_cast(glm::quat(glm::radians(girltransform.Rotation)));
    girlmodel = glm::scale(girlmodel, girltransform.Scale);

    phongshaderbase.setInt("lightCount", 2);
    phongshaderbase.setMat4(aModel, girlmodel);
    phongshaderbase.setMat4(aView, view);
    phongshaderbase.setMat4(aProjection, projection);
    phongshaderbase.setVec3(aLightLoc, static_light);

    for (auto &sub : FuturisticGirl.submeshes) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, sub.tex.BindingID);
      phongshaderbase.setInt("tex", 0);
      phongshaderbase.setVec3(aAmbient, sub.mat.ambient);
      phongshaderbase.setVec3(aDiffuse, sub.mat.diffuse);
      phongshaderbase.setVec3(aSpecular, sub.mat.specular);
      phongshaderbase.setFloat(aShine, sub.mat.shininess);

      glDrawElements(
          GL_TRIANGLES, static_cast<GLsizei>(sub.indexCount), GL_UNSIGNED_INT,
          reinterpret_cast<void *>(sub.indexOffset * sizeof(unsigned int)));
    }

    glBindVertexArray(0);
    ///////////////light rendering///////////////////////////////////
    csh.use();
    glBindVertexArray(LightSource_M.VAO);
    auto &Lc1_transform = LightSource_1.getComponent<Components::Transform>();
    glm::mat4 light_model_1 = glm::mat4(1.0f);
    light_model_1 = glm::translate(light_model_1, Lc1_transform.Position);
    light_model_1 *=
        glm::mat4_cast(glm::quat(glm::radians(Lc1_transform.Rotation)));
    light_model_1 = glm::scale(light_model_1, Lc1_transform.Scale);

    csh.setMat4(aModel, light_model_1);
    csh.setMat4(aView, view);
    csh.setMat4(aProjection, projection);
    csh.setVec3(aLightLoc, lightloc);
    glm::vec3 lcamb_1 = glm::vec3(0.10f, 0.02f, 0.02f);
    glm::vec3 lcdiff_1 = glm::vec3(0.80f, 0.15f, 0.10f);
    glm::vec3 lcspec_1 = glm::vec3(1.00f, 1.00f, 1.00f);
    csh.setVec3(aAmbient, lcamb_1);
    csh.setVec3(aDiffuse, lcdiff_1);
    csh.setVec3(aSpecular, lcspec_1);
    csh.setFloat(aShine, 50.0f);
    glDrawArrays(GL_TRIANGLES, 0, Cube.verts_normals.size() / 6);

    glBindVertexArray(0);

    csh.use();
    ///////////////light rendering///////////////////////////////////
    glBindVertexArray(LightSource_M_2.VAO);
    auto &Lc2_transform = LightSource_2.getComponent<Components::Transform>();
    glm::mat4 Lc2_model = glm::mat4(1.0f);
    Lc2_model = glm::translate(Lc2_model, Lc2_transform.Position);
    Lc2_model *=
        glm::mat4_cast(glm::quat(glm::radians(Lc2_transform.Rotation)));
    Lc2_model = glm::scale(Lc2_model, Lc2_transform.Scale);

    csh.setMat4(aModel, Lc2_model);
    csh.setMat4(aView, view);
    csh.setMat4(aProjection, projection);
    csh.setVec3(aLightLoc, lightloc);
    glm::vec3 lc2amb = glm::vec3(0.10f, 0.02f, 0.02f);
    glm::vec3 lc2diff = glm::vec3(0.80f, 0.15f, 0.10f);
    glm::vec3 lc2spec = glm::vec3(1.00f, 1.00f, 1.00f);
    csh.setVec3(aAmbient, lc2amb);
    csh.setVec3(aDiffuse, lc2diff);
    csh.setVec3(aSpecular, lc2spec);
    csh.setFloat(aShine, 100.0f);
    glDrawArrays(GL_TRIANGLES, 0, Cube.verts_normals.size() / 6);
    //////////////////////////////////////////////////////////////////////////////////////////////////
    glDepthMask(GL_FALSE);
    skyboxSh.use();

    glm::mat4 SkyView = glm::mat4(glm::mat3(view));
    skyboxSh.setMat4(aProjection, projection);
    skyboxSh.setMat4(aView, SkyView);

    glBindVertexArray(VAOSkybox);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(glGetUniformLocation(skyboxSh.ID, "skybox"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  // glDeleteBuffers(1, &EBO);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
