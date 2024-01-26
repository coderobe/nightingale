#include "cartridge.h"

Cartridge::Cartridge(string path){
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
    throw runtime_error("Unable to read cartridge");
  }

  if(rom.size() >= 0x103 && rom[0x100] == 0x00 && rom[0x101] == 0xC3){
    cout << "Gameboy cartridge found" << endl;
  }else{
    throw runtime_error("Not a gameboy cartridge (Magic bytes not found)");
  }

  name = reinterpret_cast<char*>(&rom[0x134]);
  cout << "Cartridge name: " << name << endl;

  cout << "Game type: ";
  switch(rom[0x143]){
    case 0x80:
      cout << "Color game";
      break;
    default:
      cout << "Black & White game";
  }
  cout << endl;

  cout << "Console type: ";
  switch(rom[0x146]){
    case 0x00:
      cout << "Standard GameBoy";
      break;
    case 0x03:
      cout << "Super GameBoy";
      break;
    default:
      cout << "Unknown";
  }
  cout << endl;

  cart_type = rom[0x147];
  string cart_types[] = {
    "ROM ONLY", //0
    "ROM+MBC1", //1
    "ROM+MBC1+RAM", //2
    "ROM+MBC1+RAM+BATTERY", //3
    "UNKNOWN", //4
    "ROM+MBC2", //5
    "ROM+MBC2+BATTERY", //6
    "UNKNOWN" //7
    "ROM+RAM", //8
    "ROM+RAM+BATTERY", //9
    "UNKNOWN", //a
    "ROM+MMM01", //b
    "ROM+MMM01+SRAM", //c
    "ROM+MMM01+SRAM+BATTERY", //d
    "UNKNOWN", //e
    "ROM+MBC3+TIMER+BATTERY", //f
    "ROM+MBC3+TIMER+RAM+BATTERY", //10
    "ROM+MBC3" //11
    "ROM+MBC3+RAM", //12
    "ROM+MBC3+RAM+BATTERY" //13
    "UNKNOWN", //14
    "UNKNOWN", //15
    "UNKNOWN", //16
    "UNKNOWN", //17
    "UNKNOWN", //18
    "ROM+MBC5", //19
    "ROM+MBC5+RAM", //1a
    "ROM+MBC5+RAM+BATTERY", //1b
    "ROM+MBC5+RUMBLE", //1c
    "ROM+MBC5+RUMBLE+SRAM", //1d
    "ROM+MBC5+RUMBLE+SRAM+BATTERY", //1e
    "Pocket Camera" //1f
  };
  cout << "Cartridge type: " << cart_types[cart_type] << endl;

  string rom_sizes[] = {
    "256Kbit = 32KiB = 2 banks",
    "512Kbit = 64KiB = 4 banks",
    "1Mbit = 128KiB = 8 banks",
    "2Mbit = 256KiB = 16 banks",
    "4Mbit = 512KiB = 32 banks",
    "8Mbit = 1MiB = 64 banks",
    "16Mbit = 2MiB = 128 banks"
  };
  cout << "ROM size: " << rom_sizes[rom[0x148]] << endl;

  string ram_sizes[] = {
    "None",
    "16Kbit = 2KiB = 1 bank",
    "64Kbit = 8KiB = 1 bank",
    "256Kbit = 32KiB = 4 banks",
    "1Mbit = 128KiB = 16 banks"
  };
  cout << "RAM size: " << ram_sizes[rom[0x149]] << endl;

  string languages[] = {
    "Japanese",
    "English"
  };
  cout << "Language: " << languages[rom[0x14a]] << endl;

  switch(rom[0x14b]){
    case 0x33:
      manufacturer = reinterpret_cast<char*>(&rom[0x144]);
      break;
    case 0x79:
      manufacturer = "Accolade";
      break;
    case 0xa4:
      manufacturer = "Konami";
      break;
    default:
      manufacturer = "Unknown";
  }
  cout << "Manufacturer: " << manufacturer << endl;

  cout << "Version: " << static_cast<unsigned int>(rom[0x14c]) << endl;

  cout << "Complement check: " << hex << static_cast<unsigned int>(rom[0x14d]) << endl;

  uint16_t* checksum_ptr = reinterpret_cast<uint16_t*>(&rom[0x14e]);
  uint16_t checksum_orig = ((*checksum_ptr & 0xff) << 8) | ((*checksum_ptr & 0xff00) >> 8);       
  cout << "Checksum (rom): " << hex << checksum_orig << endl;

  size_t i = 0;
  size_t checksum_calc = 0;
  for(uint8_t byte : rom){
    if(i != 0x14e && i != 0x14f){
      checksum_calc += byte;
    }
    i++;
  }
  uint16_t checksum = (checksum_calc & 0xffff);
  cout << "Checksum (calculated): " << hex << checksum << endl;
  cout << "Checksum " << (checksum_orig == checksum ? "matches" : "mismatch!") << dec << endl;
  if(checksum_orig != checksum){
    //throw runtime_error("Invalid ROM checksum");
  }
}
void Cartridge::connect(MMU* m){
  mmu = m;
}
void Cartridge::load(){
  cout << "Loading cartridge" << endl;
  size_t i = 0;
  for(uint8_t byte : rom){
    mmu->write(i++, byte);
  }

  if(!remapped) mmu->register_trigger(0xff50, this);
}
void Cartridge::trigger(uint16_t pos, uint8_t val){
  switch(pos){
    case 0xff50: {
      if(!remapped){
        cout << "Triggered cartridge remapping via " << hex << pos << endl;
        remapped = true;
        load();
      }
    } break;
    default:
      throw runtime_error("Unknown trigger pos in Cartridge");
  }
}
