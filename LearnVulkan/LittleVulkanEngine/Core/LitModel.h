#pragma once
#include "LitDevice.h"
#include "LitBuffer.h"

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
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> GetVertexInputBindingDesc();
			static std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDesc();
			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal &&
					uv == other.uv;
			}
		};
		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
			void LoadModel(const std::string& filepath);
		};

		LitModel(LitDevice& device, const Builder& builder);
		~LitModel();

		LitModel(const LitModel&) = delete;
		LitModel& operator=(const LitModel&) = delete;

		static std::unique_ptr<LitModel> CreateModelFromFile(
			LitDevice& device, const std::string& filepath);

		void Draw(VkCommandBuffer commandBuffer);

		void Bind(VkCommandBuffer commandBuffer);
	private:
		void CreateVertexBuffer(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

	private:
		LitDevice& device;

		std::unique_ptr<LitBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<LitBuffer> indexBuffer;
		uint32_t indexCount;

	};
}
