#define _CRT_SECURE_NO_WARNINGS
#include "Window.h"

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->m_framebufferResized = true;
}

Window::Window(int width, int height, std::string title):
	m_width(width),m_height(height),m_title(title)
{
	initVulkanWindow();
}

void Window::cleanup()
{
	glfwDestroyWindow(m_window);
}

void Window::initVulkanWindow()
{
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//不要创建OPEN_GL上下文
	m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);//回调，如果我改变窗口大小，就会调用这个回调函数


}
