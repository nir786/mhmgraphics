#include "Camera.hpp"
#include <glm/common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
void Camera::SetPosition(glm::vec3 &_pos) { this->Position = _pos; }
glm::vec3 Camera::getPosition() { return this->Position; }

Cluster Camera::RotationAngles(const float &thetax, const float &thetay) {
  glm::vec3 rotx = glm::vec3(thetax, 0.0f, 0.0f);
  glm::vec3 roty = glm::vec3(0.0f, 0.0f, thetay);
  rotx = glm::sign(rotx);
  roty = glm::sign(roty);
  return {roty, rotx};
}
void Camera::RotationQauaterion(const glm::vec2 &angles, Cluster &rot,
                                glm::vec3 &F, glm::vec3 &U, glm::vec3 &R) {

  glm::quat qx = glm::angleAxis(angles.x, U);
  glm::quat qy = glm::angleAxis(angles.y, R);

  glm::quat q = qx * qy;

  F = q * F;
  U = q * U;
  R = q * R;
}

glm::mat4 Camera::GetViewMatrix(glm::vec3 &F, glm::vec3 &U,
                                glm::vec3 &location) {

  return glm::lookAt(location, location + F, U);
}
