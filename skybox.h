#ifndef SKYBOX_H
#define SKYBOX_H

#include "shader.h"
#include <string>
#include <vector>
#include <GL/glew.h>
#include <stb_image.h>
#include "processVertices.h"

#include "meshPipelineSetup.h"
#include "shader.h"
#include "texture_utils.h"
#include "meshCreation.h"
#include "shader.h"
#include "texture_utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Skybox {
public:
    Mesh mesh;
    GLuint cubemapTexture = 0;

    bool load(const std::vector<std::string>& faces) {
        cubemapTexture = texutils::loadCubemap(faces);
        if (!cubemapTexture) {
            std::cerr << "Failed to load cubemap texture" << std::endl;
            return false;
        }

        mesh = createSkybox();
        return true;
    }

    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
        glDepthFunc(GL_LEQUAL);
        shader.use();

        // remove translation from the view matrix
        glm::mat4 viewNoTrans = glm::mat4(glm::mat3(view));
        shader.setMat4("view", viewNoTrans);
        shader.setMat4("projection", projection);

        glBindVertexArray(mesh.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        shader.setInt("skybox", 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
    }
};


#endif // SKYBOX_H
