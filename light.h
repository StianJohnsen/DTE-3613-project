#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>
#include "shader.h"

// struct Light {
//     glm::vec3 position;
//     glm::vec3 ambient  = {0.3f, 0.3f, 0.3f};
//     glm::vec3 diffuse  = {0.8f, 0.8f, 0.8f};
//     glm::vec3 specular = {1.0f, 1.0f, 1.0f};

//     // Set uniforms on a shader
//     void apply(Shader& shader, const std::string& uniformName = "light") const {
//         shader.setVec3(uniformName + ".position", position);
//         shader.setVec3(uniformName + ".ambient", ambient);
//         shader.setVec3(uniformName + ".diffuse", diffuse);
//         shader.setVec3(uniformName + ".specular", specular);
//     }
// };



struct Light {
    glm::vec3 position;     //{0.0f, 2.0f, 3.0f};
    glm::vec3 ambient{0.2f};
    glm::vec3 diffuse{0.8f};
    glm::vec3 specular{1.0f};

    void apply(Shader& shader, const std::string& uniformName, int index) const {
        // shader.setVec3(uniformName + ".position", position);
        // shader.setVec3(uniformName + ".ambient", ambient);
        // shader.setVec3(uniformName + ".diffuse", diffuse);
        // shader.setVec3(uniformName + ".specular", specular);

        std::string prefix = uniformName + "[" + std::to_string(index) + "]";
        shader.setVec3(prefix + ".position", position);
        shader.setVec3(prefix + ".ambient", ambient);
        shader.setVec3(prefix + ".diffuse", diffuse);
        shader.setVec3(prefix + ".specular", specular);

    }
};
#endif // LIGHT_H
