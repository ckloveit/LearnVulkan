#pragma once
#include "LitWindow.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std lib headers
#include <string>
#include <vector>

namespace Lit
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily;
		uint32_t presentFamily;
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;
		bool IsComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
	};

	class LitDevice
	{
	public:
		LitDevice(LitWindow& windowRef) : window(windowRef) { Init(); }

		LitDevice(const LitDevice&) = delete;
		LitDevice& operator=(const LitDevice&) = delete;

		~LitDevice() { CleanUp(); }

		VkDevice GetDevice() { return device; }
		VkSurfaceKHR GetSurface() { return surface; }
		VkQueue GetGraphicsQueue() { return graphicsQueue; }
		VkQueue GetPresentQueue() { return presentQueue; }

		VkInstance GetInstance() { return instance; }
		VkPhysicalDevice GetPhysicalDevice() { return physicalDevice; }
		uint32_t GetGraphicsQueueFamily() { return FindPhysicalQueueFamilies().graphicsFamily; }

		// Command Pool
		VkCommandPool GetCommandPool() { return commandPool; }
		SwapChainSupportDetails GetSwapChainSupportDetail() { return QuerySwapChainSupport(physicalDevice); }
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(physicalDevice); }
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		// Buffer And Image Helper Functions
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
				VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		void CopyBuffer(VkBuffer srcBuffer, VkBuffer destBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

		bool HasStencilFormat(VkFormat format);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, 
				VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount);

		void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory,
			VkImageCreateFlags flags, uint32_t arrayLayers);
		VkImageView CreateImageView(VkImage image, VkFormat format, 
			VkImageAspectFlags aspectFlags, uint32_t mipLevels, VkImageViewType viewType);
		void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	private:
		void Init();
		void CleanUp();

		// Vulkan Object Create
		void CreateInstance();
		void CreateDebugMessenger();
		void CreateSurface();
		void PickPhyscialDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();

		// Helper functions
		bool IsDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void HasGLFWRequiredInstanceExtensions();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	private:
		VkDevice device;
		VkSurfaceKHR surface;

		VkQueue graphicsQueue;
		VkQueue presentQueue;
		// The VK_LAYER_KHRONOS_validation contains all current validation functionality.
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkCommandPool commandPool;
		LitWindow& window;
	};


}
