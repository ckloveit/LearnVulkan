#include "LitWindow.h"
#include <stdexcept>
namespace Lit
{
	LitWindow::LitWindow(int InWidth, int InHeight, std::string InWindowName)
		:width(static_cast<uint32_t>(InWidth)), height(static_cast<uint32_t>(InHeight)), windowName(InWindowName)
	{
		InitWindow();
	}
	LitWindow::~LitWindow()
	{
		glfwDestroyWindow(Window);
		glfwTerminate();
	}

	void LitWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, Window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void LitWindow::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		Window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(Window, this);
		glfwSetFramebufferSizeCallback(Window, &LitWindow::FrameBufferResizeCallback);
	}

	void LitWindow::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		LitWindow* appWindow = reinterpret_cast<LitWindow*>(glfwGetWindowUserPointer(window));
		appWindow->bFramebufferResized = true;
		appWindow->width = width;
		appWindow->height = height;
	}

}