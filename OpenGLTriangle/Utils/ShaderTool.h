#pragma once
#include"../const.h"
namespace vertexShader {
    extern const char* vertexShaderSource;
}

namespace fragmentShader {
    extern const char* fragmentShaderSource;
}

class ShaderTool
{
public:
    static void compileShader();
};

