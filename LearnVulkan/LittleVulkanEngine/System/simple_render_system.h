#pragma once
#include "Core/LitCamera.h"
#include "Core/LitDevice.h"
#include "Core/LitGameObject.h"
#include "Core/LitPipeline.h"
#include "Core/LitFrameInfo.h"


// std
#include <memory>
#include <vector>

namespace Lit 
{
	class SimpleRenderSystem 
	{
	public:
		SimpleRenderSystem(LitDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(FrameInfo& frameInfo, std::vector<LitGameObject>& gameObjects);
	private:																						  
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

		LitDevice& litDevice;

		std::unique_ptr<LitPipeline> litPipeline;
		VkPipelineLayout pipelineLayout;
	};
}  // namespace lve