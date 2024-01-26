#ifndef CORE_H
#define CORE_H

class InterruptDispatcher;

#include "clockable.h"
#include "mmu.h"
#include "interruptdispatcher.h"

#include <string>
using std::string;

#include <stdexcept>
using std::runtime_error;

#include <tuple>
using std::tuple;
using std::get;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <ios>
using std::dec;
using std::hex;

#include <iomanip>
using std::setfill;
using std::setw;

class Core: public Clockable {
  public:
    uint8_t reg_a = 0;
    uint8_t reg_b = 0;
    uint8_t reg_c = 0;
    uint8_t reg_d = 0;
    uint8_t reg_e = 0;
    uint8_t reg_f = 0;
    uint8_t reg_h = 0;
    uint8_t reg_l = 0;
    uint8_t reg_flag = 0;
    uint16_t reg_pc = 0;
    uint16_t reg_sp = 0;
    MMU* mmu = nullptr;
    InterruptDispatcher* interrupt = nullptr;
    Core(MMU* m, InterruptDispatcher* i);
    void advance_clock(size_t ticks);
    bool get_flag_z();
    bool get_flag_n();
    bool get_flag_h();
    bool get_flag_c();
    void flag_z(bool value);
    void flag_z_if(uint8_t n);
    void flag_n(bool value);
    void flag_h(bool value);
    void flag_h_carry(int16_t n1, uint8_t n2);
    void flag_h_noborrow(uint8_t n1, uint8_t n2);
    void flag_c(bool value);
    void flag_c_carry(int16_t n1, int16_t n2);
    void flag_c_noborrow(int16_t n1, int16_t n2);
    void trampoline();
    void shutdown();
  private:
    tuple<string,void (*)(Core*, InterruptDispatcher*),size_t> instructions[UINT16_MAX];
};

#endif