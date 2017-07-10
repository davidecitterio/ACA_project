#include <iostream>
#include <chrono>

#include "dashboard/myMosquitto.hpp"
#include "dashboard/config.hpp"
#include "dashboard/consumer.hpp"

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

  // Loading configuration from config.xml
  map<string, list<string>> mosquittoConfig = {{"RootNode", {"MosquittoConfig"}}, {"SettingNode", {"MosquittoSetting"}}, {"clientName", {}}, {"topic", {}}, {"hostAddress", {}}, {"hostPort", {}}, {"QoS", {}}};
  loadConfig(mosquittoConfig);
  myMosq client(mosquittoConfig["clientName"].front().c_str(), mosquittoConfig["topic"].front().c_str(), mosquittoConfig["hostAddress"].front().c_str(), atoi(mosquittoConfig["hostPort"].front().c_str()));  //creating mosquitto client
  client.subscribe(NULL, mosquittoConfig["topic"].front().c_str(), atoi(mosquittoConfig["QoS"].front().c_str()));

  map<string, list<string>> consumerConfig = {{"RootNode", {"PlotConfig"}}, {"SettingNode", {"PlotSetting"}}, {"preamble", {}}, {"delimiter", {}}, {"howManyDataToPlot", {}}};
  loadConfig(consumerConfig);
  setConfig(consumerConfig);
  boost::thread consumer(consume, std::ref(MyQueue));         // starting the consumer thread passing as a parameter a reference to the queue

  boost::this_thread::sleep(boost::posix_time::seconds(60));  // send stop signal to the queue after 60 seconds
  MyQueue.StopQueue();
  client.disconnect();
  consumer.join();

  return 0;
}
