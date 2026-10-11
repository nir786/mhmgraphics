#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <string>

class Shader {

public:
  unsigned int ID;

  Shader(const char *path_Frag, const char *path_Vert) {

    std::ifstream frag(path_Frag);
    std::ifstream vert(path_Vert);

    if (!frag.is_open()) {
      std::cerr << "couldnt open file at " << path_Frag << std::endl;
    } else if (!vert.is_open()) {
      std::cerr << "couldnt open file at " << path_Vert << std::endl;
    }

    std::stringstream buffer_vert;
    std::stringstream buffer_frag;

    buffer_vert << vert.rdbuf();
    buffer_frag << frag.rdbuf();

    std::string vert_contence = buffer_vert.str();
    std::string frag_contence = buffer_frag.str();
    const char *vert_const = vert_contence.c_str();
    const char *frag_const = frag_contence.c_str();

    unsigned int sfrag, svert;

    sfrag = glCreateShader(GL_FRAGMENT_SHADER);
    svert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(svert, 1, &vert_const, NULL);
    glShaderSource(sfrag, 1, &frag_const, NULL);
    glCompileShader(svert);
    glCompileShader(sfrag);

    char infoLog[512]{};
    int status;

    glGetShaderiv(svert, GL_COMPILE_STATUS, &status);
    if (!status) {
      glGetShaderInfoLog(svert, 512, NULL, infoLog);
      /// throw runtime error
      std::cerr << infoLog << std::endl;
    }

    std::cout << "fdsfsfs" << std::endl;
    glGetShaderiv(sfrag, GL_COMPILE_STATUS, &status);
    if (!status) {
      glGetShaderInfoLog(sfrag, 512, NULL, infoLog);
      /// throw runtime error
      std::cerr << infoLog << std::endl;
    }

    ID = glCreateProgram();

    std::cout << ID << " ID " << svert << " <-svert " << sfrag << " <-sfrag"
              << std::endl;
    std::cout << "fdsfsfs" << std::endl;
    glAttachShader(ID, svert);
    glAttachShader(ID, sfrag);

    std::cout << "fdsfsfs" << std::endl;

    std::cout << reinterpret_cast<void *>(glLinkProgram) << std::endl;

    glLinkProgram(ID);
    std::cout << "hello";

    glGetProgramiv(ID, GL_LINK_STATUS, &status);
    if (!status) {
      glGetProgramInfoLog(ID, 512, NULL, infoLog);
      /// throw runtime error
      std::cerr << infoLog << std::endl;
    }

    glDeleteShader(svert);
    glDeleteShader(sfrag);
  }

  ~Shader() { glDeleteProgram(ID); }

  void use();
  void setBool(std::string &Uniform_name, bool value) const;
  void setInt(const char *Uniform_name, int value) const;
  void setFloat(const char *Uniform_name, float value) const;
  void setMat4(std::string &Uniform_name, glm::mat4 &value) const;
  void setVec3(std::string &Uniform_name, glm::vec3 &value) const;
};
