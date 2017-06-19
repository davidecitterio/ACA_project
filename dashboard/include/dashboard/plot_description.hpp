#ifndef DASHBOARD_PLOT_DESCRIPTOR_HDR
#define DASHBOARD_PLOT_DESCRIPTOR_HDR

#include <string>
#include <vector>

#include "dashboard/string_utility.hpp"
#include "dashboard/gnuplot_api.hpp"

namespace dashboard
{
	struct gnuplot_commands
	{
		using commands_t = std::vector< std::string >;

		std::string operator()( struct gnuplot_terminal plotter ) const;

		template<class ...Y>
		inline void push( const Y... arguments )
		{
			description.emplace_back(dashboard::compose(arguments...));
		}

	private:

		commands_t description;
	};
}




#endif // DASHBOARD_PLOT_DESCRIPTOR_HDR
