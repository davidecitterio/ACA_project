#include "dashboard/plotter.hpp"
#include <list>
#include <map>

using namespace std;

template <typename T>
class SynchronisedQueue;        //forward declaration of SynchronisedQueue class


class Consumer {
public:
  Consumer(map<string, list<string>> config, SynchronisedQueue<string> &MyQueue);
  void dump();

private:
  list<string> dataToPlot;
  list<string> preamble;
  string plotSettings;
  string id;
  string delimiter;
  int howManyDataToPlot;
  int functionsQuantity;

  void consume(SynchronisedQueue<string> &MyQueue, map<string, list<string>> config);
  void setConfig(map<string, list<string>> config);
  void addToFile(double x, std::list<string> data);
  struct dashboard::gnuplot_commands demo_preamble ( void );
  struct dashboard::gnuplot_commands data( double x, std::list<string> data, bool begin );
};
