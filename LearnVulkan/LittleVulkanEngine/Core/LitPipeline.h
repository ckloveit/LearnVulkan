#pragma once
#include <string>
#include <vector>
#include "LitDevice.h"
#include "LitSwapChain.h"
// libs
#include <vulkan/vulkan.h>

namespace Lit
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;

		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};


	class LitPipeline
	{
	public:
		LitPipeline(
			LitDevice& device,
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);
		~LitPipeline();

		LitPipeline(const LitPipeline&) = delete;
		LitPipeline& operator=(const LitPipeline&) = delete;
		void Bind(VkCommandBuffer comamndBuffer);
		
		static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> ReadFile(const std::string& filename);

		void CreateGraphicsPipeline(const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
	private:
		std::string filePrefix;
		LitDevice& device;

		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};

}  // namespace Lit