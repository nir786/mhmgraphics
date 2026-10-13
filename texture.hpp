#pragma once
// clang-format off

#include <string_view>
#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "stb_image.h"
#include <string>


using Color = std::array<unsigned char , 4>;
constexpr Color BG_TEX_DEFAULT {255 ,0   ,255 ,255}; 
constexpr Color BG_TEX_WHITE   {255 ,255 ,255 ,255}; 
constexpr Color BG_TEX_GRAY    {128 ,128 ,128 ,255}; 
constexpr Color BG_TEX_BLACK   {0   ,0   ,0   ,255}; 
constexpr Color BG_TEX_MAGENTA {255 ,0   ,255 ,255}; 
constexpr Color BG_TEX_RED     {255 ,0   ,0   ,255}; 
#define BF_WHITE   "white" 
#define BF_GRAY    "gray" 
#define BF_BLACK   "black" 
#define BF_MAGENTA "magenta" 
#define BF_RED     "red"
#define BF_DEFAULT "defualt"

#define Picker (color , name) 









// glfw
// clang-format on

class Texture {
public:
  GLuint textureID;
  std::string keyName;
};

class textureManager {
public:
  textureManager() {
    LoadDefault(BG_TEX_DEFAULT, BF_DEFAULT);
    LoadDefault(BG_TEX_WHITE, BF_WHITE);
    LoadDefault(BG_TEX_BLACK, BF_BLACK);
    LoadDefault(BG_TEX_GRAY, BF_GRAY);
    LoadDefault(BG_TEX_MAGENTA, BF_MAGENTA);
    LoadDefault(BG_TEX_RED, BF_RED);
  }

  std::unordered_map<std::string, Texture> map;
  void LoadDefault(const Color &color, const char *name);

  const Texture &getTexture(std::string_view key) {
    auto it = map.find(std::string(key));
    if (it != map.end()) {
      return it->second;

    } else {
      auto it = map.find(BF_DEFAULT);
      return it->second;
    }
  }
};
