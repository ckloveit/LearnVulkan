#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <functional>

const int WIDTH = 800;
const int HEIGHT = 600;

class HelloTriangleApplication
{
public:

	void Run()
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		CleanUp();
	}

private:
	void InitVulkan()
	{
	}

	void MainLoop()
	{
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void CleanUp()
	{
		glfwDestroyWindow(window);

		glfwTerminate();
	}

	// Window
	void InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//不要调用OpenGL相关的初始化操作
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
	}

private:
	GLFWwindow* window;
};

int main() {
	HelloTriangleApplication app;

	try {
		app.Run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

