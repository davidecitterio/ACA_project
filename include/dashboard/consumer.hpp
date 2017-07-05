#include <iostream>
#include <string>

using namespace std;

void addToFile(int x, int y);

template <typename T>
class SynchronisedQueue;                    //forward declaration of SynchronisedQueue class
void consume(SynchronisedQueue<string> &q);
