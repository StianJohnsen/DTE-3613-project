#ifndef MESHPIPELINESETUP_H
#define MESHPIPELINESETUP_H
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct VertexAttribute
{
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    size_t offset;
};

struct Mesh{
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<VertexAttribute> attributes;
    GLenum primitiveType = GL_TRIANGLES;

    void setup(){
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glGenBuffers(1,&EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(float), vertices.data(),GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


        for(auto &attr: attributes){
            glEnableVertexAttribArray(attr.index);
            glVertexAttribPointer(attr.index,
                                  attr.size,
                                  attr.type,
                                  attr.normalized,
                                  attr.stride,
                                  (void*)attr.offset);

        }

        glBindVertexArray(0);

    }

    void draw(GLuint shaderProgram) const {
        glBindVertexArray(VAO);
        if(!indices.empty()){
            glDrawElements(primitiveType, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        }
        else{
            glDrawArrays(primitiveType, 0, (GLsizei)(vertices.size() / 3));
        }
        glBindVertexArray(0);
    }

};
#endif // MESHPIPELINESETUP_H
