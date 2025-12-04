#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "meshPipelineSetup.h"
#include "material.h"

    struct Renderable {
        Mesh mesh;
        Material material;
        glm::mat4 model{1.0f};
        bool doubleSided = false;

        GLuint textureID = 0;
        bool useTexture = false;

        void draw(Shader& shader) const {

            if(doubleSided)
                glDisable(GL_CULL_FACE);
            else
                glEnable(GL_CULL_FACE);

            shader.setMat4("model", model);
            shader.setBool("useTexture",useTexture);

            shader.setInt("texture_diffuse", 0);
            if(useTexture && textureID != 0){
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureID);
            }

            material.apply(shader);
            mesh.draw(shader.getId());
        }
    };
    #endif // RENDERABLE_H
