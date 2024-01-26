#ifndef CLOCK_H
#define CLOCK_H

#include "clockable.h"

#include <vector>
using std::vector;

#include <ratio>
using std::nano;

#include <iostream>
using std::cout;
using std::endl;

#include <chrono>
using std::chrono::nanoseconds;

#include <thread>
using std::this_thread::sleep_for;
using std::thread;

#include <algorithm>
using std::find;

#include <utility>
using std::swap;

#include <string>
using std::string;

#include <ios>
using std::dec;

class Clock {
  public:
    ~Clock();
    Clock(string name);
    void add_device(Clockable* device);
    bool remove_device(Clockable* device);
    void stop();
    void start();
    void run();
    void change_clock(size_t speed_hz);
  private:
  	string name;
    size_t speed = 4194300; // hz
    static const intmax_t time_ref = nano::den;
    atomic<double> time = 0;
    atomic<bool> running = false;
    atomic<bool> stopping = true;
    condition_variable stopped;
    vector<Clockable*> devices;
};

#endif