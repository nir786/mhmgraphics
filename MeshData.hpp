#pragma once
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
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "BFRenderData.hpp"
#include "texture.hpp"
#include <string>
// clang-format on
//
//
//

class SubMeshData {
public:
  std::string name;
  blackFrame::Scene::Material mat;
  blackFrame::Scene::Tex tex;
  unsigned int indexOffset;
  unsigned int indexCount;
};
class MeshData {
public:
  std::string name;
  std::vector<SubMeshData> submeshes;
  GLuint VAO, VBO, EBO;
  std::vector<float> verts_normals;
  std::vector<unsigned int> ind;
};

namespace Importer {
enum Primitives {
  Triangle,
  Quad,
  Cube,
  Sphere

};

MeshData LoadPrimitive(Primitives p);
MeshData LoadMesh(const char *model_path,
                  std::unordered_map<std::string, Texture> &map);

}; // namespace Importer
