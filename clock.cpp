#include "clock.h"

Clock::~Clock(){
  stop();
}
Clock::Clock(string n){
  name = n;
  change_clock(speed);
}
void Clock::add_device(Clockable* device){
  devices.push_back(device);
}
bool Clock::remove_device(Clockable* device){
  auto elem = find(devices.begin(), devices.end(), device);
  if(elem != devices.end()){
    swap(*elem, devices.back()); // prevent moving all items after `device`
    devices.pop_back();
    return true;
  }else{
    return false;
  }
}
void Clock::stop(){
  if(running && !stopping){
    mutex mut;
    unique_lock<mutex> lock(mut);
    stopping = true;
    while(running) stopped.wait_for(lock, milliseconds(1));
  }
}
void Clock::start(){
  if(!running && !stopping){
    thread clock_thread(&Clock::run, this);
  }
}
void Clock::run(){
  running = true;
  stopping = false;
  while(!stopping){
    sleep_for(nanoseconds(time));
    for(Clockable* device : devices){
      device->tick(1);
      device->synchronize();
    }
  }
  stopped.notify_all();
  running = false;
}
void Clock::change_clock(size_t speed_hz){
  if(speed != speed_hz) cout << "Clock " << name << " changed to " << dec << time << "ns cycle timing (" << speed << "hz)" << endl;
  speed = speed_hz;
  time = static_cast<double>(time_ref) / speed;
}
