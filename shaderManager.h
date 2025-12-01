#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include "shader.h"

class ShaderManager {
public:
    Shader& load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
        auto shader = std::make_unique<Shader>(vertexPath, fragmentPath);
        Shader& ref = *shader;
        shaders[name] = std::move(shader);
        return ref;
    }

    Shader& get(const std::string& name) {
        if (shaders.find(name) == shaders.end()) {
            throw std::runtime_error("Shader not found: " + name);
        }
        return *shaders[name];
    }

    void clear() {
        shaders.clear();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
};
#endif // SHADERMANAGER_H
