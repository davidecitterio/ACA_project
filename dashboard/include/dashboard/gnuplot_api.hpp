#ifndef DASHBOARD_GNUPLOT_API_HDR
#define DASHBOARD_GNUPLOT_API_HDR


#include <string>
#include <type_traits>
#include <memory>

#include "dashboard/process_communicator.hpp"
#include "dashboard/string_utility.hpp"

namespace dashboard
{
	struct gnuplot_terminal
	{
		using communicator_ptr = std::shared_ptr<struct process_communicator>;

		void start( void );


		// These functions send the command, wait few millis to let gnuplot process the command
		// then they check for errors and retrieve the output
		template<class ...Y>
		inline std::string operator()( const Y... arguments )
		{
			return send_command(dashboard::compose(arguments...));
		}
		inline std::string operator()( void ) const { return std::string(""); }

		// This function sends ONLY the command
		void send( const std::string line );

		// This function check for errors and retrieves the output
		std::string test_and_receive( void );

	private:

		std::string send_command(const std::string line);

		communicator_ptr terminal;
	};
}


#endif // DASHBOARD_GNUPLOT_API_HDR
