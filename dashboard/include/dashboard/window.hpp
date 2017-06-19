#ifndef DASHBOARD_WINDOW_HDR
#define DASHBOARD_WINDOW_HDR

#include <string>
#include <chrono>
#include <memory>

#include <opencv2/opencv.hpp>

#include "dashboard/render.hpp"


namespace dashboard
{


	/**
	 * @details
	 * This class represent a canvas that the user is able to fill
	 * with its content according to the policy
	 */
	class Window
	{
	private:

		using render_t = std::shared_ptr<RendererInterface>;

		// a reference to the renderer
		render_t interface;

	protected:

		// the name of the window
		const std::string name;

		// this is the scratchpad image which might be drawn
		cv::Mat image;

	public:

		Window(const std::string name = "Dashboard");

		template< bool zero_latency = false >
		void start(const std::chrono::milliseconds refresh_rate = std::chrono::milliseconds(50))
		{
			interface.reset( new Renderer<zero_latency>(name, refresh_rate));
		}
		void stop( void );
		void enqueue( void );

	};





}


#endif // DASHBOARD_WINDOW_HDR
