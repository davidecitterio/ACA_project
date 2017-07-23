#include "dashboard/SynchronizedQueue.hpp"

#include "dashboard/window.hpp"
#include "dashboard/plotter.hpp"

#include "rapidxml/rapidxml.hpp"
#include "dashboard/config.hpp"

using namespace rapidxml;
using namespace std;

class Consumer {
public:
  Consumer(map<string, list<string>> config, SynchronisedQueue<string> &MyQueue);
  void dump();

private:
  list<string> dataToPlot;
  list<string> preamble;
  string plotSettings = "";
  string id;
  string delimiter;
  int howManyDataToPlot = 10;
  int functionsQuantity = 0;

  void consume(SynchronisedQueue<string> &MyQueue, map<string, list<string>> config);
  void setConfig(map<string, list<string>> config);
  void addToFile(double x, std::list<string> data);
  struct dashboard::gnuplot_commands demo_preamble ( void );
  struct dashboard::gnuplot_commands data( double x, std::list<string> data, bool begin );
};

Consumer::Consumer(map<string, list<string>> config, SynchronisedQueue<string> &MyQueue) {
  boost::thread consumerThread(&Consumer::consume, this, std::ref(MyQueue), config);
}

void Consumer::dump() {
  cout << "ID: " << id << "\nDelimiter: " << delimiter << endl;
}

void Consumer::setConfig(map<string, list<string>> config) {

  while (!config["preamble"].empty()){
    preamble.push_back((config["preamble"].front()));
    config["preamble"].pop_front();
  }
  while (!config["plotOption"].empty()){
    plotSettings += config["plotOption"].front() + ", ";
    config["plotOption"].pop_front();
    functionsQuantity++;  // A single plotOption node for each function to plot
  }
  delimiter = config["delimiter"].front();
  howManyDataToPlot = atoi(config["howManyDataToPlot"].front().c_str());
  id = config["id"].front();
}

void Consumer::addToFile(double x, std::list<string> data){
  //ofstream file;
  string dataToPrint = "";
  dataToPrint += std::to_string(x)+" ";

  while (!data.empty()){
    if (data.front().compare("*") != 0)
      dataToPrint += data.front();
    else
      dataToPrint += "?";
    data.pop_front();
    if (!data.empty())
      dataToPrint += " ";
  }
  dataToPrint += "\n";

  if (dataToPlot.size() >= howManyDataToPlot){
    dataToPlot.pop_front();
    dataToPlot.push_back(dataToPrint);
  }
  else
    dataToPlot.push_back(dataToPrint);
}

struct dashboard::gnuplot_commands Consumer::demo_preamble( void )
{
  struct dashboard::gnuplot_commands result;
  while (!preamble.empty()){
    result.push(preamble.front());
    preamble.pop_front();
  }

  return result;
}

struct dashboard::gnuplot_commands Consumer::data( double x, std::list<string> data, bool begin )
{
  string resultString = "";

  if (!begin){
    addToFile(x, data);
  }
  struct dashboard::gnuplot_commands result;
  std::string dataList;
  for (auto v:dataToPlot)
    dataList += v;

  dataList += "EOF";

  resultString = "plot " + plotSettings;
  for (int i = 0; i < functionsQuantity; i++) {
    resultString += "\n" + dataList;
  }

  result.push(resultString);
  return result;
}


void Consumer::consume(SynchronisedQueue<string> &MyQueue, map<string, list<string>> config) {

  setConfig(config);

  dashboard::gnuplotter_asynch my_gnuplot_window;
  my_gnuplot_window.start();

  // initialize the plot
  my_gnuplot_window(demo_preamble());
  //show plot
  my_gnuplot_window(data(0,{},true));


  while(true) {
    string value;
    string x;
    list<string> dataY;
    string::size_type sz;
    size_t pos;

    bool success = MyQueue.TryDequeueIdDelimiter(value, id, delimiter);
    if(success) {
      if (id == value.substr(0, value.find(delimiter))) {
        value.erase (0, value.find(delimiter)+delimiter.length());

        x = value.substr(0, value.find(delimiter));
        value.erase (0, value.find(delimiter)+delimiter.length());


        while ((pos = value.find(delimiter)) != std::string::npos){
          dataY.push_back(value.substr(0, pos));
          value.erase(0, pos + delimiter.length());
        }
        //last element
        dataY.push_back(value);

        my_gnuplot_window(data(std::stod(x, &sz), dataY, false));
        dataY.clear();
      }
    }
    else {
      break;
    }
  }

  // wait that the queue is complete
  my_gnuplot_window.wait();
}
