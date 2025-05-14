#include "Render.h"

Render::Render()
{
}

void Render::render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	mesh.initVBO();
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

Render::~Render()
{
}
