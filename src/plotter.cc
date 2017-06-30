#include "dashboard/plotter.hpp"
#include <chrono>
#include <iostream>
#include <memory>

static constexpr auto minimum_wait_time_between_plots = std::chrono::milliseconds(0);

namespace dashboard
{

	void gnuplotter_synch::exec( struct gnuplot_commands new_job )
	{
		// this is used to wrap the gnuplot terminal
		std::unique_ptr<struct gnuplot_terminal> term;
		term.reset( new struct gnuplot_terminal());

		// spawn the gnuplot terminal
		term->start();

		// append the quit command
		new_job.push("quit");

		// execute the command
		new_job(*term);

		// term & wait gnuplot
		term.reset();
	}


	void gnuplotter_asynch::start( void )
	{
		// create the command queue
		queue.reset(new plot_queue());

		// create the lambda
		auto dummy_copy_for_gcc = queue;
		const auto worker = [dummy_copy_for_gcc] (void)
		{
			dummy_copy_for_gcc->execute();
		};

		// execute the thread
		plotter.reset(new std::thread(worker));

	}

	gnuplotter_asynch::~gnuplotter_asynch( void )
	{
		queue->send_terminate_signal();
		if (plotter->joinable())
		{
			plotter->join();
		}
	}

	void gnuplotter_asynch::wait( void )
	{
		while(!queue->empty())
		{
			// not so busy waiting
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		// to le gnuplot finish the rendering before quitting
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	void gnuplotter_asynch::operator()( const job_t new_job )
	{
		queue->enqueue(new_job);
	}


	void gnuplotter_asynch::plot_queue::execute()
	{
		// declare the time point needed to slow down the plot thing
		std::chrono::steady_clock::time_point last_execution;


		// infinite loop of the plotter thread
		while( true )
		{
			// this is the new job to be done
			job_t output_job;

			// take the timestamp befere asking a new job
			last_execution = std::chrono::steady_clock::now();

			// lock block
			{
				// get the lock on the operation
				std::unique_lock<std::mutex> lock(operation_mutex);

				// loop until there is some work to do
				while(!signal_terminate && task_queue.empty())
				{
					condition.wait(lock);  // release lock -> wait for a wake_up -> reaquire lock
				}

				// check if the termination condition is met
				if (signal_terminate)
					return;

				// get the first available job
				output_job = task_queue.back();
				task_queue.pop_back();
			}

			// execute the job
			output_job(plotter);

			// make sure that we have spent the minimum amount of time
			const auto elapsed_time = std::chrono::steady_clock::now() - last_execution;
			if (elapsed_time < minimum_wait_time_between_plots)
			{
				std::this_thread::sleep_for(minimum_wait_time_between_plots - elapsed_time);
			}
		}
	}

	void gnuplotter_asynch::plot_queue::enqueue(job_t input_job)
	{
		// get the lock on the operation
		std::unique_lock<std::mutex> lock(operation_mutex);

		// actually enqueue the new element
		task_queue.emplace_front(input_job);

		// wake up an element
		condition.notify_one();
	}
}
