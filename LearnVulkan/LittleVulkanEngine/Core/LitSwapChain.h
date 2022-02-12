#pragma once
#include "LitDevice.h"
#include "LitWindow.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace Lit
{
	class LitSwapChain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		LitSwapChain(LitDevice& deviceRef, VkExtent2D inWindowExtent) : windowExtent(inWindowExtent), device(deviceRef)
		{
			Init();
		}
		LitSwapChain(LitDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<LitSwapChain> previousSwapChain);

		LitSwapChain(const LitSwapChain&) = delete;
		LitSwapChain& operator=(const LitSwapChain&) = delete;

		~LitSwapChain()
		{
			CleanupSwapChain();
			CleanUpSyncObjects();
		}

		size_t ImageCount() { return swapChainImages.size(); }
		VkFormat GetSwapChainImageFormat() { return swapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() { return swapChainExtent; }
		float AspectRatio() { return swapChainExtent.width / (float)swapChainExtent.height; }
		VkFormat FindDepthFormat();
		VkRenderPass GetRenderPass() { return renderPass; }
		VkFramebuffer GetFrameBuffer(int index) { return swapChainFrameBuffers[index]; }

		VkResult AcquireNextImage(uint32_t* imageIndex);
		VkResult SumitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

		bool CompareSwapFormats(const LitSwapChain& swapChain) const
		{
			return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
				swapChain.swapChainImageFormat == swapChainImageFormat;
		}
	private:
		void Init();
		void CleanUpSyncObjects();
		void CleanupSwapChain();

		void CreateSwapChain();

		void CreateImageViews();
		void CreateDepthResources();
		void CreateRenderPass();
		void CreateFrameBuffers();
		void CreateSyncObjects();

		// Helper functions
		VkSurfaceFormatKHR ChooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		LitDevice& device;
		VkExtent2D windowExtent;

		VkRenderPass renderPass;

		std::vector<VkImage> depthImages;
		std::vector<VkDeviceMemory> depthImageMemorys;
		std::vector<VkImageView> depthImageViews;

		VkSwapchainKHR swapChain;
		std::shared_ptr<LitSwapChain> oldSwapChain;

		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;
		VkFormat swapChainDepthFormat;

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFrameBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFight;
		size_t currentFrame = 0;
	};


}


