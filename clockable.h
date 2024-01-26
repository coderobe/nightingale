#ifndef CLOCKABLE_H
#define CLOCKABLE_H

#include <cstddef>

#include <atomic>
using std::atomic;

#include <condition_variable>
using std::condition_variable;

#include <mutex>
using std::mutex;
using std::unique_lock;

#include <chrono>
using std::chrono::milliseconds;


class Clockable {
  public:
    virtual ~Clockable(){};
    virtual void trampoline() = 0;
    virtual void shutdown(){};
    volatile atomic<bool> stopped = false;
    volatile atomic<size_t> clock = 0;
    volatile atomic<int> drift = 1;
    condition_variable synchronous;
    condition_variable drifted;
    void tick(size_t cycles = 0);
    void stop();
    void run();
    void synchronize();
};

#endif