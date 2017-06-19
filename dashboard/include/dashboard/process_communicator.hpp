#ifndef DASHBOARD_PROCESS_COMMUNICATOR_HDR
#define DASHBOARD_PROCESS_COMMUNICATOR_HDR

#include <vector>
#include <string>
#include <functional>
#include <unistd.h>

namespace dashboard
{
	struct process_communicator
	{

		process_communicator( void );
		~process_communicator( void );


		// since the deconstructor close a pipe
		// we cannot just copy this object
		process_communicator( const struct process_communicator& ) = delete;
		process_communicator( const struct process_communicator&& ) = delete;


		template<class T , class ...Y>
		inline void execute( const T program_name, const Y... args )
		{
			const struct command_exec my_command(program_name, args... );
			launch_command(my_command);
		}

		bool send( const std::string what ) const;
		std::string receive_output( void ) const;
		std::string receive_error( void ) const;



		inline pid_t get_process_pid( void ) const
		{
			return child;
		}

	private:

		struct command_exec
		{
			template<class T , class ...Y>
			command_exec( const T program_name, const Y... args ):
				my_exec_call([program_name, args...] (void)
				{
					execlp(program_name, program_name, args..., NULL);
				}
				), program_name(std::string(program_name))
			{}

			inline void operator()( void ) const
			{
				my_exec_call();
			}

			inline std::string get_program_name( void ) const
			{
				return program_name;
			}


		private:

			const std::function<void()> my_exec_call;
			const std::string program_name;
		};


		void launch_command( const struct command_exec command );



		int input_pipe_fd[2];
		int output_pipe_fd[2];
		int error_pipe_fd[2];
		pid_t child;
		std::string program_name;
	};

}


#endif // DASHBOARD_PROCESS_COMMUNICATOR_HDR
