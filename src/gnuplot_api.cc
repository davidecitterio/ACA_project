#include <stdexcept>
#include <chrono>
#include <thread>

#include "dashboard/gnuplot_api.hpp"

namespace dashboard
{
	static constexpr auto command_wait_time = std::chrono::milliseconds(5);

	void gnuplot_terminal::start( void )
	{
		terminal.reset(new struct process_communicator());
		terminal->execute("gnuplot");
	}

	void gnuplot_terminal::send( const std::string line )
	{
		// send the command to the communicator
		if (!terminal->send(line + std::string("\n")))
		{
			throw std::runtime_error(std::string("[GNUPLOT TERMINAL] Unable to send characters to the gnuplot terminal (PID:")
								+ std::to_string(terminal->get_process_pid()) + ")");
		}
	}

	std::string gnuplot_terminal::test_and_receive( void )
	{
		// check for errors not warning
		const std::string gnuplot_errors = terminal->receive_error();
		if (gnuplot_errors.find("warning") == std::string::npos &&
		    gnuplot_errors.find("Warning") == std::string::npos &&
		    gnuplot_errors.find("WARNING") == std::string::npos &&
		    !gnuplot_errors.empty())
		{
			throw std::runtime_error(std::string(std::string("[GNUPLOT TERMINAL] Gnuplot error: \"") +
												 gnuplot_errors + std::string("\"")));
		}

		// check for the output
		return terminal->receive_output();
	}



	std::string gnuplot_terminal::send_command(const std::string line)
	{
		send(line);

		// wait for gnuplot to process the command
		std::this_thread::sleep_for(command_wait_time);

		// check for errors and retrieves the output
		return test_and_receive();
	}

}
