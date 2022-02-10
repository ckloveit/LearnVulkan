#include "LitWindow.h"
#include <stdexcept>

#if defined(VK_USE_PLATFORM_WIN32_KHR)
#include <GLFW/glfw3native.h>
#endif

#define GLFW_WINDOWSURFACECREATE 0
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
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { };
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hinstance = (HINSTANCE)GetModuleHandle(NULL);
		surfaceCreateInfo.hwnd = (HWND)glfwGetWin32Window(Window);
		if (vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
#else
		if (glfwCreateWindowSurface(instance, Window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
#endif
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