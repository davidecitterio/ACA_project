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

void addToFile(double x, double y){
  ofstream file;
  string data = ""+std::to_string(y)+" "+std::to_string(x)+"\n";
  file.open("data.dat", std::ios_base::app);
  file << data;
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

struct dashboard::gnuplot_commands data( double x, double y, bool begin )
{
  if (!begin){
    addToFile(x, y);
  }
  struct dashboard::gnuplot_commands result;
  result.push("plot '< sort -nk1 data.dat | tail -n ",howManyDataToPlot,"'"+ plotSettings);
  return result;
}


void consume(SynchronisedQueue<string> &MyQueue, map<string, list<string>> config) {

  setConfig(config);

  dashboard::gnuplotter_asynch my_gnuplot_window;
  my_gnuplot_window.start();

  // initialize the plot
  my_gnuplot_window(demo_preamble());
  //show plot
  my_gnuplot_window(data(0,0,true));

  while(true) {
    string value;
    std::string x,y;
    std::string::size_type sz;

    bool success = MyQueue.TryDequeue(value);
    if(success) {
      x = value.substr(0, value.find(delimiter));
      y = value.substr(value.find(delimiter)+1, strlen(value.c_str()));
      my_gnuplot_window(data(std::stod(x, &sz), std::stod(y, &sz), false));
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
