#include "Core/LitApp.h"

// sys headers
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#if _WINDOWS
#include <Windows.h>
int WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
#else
int main()
#endif
{
	Lit::LitApp app;
	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return EXIT_SUCCESS;
}
