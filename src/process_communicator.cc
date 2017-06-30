#include <cerrno>
#include <stdexcept>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>

#include "dashboard/process_communicator.hpp"

namespace dashboard
{
	process_communicator::process_communicator( void )
	{
		input_pipe_fd[0] = -1;
		input_pipe_fd[1] = -1;
		output_pipe_fd[0] = -1;
		output_pipe_fd[1] = -1;
		error_pipe_fd[0] = -1;
		error_pipe_fd[1] = -1;
		child = -1;
		program_name = "N/A";
	}


	void process_communicator::launch_command( struct command_exec command )
	{
		// check if the wrapper is executing a program
		if (child == -1)
		{
			// create the pipes for the standard input and output
			if (pipe2(input_pipe_fd, O_CLOEXEC))
			{
				throw std::runtime_error("[WRAPPER] Pipe creation failed: " + std::string(std::strerror(errno)));
			}
			if (pipe2(output_pipe_fd, O_CLOEXEC))
			{
				throw std::runtime_error("[WRAPPER] Pipe creation failed: " + std::string(std::strerror(errno)));
			}
			if (pipe2(error_pipe_fd, O_CLOEXEC))
			{
				throw std::runtime_error("[WRAPPER] Pipe creation failed: " + std::string(std::strerror(errno)));
			}

			// fork the process
			child = fork();

			// check for errors
			if (child == -1)
			{
				throw std::runtime_error("[WRAPPER] Fork failed: " + std::string(std::strerror(errno)));
			}

			// check if we are the child
			if (child == 0)
			{
				// close the write fd of the input pipe
				if (close(input_pipe_fd[1]) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to close input_pipe: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}

				// close the read fd of the output pipe
				if (close(output_pipe_fd[0]) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to close output_pipe: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}

				// close the read fd of the error pipe
				if (close(error_pipe_fd[0]) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to close error_pipe: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}

				// set the standard input to the pipe file
				if (close(0) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to close the stdinput: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}
				if (dup(input_pipe_fd[0]) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to duplicate the stdinput: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}

				// set the standard output to the pipe file
				if (close(1) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to close the stdoutput: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}
				if (dup(output_pipe_fd[1]) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to duplicate the stdoutput: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}

				// set the standard error to the pipe file
				if (close(2) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to close the stderror: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}
				if (dup(error_pipe_fd[1]) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to duplicate the stderror: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}

				// close the unused file handlers
				if (close(output_pipe_fd[1]) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to close output_pipe2: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}
				if (close(error_pipe_fd[1]) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to close output_pipe2: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}
				if (close(input_pipe_fd[0]) < 0)
				{
					const auto error_message = std::string("[WRAPPER:CHILD] Unable to close input_pipe2: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}

				// call the executable
				command();
				const auto error_message = std::string("[WRAPPER:CHILD] Unable to exec: \"") +
				                           command.get_program_name() + std::string("\": ") +
				                           std::string(std::strerror(errno));
				throw std::runtime_error(error_message);
			}
			else // we are the parent
			{
				// store the program name
				program_name = command.get_program_name();

				// close the read fd of the input pipe
				if (close(input_pipe_fd[0]) < 0)
				{
					const auto error_message = std::string("[WRAPPER] Unable to close input_pipe: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}

				// close the write fd of the output pipe
				if (close(output_pipe_fd[1]) < 0)
				{
					const auto error_message = std::string("[WRAPPER] Unable to close output_pipe: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}

				// close the write fd of the error pipe
				if (close(error_pipe_fd[1]) < 0)
				{
					const auto error_message = std::string("[WRAPPER] Unable to close output_pipe: ") +
					                           std::string(std::strerror(errno));
					throw std::runtime_error(error_message);
				}

				// define the "read" pipes to be non-blocking
				int flags = fcntl(output_pipe_fd[0], F_GETFL, 0);
				fcntl(output_pipe_fd[0], F_SETFL, flags | O_NONBLOCK);
				flags = fcntl(error_pipe_fd[0], F_GETFL, 0);
				fcntl(error_pipe_fd[0], F_SETFL, flags | O_NONBLOCK);
			}
		}
		else // there is already a program in execution
		{
			throw std::runtime_error("[WRAPPER] Unable to execute a new process, configured for \"" +
			                         program_name + "\"");
		}
	}

	process_communicator::~process_communicator( void )
	{
		// close the remaining file descriptor
		close(output_pipe_fd[0]);
		close(error_pipe_fd[0]);
		close(input_pipe_fd[1]);

		// wait for the child
		waitpid(child, NULL, 0);
	}


	bool process_communicator::send( const std::string what ) const
	{
		return write(input_pipe_fd[1], what.c_str(), what.size()) > 0 ? true : false;
	}

	inline std::string read_program_input(int fd)
	{
		std::string result;
		int read_characters;
		char c;
		do
		{
			read_characters = read(fd, &c, sizeof(c));
			if (read_characters > 0)
			{
				result.push_back(c);
			}
		} while(read_characters > 0);
		return result;
	}

	std::string process_communicator::receive_output( void ) const
	{
		return read_program_input(output_pipe_fd[0]);
	}

	std::string process_communicator::receive_error( void ) const
	{
		return read_program_input(error_pipe_fd[0]);
	}
}
