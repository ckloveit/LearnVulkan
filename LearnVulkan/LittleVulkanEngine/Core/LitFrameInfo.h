#pragma once

#include "LitCamera.h"
#include <vulkan/vulkan.h>

namespace Lit
{
	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		LitCamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};

}