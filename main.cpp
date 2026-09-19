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
#include <glm/glm.hpp>
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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// glfw
// clang-format on

#include "Camera.hpp"
#include "Shader.hpp"
#include <cstring>
#include <iostream>
#include <math.h>
#include <vector>
////
///

#define vertShaderSource "/home/nir/cpp_graphics/simpleShade.vert"
#define fragShaderSource "/home/nir/cpp_graphics/simpleShade.frag"

float _width = 500.0f, _height = 500.0f;
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  _width = width;
  _height = height;
  // glm::ortho(0.0f, float(width), 0.0f, float(height), 0.1f, 100.0f);
}
struct Material {
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;
};
struct MeshGl {
  GLuint VAO, VBO, EBO;
  unsigned int EBOSize;
  Material mmat;
};
int main() {

  Assimp::Importer importer;
  const char *path = "Assets/dragon/fbx/Dragon 2.5_fbx.fbx";

  const aiScene *scene = nullptr;
  // clang-format off
  scene = importer.ReadFile(
      path,

      aiProcess_CalcTangentSpace       | 
      aiProcess_Triangulate            |
      aiProcess_JoinIdenticalVertices  | 
      aiProcess_SortByPType            |
      aiProcess_GenNormals             );

  // clang-format on
  if (!scene) {
    std::cout << importer.GetErrorString() << std::endl;
  }

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(500, 500, "testing", NULL, NULL);
  if (window == NULL) {
    std::cerr << "GLFW window wasnt initialized" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  int nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes
            << std::endl;

  // glm::ortho(0.0f, 500.0f, 0.0f, 500.0f, 0.1f, 100.0f);
  // ###### a single object in our program#######
  //

  std::vector<MeshGl> CarMeshes;

  std::vector<float> carv;
  std::vector<unsigned int> cari;

  for (unsigned int t = 0; t < scene->mNumMeshes; ++t) {
    std::vector<float> carv;
    std::vector<unsigned int> cari;

    GLuint VAO, VBO, EBO;

    aiMesh *mesh = scene->mMeshes[t];
    aiFace *faces = mesh->mFaces;
    aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
      aiVector3D &vert3d = mesh->mVertices[i];
      aiVector3D &normal3d = mesh->mNormals[i];

      carv.push_back(vert3d.x);
      carv.push_back(vert3d.y);
      carv.push_back(vert3d.z);

      carv.push_back(normal3d.x);
      carv.push_back(normal3d.y);
      carv.push_back(normal3d.z);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
      aiFace &face = mesh->mFaces[i];
      for (unsigned int c = 0; c < face.mNumIndices; ++c) {
        cari.push_back(face.mIndices[c]);
      }
    }
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, carv.size() * sizeof(float), carv.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));

    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cari.size() * sizeof(unsigned int),
                 cari.data(), GL_STATIC_DRAW);
    MeshGl mmesh;
    mmesh.EBO = EBO;
    mmesh.VAO = VAO;
    mmesh.VBO = VBO;

    aiColor3D ambient;
    aiColor3D diffuse;
    aiColor3D specular;
    float shininess;

    mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    mat->Get(AI_MATKEY_SHININESS, shininess);

    mmesh.mmat =
        Material{glm::vec3{ambient.r, ambient.g, ambient.b},
                 glm::vec3{diffuse.r, diffuse.g, diffuse.b},
                 glm::vec3{specular.r, specular.g, specular.b}, shininess};
    //
    // if (t == 1) {
    //   mmesh.mmat.ambient = glm::vec3{1.0f, 0.0f, 1.0f};
    // }
    //
    // if (t == 2) {
    //   mmesh.mmat.ambient = glm::vec3{0.0f, 1.0f, 0.0f};
    // }
    // if (t == 3) {
    //   mmesh.mmat.ambient = glm::vec3{1.0f, 0.25f, 0.0f};
    // }

    mmesh.EBOSize = cari.size();

    CarMeshes.push_back(mmesh);
    std::cout << EBO << " ,EBO " << VAO << " ,VAO " << VBO << " ,VBO "
              << std::endl;
  }

  GLuint VBO, VAO;
  // GLuint EBO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
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
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertecies.size() * sizeof(float),
               vertecies.data(), GL_STATIC_DRAW);

  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned
  // int),
  //              indices.data(), GL_STATIC_DRAW);

  //
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

  stbi_set_flip_vertically_on_load(true);
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrChannels;
  unsigned char *data = stbi_load("/home/nir/cpp_graphics/container.jpg",
                                  &width, &height, &nrChannels, 0);
  if (data) {

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  unsigned int texture2;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  data = stbi_load("/home/nir/cpp_graphics/awesomeface.png", &width, &height,
                   &nrChannels, 0);
  if (data) {

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  sh.use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture2);

  sh.setInt(aTexture1, 0);
  sh.setInt(aTexture2, 1);

  float BlendValue = 0.5f;

  // glm::mat4 trans = glm::mat4(2.0f);
  // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f,
  // 0.0f, 1.0f)); trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
  //
  // sh.setMat4(aTransform, trans);
  //

  double oldx, oldy;

  glfwGetCursorPos(window, &oldx, &oldy);

  Camera c;
  double newx, newy;

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

  while (!glfwWindowShouldClose(window)) {

    glfwGetCursorPos(window, &newx, &newy);

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {

      if (state == 0) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        state = 1;
      } else if (state == 1) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        state = 0;
      }
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
      BlendValue += 0.01f;
      std::cout << " increasing " << BlendValue << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {

      BlendValue -= 0.01f;

      std::cout << " Decreasing " << BlendValue << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

      P += F * 0.1f;
      c.SetPosition(P);
      std::cout << " increasing " << BlendValue << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

      P -= F * 0.1f;

      c.SetPosition(P);
      std::cout << " Decreasing " << BlendValue << std::endl;
    }

    float deltax = float(newx - oldx);
    float deltay = float(newy - oldy);
    deltax *= 0.001f;
    deltay *= 0.001f;
    //
    // std::cout << "Theta x:=" << deltax << std::endl;
    // std::cout << "Theta y:=" << deltay << std::endl;
    //
    // if (deltay != 0) {
    //   exit(1);
    // }
    Cluster rot = c.RotationAngles(deltax, deltay);
    //
    // std::cout << "rotation:=(" << rot.Ax.x << "," << rot.Ax.y << "," <<
    // rot.Ax.z
    //           << ")" << std::endl;
    //
    // std::cout << "rotation:=(" << rot.Ay.x << "," << rot.Ay.y << "," <<
    // rot.Ay.z
    //           << ")" << std::endl;
    c.RotationQauaterion({deltax, deltay}, rot, F, U, R);
    // std::cout << "F - rotation:=(" << F.x << "," << F.y << "," << F.z << ")"
    //           << std::endl;
    // std::cout << "U - rotation:=(" << U.x << "," << U.y << "," << U.z << ")"
    //           << std::endl;

    oldx = newx;
    oldy = newy;

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

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

      //
      // trans = glm::translate(trans, glm::vec3(0.1f, -0.1f, 0.0f));
      // trans = glm::rotate(trans, timeValue, glm::vec3(0.0f, 0.0f, 1.0f));
      // sh.setMat4(aTransform, trans);
      //

      glm::vec3 model_pos_temp{0.5f, 1.0f - float(i), 0.0f};
      sh.setInt(aLightObject, 0);
      sh.setMat4(aModel, model);
      sh.setMat4(aView, view);
      sh.setMat4(aProjection, projection);

      sh.setFloat(aAlpha, alphaValue);
      sh.setFloat(aBlend, BlendValue);
      sh.setVec3(aLightSource, lightProj);
      glm::vec3 c_pos = c.getPosition();
      sh.setVec3(aViewPos, c_pos);
      sh.setVec3(aObjectPos, model_pos_temp);

      /// Scenelight drawin
      view = c.GetViewMatrix(F, U, P);
      glBindVertexArray(VAO);
      // glDrawElement(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    sh.setInt(aLightObject, 1);
    sh.setMat4(aModel, modelLight);
    sh.setMat4(aView, view);
    sh.setMat4(aProjection, projection);

    sh.setFloat(aAlpha, alphaValue);
    sh.setFloat(aBlend, BlendValue);

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

    for (int i = 0; i < CarMeshes.size(); ++i) {
      csh.setVec3(aAmbient, CarMeshes[i].mmat.ambient);
      csh.setVec3(aDiffuse, CarMeshes[i].mmat.diffuse);
      csh.setVec3(aSpecular, CarMeshes[i].mmat.specular);
      csh.setFloat(aShine, CarMeshes[i].mmat.shininess);

      glBindVertexArray(CarMeshes[i].VAO);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glDrawElements(GL_TRIANGLES, CarMeshes[i].EBOSize, GL_UNSIGNED_INT,
                     nullptr);
    }
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glDrawElements(GL_TRIANGLES, cari.size(), GL_UNSIGNED_INT, nullptr);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  for (int i = 0; i < CarMeshes.size(); i++) {
    glDeleteVertexArrays(1, &CarMeshes[i].VAO);
    glDeleteBuffers(1, &CarMeshes[i].VBO);
  }

  // glDeleteBuffers(1, &EBO);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
