
#include "Shader.hpp"
#include <glm/gtc/type_ptr.hpp>

void Shader::use() { glUseProgram(ID); }
void Shader::setBool(std::string &Uniform_name, bool value) const {
  int uniformBoolLocation = glGetUniformLocation(ID, Uniform_name.c_str());
  glUniform1i(uniformBoolLocation, value);
}
void Shader::setInt(const char *Uniform_name, int value) const {
  int uniformIntLocation = glGetUniformLocation(ID, Uniform_name);
  glUniform1i(uniformIntLocation, value);
}
void Shader::setFloat(const char *Uniform_name, float value) const {
  int uniformFloatLocation = glGetUniformLocation(ID, Uniform_name);
  glUniform1f(uniformFloatLocation, value);
}
void Shader::setMat4(std::string &Uniform_name, glm::mat4 &value) const {
  int uniformFloatLocation = glGetUniformLocation(ID, Uniform_name.c_str());
  glUniformMatrix4fv(uniformFloatLocation, 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setVec3(std::string &Uniform_name, glm::vec3 &value) const {
  int uniformFloatLocation = glGetUniformLocation(ID, Uniform_name.c_str());
  glUniform3f(uniformFloatLocation, value.x, value.y, value.z);
}
