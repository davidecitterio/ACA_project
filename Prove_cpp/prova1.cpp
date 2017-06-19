#include <boost/thread.hpp>

void enqueue_task() {
  std::cout << "Enqueue started\n";
  for (int i = 0; i < 10; i++) {
    std::cout << "Enqueue: " << i << "\n";
  }
}

void dequeue_task() {
  std::cout << "Dequeue started\n";
  for (int i = 0; i < 10; i++) {
    std::cout << "Dequeue: " << i << "\n";
  }
}

int main(int argc, char* argv[]) {

  boost::thread enqueue_thread = boost::thread(enqueue_task);
  boost::thread dequeue_thread = boost::thread(dequeue_task);

  std::cout << "main waiting...\n";

  enqueue_thread.join();
  dequeue_thread.join();

  std::cout << "main ended\n";
}
