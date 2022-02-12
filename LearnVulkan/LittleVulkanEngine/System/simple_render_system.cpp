#include "simple_render_system.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

#define PI 3.1415926f

namespace Lit 
{
	struct SimplePushConstantData 
	{
		glm::mat2 transform{ 1.f };
		glm::vec2 translation;
		alignas(16) glm::vec3 color;
	};

	SimpleRenderSystem::SimpleRenderSystem(LitDevice& device, VkRenderPass renderPass)
		: litDevice{ device }
	{
		CreatePipelineLayout();
		CreatePipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() {
		vkDestroyPipelineLayout(litDevice.GetDevice(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::CreatePipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(litDevice.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		LitPipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		litPipeline = std::make_unique<LitPipeline>(
			litDevice,
			"../Shaders/Spv/simple_shader.vert.spv",
			"../Shaders/Spv/simple_shader.frag.spv",
			pipelineConfig);
	}

	void SimpleRenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<LitGameObject>& gameObjects) {
		litPipeline->Bind(commandBuffer);

		for (auto& obj : gameObjects)
		{
			obj.transform2DComp.rotation = glm::mod(obj.transform2DComp.rotation + 0.01f, 2.0f * PI);
			SimplePushConstantData push{};
			push.translation = obj.transform2DComp.translation;
			push.color = obj.color;
			push.transform = obj.transform2DComp.LocalToWorldMatrix();

			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0, sizeof(SimplePushConstantData), &push);
			obj.model->Bind(commandBuffer);
			obj.model->Draw(commandBuffer);
		}
	}

}  // namespace lve