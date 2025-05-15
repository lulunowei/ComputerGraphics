#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include"const.h"
class Window
{
public:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	Window(int width,int height,std::string title);


	virtual ~Window() {
		windowCleanup();
	}
	void windowCleanup();

	
	bool getFramebufferResized() { return m_framebufferResized; }
	void setFramebufferResized(bool framebufferResized) { m_framebufferResized = framebufferResized; }
	
	GLFWwindow* getWindow() const { return m_window; }

private:
	int m_width;
	int m_height;
    std::string m_title;
	GLFWwindow* m_window = nullptr;
	bool m_framebufferResized = false;//窗口是否变化

	void initVulkanWindow();
};

