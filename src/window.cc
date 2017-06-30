#include "dashboard/window.hpp"


namespace dashboard
{


	Window::Window(const std::string name):interface(nullptr),name(name)
	{
	}

	void Window::enqueue( void )
	{
		interface->enqueue(image);
	}

	void Window::stop( void )
	{
		interface.reset();
	}

}
