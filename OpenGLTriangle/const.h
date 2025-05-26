#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include<memory>


static void loadGlad(){
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
}//调用GL先加载这个函数

const std::string VERTEX_SHADER_PATH = "shaders/shader.vert";//顶点着色器路径
const std::string FRAGMENT_SHADER_PATH = "shaders/shader.frag";//片段着色器路径
const std::string TEXTURE_PATH_01 = "textures/砖块.png";//纹理图1
const std::string TEXTURE_PATH_02 = "textures/wall.jpg";//纹理图2
