#include "LitApp.h"
#include <array>
#include <stdexcept>
#define PI 3.1415926f

#include "System/simple_render_system.h"
#include "ImGui/LitImGui.h"

namespace Lit
{
	LitApp::LitApp() 
	{
		LoadGameObjects();
	}
	LitApp::~LitApp()
	{
	}
	void LitApp::Run()
	{
		  // create imgui, and pass in dependencies
		LitImGui litImgui{
			window,
			device,
			litRenderer.GetSwapChainRenderPass(),
			litRenderer.GetImageCount() };
		SimpleRenderSystem simpleRenderSystem{ device,litRenderer.GetSwapChainRenderPass() };

		while (!window.ShouldClose())
		{
			glfwPollEvents();
			if (auto commandBuffer = litRenderer.BeginFrame())
			{
				// tell imgui that we're starting a new frame
				litImgui.NewFrame();
				
				litRenderer.BeginSwapChainRenderPass(commandBuffer);
				// render game objects first, so they will be rendered in the background. This
				// is the best we can do for now.
				// Once we cover offscreen rendering, we can render the scene to a image/texture rather than
				// directly to the swap chain. This texture of the scene can then be rendered to an imgui
				// subwindow
				simpleRenderSystem.RenderGameObjects(commandBuffer, gameObjects);

				// example code telling imgui what windows to render, and their contents
				// this can be replaced with whatever code/classes you set up configuring your
				// desired engine UI
				litImgui.RunExample();
				// as last step in render pass, record the imgui draw commands
				litImgui.Render(commandBuffer);

				litRenderer.EndSwapChainRenderPass(commandBuffer);
				litRenderer.EndFrame();
			}
		}
		vkDeviceWaitIdle(device.GetDevice());
	}

	void LitApp::LoadGameObjects()
	{
		std::vector<LitModel::Vertex> vertices;
		vertices.emplace_back(LitModel::Vertex{ glm::vec2{0.0f, -0.5f} , glm::vec3(1.0f,0.0,0.0f)});
		vertices.emplace_back(LitModel::Vertex{ glm::vec2{0.5f, 0.5f} , glm::vec3(0.0f,0.0,0.0f) });
		vertices.emplace_back(LitModel::Vertex{ glm::vec2{-0.5f, 0.5f}, glm::vec3(0.0f,0.0,1.0f) });
		auto litModel = std::make_shared<LitModel>(device, vertices);

		auto triangle = LitGameObject::CreateGameObject();
		triangle.model = litModel;
		triangle.color = glm::vec3{ 1.0f, 0.0f, 0.1f };
		triangle.transform2DComp.translation.x = .2f;
		triangle.transform2DComp.scale = glm::vec2{ 2.f, .5f };
		triangle.transform2DComp.rotation = .25f * 2.0f * PI;

		gameObjects.push_back(std::move(triangle));
	}
}