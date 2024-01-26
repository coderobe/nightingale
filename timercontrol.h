#ifndef TIMERCONTROL_H
#define TIMERCONTROL_H

#include "mmu.h"
#include "clock.h"
#include "mmutrigger.h"
#include "clockable.h"

#include <cstdint>

#include <iostream>
using std::cout;
using std::endl;

#include <stdexcept>
using std::runtime_error;

#include <ios>
using std::hex;

#include <bitset>
using std::bitset;

class TimerControl: public Clockable, public MMUTrigger {
  public:
    TimerControl(MMU* m);
    ~TimerControl();
    void trigger(uint16_t pos, uint8_t val);
    void trampoline();
  private:
    Clock* clock = new Clock("TimerControl");
    MMU* mmu = nullptr;
    uint8_t clock_selection = 0;
    uint8_t timer_state = 0;
    static constexpr size_t clocks[] = {
      4096000,
      262144000,
      65536000,
      16384000
    };
    void clock_select(uint8_t selection);
};

#endif