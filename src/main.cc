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
  map<string, list<string>> mainConfig = {{"RootNode", {"MainConfig"}}, {"SettingNode", {"MainSetting"}}, {"execTimeSeconds", {}}};
  loadConfig(mainConfig);
  int execTimeSeconds = atoi(mainConfig["execTimeSeconds"].front().c_str());
  cout << "execTimeSeconds: " << execTimeSeconds << endl;

  map<string, list<string>> mosquittoConfig = {{"RootNode", {"MosquittoConfig"}}, {"SettingNode", {"MosquittoSetting"}}, {"clientName", {}}, {"topic", {}}, {"hostAddress", {}}, {"hostPort", {}}, {"QoS", {}}};
  loadConfig(mosquittoConfig);
  myMosq client(mosquittoConfig);  //creating mosquitto client
  client.subscribe(NULL, mosquittoConfig["topic"].front().c_str(), atoi(mosquittoConfig["QoS"].front().c_str()));

  map<string, list<string>> consumerConfig = {{"RootNode", {"PlotConfig"}}, {"SettingNode", {"PlotSetting"}}, {"preamble", {}}, {"plotOption", {}}, {"delimiter", {}}, {"howManyDataToPlot", {}}};
  loadConfig(consumerConfig);
  boost::thread consumer(consume, std::ref(MyQueue), consumerConfig);         // starting the consumer thread passing as a parameter a reference to the queue

  if (execTimeSeconds) {
    boost::this_thread::sleep(boost::posix_time::seconds(execTimeSeconds));  // send stop signal to the queue after as specified in the config file
    MyQueue.StopQueue();
    client.disconnect();
  }

  consumer.join();
  return 0;
}
