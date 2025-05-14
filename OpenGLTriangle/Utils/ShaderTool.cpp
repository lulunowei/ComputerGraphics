#include "ShaderTool.h"

namespace vertexShader {
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
}

namespace fragmentShader {
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";
}



void ShaderTool::compileShader()
{
    unsigned int vertexShader;//创建一个着色器
    vertexShader = glCreateShader(GL_VERTEX_SHADER);//着色器类型
    //编译着色器
    glShaderSource(vertexShader,
        1,//源代码字符串数量
        &vertexShader::vertexShaderSource,
        NULL);
    glCompileShader(vertexShader);
    //==============================================//
    unsigned int fragmentShader;//创建一个着色器
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);//着色器类型
    //编译着色器
    glShaderSource(fragmentShader,
        1,//源代码字符串数量
        &fragmentShader::fragmentShaderSource,
        NULL);
    glCompileShader(fragmentShader);



    //检查编译是否成功
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
