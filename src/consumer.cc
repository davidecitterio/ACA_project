#include <iostream>
#include <string>
#include <chrono>

#include "dashboard/consumer.hpp"
#include "dashboard/SynchronizedQueue.hpp"

#include "dashboard/window.hpp"
#include "dashboard/plotter.hpp"

using namespace std;

struct dashboard::gnuplot_commands demo_preamble( void )
{
	struct dashboard::gnuplot_commands result;
	//result.push("set key left box");
	//result.push("set samples ", 50);
	result.push("set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5");
  result.push("set xrange [0 : *]");
  result.push("set yrange [0 : *]");
	return result;
}

struct dashboard::gnuplot_commands data( int x, int y )
{
  addToFile(x, y);
	struct dashboard::gnuplot_commands result;
	result.push("set title \"Simple Plots (",x,y, ")\" font \",20\"");
	//result.push("plot [", -((i+1)*10), ":", ((i+1)*10), "] ");
  result.push("plot 'data.dat' with linespoints ls 1");
	return result;
}


void consume(SynchronisedQueue<string> &MyQueue) {

  dashboard::gnuplotter_asynch my_gnuplot_window;
	my_gnuplot_window.start();

	// initialize the plot
	my_gnuplot_window(demo_preamble());

/*
	for(int i = 0; i < 2000; i++)
	{
		// plot the data using the index of the loop
		my_gnuplot_window(data(i));
	}
*/


  while(true) {
    string value;
    std::string x,y;
    std::string delimiter = ";";
    size_t pos = 0;

    bool success = MyQueue.TryDequeue(value);
    if(success) {
      x = value.substr(0, value.find(delimiter));
      y = value.substr(value.find(delimiter)+1, strlen(value.c_str()));
      my_gnuplot_window(data(std::stoi(x), std::stoi(y)));
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
