#include "dashboard/SynchronizedQueue.hpp"

#include "dashboard/window.hpp"
#include "dashboard/plotter.hpp"

#include "rapidxml/rapidxml.hpp"
#include "dashboard/config.hpp"

using namespace rapidxml;
using namespace std;

//parametrization variables
std::list<string> preamble;
std::string plotSettings = "";
static std::string delimiter;
int howManyDataToPlot = 3;
map<double, std::list<double>> dataToPlot;

void setConfig(map<string, list<string>> config) {
  while (!config["preamble"].empty()){
    preamble.push_back((config["preamble"].front()));
    config["preamble"].pop_front();
  }
  while (!config["plotOption"].empty()){
    plotSettings += config["plotOption"].front() + " ";
    config["plotOption"].pop_front();
  }
  delimiter = config["delimiter"].front();
  howManyDataToPlot = atoi(config["howManyDataToPlot"].front().c_str());
}

void addToFile(double x, std::list<string> data){
  ofstream file;
  string dataToPrint = "";
  dataToPrint += std::to_string(x)+" ";

  while (!data.empty()){
    if (data.front().compare("*") != 0)
      dataToPrint += data.front();
    else
      dataToPrint += "?";
    dataToPrint += " ";
    data.pop_front();
  }
  dataToPrint += "\n";
  file.open("data.dat", std::ios_base::app);
  file << dataToPrint;
  file.close();
}

struct dashboard::gnuplot_commands demo_preamble( void )
{
  struct dashboard::gnuplot_commands result;

  while (!preamble.empty()){
    result.push(preamble.front());
    preamble.pop_front();
  }

  return result;
}

struct dashboard::gnuplot_commands data( double x, std::list<string> data, bool begin )
{
  if (!begin){
    addToFile(x, data);
  }
  struct dashboard::gnuplot_commands result;
  result.push("plot '< sort -nk1 data.dat | tail -n ", howManyDataToPlot ,"' " + plotSettings);
  return result;
}


void consume(SynchronisedQueue<string> &MyQueue, map<string, list<string>> config) {

  setConfig(config);

  dashboard::gnuplotter_asynch my_gnuplot_window;
  my_gnuplot_window.start();

  // initialize the plot
  my_gnuplot_window(demo_preamble());
  //show plot
  my_gnuplot_window(data(0,{},true));

  while(true) {
    string value;
    std::string x;
    std::list<string> dataToPlot;
    std::string::size_type sz;
    size_t pos;

    bool success = MyQueue.TryDequeue(value);
    if(success) {
      x = value.substr(0, value.find(delimiter));
      value.erase (0, value.find(delimiter)+delimiter.length());

      while ((pos = value.find(delimiter)) != std::string::npos){
        dataToPlot.push_back(value.substr(0, pos));
        value.erase(0, pos + delimiter.length());
      }
      //last element
      dataToPlot.push_back(value);

      my_gnuplot_window(data(std::stod(x, &sz), dataToPlot, false));
      dataToPlot.clear();
    }
    else {
      cout << " queue is stopped" << endl;
      break;
    }
  }

  // wait that the queue is complete
  my_gnuplot_window.wait();
  cout << "Que size is : " << MyQueue.Size() <<  endl;
}
