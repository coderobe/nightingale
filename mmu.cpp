#include "mmu.h"

MMU::MMU(){
  write(0xff10, 0x80);
  write(0xff11, 0xbf);
  write(0xff12, 0xf3);
  write(0xff14, 0xbf);
  write(0xff16, 0x3f);
  write(0xff19, 0xbf);
  write(0xff1a, 0x7f);
  write(0xff1b, 0xff);
  write(0xff1c, 0x9f);
  write(0xff1e, 0xbf);
  write(0xff20, 0xff);
  write(0xff23, 0xbf);
  write(0xff24, 0x77);
  write(0xff25, 0xf3);
  write(0xff26, 0xf1);
  write(0xff40, 0x91);
  write(0xff47, 0xfc);
  write(0xff48, 0xff);
  write(0xff49, 0xff);
}
void MMU::write(uint16_t pos, uint8_t val){
  ram[pos] = val;
  for(MMUTrigger* trigger : triggers[pos]){
    trigger->trigger(pos, val);
  }
}
uint8_t MMU::read(uint16_t pos){
  return ram[pos];
}
void MMU::register_trigger(uint16_t pos, MMUTrigger* trigger){
  cout << "Registering MMU trigger at " << hex << setw(4) << setfill('0') << pos << endl;
  triggers[pos].push_back(trigger);
}
