#include <iostream>
#include <string>

#include "../include/consumer.hpp"
#include "../include/SynchronizedQueue.hpp"


using namespace std;

// void consume(SynchronisedQueue<int> &MyQueue) {
//   while(true) {
//     int value;
//     // cout << "Now try to dequeue" << endl;
//     bool success = MyQueue.TryDequeue(value);
//     if(success) {
//       cout << "\tvalue is " << value << endl;
//     }
//     else {
//       cout << " queue is stopped" << endl;
//       break;
//     }
//
//    cout << "MERDAAA" << endl;
//    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
//
//   }
//   cout << "Que size is : " << MyQueue.Size() <<  endl;
// }

// void consume(T i) {
  // void consume(SynchronisedQueue<int> &MyQueue) {
  // template <typename T>
  // template<>
  // void C<int>::consume(int i){
  //   for (int n = 0; n < 10; n++) {
  //     boost::this_thread::sleep(boost::posix_time::milliseconds(500));
  //     cout << n << ") " << endl;
  //   }
  // }

  void consume(SynchronisedQueue<string> &MyQueue) {
    for (int n = 0; n < 10; n++) {
      boost::this_thread::sleep(boost::posix_time::milliseconds(100));
      cout << n << ") " << endl;
    }

    while(true) {
      string value;
      // cout << "Now try to dequeue" << endl;
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
