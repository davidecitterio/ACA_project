#include <iostream>
#include <string>

#include "SynchronizedQueue.h"

using namespace std;

SynchronisedQueue<int> MyQueue;

void InsertToQueue()
{
    int i= 0;

    while(true)
    {
      boost::this_thread::sleep(boost::posix_time::milliseconds(100));
      MyQueue.Enqueue(++i);
    }

}

void ConsumeFromQueue()
{
    while(true)
    {
        int number;

        // cout << "Now try to dequeue" << endl;

        bool success = MyQueue.TryDequeue(number);

        if(success)
        {

            // cout << "value is " << number << endl;

        }

        else
        {
            cout << " queue is stopped" << endl;
            break;

        }
    }


    cout << "Que size is : " << MyQueue.Size() <<  endl;
}



int main()
{

    cout << "Test Started" << endl;

    boost::thread startInsertIntoQueue = boost::thread(InsertToQueue);
    boost::thread consumeFromQueue = boost::thread(ConsumeFromQueue);

    boost::this_thread::sleep(boost::posix_time::seconds(5)); //After 5 seconds

    MyQueue.StopQueue();

    int endMain;

    cin >> endMain;


    return 0;
}
