#include <iostream>
#include <string>

#include "dashboard/consumer.hpp"
#include "dashboard/SynchronizedQueue.hpp"

using namespace std;

void consume(SynchronisedQueue<string> &MyQueue) {

  while(true) {
    string value;
    bool success = MyQueue.TryDequeue(value);
    if(success) {
      cout << "\tvalue is " << value << endl;
    }
    else {
      cout << " queue is stopped" << endl;
      break;
    }
  }
  cout << "Que size is : " << MyQueue.Size() <<  endl;
}
