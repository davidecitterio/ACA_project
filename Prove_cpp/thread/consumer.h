#include <iostream>
#include <string>

// #include "SynchronizedQueue.h"

using namespace std;

template <typename T>
class SynchronisedQueue {
public:
  void Enqueue(const T& data);
  bool TryDequeue(T& result);
  void StopQueue();
  int Size();
};

void consume(SynchronisedQueue<int>& MyQueue)
{
    while(true)
    {
        int number;

        // cout << "Now try to dequeue" << endl;

        bool success = MyQueue.TryDequeue(number);

        if(success)
        {

            cout << "CONSUME: value is " << number << endl;

        }

        else
        {
            cout << " queue is stopped" << endl;
            break;

        }
    }


    cout << "Que size is : " << MyQueue.Size() <<  endl;
}
