#include <list>
#include <map>

using namespace std;

// void addToFile(int x, int y);
// void loadInitialSetup();
void setConfig(map<string, list<string>>);

template <typename T>
class SynchronisedQueue;                    //forward declaration of SynchronisedQueue class
void consume(SynchronisedQueue<string> &q, map<string, list<string>> config);
