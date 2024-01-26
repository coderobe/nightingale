#ifndef BOOTROM_H
#define BOOTROM_H

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

class Bootrom{
  public:
    Bootrom(string path);
    void load_to(MMU* mmu);
  private:
    vector<uint8_t> rom;
};

#endif