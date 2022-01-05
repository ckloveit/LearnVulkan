#pragma once
#include "LitWindow.h"

namespace Lit
{
	class LitApp
	{
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
	public:
		LitApp() = default;
		LitApp(const LitApp&) = delete;
		LitApp& operator=(const LitApp&) = delete;
		
		void Run();

	private:
		LitWindow litWindow = { WIDTH, HEIGHT, "Hello Vulkan" };
	};
}