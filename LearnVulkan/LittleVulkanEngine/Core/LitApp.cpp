#include "LitApp.h"
#include "LitCamera.h"
#include <array>
#include <stdexcept>
#include <chrono>
#define PI 3.1415926f

#include "LitBuffer.h"
#include "System/simple_render_system.h"
#include "System/InputSystem.h"
#include "LitFrameInfo.h"

#include "ImGui/LitImGui.h"

namespace Lit
{
	struct GlobalUBO
	{
		glm::mat4 projectionView{ 1.0f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, -3.0f, -1.0f));
	};


	LitApp::LitApp() 
	{
		globalDescriptorPool = LitDescriptorPool::Builder(device)
			.SetMaxSets(LitSwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LitSwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();


		LoadGameObjects();
	}
	LitApp::~LitApp()
	{
	}
	void LitApp::Run()
	{
		VkDeviceSize uniformBufferSize = sizeof(GlobalUBO);
		std::vector<std::unique_ptr<LitBuffer>> uboBuffers(LitSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<LitBuffer>(
				device,
				uniformBufferSize,
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->Map();
		}

		  // create imgui, and pass in dependencies
		LitImGui litImgui{
			window,
			device,
			litRenderer.GetSwapChainRenderPass(),
			litRenderer.GetImageCount() };
		LitCamera camera{};
		/*camera.SetViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));*/


		auto globalSetLayout = LitDescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.Build();
		std::vector<VkDescriptorSet> globalDescriptorSets(LitSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) 
		{
			auto bufferInfo = uboBuffers[i]->DescriptorInfo();
			LitDescriptorWriter(*globalSetLayout, *globalDescriptorPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ device, litRenderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout() };

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
				int frameIndex = litRenderer.GetFrameIndex();
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera , globalDescriptorSets[frameIndex]};

				//update
				GlobalUBO ubo{};
				ubo.projectionView = camera.GetProjection() * camera.GetView();
				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				// tell imgui that we're starting a new frame
				litImgui.NewFrame();
				
				litRenderer.BeginSwapChainRenderPass(commandBuffer);
				// render game objects first, so they will be rendered in the background. This
				// is the best we can do for now.
				// Once we cover offscreen rendering, we can render the scene to a image/texture rather than
				// directly to the swap chain. This texture of the scene can then be rendered to an imgui
				// subwindow
				simpleRenderSystem.RenderGameObjects(frameInfo, gameObjects);

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
		/*std::shared_ptr<LitModel> lveModel = CreateCubeModel(device, glm::vec3{ .0f, .0f, .0f });
		auto cube = LitGameObject::CreateGameObject();
		cube.model = lveModel;
		cube.transform.translation = glm::vec3{ .0f, .0f, 2.5f };
		cube.transform.scale = glm::vec3{ .5f, .5f, .5f };
		gameObjects.push_back(std::move(cube));*/

	/*	std::shared_ptr<LitModel> lveModel =
			LitModel::CreateModelFromFile(device, "../models/smooth_vase.obj");
		auto gameObj = LitGameObject::CreateGameObject();
		gameObj.model = lveModel;
		gameObj.transform.translation = glm::vec3{ .0f, .0f, 2.5f };
		gameObj.transform.scale = glm::vec3(3.f);
		gameObjects.push_back(std::move(gameObj));*/

		std::shared_ptr<LitModel> litModel; /*= LitModel::CreateModelFromFile(device, "../models/flat_vase.obj");
		auto flatVase = LitGameObject::CreateGameObject();
		flatVase.model = litModel;
		flatVase.transform.translation = glm::vec3{ -.5f, .5f, 2.5f };
		flatVase.transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		gameObjects.push_back(std::move(flatVase));*/

		litModel = LitModel::CreateModelFromFile(device, "../models/smooth_vase.obj");
		auto smoothVase = LitGameObject::CreateGameObject();
		smoothVase.model = litModel;
		smoothVase.transform.translation = glm::vec3{ .5f, .5f, 2.5f };
		smoothVase.transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		gameObjects.push_back(std::move(smoothVase));
	}
}