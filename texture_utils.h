#ifndef TEXTURE_UTILS_H
#define TEXTURE_UTILS_H
#include <GL/glew.h>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <string>

namespace texutils {

    inline GLuint loadCubemap(const std::vector<std::string>& faces) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(false);

        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                GLenum format;
                if (nrChannels == 1)
                    format = GL_RED;
                else if (nrChannels == 3)
                    format = GL_RGB;
                else if (nrChannels == 4)
                    format = GL_RGBA;
                else
                    format = GL_RGB;

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

                stbi_image_free(data);
            } else {
                std::cerr << "Failed to load cubemap face: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }


    inline GLuint loadTexture(const std::string& path){
        GLuint texID;
        glGenTextures(1,&texID);

        int width, height, channels;

        unsigned char* data = stbi_load(path.c_str(),&width,&height,&channels,0);

        if(!data){
            std::cerr << "Failed to load texture: " << path << "\n";
        }

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D,texID);
        glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,
                     format,GL_UNSIGNED_BYTE,data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        return texID;

    }
}

#endif // TEXTURE_UTILS_H
