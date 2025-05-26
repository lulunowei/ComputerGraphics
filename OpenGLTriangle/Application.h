#pragma once
#include"const.h"
#include"Render.h"
#include"Window.h"
class Application
{
public:
	Application();
	void cleanup();
	void run();
private:
	std::unique_ptr<Window> m_window;
	std::unique_ptr<Render> m_render;

};

