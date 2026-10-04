#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

#include "Shader.hpp"
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace blackFrame::Scene {
using MaterialID = uint32_t;
using TextureID = uint32_t;
using BucketKey = std::pair<MaterialID, TextureID>;

struct BucketHashKey {
  std::size_t operator()(const BucketKey &key) const noexcept {
    std::size_t f = std::hash<MaterialID>{}(key.first);
    std::size_t s = std::hash<TextureID>{}(key.second);
    return f & (s << 1);
  }
};

struct DrawPacket {

  uint32_t TextureID;
  uint32_t MaterialID;
  std::vector<float> renderData;
  std::vector<unsigned int> objects;
  bool dirty;
  // the texture and the material will be perfect match index from the buffer
  // aka well have to change them everytime wedelete something
};

class RenderData {
  std::unordered_map<TextureID, int> textureMapping;
  std::unordered_map<MaterialID, int> materialMapping;
  std::unordered_map<BucketKey, DrawPacket, BucketHashKey> Renderables;
};
struct Material {

  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;

  unsigned int ShaderID;
  unsigned int materialID;
};

struct Tex {
  // unsigned int ShaderID;
  // unsigned int TextureID;
  GLuint BindingID;
  std::string name;
};

} // namespace blackFrame::Scene
