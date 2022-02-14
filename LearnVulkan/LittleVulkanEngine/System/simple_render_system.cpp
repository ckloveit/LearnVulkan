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
		glm::mat4 modelMatrix{ 1.f };
	/*	alignas(16) glm::vec3 color{};*/
		glm::mat4 normalMatrix{ 1.f };
	};

	SimpleRenderSystem::SimpleRenderSystem(LitDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
		: litDevice{ device }
	{
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() {
		vkDestroyPipelineLayout(litDevice.GetDevice(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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

	void SimpleRenderSystem::RenderGameObjects(FrameInfo& frameInfo, std::vector<LitGameObject>& gameObjects)
	{
		litPipeline->Bind(frameInfo.commandBuffer);
		auto projectionView = frameInfo.camera.GetProjection() * frameInfo.camera.GetView();

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);

		for (auto& obj : gameObjects)
		{
			SimplePushConstantData push{};
			/*obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.0001f, 2.0f * PI);
			obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.0005f, 2.0f * PI);*/
			/*push.transform = projectionView * obj.transform.mat4();*/

			push.modelMatrix = obj.transform.mat4();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0, sizeof(SimplePushConstantData), &push);
			obj.model->Bind(frameInfo.commandBuffer);
			obj.model->Draw(frameInfo.commandBuffer);
		}
	}

}  // namespace lve