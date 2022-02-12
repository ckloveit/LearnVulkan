#pragma once

//#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Lit
{
	class LitWindow
	{
	public:
		LitWindow(int InWidth, int InHeight, std::string InWindowName);
		~LitWindow();
		LitWindow(const LitWindow&) = delete;
		LitWindow& operator=(const LitWindow&) = delete;

		bool ShouldClose() const { return glfwWindowShouldClose(Window); }
		bool IsWindowResized() const { return bFramebufferResized; }
		void ResetWindowResized() { bFramebufferResized = false; }
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		const uint32_t GetWidth() const { return width; }
		const uint32_t GetHeight() const { return height; }
		VkExtent2D GetExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		
		bool IsWindowResized() { return framebufferResized; }
		void ResetWindowResizedFlag() { framebufferResized = false; }

	private:
		void InitWindow();
		static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
	public:
		GLFWwindow* Window;
	private:
		uint32_t width;
		uint32_t height;
		bool framebufferResized = false;
		std::string windowName;
		bool bFramebufferResized = false;
	};
}// namespace Lit

