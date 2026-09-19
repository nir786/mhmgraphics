#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

struct Cluster {
  glm::vec3 Ax, Ay;
};

class Camera {
  glm::vec3 Position;

public:
  Cluster RotationAngles(const float &thetax, const float &thetay);

  void RotationQauaterion(const glm::vec2 &angles, Cluster &rot, glm::vec3 &F,
                          glm::vec3 &U, glm::vec3 &R);
  glm::mat4 GetViewMatrix(glm::vec3 &F, glm::vec3 &U, glm::vec3 &location);
  void SetPosition(glm::vec3 &_pos);
  glm::vec3 getPosition();
};
