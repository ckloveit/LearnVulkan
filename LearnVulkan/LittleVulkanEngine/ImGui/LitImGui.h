#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include "Core/LitDevice.h"
#include "Core/LitWindow.h"

#include <stdexcept>

namespace Lit 
{
	static void check_vk_result(VkResult err) {
		if (err == 0) return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0) abort();
	}

	class LitImGui {
	public:
		LitImGui(LitWindow& window, LitDevice& device, VkRenderPass renderPass, uint32_t imageCount);
		~LitImGui();

		void NewFrame();

		void Render(VkCommandBuffer commandBuffer);

		// Example state
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		void RunExample();

	private:
		LitDevice& litDevice;

		// We haven't yet covered descriptor pools in the tutorial series
		// so I'm just going to create one for just imgui and store it here for now.
		// maybe its preferred to have a separate descriptor pool for imgui anyway,
		// I haven't looked into imgui best practices at all.
		VkDescriptorPool descriptorPool;
	};
}  // namespace lve