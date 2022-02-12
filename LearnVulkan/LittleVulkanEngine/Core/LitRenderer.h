#pragma once
#include "LitDevice.h"
#include "LitSwapChain.h"
#include "LitWindow.h"


// std
#include <cassert>
#include <memory>
#include <vector>

namespace Lit
{
	class LitRenderer
	{
	public:
		LitRenderer(LitWindow& window, LitDevice& device);
		~LitRenderer();

		LitRenderer(const LitRenderer&) = delete;
		LitRenderer& operator=(const LitRenderer&) = delete;

		VkRenderPass GetSwapChainRenderPass() const
		{
			return litSwapChain->GetRenderPass(); 
		}
		bool IsFrameInProgress() const { return bIsFrameStarted; }
		uint32_t GetImageCount() const { return litSwapChain->ImageCount(); }
		VkCommandBuffer GetCurrentCommandBuffer() const 
		{
			assert(bIsFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int GetFrameIndex() const
		{
			assert(bIsFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();

	private:
		LitWindow& litWindow;
		LitDevice& litDevice;
		std::shared_ptr<LitSwapChain> litSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		
		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool bIsFrameStarted;

	};
}
