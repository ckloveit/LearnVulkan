#include "Core/LitApp.h"

// sys headers
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
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
