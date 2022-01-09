#pragma once
#include "LitDevice.h"
#include "LitWindow.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std lib headers
#include <string>
#include <vector>

namespace Lit
{
	class LitSwapChain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		LitSwapChain(LitWindow& inWindow, LitDevice& inDevice) : window(inWindow), device(inDevice)
		{
			Init();
		}
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

	private:
		void Init();
		void CleanUpSyncObjects();
		void CleanupSwapChain();

		void CreateSwapChain();
		void RecreateSwapChain();

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
		LitWindow& window;
		LitDevice& device;

		VkRenderPass renderPass;

		std::vector<VkImage> depthImages;
		std::vector<VkDeviceMemory> depthImageMemorys;
		std::vector<VkImageView> depthImageViews;

		VkSwapchainKHR swapChain;
		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;
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


