#include "MeshData.hpp"
#include "Components.hpp"
#include "texture.hpp"
#include <assimp/types.h>
#include <assimp/vector3.h>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <unordered_map>
namespace Importer {
// Triangle lists: each vertex is {x, y, z, nx, ny, nz, u, v}.
// Counterclockwise winding viewed from outside; primitives span [-0.5, 0.5].
// clang-format off
std::vector<float> Triangle_vert = {
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.5f, 1.0f,
};
std::vector<float> Quad_vert = {
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
};
std::vector<float> Cube_vert = {
    // Front
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    // Back
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    // Right
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    // Left
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    // Top
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    // Bottom
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
};
// clang-format on

// Generate once: 32 sectors, 16 stacks, radius 0.5, smooth unit normals.
std::vector<float> Sphere_vert = [] {
  constexpr int sectors = 32;
  constexpr int stacks = 16;
  constexpr float radius = 0.5f;
  std::vector<float> vertices;
  vertices.reserve(sectors * (stacks - 1) * 6 * 8);

  const auto normalAt = [](int stack, int sector) -> glm::vec3 {
    if (stack == 0)
      return {0.0f, 1.0f, 0.0f};
    if (stack == stacks)
      return {0.0f, -1.0f, 0.0f};
    const float theta = glm::pi<float>() * stack / stacks;
    // Wrap the last sector so the seam matches exactly.
    const float phi = glm::two_pi<float>() * (sector % sectors) / sectors;
    return {glm::sin(theta) * glm::cos(phi), glm::cos(theta),
            glm::sin(theta) * glm::sin(phi)};
  };
  const auto append = [&](const glm::vec3 &normal, int stack, int sector) {
    // Keep sector == sectors at u == 1 even though its position wraps to 0.
    const float u = static_cast<float>(sector) / sectors;
    const float v = 1.0f - static_cast<float>(stack) / stacks;
    vertices.insert(vertices.end(),
                    {radius * normal.x, radius * normal.y, radius * normal.z,
                     normal.x, normal.y, normal.z, u, v});
  };

  for (int stack = 0; stack < stacks; ++stack) {
    for (int sector = 0; sector < sectors; ++sector) {
      const glm::vec3 a = normalAt(stack, sector);
      const glm::vec3 b = normalAt(stack + 1, sector);
      const glm::vec3 c = normalAt(stack + 1, sector + 1);
      const glm::vec3 d = normalAt(stack, sector + 1);
      // One triangle per sector at each pole avoids degenerate triangles.
      if (stack != 0) {
        append(a, stack, sector);
        append(d, stack, sector + 1);
        append(b, stack + 1, sector);
      }
      if (stack != stacks - 1) {
        append(d, stack, sector + 1);
        append(c, stack + 1, sector + 1);
        append(b, stack + 1, sector);
      }
    }
  }
  return vertices;
}();

Assimp::Importer importer;

MeshData LoadPrimitive(Primitives p) {
  MeshData m;

  switch (p) {
  case Triangle:
    m.verts_normals = Triangle_vert;
    break;
  case Quad:
    m.verts_normals = Quad_vert;
    break;
  case Cube:
    m.verts_normals = Cube_vert;
    break;
  case Sphere:
    m.verts_normals = Sphere_vert;
    break;
  }

  glGenVertexArrays(1, &m.VAO);
  std::cout << m.VAO << "bullshit" << std::endl;

  glBindVertexArray(m.VAO);
  glGenBuffers(1, &m.VBO);

  glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
  glBufferData(GL_ARRAY_BUFFER, m.verts_normals.size() * sizeof(float),
               m.verts_normals.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));

  glEnableVertexAttribArray(2);

  return m;
}

MeshData LoadMesh(const char *model_path,
                  std::unordered_map<std::string, Texture> &map) {
  MeshData m;

  const aiScene *scene = nullptr;
  // clang-format off
  scene = importer.ReadFile(
      model_path,

      aiProcess_CalcTangentSpace       | 
      aiProcess_Triangulate            |
      aiProcess_JoinIdenticalVertices  | 
      aiProcess_SortByPType            |
      aiProcess_GenNormals             );

  // clang-format on
  if (!scene) {
    std::cout << importer.GetErrorString() << std::endl;
    exit(1);
  }

  unsigned int ind_offset = 0;
  for (unsigned int t = 0; t < scene->mNumMeshes; ++t) {

    SubMeshData submesh;

    aiMesh *mesh = scene->mMeshes[t];
    aiFace *faces = mesh->mFaces;
    aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
    aiString path;
    if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
      const aiTexture *text = scene->GetEmbeddedTexture((path.C_Str()));
      if (text) {
        if (text->mHeight == 0) {
          GLuint TextureID;
          glGenTextures(1, &TextureID);
          glBindTexture(GL_TEXTURE_2D, TextureID);

          int width, height, nrChannels;

          unsigned char *data = stbi_load_from_memory(
              reinterpret_cast<const stbi_uc *>(text->pcData),
              static_cast<int>(text->mWidth), &width, &height, &nrChannels, 0);

          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

          stbi_image_free(data);
          submesh.tex.BindingID = TextureID;
          submesh.tex.name = path.C_Str();
          std::cout << "used imbeded texture" << std::endl;
        }
      } else {
        if (map.contains(path.C_Str())) {
          std::cout << "hit on " << path.C_Str() << std::endl;
          submesh.tex.BindingID = map[path.C_Str()].textureID;
          submesh.tex.name = path.C_Str();
        } else {
          std::cout << "couldnt find texture " << path.C_Str() << std::endl;
        }
      }
    }
    submesh.indexOffset = m.ind.size();
    submesh.indexCount =
        mesh->mNumFaces * 3; // numof faces always 3 cause triangulate

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
      aiVector3D &vert3d = mesh->mVertices[i];
      aiVector3D &normal3d = mesh->mNormals[i];
      aiVector3D &uv3d = mesh->mTextureCoords[0][i];

      m.verts_normals.push_back(vert3d.x);
      m.verts_normals.push_back(vert3d.y);
      m.verts_normals.push_back(vert3d.z);

      m.verts_normals.push_back(normal3d.x);
      m.verts_normals.push_back(normal3d.y);
      m.verts_normals.push_back(normal3d.z);
      m.verts_normals.push_back(uv3d.x);
      m.verts_normals.push_back(uv3d.y);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
      aiFace &face = mesh->mFaces[i];
      for (unsigned int c = 0; c < face.mNumIndices; ++c) {
        m.ind.push_back(face.mIndices[c] + ind_offset);
      }
    }

    aiColor3D ambient;
    aiColor3D diffuse;
    aiColor3D specular;
    float shininess;

    mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    mat->Get(AI_MATKEY_SHININESS, shininess);

    submesh.mat = blackFrame::Scene::Material{
        glm::vec3{ambient.r, ambient.g, ambient.b},
        glm::vec3{diffuse.r, diffuse.g, diffuse.b},
        glm::vec3{specular.r, specular.g, specular.b}, shininess};

    m.submeshes.push_back(submesh);

    std::cout << t << std::endl;
    ind_offset += mesh->mNumVertices;
  }

  glGenVertexArrays(1, &m.VAO);
  glBindVertexArray(m.VAO);
  glGenBuffers(1, &m.VBO);

  glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
  glBufferData(GL_ARRAY_BUFFER, m.verts_normals.size() * sizeof(float),
               m.verts_normals.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));

  glEnableVertexAttribArray(2);

  glGenBuffers(1, &m.EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.ind.size() * sizeof(unsigned int),
               m.ind.data(), GL_STATIC_DRAW);

  return m;
}

}; // namespace Importer
