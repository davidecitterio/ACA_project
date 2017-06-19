#include <iostream>
#include <chrono>


#include "dashboard/window.hpp"
#include "dashboard/plotter.hpp"


struct dashboard::gnuplot_commands demo_preamble( void )
{
	struct dashboard::gnuplot_commands result;
	result.push("set key left box");
	result.push("set samples ", 50);
	result.push("set style data points");
	return result;
}

struct dashboard::gnuplot_commands data( int i )
{
	struct dashboard::gnuplot_commands result;
	result.push("set title \"Simple Plots ", i, "\" font \",20\"");
	result.push("plot [", -((i+1)*10), ":", ((i+1)*10), "] sin(x),atan(x),cos(atan(x))");
	return result;
}



int main()
{

	dashboard::Window my_window;
	my_window.start();

	struct dashboard::gnuplot_commands plot_commands;
	plot_commands.push("set key left box");
	plot_commands.push("set terminal png");
	plot_commands.push("set output \"demo.png\"");
	plot_commands.push("set samples ", 50);
	plot_commands.push("set style data points");
	plot_commands.push("set title \"Simple Plots 50\" font \",20\"");
	plot_commands.push("plot [1:50] sin(x),atan(x),cos(atan(x))");

	dashboard::gnuplotter_synch::exec(plot_commands);

	dashboard::gnuplotter_asynch my_gnuplot_window;
	my_gnuplot_window.start();

	// initialize the plot
	my_gnuplot_window(demo_preamble());

	for(int i = 0; i < 2000; i++)
	{
		// plot the data using the index of the loop
		my_gnuplot_window(data(i));
	}

	// wait that the queue is complete
	my_gnuplot_window.wait();


	std::cout << "That's all, check for file \"demo.png\"" << std::endl;
}
