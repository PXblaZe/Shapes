#pragma once

#include <map>
#include <string>

class ShaderCode {
    std::map<std::string, const char*> shadermap;
public:
    ShaderCode(const char* filepath, const char* ind = "//Shader=");
    inline const char* const& parse(const char* const& shaderType) const {
        return this->shadermap.at(shaderType);
    }
    ~ShaderCode();
};
