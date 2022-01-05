#include "LitApp.h"

namespace Lit
{
	void LitApp::Run()
	{
		while (!litWindow.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}