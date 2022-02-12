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
			glm::vec3 position;
			glm::vec3 color;

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
				std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2, VkVertexInputAttributeDescription{});
				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, position);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, color);

				return attributeDescriptions;
			}
		};
		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		LitModel(LitDevice& device, const Builder& builder);
		~LitModel() { CleanUp(); }

		LitModel(const LitModel&) = delete;
		LitModel& operator=(const LitModel&) = delete;

		void Draw(VkCommandBuffer commandBuffer);

		void Bind(VkCommandBuffer commandBuffer);
	private:
		void CreateVertexBuffer(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
		void CleanUp();

	private:
		LitDevice& device;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;

	};
}
