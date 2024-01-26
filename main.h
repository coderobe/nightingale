#ifndef MAIN_H
#define MAIN_H

#include "clockable.h"
#include "clock.h"
#include "mmutrigger.h"
#include "mmu.h"
#include "timercontrol.h"
#include "core.h"
#include "interruptdispatcher.h"
#include "cartridge.h"
#include "bootrom.h"
#include "serialio.h"

#include <csignal>

#include <utility>
using std::pair;

#include <vector>
using std::vector;

#include <thread>
using std::thread;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#endif