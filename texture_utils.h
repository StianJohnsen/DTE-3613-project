#ifndef TEXTURE_UTILS_H
#define TEXTURE_UTILS_H
#include <GL/glew.h>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <string>

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



#endif // TEXTURE_UTILS_H
