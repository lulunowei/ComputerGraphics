#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

static void loadGlad(){
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
}//调用GL先加载这个函数
