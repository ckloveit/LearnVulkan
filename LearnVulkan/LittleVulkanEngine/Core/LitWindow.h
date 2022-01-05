#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Lit
{
	class LitWindow
	{
	public:
		LitWindow(int width, int height, std::string windowName);
		~LitWindow();

		LitWindow(const LitWindow&) = delete;


		bool ShouldClose() const { return glfwWindowShouldClose(Window); }
		bool IsWindowResized() const { return bFramebufferResized; }
		void ResetWindowResized() { bFramebufferResized = false; }

	private:
		void InitWindow();
		static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
	public:
		GLFWwindow* Window;
	private:
		const uint32_t mWidth;
		const uint32_t mHeight;
		std::string mWindowName;
		bool bFramebufferResized = false;

	};
}// namespace Lit

