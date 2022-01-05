#include "LitWindow.h"

namespace Lit
{
	LitWindow::LitWindow(int width, int height, std::string windowName)
		:mWidth(static_cast<uint32_t>(width)), mHeight(static_cast<uint32_t>(height)), mWindowName(windowName)
	{
		InitWindow();
	}
	LitWindow::~LitWindow()
	{
		glfwDestroyWindow(Window);
		glfwTerminate();
	}
	void LitWindow::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		Window = glfwCreateWindow(mWidth, mHeight, mWindowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(Window, this);
		glfwSetFramebufferSizeCallback(Window, &LitWindow::FrameBufferResizeCallback);
	}

	void LitWindow::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		LitWindow* appWindow = reinterpret_cast<LitWindow*>(glfwGetWindowUserPointer(window));
		appWindow->bFramebufferResized = true;
	}

}