#include "Window.h"

void Window::run()
{
    initWindow();
    while (!glfwWindowShouldClose(m_window))
    {
        processInput(m_window);
        render.render();
        glfwSwapBuffers(m_window);
        glfwPollEvents();//检查鼠标按键
        
    }
    glfwTerminate();
}

void Window::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(800, 600, "GLModel", NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return ;
    }
    glfwMakeContextCurrent(m_window);//openGl上下文
    loadGlad();
    viewPort();//渲染窗口大小
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);//窗口resize回调
}


void Window::viewPort()
{
    glViewport(0, 0, 800, 600);
}

void Window::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

