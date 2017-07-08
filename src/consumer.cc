#include <iostream>
#include <string>
#include <chrono>
#include <list>

#include "dashboard/consumer.hpp"
#include "dashboard/SynchronizedQueue.hpp"

#include "dashboard/window.hpp"
#include "dashboard/plotter.hpp"

using namespace std;

//parametrization variables
std::list<string> preamble;
static std::string delimiter;
int howManyDataToPlot = 3;


struct dashboard::gnuplot_commands demo_preamble( void )
{
	struct dashboard::gnuplot_commands result;
	//result.push("set key left box");
	//result.push("set samples ", 50);
	result.push("set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5");
  result.push("set xrange [0 : *]");
  result.push("set yrange [0 : *]");
  result.push("set title \"Simple Plot\" font \",20\"");

  while (!preamble.empty())
    if (strlen(preamble.front().c_str()) > 0){
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
  result.push("plot '< tail -n ",howManyDataToPlot," data.dat' with linespoints ls 1");
	return result;
}


void consume(SynchronisedQueue<string> &MyQueue) {

  loadInitialSetup();
  dashboard::gnuplotter_asynch my_gnuplot_window;
	my_gnuplot_window.start();

	// initialize the plot
	my_gnuplot_window(demo_preamble());
  //show plot
  my_gnuplot_window(data(0,0,true));

  while(true) {
    string value;
    std::string x,y;
    delimiter = ";";
    size_t pos = 0;

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

void addToFile(int x, int y){
  ofstream file;
  string data = ""+std::to_string(y)+" "+std::to_string(x)+"\n";
  file.open("data.dat", std::ios_base::app);
  file << data;
  file.close();
}

void loadInitialSetup(){
  //open xml file and import info about template, delimiter and how many data to plot
}
