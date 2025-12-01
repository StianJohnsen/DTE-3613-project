#include "processShader.h"


std::string readShaderFile(const std::filesystem::path& filepath){
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file) throw std::runtime_error("er file: " + filepath.string());

    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

GLuint loadShader(const std::filesystem::path& path, GLenum shaderType){
    std::string code = readShaderFile(path);
    const char* codeCStr = code.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &codeCStr, nullptr);
    glCompileShader(shader);

    // Error checking
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        throw std::runtime_error("Shader compilation error (" + path.string() + "):\n" + infoLog);
    }


    return shader;
}


GLuint createShaderProgram(
    const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath
    ){
    GLuint vertexShader = loadShader(vertexPath,GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragmentPath, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();

    glAttachShader(program,vertexShader);
    glAttachShader(program,fragmentShader);
    glLinkProgram(program);

    GLint success;

    glGetProgramiv(program,GL_LINK_STATUS, &success);

    if(!success){
        char infoLog[1024];

        glGetProgramInfoLog(program,1024,nullptr,infoLog);
        throw std::runtime_error("Shader linking error:\n" + std::string(infoLog));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}



GLuint loadTexture(const std::filesystem::path& path) {
    GLuint textureID = 0;
    glGenTextures(1, &textureID);

    int width, height, channels;
    unsigned char* image = SOIL_load_image(path.string().c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);

    if (image) {
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(image);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}








