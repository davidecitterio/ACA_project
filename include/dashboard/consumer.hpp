#include <iostream>
#include <string>

using namespace std;

template <typename T>
class SynchronisedQueue;                    //forward declaration of SynchronisedQueue class
void consume(SynchronisedQueue<string> &q);
