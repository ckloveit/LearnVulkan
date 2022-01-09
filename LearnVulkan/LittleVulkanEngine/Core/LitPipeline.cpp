#include "LitPipeline.h"
#include "LitPipelineUtils.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace Lit
{
	LitPipeline::LitPipeline(std::string inFilePrefix, LitDevice& inDevice, LitSwapChain& inSwapChain) :
		filePrefix(inFilePrefix), device(inDevice), swapChain(inSwapChain)
	{
		CreateGraphicsPipeline();
	}
	LitPipeline::~LitPipeline()
	{
		vkDestroyPipeline(device.GetDevice(), graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);
	}
	void LitPipeline::Bind(VkCommandBuffer commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}
	std::vector<char> LitPipeline::ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	void LitPipeline::CreateGraphicsPipeline()
	{
		auto vertCode = ReadFile(filePrefix + ".vert.spv");
		auto fragCode = ReadFile(filePrefix + ".frag.spv");

		//std::cout << "vertShaderCode size: " << vertCode.size() << '\n';
		//std::cout << "fragShaderCode size: " << fragCode.size() << '\n';
		VkShaderModule vertShaderModule = CreateShaderModule(vertCode);
		VkShaderModule fragShaderModule = CreateShaderModule(fragCode);
		auto vertShaderStageInfo = PipelineUtils::ShaderStage(vertShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
		auto fragShaderStageInfo = PipelineUtils::ShaderStage(fragShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);
		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;  // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;  // Optional

		auto inputAssembly = PipelineUtils::VertexInputAssemblyState();
		VkExtent2D swapChainExtent = swapChain.GetSwapChainExtent();
		VkViewport viewport = { 0.0f, 0.0f, (float)swapChainExtent.width, (float)swapChainExtent.height, 0.0f, 1.0f };
		VkRect2D scissor = { {0, 0}, swapChainExtent };
		auto viewportState = PipelineUtils::ViewportState(viewport, scissor);
		auto rasterizer = PipelineUtils::RasterizationState();
		auto multisampling = PipelineUtils::MultisampleState();
		auto colorBlendAttachment = PipelineUtils::ColorBlendAttachmentState();
		auto colorBlending = PipelineUtils::ColorBlendingState(colorBlendAttachment);
		auto depthStencil = PipelineUtils::DepthStencilState();
		// pipeline layout is used passing uniform information
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;             // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr;          // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0;     // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr;  // Optional
		if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr;  // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;  // Optional
		pipelineInfo.pDepthStencilState = &depthStencil;

		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = swapChain.GetRenderPass();
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
		pipelineInfo.basePipelineIndex = -1;               // Optional

		if (vkCreateGraphicsPipelines(device.GetDevice(),
			VK_NULL_HANDLE,
			1,
			&pipelineInfo,
			nullptr,
			&graphicsPipeline) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(device.GetDevice(), fragShaderModule, nullptr);
		vkDestroyShaderModule(device.GetDevice(), vertShaderModule, nullptr);
	}

	VkShaderModule LitPipeline::CreateShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device.GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create shader module!");
		}
		return shaderModule;
	}
}