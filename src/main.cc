#include <iostream>
#include <chrono>

#include "dashboard/myMosquitto.hpp"
#include "dashboard/config.hpp"
#include "dashboard/consumer.hpp"

#include "boost/program_options.hpp"

int main(int argc, char** argv)
{
  namespace po = boost::program_options;

  string configFile = "config.xml";

  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("config,c", po::value<string>(), "set config file");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("config")) {
      configFile = vm["config"].as<string>();
  }

  // Loading configuration from config.xml
  map<string, list<string>> mainConfig = {{"RootNode", {"MainConfig"}}, {"SettingNode", {"MainSetting"}}, {"execTimeSeconds", {}}, {"windowsQuantity", {}}};
  loadConfig(mainConfig, configFile);
  int execTimeSeconds = atoi(mainConfig["execTimeSeconds"].front().c_str());
  int windowsQuantity = atoi(mainConfig["windowsQuantity"].front().c_str());

  map<string, list<string>> mosquittoConfig = {{"RootNode", {"MosquittoConfig"}}, {"SettingNode", {"MosquittoSetting"}}, {"clientName", {}}, {"topic", {}}, {"hostAddress", {}}, {"hostPort", {}}, {"QoS", {}}};
  loadConfig(mosquittoConfig, configFile);
  myMosq client(mosquittoConfig);  //creating mosquitto client
  client.subscribe(NULL, mosquittoConfig["topic"].front().c_str(), atoi(mosquittoConfig["QoS"].front().c_str()));

  for (int i = 0; i < windowsQuantity; i++) {
    map<string, list<string>> consumerConfig = {{"RootNode", {"PlotConfig_"+to_string(i+1)}}, {"SettingNode", {"PlotSetting"}}, {"preamble", {}}, {"plotOption", {}}, {"delimiter", {}}, {"howManyDataToPlot", {}}, {"id", {}}};
    loadConfig(consumerConfig, configFile);
    new Consumer(consumerConfig, MyQueue);
  }

  if (execTimeSeconds) {
    boost::this_thread::sleep(boost::posix_time::seconds(execTimeSeconds));  // send stop signal to the queue after as specified in the config file
    MyQueue.StopQueue();
    client.disconnect();
  }

  return 0;
}
