#ifndef MMU_H
#define MMU_H

#include "mmutrigger.h"

#include <cstdint>

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <iomanip>
using std::setw;
using std::setfill;

#include <ios>
using std::hex;

class MMU {
  public:
    MMU();
    void write(uint16_t pos, uint8_t val);
    uint8_t read(uint16_t pos);
    void register_trigger(uint16_t pos, MMUTrigger* trigger);
  private:
    vector<MMUTrigger*> triggers[UINT16_MAX+1];
    uint8_t ram[UINT16_MAX+1] = {0};
};

#endif