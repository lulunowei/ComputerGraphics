#pragma once
#include"const.h"
class Window
{
public:
	Window();
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);//窗口resize回调

	//void run();
	void initWindow();
	void processInput(GLFWwindow* window);//检查按键
	GLFWwindow* getWindow() { return m_window; }


private:
	void viewPort();
private:
	GLFWwindow* m_window;
};

