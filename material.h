#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm/glm.hpp>
#include "shader.h"

struct Material {
    glm::vec3 ambient  = {1.0f, 1.0f, 1.0f};
    glm::vec3 diffuse  = {1.0f, 1.0f, 1.0f};
    glm::vec3 specular = {0.5f, 0.5f, 0.5f};
    float shininess    = 32.0f;

    // Set uniforms on a shader
    void apply(Shader& shader, const std::string& uniformName = "material") const {
        shader.setVec3(uniformName + ".ambient", ambient);
        shader.setVec3(uniformName + ".diffuse", diffuse);
        shader.setVec3(uniformName + ".specular", specular);
        shader.setFloat(uniformName + ".shininess", shininess);
    }
};
#endif // MATERIAL_H
