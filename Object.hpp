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

#include "Components.hpp"
#include <memory>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>

class Object {

public:
  std::string name;

  Object(std::string _name) : name(_name) {};
  // std::vector<Components::Components> comps;
  std::unordered_map<std::type_index, std::unique_ptr<Components::Components>>
      comps;

  template <std::derived_from<std::remove_cvref_t<Components::Components>> T,
            typename... Args>
  void addComponenet(Args &&...args) {
    auto obj = std::make_unique<T>(std::forward<Args>(args)...);
    comps.emplace(typeid(T), std::move(obj));
  }

  template <std::derived_from<std::remove_cvref_t<Components::Components>> T>
  T &getComponent() {
    return static_cast<T &>(*comps.at(typeid(T)));
  }
};
