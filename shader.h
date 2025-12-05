#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "processShader.h"



class Shader {
public:
    Shader(const std::string& vertPath, const std::string& fragPath) {
        id = createShaderProgram(vertPath, fragPath);
        if(id == 0) {
            throw std::runtime_error("Shader creation failed for: " + vertPath + " / " + fragPath);
        }
    }

    ~Shader() {
        if (id != 0) glDeleteProgram(id);
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& o) noexcept : id(o.id), uniformCache(std::move(o.uniformCache)) { o.id = 0; }
    Shader& operator=(Shader&& o) noexcept { if(this!=&o){ if(id) glDeleteProgram(id); id=o.id; id=0; uniformCache=std::move(o.uniformCache);} return *this; }

    void use() const { glUseProgram(id); }

    void setBool(const std::string& name, bool value) const {
        auto loc = getLocation(name); if(loc>=0) glUniform1i(loc, (int)value);
    }
    void setInt(const std::string& name, int value) const {
        auto loc = getLocation(name); if(loc>=0) glUniform1i(loc, value);
    }
    void setFloat(const std::string& name, float value) const {
        auto loc = getLocation(name); if(loc>=0) glUniform1f(loc, value);
    }
    void setVec3(const std::string& name, const glm::vec3& v) const {
        auto loc = getLocation(name); if(loc>=0) glUniform3fv(loc, 1, glm::value_ptr(v));
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const {
        auto loc = getLocation(name); if(loc>=0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }

    GLuint getId() const { return id; }

private:
    GLuint id = 0;
    mutable std::unordered_map<std::string, GLint> uniformCache;

    GLint getLocation(const std::string& name) const {
        auto it = uniformCache.find(name);
        if (it != uniformCache.end()) return it->second;
        GLint loc = glGetUniformLocation(id, name.c_str());
        uniformCache[name] = loc;
        if (loc == -1) {
            static thread_local std::unordered_map<std::string, bool> warned;
            if (!warned[name]) {
                std::cerr << "[Shader] Warning: uniform '" << name << "' not found or is inactive.\n";
                warned[name] = true;
            }
        }
        return loc;
    }
};

#endif // SHADER_H
