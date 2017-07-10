#include "dashboard/SynchronizedQueue.hpp"

using namespace std;

SynchronisedQueue<string> MyQueue;

void produce(char* i)
{
  MyQueue.Enqueue(i);
}
