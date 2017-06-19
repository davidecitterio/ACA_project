#ifndef DASHBOARD_RENDER_HDR
#define DASHBOARD_RENDER_HDR

#include <deque>
#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <opencv2/opencv.hpp>


namespace dashboard
{

	class RendererInterface
	{
	public:
		virtual void enqueue( const cv::Mat& ) = 0;
		virtual ~RendererInterface() {}
	};



	template< bool zero_latency = false >
	class Renderer: public RendererInterface
	{
		const std::string window_name;
		const std::chrono::milliseconds minimum_wait_time;
		std::chrono::steady_clock::time_point now;

		cv::Mat displayed_image;

		std::thread worker;

		std::mutex queue_mutex;
		std::mutex condition_mutex;
		std::condition_variable rest_in_peace;
		std::deque<cv::Mat> queue;

		bool terminate;

	public:

		Renderer(const std::string window_name, const std::chrono::milliseconds minimum_wait_time)
			:window_name(window_name),minimum_wait_time(minimum_wait_time),now(std::chrono::steady_clock::now()),
			 queue(),terminate(false)
		{
			// create the loading window image
			displayed_image = cv::Mat(120, 500, CV_8UC3, cv::Scalar(51,58,48)); // background black
			cv::putText(displayed_image,                    // where to put the text
				"Loading...",                     // the written text
				cv::Point(10,75),                 // bottom left corner of the text
				cv::FONT_HERSHEY_SCRIPT_COMPLEX,  // the font
				3.0,                              // size of the font
				cv::Scalar(102,196,131)           // the color of text
				);

			// create the window
			cv::namedWindow(window_name);
			cv::imshow(window_name, displayed_image);
			cv::waitKey(100);


			worker = std::thread(&Renderer::loop, this);
		}

		~Renderer( void )
		{
			send_terminate_signal();
			if (worker.joinable())
			{
				worker.join();
			}
			cv::destroyWindow(window_name);
		}

		Renderer( const Renderer& other ) = delete;
		Renderer( const Renderer&& other ) = delete;

		inline void enqueue( const cv::Mat& image )
		{
			std::lock_guard<std::mutex> lg(queue_mutex);
			queue.emplace_back(image.clone());
		}

	private:

		inline bool deque( void )
		{
			std::lock_guard<std::mutex> lg(queue_mutex);
			if (!queue.empty())
			{
				if (zero_latency)
				{
					queue.back().copyTo(displayed_image);
					queue.clear();
				}
				else
				{
					queue.front().copyTo(displayed_image);
					queue.pop_front();
				}
				return true;
			}
			else
			{
				return false;
			}
		}

		inline void loop( void )
		{
			while(work_to_do())
			{
				if (deque())
				{
					cv::imshow(window_name, displayed_image);
					cv::waitKey(10);
				}
			}
		}

		inline bool work_to_do( void )
		{
			std::unique_lock<std::mutex> lock(condition_mutex);
			rest_in_peace.wait_until(lock, now + minimum_wait_time);
			now = std::chrono::steady_clock::now();
			return !terminate;
		}

		inline void send_terminate_signal( void )
		{
			std::unique_lock<std::mutex> lock(condition_mutex);
			terminate = true;
			rest_in_peace.notify_all();
		}

	};
}


#endif
