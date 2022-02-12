#include "LitRenderer.h"

#include <array>
#include <cassert>
#include <stdexcept>

namespace Lit
{
	LitRenderer::LitRenderer(LitWindow& window, LitDevice& device)
		:litDevice(device), litWindow(window)
	{
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	LitRenderer::~LitRenderer()
	{
		FreeCommandBuffers();
	}

	void LitRenderer::RecreateSwapChain()
	{
		auto extent = litWindow.GetExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = litWindow.GetExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(litDevice.GetDevice());

		if (litSwapChain == nullptr) {
			litSwapChain = std::make_unique<LitSwapChain>(litDevice, extent);
		}
		else {
			std::shared_ptr<LitSwapChain> oldSwapChain = std::move(litSwapChain);
			litSwapChain = std::make_unique<LitSwapChain>(litDevice, extent, oldSwapChain);

			if (!oldSwapChain->CompareSwapFormats(*litSwapChain.get())) 
			{
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
	}
	
	void LitRenderer::CreateCommandBuffers()
	{
		commandBuffers.resize(LitSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = litDevice.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(litDevice.GetDevice(), &allocInfo, commandBuffers.data()) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}
	void LitRenderer::FreeCommandBuffers() 
	{
		vkFreeCommandBuffers(
			litDevice.GetDevice(),
			litDevice.GetCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer LitRenderer::BeginFrame()
	{
		assert(!bIsFrameStarted && "Can't call beginFrame while already in progress");

		auto result = litSwapChain->AcquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		bIsFrameStarted = true;
		auto commandBuffer = GetCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		return commandBuffer;
	}

	void LitRenderer::EndFrame()
	{
		assert(bIsFrameStarted && "Can't call endFrame while frame is not in progress");
		auto commandBuffer = GetCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = litSwapChain->SumitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || litWindow.IsWindowResized()) {
			litWindow.ResetWindowResizedFlag();
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
		bIsFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % LitSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void LitRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(bIsFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == GetCurrentCommandBuffer() &&
			"Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = litSwapChain->GetRenderPass();
		renderPassInfo.framebuffer = litSwapChain->GetFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = litSwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.2f, 0.4f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(litSwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(litSwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, litSwapChain->GetSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void LitRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(bIsFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == GetCurrentCommandBuffer() &&
			"Can't end render pass on command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);
	}
}