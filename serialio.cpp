#include "serialio.h"

SerialIO::SerialIO(MMU* m){
  mmu = m;
  mmu->register_trigger(0xff01, this);
  mmu->register_trigger(0xff02, this);
}
void SerialIO::trigger(uint16_t pos, uint8_t val){
  cout << "MMU triggered SerialIO via " << hex << setw(4) << pos << " set to 0b" << bitset<8>(val) << endl;
  switch(pos){
    case 0xff01: {
      lastbyte = val;
    } break;
    case 0xff02: {
      switch(val){
        case 0x81: // Serial IO received
          cerr << "SerialIO: " << *reinterpret_cast<char*>(&lastbyte) << endl;
        default:
          cout << "Unknown SerialIO event" << endl;
      }
    } break;
    default:
      throw runtime_error("Unknown trigger position in InterruptDispatcher");
  }
}
