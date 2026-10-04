#pragma once
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// clang-format on
#include "MeshData.hpp"

namespace Components {
enum LightType { Point, Ortho, Projection };

class Components {
public:
  virtual ~Components() = default;
};
class Transform : public Components {
public:
  glm::vec3 Position;
  glm::vec3 Rotation;
  glm::vec3 Scale;
  Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 sc) {
    Position = std::move(pos);
    Rotation = std::move(rot);
    Scale = std::move(sc);
  }
};

class Mesh : public Components {

public:
  MeshData &mesh;
  Mesh(MeshData &m) : mesh(m) {};
};

class Light : public Components {
public:
  bool CastShadows;
  LightType Type;
  float Intensity;
  glm::vec3 Color;
  Light(bool should_cast, LightType _type, float _intensity,
        glm::vec3 light_color)
      : CastShadows(should_cast), Type(_type), Intensity(_intensity),
        Color(light_color) {

        };
};

class CubeMap : public Components {
public:
  GLuint VAO, VBO;
  GLuint TextureID;
  CubeMap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); ++i) {

      unsigned char *data =
          stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
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

    /////////////////////////////////////////////////bindings
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

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertSky.size() * sizeof(float), vertSky.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);

glEnableVertexAttribArray(0);
    // clang-format on
  };
};
} // namespace Components
