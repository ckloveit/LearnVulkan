#pragma once
#include "LitDevice.h"
#include "LitPipeline.h"
#include "LitSwapChain.h"
#include "LitWindow.h"
#include "LitModel.h"

namespace Lit
{
	class LitApp
	{
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
	public:
		LitApp();
		~LitApp();

		LitApp(const LitApp&) = delete;
		LitApp& operator=(const LitApp&) = delete;

		void Run();

	private:
		void CreateCommandBuffers();
		void DrawFrame();
	private:
		LitWindow window = { WIDTH, HEIGHT, "Hello Vulkan" };
		LitDevice device = { window };
		LitSwapChain swapChain = { window, device };
		LitPipeline pipeline = { "../Shaders/Spv/simple_shader", device, swapChain };
		std::vector<VkCommandBuffer> commandBuffers;
		LitModel simpleModel{ device };

	};
}