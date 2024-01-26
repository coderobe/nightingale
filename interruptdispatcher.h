#ifndef INTERRUPTDISPATCHER_H
#define INTERRUPTDISPATCHER_H

class Core;

#include "mmutrigger.h"
#include "mmu.h"
#include "core.h"

#include <iostream>
using std::cout;
using std::endl;

#include <stdexcept>
using std::runtime_error;

#include <iomanip>
using std::setw;

#include <ios>
using std::hex;

#include <bitset>
using std::bitset;

class InterruptDispatcher: public MMUTrigger {
  public:
    InterruptDispatcher(MMU* m);
    void trigger(uint16_t pos, uint8_t val);
    void interrupt_vblank();
    void interrupt_lcdc();
    void interrupt_timer_overflow();
    void interrupt_sio_complete();
    void interrupt_transition();
    bool interrupt(Core* cpu);
    void state(bool enable);
  private:
    bool enabled = false;
    static constexpr uint16_t interrupt_address[] = {
      0x40,
      0x48,
      0x50,
      0x58,
      0x60
    };
    uint8_t interrupt_queued = 0;
    bool transition = false;
    bool sio_complete = false;
    bool timer_overflow = false;
    bool lcdc = false;
    bool vblank = false;
    MMU* mmu = nullptr;
};

#endif