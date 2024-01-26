#ifndef SERIALIO_H
#define SERIALIO_H

#include "mmutrigger.h"
#include "mmu.h"
#include "core.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <stdexcept>
using std::runtime_error;

#include <ios>
using std::hex;

#include <iomanip>
using std::setw;

class SerialIO: public MMUTrigger {
  public:
    SerialIO(MMU* m);
    void trigger(uint16_t pos, uint8_t val);
  private:
    uint8_t lastbyte = 0;
    MMU* mmu = nullptr;
};

#endif