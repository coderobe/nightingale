#include "interruptdispatcher.h"

InterruptDispatcher::InterruptDispatcher(MMU* m){
  mmu = m;
  mmu->register_trigger(0xffff, this);
}
void InterruptDispatcher::trigger(uint16_t pos, uint8_t val){
  cout << "MMU triggered InterruptDispatcher via " << hex << setw(4) << pos << " set to 0b" << bitset<8>(val) << endl;
  switch(pos){
    case 0xffff: {
      bool new_transition = val & 0b10000 >> 4;
      transition = new_transition;
      bool new_sio_complete = val & 0b1000 >> 3;
      sio_complete = new_sio_complete;
      bool new_timer_overflow = val & 0b100 >> 2;
      timer_overflow = new_timer_overflow;
      bool new_lcdc = val & 0b10 >> 1;
      lcdc = new_lcdc;
      bool new_vblank = val & 0b1;
      vblank = new_vblank;
    } break;
    default:
      throw runtime_error("Unknown trigger position in InterruptDispatcher");
  }
}
void InterruptDispatcher::interrupt_vblank(){
  mmu->write(0xff0f, mmu->read(0xff0f) | 0b1);
  if(interrupt_queued < 1) interrupt_queued = 1;
}
void InterruptDispatcher::interrupt_lcdc(){
  mmu->write(0xff0f, mmu->read(0xff0f) | 0b10);
  if(interrupt_queued < 2) interrupt_queued = 2;
}
void InterruptDispatcher::interrupt_timer_overflow(){
  mmu->write(0xff0f, mmu->read(0xff0f) | 0b100);
  if(interrupt_queued < 3) interrupt_queued = 3;
}
void InterruptDispatcher::interrupt_sio_complete(){
  mmu->write(0xff0f, mmu->read(0xff0f) | 0b1000);
  if(interrupt_queued < 4) interrupt_queued = 4;
}
void InterruptDispatcher::interrupt_transition(){
  mmu->write(0xff0f, mmu->read(0xff0f) | 0b10000);
  if(interrupt_queued < 5) interrupt_queued = 5;
}
bool InterruptDispatcher::interrupt(Core* cpu){
  if(enabled){
    if(interrupt_queued != 0){
      mmu->write(0xff0f, 0);
      cpu->reg_pc = interrupt_address[interrupt_queued-1];
      interrupt_queued = 0;
      return true;
    }
  }
  interrupt_queued = 0;
  return false;
}
void InterruptDispatcher::state(bool enable){
  enabled = enable;
}
