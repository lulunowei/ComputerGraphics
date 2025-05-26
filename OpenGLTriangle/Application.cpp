#include "Application.h"

Application::Application(){
	m_window =
		std::make_unique<Window>();
	m_render =
		std::make_unique<Render>();
}

void Application::run()
{
    auto myWindow=m_window->getWindow();
    while (!glfwWindowShouldClose(myWindow))
    {
        m_window->processInput(myWindow);
        m_render->render();
        glfwSwapBuffers(myWindow);
        glfwPollEvents();

    }
    glfwTerminate();
}
