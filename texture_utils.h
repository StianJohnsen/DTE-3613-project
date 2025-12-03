#ifndef TEXTURE_UTILS_H
#define TEXTURE_UTILS_H
#include <GL/glew.h>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <string>

namespace texutils {
    // inline GLuint loadCubemap(const std::vector<std::string>& faces) {
    //     GLuint textureID;
    //     glGenTextures(1, &textureID);
    //     glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    //     int width, height, nrChannels;
    //     for (unsigned int i = 0; i < faces.size(); i++) {
    //         unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    //         if (data) {
    //             glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
    //                          width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //             stbi_image_free(data);
    //         } else {
    //             std::cerr << "Failed to load cubemap face: " << faces[i] << std::endl;
    //             stbi_image_free(data);
    //         }
    //     }

    //     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //     return textureID;
    // }

    inline GLuint loadCubemap(const std::vector<std::string>& faces) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(false); // important for cubemaps!

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
                    format = GL_RGB; // fallback

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

    // inline GLuint loadTexture(const std::string& path)
    // {
    //     // --- 4×4 RGB debug pattern (48 bytes) ---
    //     static const unsigned char data[4 * 4 * 3] =
    //         {
    //             // Row 1
    //             255,0,0,    0,255,0,    0,0,255,    255,255,0,
    //             // Row 2
    //             0,255,255,  255,255,255,  255,0,255,  0,0,0,
    //             // Row 3
    //             255,0,0,    0,255,0,    0,0,255,    255,255,0,
    //             // Row 4
    //             0,255,255,  255,255,255,  255,0,255,  0,0,0
    //         };

    //     const int width  = 4;
    //     const int height = 4;

    //     GLuint texID;
    //     glGenTextures(1, &texID);
    //     glBindTexture(GL_TEXTURE_2D, texID);

    //     // Upload the debug texture
    //     glTexImage2D(GL_TEXTURE_2D,
    //                  0,
    //                  GL_RGB,          // internal format
    //                  width,
    //                  height,
    //                  0,
    //                  GL_RGB,          // data format
    //                  GL_UNSIGNED_BYTE,
    //                  data);

    //     // No mipmaps: disable them to avoid blurring
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //     // Clamp so tiles do NOT repeat (to clearly detect UV issues)
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //     return texID;
    // }



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
