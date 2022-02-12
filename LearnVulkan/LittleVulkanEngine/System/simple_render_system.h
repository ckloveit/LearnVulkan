#pragma once

#include "../Core/LitDevice.h"
#include "../Core/LitGameObject.h"
#include "../Core/LitPipeline.h"

// std
#include <memory>
#include <vector>

namespace Lit 
{
	class SimpleRenderSystem 
	{
	public:
		SimpleRenderSystem(LitDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<LitGameObject>& gameObjects);

	private:
		void CreatePipelineLayout();
		void CreatePipeline(VkRenderPass renderPass);

		LitDevice& litDevice;

		std::unique_ptr<LitPipeline> litPipeline;
		VkPipelineLayout pipelineLayout;
	};
}  // namespace lve