#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
bool is_first_thread_done = false;

void count_up() {
    for (int i = 1; i <= 20; ++i) {
        std::cout << "Counting Up: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // delay for visibility.. optional
    }
    // signal first thread done
    {
        std::lock_guard<std::mutex> lock(mtx);
        is_first_thread_done = true;
    }
    cv.notify_one();
}

void count_down() {
    // wait til first thread signals done
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return is_first_thread_done; });
    }
    for (int i = 20; i >= 0; --i) {
        std::cout << "Counting Down: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // delay for visibility.. optional
    }
}

int main() {
    std::thread thread1(count_up);
    std::thread thread2(count_down);

    // wait til both threads done
    thread1.join();
    thread2.join();

    return 0;
}