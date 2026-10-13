#include "Input.hpp"

void InputManager::movement_early(Camera &c, glm::vec3 &P, glm::vec3 &F,
                                  glm::vec3 &R, glm::vec3 &U) {
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
  }

  if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {

    BlendValue -= 0.01f;
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

    P += F * 0.1f;
    c.SetPosition(P);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

    P -= F * 0.1f;
    c.SetPosition(P);
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

    P += R * 0.1f;
    c.SetPosition(P);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

    P -= R * 0.1f;

    c.SetPosition(P);
  }

  float deltax = float(newx - oldx);
  float deltay = float(newy - oldy);
  deltax *= 0.001f;
  deltay *= 0.001f;

  Cluster rot = c.RotationAngles(deltax, deltay);
  c.RotationQauaterion({deltax, deltay}, rot, F, U, R);
  oldx = newx;
  oldy = newy;
}

void InputManager::exposure_m(float &exp) {

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    exp += 0.01f;
  }
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    exp -= 0.01f;
  }
}

void InputManager::changemode(int &mode) {

  static bool wasPressed = false;
  bool isPressed = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
  if (isPressed && !wasPressed) {
    if (mode == 1) {
      mode = 0;
    } else {
      if (mode == 0) {
        mode = 1;
      }
    }
    wasPressed = isPressed;
  }
}
