#include <iostream>
#include <string>

#include "../include/consumer.hpp"
#include "../include/SynchronizedQueue.hpp"

using namespace std;

int i = 1;
SynchronisedQueue<int> MyQueue;
// C<int> MyConsumer;

void InsertToQueue(int i)
{
    // int i= 0;

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

int main() {

    cout << "Test Started" << endl;

    boost::thread consumer(consume, std::ref(MyQueue));
    // boost::thread consumer(&MyConsumer.consume, i);

    boost::thread startInsertIntoQueue = boost::thread(InsertToQueue, 37);
    // boost::thread consumeFromQueue = boost::thread(ConsumeFromQueue);

    boost::this_thread::sleep(boost::posix_time::seconds(60)); //After 60 seconds

    MyQueue.StopQueue();

    int endMain;

    cin >> endMain;


    return 0;
}
