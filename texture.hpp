#pragma once
// clang-format off
#include <filesystem>
#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
// glfw
// clang-format on

#include <iostream>
#include <string>
class Texture {
public:
  GLuint textureID;
};
class textureManager {
public:
  std::unordered_map<std::string, Texture> map;

  textureManager(const char *path) {

    stbi_set_flip_vertically_on_load(true);
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
      unsigned int textureID;
      glGenTextures(1, &textureID);
      glBindTexture(GL_TEXTURE_2D, textureID);
      int width, height, nrChannels;
      std::string ppath = entry.path().string();
      unsigned char *data =
          stbi_load(ppath.c_str(), &width, &height, &nrChannels, 0);
      unsigned int channel;
      switch (nrChannels) {
      case 1:
        channel = GL_RED;
        break;
      case 2:
        channel = GL_RG;
        break;
      case 3:
        channel = GL_RGB;
        break;
      case 4:
        channel = GL_RGBA;
        break;
      }

      if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, channel, width, height, 0, channel,
                     GL_UNSIGNED_BYTE, data);
        std::string key = entry.path().parent_path().filename().string() +
                          "\\" + entry.path().filename().string();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        Texture tex;
        tex.textureID = textureID;

        map.insert({key, tex});

      } else {
        std::cout << "Failed to load texture path:" << entry.path()
                  << std::endl;

        exit(1);
      }
    }
  }
  void loadMore(const char *path);
};
