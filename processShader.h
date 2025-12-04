#ifndef PROCESSSHADER_H
#define PROCESSSHADER_H
#include <string>
#include <filesystem>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <filesystem>

#include <map>

#include <SOIL2/SOIL2.h>


GLuint loadShader(const std::filesystem::path& path, GLenum shaderType);

GLuint createShaderProgram(
    const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath
    );


GLuint loadTexture(const std::filesystem::path& path);
#endif // PROCESSSHADER_H
