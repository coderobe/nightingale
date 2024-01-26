#include "clockable.h"

void Clockable::tick(size_t cycles){
  drift += cycles;
  if(drift) drifted.notify_all();
}
void Clockable::stop(){
  stopped = true;
  drifted.notify_all();
}
void Clockable::run(){
  mutex mut;
  unique_lock<mutex> lock(mut);
  while(true){
    while(drift <= 0){
      if(stopped) goto END_RUN;
      drifted.wait_for(lock, milliseconds(100));
    }

    if(drift > 0) trampoline();
    if(drift <= 0) synchronous.notify_all();
  }
  END_RUN:
  shutdown();
}
void Clockable::synchronize(){
  mutex mut;
  unique_lock<mutex> lock(mut);
  while(drift > 0) synchronous.wait_for(lock, milliseconds(1));
}