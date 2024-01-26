#include "timercontrol.h"

TimerControl::TimerControl(MMU* m){
  mmu = m;
  mmu->register_trigger(0xff07, this);
  clock_select(clock_selection);
  clock->add_device(this);
}
TimerControl::~TimerControl(){
  delete clock;
}
void TimerControl::trigger(uint16_t pos, uint8_t val){
  cout << "MMU triggered TimerControl via " << hex << pos << " set to 0b" << bitset<8>(val) << endl;
  switch(pos){
    case 0xff07: {
      uint8_t new_clock = val & 0b11;
      if(new_clock != clock_selection){
        clock_selection = new_clock;
        cout << "Timer clock changed to " << clocks[clock_selection] << "hz" << endl;
        clock_select(clock_selection);
      }

      uint8_t new_timer_state = val & 0b100 >> 2;
      if(new_timer_state != timer_state){
        timer_state = new_timer_state;
        cout << "Timer state changed to " << timer_state << endl;
        if(timer_state == 0) clock->stop();
        else clock->start();
      }
    } break;
    default:
      throw runtime_error("Unknown trigger position in TimerControl");
  }
}
void TimerControl::trampoline(){
  // TODO
}
void TimerControl::clock_select(uint8_t selection){
  clock->change_clock(clocks[selection]);
}
