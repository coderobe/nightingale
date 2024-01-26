#ifndef MMUTRIGGER_H
#define MMUTRIGGER_H

#include <cstdint>

class MMUTrigger {
  public:
    virtual ~MMUTrigger() {};
    virtual void trigger(uint16_t pos, uint8_t val) = 0;
};

#endif