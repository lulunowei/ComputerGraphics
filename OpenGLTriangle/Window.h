#pragma once
#include"const.h"
#include"Render.h"
class Window
{
public:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);//窗口resize回调

	void run();
	void initWindow();
	void viewPort();
	void processInput(GLFWwindow* window);//检查按键

public:
	GLFWwindow* m_window;
	Render render;
};

