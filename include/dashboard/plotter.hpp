#ifndef DASHBOARD_PLOTTER_HDR
#define DASHBOARD_PLOTTER_HDR

#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <memory>

#include "dashboard/plot_description.hpp"
#include "dashboard/gnuplot_api.hpp"

namespace dashboard
{

	struct gnuplotter_synch
	{
		// execute the related commands (it is blocking)
		static void exec( struct gnuplot_commands new_job );
	};


	struct gnuplotter_asynch
	{
		using job_t = struct gnuplot_commands;

		~gnuplotter_asynch( void );

		// initialize the thread
		void start( void );

		// enqueue the related commands
		void operator()( const job_t new_job );

		// wait until the queue is empty
		void wait( void );

	private:

		class plot_queue
		{
		public:

			plot_queue( void ):signal_terminate(false)
			{
				plotter.start();
			}

			inline std::size_t size( void ) const
			{
				std::unique_lock<std::mutex> lock(operation_mutex);
				return task_queue.size();
			}

			inline bool empty( void ) const
			{
				std::unique_lock<std::mutex> lock(operation_mutex);
				return task_queue.empty();
			}

			inline void send_terminate_signal( void )
			{
				std::unique_lock<std::mutex> lock(operation_mutex);
				signal_terminate = true;
				condition.notify_all();
			}

			inline void clear_terminate_signal( void )
			{
				std::unique_lock<std::mutex> lock(operation_mutex);
				signal_terminate = false;
			}

			/**
			 * @brief wait until there some work available or the thread should terminate
			 */
			void execute( void );

			void enqueue(job_t input_job);

		private:
			bool signal_terminate;
			mutable std::mutex operation_mutex;
			std::condition_variable condition;
			std::deque<job_t> task_queue;
			struct gnuplot_terminal plotter;
		};

		using plot_queue_ptr = std::shared_ptr<plot_queue>;

		plot_queue_ptr queue;
		std::shared_ptr<std::thread> plotter;
	};
}


#endif // VISUALIZER_PLOTTER_HDR
