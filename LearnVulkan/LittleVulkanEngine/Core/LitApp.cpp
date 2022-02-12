#include "LitApp.h"
#include <array>
#include <stdexcept>

namespace Lit
{
	LitApp::LitApp() 
	{
		LoadModels();
		CreatePipelineLayout();
		ReCreateSwapChain();
		CreateCommandBuffers(); 
	}
	LitApp::~LitApp()
	{
		vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);
		vkFreeCommandBuffers( device.GetDevice(), device.GetCommandPool(), static_cast<uint32_t>(commandBuffers.size()),commandBuffers.data());
	}
	void LitApp::Run()
	{
		while (!window.ShouldClose())
		{
			glfwPollEvents();
			DrawFrame();
		}
		vkDeviceWaitIdle(device.GetDevice());
	}
	void LitApp::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void LitApp::CreatePipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		LitPipeline::DefaultPipelineConfigInfo(
			pipelineConfig,
			swapChain->GetSwapChainExtent().width,
			swapChain->GetSwapChainExtent().height);
		pipelineConfig.renderPass = swapChain->GetRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipeline = std::make_unique<LitPipeline>(
			device,
			"../Shaders/Spv/simple_shader.vert.spv",
			"../Shaders/Spv/simple_shader.frag.spv",
			pipelineConfig);
	}
	void LitApp::ReCreateSwapChain()
	{
		auto extent = window.GetExtent();
		while (extent.width == 0 || extent.height == 0) 
		{
			extent = window.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device.GetDevice());

		if(swapChain == nullptr)
		{
			swapChain = std::make_unique<LitSwapChain>(device, extent);
		}
		else
		{
			swapChain = std::make_unique<LitSwapChain>(device, extent, std::move(swapChain));
		}
		CreatePipeline();
	}

	void LitApp::LoadModels()
	{
		std::vector<LitModel::Vertex> vertices;
		vertices.emplace_back(LitModel::Vertex{ glm::vec2{0.0f, -0.5f} , glm::vec3(1.0f,0.0,0.0f)});
		vertices.emplace_back(LitModel::Vertex{ glm::vec2{0.5f, 0.5f} , glm::vec3(0.0f,0.0,0.0f) });
		vertices.emplace_back(LitModel::Vertex{ glm::vec2{-0.5f, 0.5f}, glm::vec3(0.0f,0.0,1.0f) });
		litModel = std::make_unique<LitModel>(device, vertices);
	}

	void LitApp::CreateCommandBuffers()
	{
		commandBuffers.resize(swapChain->ImageCount());
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = device.GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
		if (vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void LitApp::RecordCommandBuffer(int imageIndex)
	{
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0;                   // Optional
			beginInfo.pInheritanceInfo = nullptr;  // Optional

			if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = swapChain->GetRenderPass();
			renderPassInfo.framebuffer = swapChain->GetFrameBuffer(imageIndex);

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChain->GetSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.2f, 0.4f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				pipeline->Bind(commandBuffers[imageIndex]);

				//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
				litModel->Bind(commandBuffers[imageIndex]);
				litModel->Draw(commandBuffers[imageIndex]);
			}
			vkCmdEndRenderPass(commandBuffers[imageIndex]);
			
			if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to record command buffer!");
			}
		}

	}
	void LitApp::DrawFrame()
	{
		uint32_t imageIndex;

		auto result = swapChain->AcquireNextImage(&imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) 
		{
			ReCreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		RecordCommandBuffer(imageIndex);

		result = swapChain->SumitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.IsWindowResized()) 
		{
			window.ResetWindowResized();
			ReCreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

	}
}