#include "dashboard/SynchronizedQueue.hpp"

#include "dashboard/window.hpp"
#include "dashboard/plotter.hpp"

#include "rapidxml/rapidxml.hpp"
#include "dashboard/config.hpp"

using namespace rapidxml;
using namespace std;

//parametrization variables
std::list<string> preamble;
static std::string delimiter;
int howManyDataToPlot = 3;

void setConfig(map<string, list<string>> config) {
  while (!config["preamble"].empty()){
    preamble.push_back((config["preamble"].front()));
    config["preamble"].pop_front();
  }
  delimiter = config["delimiter"].front();
  howManyDataToPlot = atoi(config["howManyDataToPlot"].front().c_str());
}

void addToFile(int x, int y){
  ofstream file;
  string data = ""+std::to_string(y)+" "+std::to_string(x)+"\n";
  file.open("data.dat", std::ios_base::app);
  file << data;
  file.close();
}

struct dashboard::gnuplot_commands demo_preamble( void )
{
  struct dashboard::gnuplot_commands result;
  //result.push("set key left box");
  //result.push("set samples ", 50);
  //result.push("set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5");
  //result.push("set xrange [0 : *]");
  //result.push("set yrange [0 : *]");
  //result.push("set title \"Simple Plot\" font \",20\"");

  while (!preamble.empty()){
    result.push(preamble.front());
    preamble.pop_front();
  }

  return result;
}

struct dashboard::gnuplot_commands data( int x, int y, bool begin )
{
  if (!begin){
    addToFile(x, y);
  }
  struct dashboard::gnuplot_commands result;
  result.push("plot '< tail -n ",howManyDataToPlot," data.dat' with lines ls 1");
  return result;
}


void consume(SynchronisedQueue<string> &MyQueue) {

  dashboard::gnuplotter_asynch my_gnuplot_window;
  my_gnuplot_window.start();

  // initialize the plot
  my_gnuplot_window(demo_preamble());
  //show plot
  my_gnuplot_window(data(0,0,true));

  while(true) {
    string value;
    std::string x,y;

    bool success = MyQueue.TryDequeue(value);
    if(success) {
      x = value.substr(0, value.find(delimiter));
      y = value.substr(value.find(delimiter)+1, strlen(value.c_str()));
      my_gnuplot_window(data(std::stoi(x), std::stoi(y), false));
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
