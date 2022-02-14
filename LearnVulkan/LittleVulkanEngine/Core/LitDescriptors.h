#pragma once
#include "LitDevice.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace Lit
{
	class LitDescriptorSetLayout
	{
		friend class LitDescriptorWriter;
	public:
		class Builder
		{
		public:
			Builder(LitDevice& litDevice) : device(litDevice)
			{
			}

			Builder& AddBinding(uint32_t bindingIndex, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
			std::unique_ptr<LitDescriptorSetLayout> Build() const;

		private:
			LitDevice& device;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		LitDescriptorSetLayout(LitDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~LitDescriptorSetLayout();

		LitDescriptorSetLayout(const LitDescriptorSetLayout&) = delete;
		LitDescriptorSetLayout& operator=(const LitDescriptorSetLayout&) = delete;
		VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }

	private:
		LitDevice& device;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
	};

	class LitDescriptorPool
	{
		friend class LitDescriptorWriter;
	public:
		class Builder
		{
		public:
			Builder(LitDevice& litDevice) : device(litDevice) {}
			Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& SetMaxSets(uint32_t count);
			std::unique_ptr<LitDescriptorPool> Build() const;

		private:
			LitDevice& device;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;
		};
		LitDescriptorPool(
			LitDevice& litDevice,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);

		~LitDescriptorPool();
		LitDescriptorPool(const LitDescriptorPool&) = delete;
		LitDescriptorPool& operator=(const LitDescriptorPool&) = delete;

		bool AllocateDescriptor(
			const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void ResetPool();

	private:
		LitDevice& device;
		VkDescriptorPool descriptorPool;
	};

	class LitDescriptorWriter
	{
	public:
		LitDescriptorWriter(LitDescriptorSetLayout& setLayout, LitDescriptorPool& pool);

		LitDescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		LitDescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool Build(VkDescriptorSet& set);
		void OverWrite(VkDescriptorSet& set);
	private:
		LitDescriptorSetLayout& setLayout;
		LitDescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;
	};

}
