#include <iostream>
#include <chrono>

#include "dashboard/consumerMain.hpp"

int main()
{
 /*
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
  */


  //std::cout << "That's all, check for file \"demo.png\"" << std::endl;

  // my stuff
  cout << "Test Started" << endl;

  myMosq client("myProva", "home/prova", "127.0.0.1", 1883);  //creating mosquitto client
  client.subscribe(NULL, "home/prova", 1);

  boost::thread consumer(consume, std::ref(MyQueue));         // starting the consumer thread passing as a parameter a reference to the queue

  boost::this_thread::sleep(boost::posix_time::seconds(60));  // send stop signal to the queue after 60 seconds
  MyQueue.StopQueue();
  client.disconnect();
  consumer.join();

  return 0;
}
