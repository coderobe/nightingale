#include "bootrom.h"

Bootrom::Bootrom(string path){
  ifstream file(path, ios::in|ios::binary);
  if(file.is_open()){
    file.seekg(0, ios::end);
    size_t length = file.tellg();
    file.seekg(0, ios::beg);
    uint8_t byte = 0;
    while(file.tellg() != length){
      file.read(reinterpret_cast<char*>(&byte), 1);
      rom.push_back(byte);
    }
  }else{
    throw runtime_error("Unable to read bootrom at "+path);
  }
}
void Bootrom::load_to(MMU* mmu){
  size_t i = 0;
  for(uint8_t byte : rom){
    mmu->write(i++, byte);
  }
}
