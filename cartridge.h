#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "mmu.h"

#include <cstdint>

#include <string>
using std::string;

#include <fstream>
using std::ifstream;

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

#include <ios>
using std::ios;
using std::dec;
using std::hex;

#include <stdexcept>
using std::runtime_error;

class Cartridge: public MMUTrigger{
  public:
    Cartridge(string path);
    void connect(MMU* m);
    void load();
    void trigger(uint16_t pos, uint8_t val);
  private:
  	bool remapped = false;
  	MMU* mmu = nullptr;
    string name;
    string manufacturer;
    uint8_t cart_type;
    vector<uint8_t> rom;
};

#endif