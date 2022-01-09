#pragma once
#include <string>
#include <vector>
#include "LitDevice.h"
#include "LitSwapChain.h"
// libs
#include <vulkan/vulkan.h>

namespace Lit
{
	class LitPipeline
	{
	public:
		LitPipeline(std::string inFilePrefix, LitDevice& inDevice, LitSwapChain& inSwapChain);
		~LitPipeline();

		LitPipeline(const LitPipeline&) = delete;
		LitPipeline& operator=(const LitPipeline&) = delete;
		void Bind(VkCommandBuffer comamndBuffer);
	private:
		static std::vector<char> ReadFile(const std::string& filename);

		void CreateGraphicsPipeline();

		VkShaderModule CreateShaderModule(const std::vector<char>& code);
	private:
		std::string filePrefix;
		LitDevice& device;
		LitSwapChain& swapChain;

		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
	};

}  // namespace Lit