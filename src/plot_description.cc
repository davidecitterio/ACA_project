#include "dashboard/plot_description.hpp"

#include <iostream>

namespace dashboard
{
	std::string gnuplot_commands::operator()( struct gnuplot_terminal plotter ) const
	{
		// execute the command
		for( const auto command : description)
		{
			plotter.send(command);
		}
		return plotter.test_and_receive();
	}

}
