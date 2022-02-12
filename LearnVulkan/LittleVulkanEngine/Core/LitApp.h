#pragma once
#include "LitDevice.h"
#include "LitPipeline.h"
#include "LitSwapChain.h"
#include "LitWindow.h"
#include "LitGameObject.h"
#include "LitRenderer.h"

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
		//void CreatePipelineLayout();
		//void CreatePipeline();
		//void CreateCommandBuffers();
		//void DrawFrame();

		//void ReCreateSwapChain();
		//void RecordCommandBuffer(int imageIndex);


		//void RenderGameObjects(VkCommandBuffer commonBuffer);

		// 
		void LoadGameObjects();

	private:
		LitWindow window = { WIDTH, HEIGHT, "Hello Vulkan" };
		LitDevice device = { window };
		LitRenderer litRenderer { window, device };
		std::vector<LitGameObject> gameObjects;
	};
}