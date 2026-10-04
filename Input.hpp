#pragma once
#include "Camera.hpp"
// clang-format off

#include <GLFW/glfw3.h>

//clang-format on
class InputManager {
	public : 
  GLFWwindow *window;
  double newx, newy , oldx , oldy;
  int state ; 
  float BlendValue;


  InputManager(GLFWwindow *window_) : window(window_) {
  glfwGetCursorPos(window, &oldx, &oldy);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  };
  void movement_early(Camera &c, glm::vec3 &P, glm::vec3 &F, glm::vec3 &R,glm::vec3 &U );
};
