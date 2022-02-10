#pragma once
#include "LitDevice.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE  // open gl uses -1 to 1, vk is 0 to 1
#include <glm/glm.hpp>

//std
#include <memory>
#include <vector>

namespace Lit
{
	class LitModel
	{
	public:
		struct Vertex
		{
			glm::vec2 position;

			static std::vector<VkVertexInputBindingDescription> GetVertexInputBindingDesc()
			{
				std::vector<VkVertexInputBindingDescription> bindingDescriptions(1, VkVertexInputBindingDescription{});
				bindingDescriptions[0].binding = 0;
				bindingDescriptions[0].stride = sizeof(Vertex);
				bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescriptions;
			}

			static std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDesc()
			{
				std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1, VkVertexInputAttributeDescription{});
				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = 0;
				return attributeDescriptions;
			}
		};
		
		LitModel(LitDevice& device);
		~LitModel() {}

		LitModel(const LitModel&) = delete;
		LitModel& operator=(const LitModel&) = delete;

		void Draw(VkCommandBuffer commandBuffer);

		void Bind(VkCommandBuffer commandBuffer);
	private:
		void InitMeshVertices();
		void CreateVertexBuffer();
		void CleanUp();

	private:
		LitDevice& device;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
		
		std::vector<Vertex> vertices;


	};
}
