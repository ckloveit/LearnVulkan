#include "LitApp.h"
#include "LitCamera.h"
#include <array>
#include <stdexcept>
#include <chrono>
#define PI 3.1415926f

#include "System/simple_render_system.h"
#include "System/InputSystem.h"

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
		LitCamera camera{};
		/*camera.SetViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));*/

		SimpleRenderSystem simpleRenderSystem{ device,litRenderer.GetSwapChainRenderPass() };
		auto viewerObject = LitGameObject::CreateGameObject();
		InputSystem inputSystem;
		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!window.ShouldClose())
		{
			glfwPollEvents();
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime =
				std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			inputSystem.MoveInPlaneXZ(window.GetWindow(), frameTime, viewerObject);

			camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = litRenderer.GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
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
				simpleRenderSystem.RenderGameObjects(commandBuffer, gameObjects, camera);

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

	std::unique_ptr<LitModel> CreateCubeModel(LitDevice& device, glm::vec3 offset)
	{
		LitModel::Builder modelBuilder{};
		modelBuilder.vertices = 
		{
			// left face (white)
			 {glm::vec3{-.5f, -.5f, -.5f}, glm::vec3{.9f, .9f, .9f}},
			 {glm::vec3{-.5f, .5f,  .5f},  glm::vec3{.9f, .9f, .9f}},
			 {glm::vec3{-.5f, -.5f, .5f},  glm::vec3{.9f, .9f, .9f}},
			 {glm::vec3{-.5f, .5f,  -.5f}, glm::vec3{.9f, .9f, .9f}},


			 // right face (yellow)
			 {glm::vec3{.5f, -.5f, -.5f}, glm::vec3{.8f, .8f, .1f}},
			 {glm::vec3{.5f, .5f, .5f}, glm::vec3{.8f, .8f, .1f}},
			 {glm::vec3{.5f, -.5f, .5f}, glm::vec3{.8f, .8f, .1f}},
			 {glm::vec3{.5f, .5f, -.5f}, glm::vec3{.8f, .8f, .1f}},

			 // top face (orange, remember y axis points down)
			 {glm::vec3{-.5f, -.5f, -.5f}, glm::vec3{.9f, .6f, .1f}},
			 {glm::vec3{.5f, -.5f, .5f}, glm::vec3{.9f, .6f, .1f}},
			 {glm::vec3{-.5f, -.5f, .5f}, glm::vec3{.9f, .6f, .1f}},
			 {glm::vec3{.5f, -.5f, -.5f}, glm::vec3{.9f, .6f, .1f}},

			 // bottom face (red)
			 {glm::vec3{-.5f, .5f, -.5f}, glm::vec3{.8f, .1f, .1f}},
			 {glm::vec3{.5f, .5f, .5f}, glm::vec3{.8f, .1f, .1f}},
			 {glm::vec3{-.5f, .5f, .5f}, glm::vec3{.8f, .1f, .1f}},
			 {glm::vec3{.5f, .5f, -.5f}, glm::vec3{.8f, .1f, .1f}},

			 // nose face (blue)
			 {glm::vec3{-.5f, -.5f, 0.5f}, glm::vec3{.1f, .1f, .8f}},
			 {glm::vec3{.5f, .5f, 0.5f}, glm::vec3{.1f, .1f, .8f}},
			 {glm::vec3{-.5f, .5f, 0.5f}, glm::vec3{.1f, .1f, .8f}},
			 {glm::vec3{.5f, -.5f, 0.5f}, glm::vec3{.1f, .1f, .8f}},

			 // tail face (green)
			 {glm::vec3{-.5f, -.5f, -0.5f}, glm::vec3{.1f, .8f, .1f}},
			 {glm::vec3{.5f, .5f, -0.5f}, glm::vec3{.1f, .8f, .1f}},
			 {glm::vec3{-.5f, .5f, -0.5f}, glm::vec3{.1f, .8f, .1f}},
			 {glm::vec3{.5f, -.5f, -0.5f}, glm::vec3{.1f, .8f, .1f}},
		};

		for (auto& v : modelBuilder.vertices) 
		{
			v.position += offset;
		}
		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
						 12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };
		return std::make_unique<LitModel>(device, modelBuilder);
	}
	void LitApp::LoadGameObjects()
	{
		std::shared_ptr<LitModel> lveModel = CreateCubeModel(device, glm::vec3{ .0f, .0f, .0f });
		auto cube = LitGameObject::CreateGameObject();
		cube.model = lveModel;
		cube.transform.translation = glm::vec3{ .0f, .0f, 2.5f };
		cube.transform.scale = glm::vec3{ .5f, .5f, .5f };
		gameObjects.push_back(std::move(cube));
	}
}