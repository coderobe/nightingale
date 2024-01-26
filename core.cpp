#include "core.h"

Core::Core(MMU* m, InterruptDispatcher* i){
  interrupt = i;
  mmu = m;
  #define INSTR [](Core* cpu, InterruptDispatcher* interrupt)
  instructions[0x0000] = {"NOP", INSTR{
  }, 4};
  instructions[0x00c3] = {"JP a16", INSTR{
    cpu->reg_pc = cpu->mmu->read(cpu->reg_pc++) | cpu->mmu->read(cpu->reg_pc) << 8;
  }, 12};
  instructions[0x00f3] = {"DI", INSTR{
    interrupt->state(false);
  }, 4};
  instructions[0x00fb] = {"EI", INSTR{
    interrupt->state(true);
  }, 4};
  #define INC16(X1,X2) {"INC " #X1 #X2, INSTR{uint16_t val = (cpu->reg_ ## X1 << 8 | cpu->reg_ ## X2) + 1; cpu->reg_ ## X1 = val >> 8; cpu->reg_ ## X2 = val & 0xff;}, 8}
  instructions[0x0003] = INC16(b,c);
  instructions[0x0013] = INC16(d,e);
  instructions[0x0023] = INC16(h,l);
  instructions[0x0033] = {"INC SP", INSTR{cpu->reg_sp++;}, 8};
  #undef INC16
  #define INC(X) {"INC " #X, INSTR{cpu->flag_n(false); cpu->flag_h_carry(cpu->reg_ ## X, 1); cpu->reg_ ## X ++; cpu->flag_z_if(cpu->reg_ ## X);}, 4}
  instructions[0x0004] = INC(b);
  instructions[0x0014] = INC(d);
  instructions[0x0024] = INC(h);
  instructions[0x000c] = INC(c);
  instructions[0x001c] = INC(e);
  instructions[0x002c] = INC(l);
  instructions[0x003c] = INC(a);
  #undef INC
  #define DEC(X) {"DEC " #X, INSTR{cpu->flag_n(false); cpu->flag_h_noborrow(cpu->reg_ ## X, 1); cpu->reg_ ## X ++; cpu->flag_z_if(cpu->reg_ ## X);}, 4}
  instructions[0x0005] = DEC(b);
  instructions[0x0015] = DEC(d);
  instructions[0x0025] = DEC(h);
  instructions[0x000d] = DEC(c);
  instructions[0x001d] = DEC(e);
  instructions[0x002d] = DEC(l);
  instructions[0x003d] = DEC(a);
  #undef DEC
  #define LD(X,Y) {"LD " #X "," #Y, INSTR{cpu->reg_ ## X = cpu->reg_ ## Y ;}, 4}
  instructions[0x0040] = LD(b,b);
  instructions[0x0041] = LD(b,c);
  instructions[0x0042] = LD(b,d);
  instructions[0x0043] = LD(b,e);
  instructions[0x0044] = LD(b,h);
  instructions[0x0045] = LD(b,l);
  instructions[0x0047] = LD(b,a);
  instructions[0x0048] = LD(c,b);
  instructions[0x0049] = LD(c,c);
  instructions[0x004a] = LD(c,d);
  instructions[0x004b] = LD(c,e);
  instructions[0x004c] = LD(c,h);
  instructions[0x004d] = LD(c,l);
  instructions[0x004f] = LD(c,a);
  instructions[0x0050] = LD(d,b);
  instructions[0x0051] = LD(d,c);
  instructions[0x0052] = LD(d,d);
  instructions[0x0053] = LD(d,e);
  instructions[0x0054] = LD(d,h);
  instructions[0x0055] = LD(d,l);
  instructions[0x0057] = LD(d,a);
  instructions[0x0058] = LD(e,b);
  instructions[0x0059] = LD(e,c);
  instructions[0x005a] = LD(e,d);
  instructions[0x005b] = LD(e,e);
  instructions[0x005c] = LD(e,h);
  instructions[0x005d] = LD(e,l);
  instructions[0x005f] = LD(e,a);
  instructions[0x0060] = LD(h,b);
  instructions[0x0061] = LD(h,c);
  instructions[0x0062] = LD(h,d);
  instructions[0x0063] = LD(h,e);
  instructions[0x0064] = LD(h,h);
  instructions[0x0065] = LD(h,l);
  instructions[0x0067] = LD(h,a);
  instructions[0x0068] = LD(l,b);
  instructions[0x0069] = LD(l,c);
  instructions[0x006a] = LD(l,d);
  instructions[0x006b] = LD(l,e);
  instructions[0x006c] = LD(l,h);
  instructions[0x006d] = LD(l,l);
  instructions[0x006f] = LD(l,a);
  instructions[0x0078] = LD(a,b);
  instructions[0x0079] = LD(a,c);
  instructions[0x007a] = LD(a,d);
  instructions[0x007b] = LD(a,e);
  instructions[0x007c] = LD(a,h);
  instructions[0x007d] = LD(a,l);
  instructions[0x007f] = LD(a,a);
  #undef LD
  #define LD_from_direct8(X) {"LD " #X ",d8", INSTR{cpu->reg_ ## X = cpu->mmu->read(cpu->reg_pc++);}, 8}
  instructions[0x0006] = LD_from_direct8(b);
  instructions[0x0016] = LD_from_direct8(d);
  instructions[0x0026] = LD_from_direct8(h);
  instructions[0x000e] = LD_from_direct8(c);
  instructions[0x001e] = LD_from_direct8(e);
  instructions[0x002e] = LD_from_direct8(l);
  instructions[0x003e] = LD_from_direct8(a);
  #undef LD_from_direct8
  #define LD16_from_direct16(X1,X2) {"LD " #X1 #X2 ",d16", INSTR{cpu->reg_ ## X1 = cpu->mmu->read(cpu->reg_pc++); cpu->reg_ ## X2 = cpu->mmu->read(cpu->reg_pc++);}, 12}
  instructions[0x0001] = LD16_from_direct16(b,c);
  instructions[0x0011] = LD16_from_direct16(d,e);
  instructions[0x0021] = LD16_from_direct16(h,l);
  instructions[0x0031] = {"LD SP,d16", INSTR{
    cpu->reg_sp = cpu->mmu->read(cpu->reg_pc++) | cpu->mmu->read(cpu->reg_pc++) << 8;
  }, 12};
  #undef LD16_from_direct16
  #define LD_from_deref8(X) {"LD " #X ",($FF00+d8)", INSTR{printf("%#01x\n",0xff00 + (cpu->mmu->read(cpu->reg_pc+1)));cpu->reg_ ## X = cpu->mmu->read(0xff00 + (cpu->mmu->read(cpu->reg_pc++)));}, 12}
  instructions[0x00f0] = LD_from_deref8(a);
  #undef LD_from_direct8
  #define LD_from_deref16(X,Y1,Y2) {"LD " #X ",(" #Y1 #Y2 ")", INSTR{cpu->reg_ ## X = cpu->mmu->read(cpu->reg_ ## Y1 << 8 | cpu->reg_ ## Y2);}, 8}
  instructions[0x000a] = LD_from_deref16(a, b,c);
  instructions[0x001a] = LD_from_deref16(a, d,e);
  instructions[0x0046] = LD_from_deref16(b, h,l);
  instructions[0x004e] = LD_from_deref16(c, h,l);
  instructions[0x0056] = LD_from_deref16(d, h,l);
  instructions[0x005e] = LD_from_deref16(e, h,l);
  instructions[0x0066] = LD_from_deref16(h, h,l);
  instructions[0x006e] = LD_from_deref16(l, h,l);
  instructions[0x007e] = LD_from_deref16(a, h,l);
  instructions[0x002a] = {"LD a,(hl+)", INSTR{
    cpu->reg_a = cpu->mmu->read(cpu->reg_h << 8 | cpu->reg_l);
    uint16_t val = (cpu->reg_h << 8 | cpu->reg_l) + 1;
    cpu->reg_h = val >> 8;
    cpu->reg_l = val & 0xff;
  }, 8};
  instructions[0x003a] = {"LD a,(hl-)", INSTR{
    cpu->reg_a = cpu->mmu->read(cpu->reg_h << 8 | cpu->reg_l);
    uint16_t val = (cpu->reg_h << 8 | cpu->reg_l) - 1;
    cpu->reg_h = val >> 8;
    cpu->reg_l = val & 0xff;
  }, 8};
  #undef LD_from_deref16
  #define LD_to_deref16(X1,X2,Y) {"LD (" #X1 #X2")," #Y, INSTR{cpu->mmu->write(cpu->reg_ ## X1 << 8 | cpu->reg_ ## X2, cpu->reg_ ## Y );}, 8}
  instructions[0x0002] = LD_to_deref16(b,c, a);
  instructions[0x0012] = LD_to_deref16(d,e, a);
  instructions[0x0070] = LD_to_deref16(h,l, b);
  instructions[0x0071] = LD_to_deref16(h,l, c);
  instructions[0x0072] = LD_to_deref16(h,l, d);
  instructions[0x0073] = LD_to_deref16(h,l, e);
  instructions[0x0074] = LD_to_deref16(h,l, h);
  instructions[0x0075] = LD_to_deref16(h,l, l);
  instructions[0x0077] = LD_to_deref16(h,l, a);
  instructions[0x0022] = {"LD (hl+),a", INSTR{
    cpu->mmu->write(cpu->reg_h << 8 | cpu->reg_l, cpu->reg_a);
    uint16_t val = (cpu->reg_h << 8 | cpu->reg_l) + 1;
    cpu->reg_h = val >> 8;
    cpu->reg_l = val & 0xff;
  }, 8};
  instructions[0x0032] = {"LD (hl-),a", INSTR{
    cpu->mmu->write(cpu->reg_h << 8 | cpu->reg_l, cpu->reg_a);
    uint16_t val = (cpu->reg_h << 8 | cpu->reg_l) - 1;
    cpu->reg_h = val >> 8;
    cpu->reg_l = val & 0xff;
  }, 8};
  #undef LD_to_deref16
  #define PUSH16(X1,X2) {"PUSH " #X1 #X2, INSTR{cpu->mmu->write(cpu->reg_sp--, cpu->reg_ ## X1); cpu->mmu->write(cpu->reg_sp--, cpu->reg_ ## X2);}, 16}
  instructions[0x00c5] = PUSH16(b,c);
  instructions[0x00d5] = PUSH16(d,e);
  instructions[0x00e5] = PUSH16(h,l);
  instructions[0x00f5] = PUSH16(a,f);
  #undef PUSH16
  #define POP16(X1,X2) {"POP " #X1 #X2, INSTR{cpu->reg_ ## X2 = cpu->mmu->read(++cpu->reg_sp); cpu->reg_ ## X1 = cpu->mmu->read(++cpu->reg_sp);}, 12}
  instructions[0x00c1] = POP16(b,c);
  instructions[0x00d1] = POP16(d,e);
  instructions[0x00e1] = POP16(h,l);
  instructions[0x00f1] = POP16(a,f);
  #undef POP16
  #define XOR(X) {"XOR " #X, INSTR{cpu->reg_a ^= cpu->reg_ ## X; cpu->flag_z_if(cpu->reg_a); cpu->flag_n(false); cpu->flag_h(false); cpu->flag_c(false);}, 4}
  instructions[0x00af] = XOR(a);
  instructions[0x00a8] = XOR(b);
  instructions[0x00a9] = XOR(c);
  instructions[0x00aa] = XOR(d);
  instructions[0x00ab] = XOR(e);
  instructions[0x00ac] = XOR(h);
  instructions[0x00ad] = XOR(l);
  #undef XOR
  #define BIT(B,X) {"BIT " #B "," #X, INSTR{cpu->flag_z_if(cpu->reg_ ## X >> B & 1); cpu->flag_n(false); cpu->flag_h(true);}, 8}
  instructions[0xcb40] = BIT(0,b);
  instructions[0xcb41] = BIT(0,c);
  instructions[0xcb42] = BIT(0,d);
  instructions[0xcb43] = BIT(0,e);
  instructions[0xcb44] = BIT(0,h);
  instructions[0xcb45] = BIT(0,l);
  instructions[0xcb47] = BIT(0,a);
  instructions[0xcb48] = BIT(1,b);
  instructions[0xcb49] = BIT(1,c);
  instructions[0xcb4a] = BIT(1,d);
  instructions[0xcb4b] = BIT(1,e);
  instructions[0xcb4c] = BIT(1,h);
  instructions[0xcb4d] = BIT(1,l);
  instructions[0xcb4f] = BIT(1,a);
  instructions[0xcb50] = BIT(2,b);
  instructions[0xcb51] = BIT(2,c);
  instructions[0xcb52] = BIT(2,d);
  instructions[0xcb53] = BIT(2,e);
  instructions[0xcb54] = BIT(2,h);
  instructions[0xcb55] = BIT(2,l);
  instructions[0xcb57] = BIT(2,a);
  instructions[0xcb58] = BIT(3,b);
  instructions[0xcb59] = BIT(3,c);
  instructions[0xcb5a] = BIT(3,d);
  instructions[0xcb5b] = BIT(3,e);
  instructions[0xcb5c] = BIT(3,h);
  instructions[0xcb5d] = BIT(3,l);
  instructions[0xcb5f] = BIT(3,a);
  instructions[0xcb60] = BIT(4,b);
  instructions[0xcb61] = BIT(4,c);
  instructions[0xcb62] = BIT(4,d);
  instructions[0xcb63] = BIT(4,e);
  instructions[0xcb64] = BIT(4,h);
  instructions[0xcb65] = BIT(4,l);
  instructions[0xcb67] = BIT(4,a);
  instructions[0xcb68] = BIT(5,b);
  instructions[0xcb69] = BIT(5,c);
  instructions[0xcb6a] = BIT(5,d);
  instructions[0xcb6b] = BIT(5,e);
  instructions[0xcb6c] = BIT(5,h);
  instructions[0xcb6d] = BIT(5,l);
  instructions[0xcb6f] = BIT(5,a);
  instructions[0xcb70] = BIT(6,b);
  instructions[0xcb71] = BIT(6,c);
  instructions[0xcb72] = BIT(6,d);
  instructions[0xcb73] = BIT(6,e);
  instructions[0xcb74] = BIT(6,h);
  instructions[0xcb75] = BIT(6,l);
  instructions[0xcb77] = BIT(6,a);
  instructions[0xcb78] = BIT(7,b);
  instructions[0xcb79] = BIT(7,c);
  instructions[0xcb7a] = BIT(7,d);
  instructions[0xcb7b] = BIT(7,e);
  instructions[0xcb7c] = BIT(7,h);
  instructions[0xcb7d] = BIT(7,l);
  instructions[0xcb7f] = BIT(7,a);
  #undef BIT
  #define RES(B,X) {"RES " #B "," #X, INSTR{cpu->reg_ ## X &= (0xff ^ (0x1 << B));}, 8}
  instructions[0xcb80] = RES(0,b);
  instructions[0xcb81] = RES(0,c);
  instructions[0xcb82] = RES(0,d);
  instructions[0xcb83] = RES(0,e);
  instructions[0xcb84] = RES(0,h);
  instructions[0xcb85] = RES(0,l);
  instructions[0xcb87] = RES(0,a);
  instructions[0xcb88] = RES(1,b);
  instructions[0xcb89] = RES(1,c);
  instructions[0xcb8a] = RES(1,d);
  instructions[0xcb8b] = RES(1,e);
  instructions[0xcb8c] = RES(1,h);
  instructions[0xcb8d] = RES(1,l);
  instructions[0xcb8f] = RES(1,a);
  instructions[0xcb90] = RES(2,b);
  instructions[0xcb91] = RES(2,c);
  instructions[0xcb92] = RES(2,d);
  instructions[0xcb93] = RES(2,e);
  instructions[0xcb94] = RES(2,h);
  instructions[0xcb95] = RES(2,l);
  instructions[0xcb97] = RES(2,a);
  instructions[0xcb98] = RES(3,b);
  instructions[0xcb99] = RES(3,c);
  instructions[0xcb9a] = RES(3,d);
  instructions[0xcb9b] = RES(3,e);
  instructions[0xcb9c] = RES(3,h);
  instructions[0xcb9d] = RES(3,l);
  instructions[0xcb9f] = RES(3,a);
  instructions[0xcba0] = RES(4,b);
  instructions[0xcba1] = RES(4,c);
  instructions[0xcba2] = RES(4,d);
  instructions[0xcba3] = RES(4,e);
  instructions[0xcba4] = RES(4,h);
  instructions[0xcba5] = RES(4,l);
  instructions[0xcba7] = RES(4,a);
  instructions[0xcba8] = RES(5,b);
  instructions[0xcba9] = RES(5,c);
  instructions[0xcbaa] = RES(5,d);
  instructions[0xcbab] = RES(5,e);
  instructions[0xcbac] = RES(5,h);
  instructions[0xcbad] = RES(5,l);
  instructions[0xcbaf] = RES(5,a);
  instructions[0xcbb0] = RES(6,b);
  instructions[0xcbb1] = RES(6,c);
  instructions[0xcbb2] = RES(6,d);
  instructions[0xcbb3] = RES(6,e);
  instructions[0xcbb4] = RES(6,h);
  instructions[0xcbb5] = RES(6,l);
  instructions[0xcbb7] = RES(6,a);
  instructions[0xcbb8] = RES(7,b);
  instructions[0xcbb9] = RES(7,c);
  instructions[0xcbba] = RES(7,d);
  instructions[0xcbbb] = RES(7,e);
  instructions[0xcbbc] = RES(7,h);
  instructions[0xcbbd] = RES(7,l);
  instructions[0xcbbf] = RES(7,a);
  #undef RES
  // CP n
  instructions[0x00B8] = {"CP B", INSTR{
    uint16_t res = cpu->reg_a - cpu->reg_b;
    cpu->flag_n(true);
    cpu->flag_h_noborrow(cpu->reg_a, res);
    cpu->flag_c_noborrow(cpu->reg_a, res);
    cpu->flag_z_if(res);
  }, 4};
  instructions[0x00FE] = {"CP d8", INSTR{
    uint16_t res = cpu->reg_a - cpu->mmu->read(cpu->reg_pc++);
    cpu->flag_n(true);
    cpu->flag_h_noborrow(cpu->reg_a, res);
    cpu->flag_c_noborrow(cpu->reg_a, res);
    cpu->flag_z_if(res);
  }, 4};

  instructions[0x00e0] = {"LDH (a8),a", INSTR{
    cpu->mmu->write(0xff00 | cpu->mmu->read(cpu->reg_pc++), cpu->reg_a);
  }, 12};
  instructions[0x00ea] = {"LD (a16),a", INSTR{
    cpu->mmu->write(cpu->mmu->read(cpu->reg_pc++) | cpu->mmu->read(cpu->reg_pc++) << 8, cpu->reg_a);
  }, 16};

  instructions[0x00cd] = {"CALL a16", INSTR{
    uint8_t lower_call = cpu->mmu->read(cpu->reg_pc++);
    uint8_t upper_call = cpu->mmu->read(cpu->reg_pc++);
    cpu->mmu->write(cpu->reg_sp--, cpu->reg_pc >> 8);
    cpu->mmu->write(cpu->reg_sp--, cpu->reg_pc);
    cpu->reg_pc = upper_call << 8 | lower_call;
  }, 24};

  instructions[0x00c9] = {"RET", INSTR{
    cpu->reg_pc = cpu->mmu->read(++cpu->reg_sp) | cpu->mmu->read(++cpu->reg_sp) << 8;
  }, 16};

  instructions[0x0018] = {"JR r8", INSTR{
    uint8_t d8 = cpu->mmu->read(cpu->reg_pc++);
    cout << hex << (int)d8 << " -> " << hex << (int)*reinterpret_cast<int8_t*>(&d8) << endl;
    cpu->reg_pc += *reinterpret_cast<int8_t*>(&d8);
  }, 12};
  instructions[0x0020] = {"JR NZ,r8", INSTR{
    if(cpu->get_flag_z()) return;
    cpu->advance_clock(4);
    uint8_t d8 = cpu->mmu->read(cpu->reg_pc++);
    cpu->reg_pc += *reinterpret_cast<int8_t*>(&d8);
  }, 8};
  instructions[0x0028] = {"JR Z,r8", INSTR{
    if(!cpu->get_flag_z()) return;
    cpu->advance_clock(4);
    uint8_t d8 = cpu->mmu->read(cpu->reg_pc++);
    cpu->reg_pc += *reinterpret_cast<int8_t*>(&d8);
  }, 8};
  instructions[0x0030] = {"JR NC,r8", INSTR{
    if(cpu->get_flag_c()) return;
    cpu->advance_clock(4);
    uint8_t d8 = cpu->mmu->read(cpu->reg_pc++);
    cpu->reg_pc += *reinterpret_cast<int8_t*>(&d8);
  }, 8};
  instructions[0x0038] = {"JR C,r8", INSTR{
    if(!cpu->get_flag_c()) return;
    cpu->advance_clock(4);
    uint8_t d8 = cpu->mmu->read(cpu->reg_pc++);
    cpu->reg_pc += *reinterpret_cast<int8_t*>(&d8);
  }, 8};

  instructions[0x0076] = {"HALT", INSTR{
    cerr << "Halt & Catch Fire" << endl;
    throw runtime_error("HALT implementation unfinished");
  }, 4};
  instructions[0x0010] = {"STOP", INSTR{
    cerr << "STOP" << endl;
    throw runtime_error("STOP implementation unfinished");
  }, 4};

  instructions[0x0011] = {"LD DE,d16", INSTR{
    cpu->reg_e = cpu->mmu->read(cpu->reg_pc++);
    cpu->reg_d = cpu->mmu->read(cpu->reg_pc++);
  }, 12};

  instructions[0x0090] = {"SUB B", INSTR{
    cpu->flag_n(true);
    cpu->flag_h_noborrow(cpu->reg_a, cpu->reg_b);
    cpu->flag_c_noborrow(cpu->reg_a, cpu->reg_b);
    cpu->reg_a -= cpu->reg_b;
    cpu->flag_z_if(cpu->reg_a);
  }, 4};
  #undef INSTR
}
void Core::advance_clock(size_t ticks){
  clock += ticks;
  drift -= ticks;
}
bool Core::get_flag_z(){
  return reg_flag >> 7 & 1;
}
bool Core::get_flag_n(){
  return reg_flag >> 6 & 1;
}
bool Core::get_flag_h(){
  return reg_flag >> 5 & 1;
}
bool Core::get_flag_c(){
  return reg_flag >> 4 & 1;
}
void Core::flag_z(bool value){ //zero
  if(value) reg_flag |= 0b10000000;
  else reg_flag &= 0b01111111;
}
void Core::flag_z_if(uint8_t n){
  if(n == 0) flag_z(true);
  else flag_z(false);
}
void Core::flag_n(bool value){ //subtract
  if(value) reg_flag |= 0b01000000;
  else reg_flag &= 0b10111111;
}
void Core::flag_h(bool value){ //half carry
  if(value) reg_flag |= 0b00100000;
  else reg_flag &= 0b11011111;
}
void Core::flag_h_carry(int16_t n1, uint8_t n2){
  if((n1 & 0xf) + (n2 & 0xf) == 0x10) flag_h(true);
  else flag_h(false);
}
void Core::flag_h_noborrow(uint8_t n1, uint8_t n2){
  // todo
}
void Core::flag_c(bool value){ //carry
  if(value) reg_flag |= 0b00010000;
  else reg_flag &= 0b11101111;
}
void Core::flag_c_carry(int16_t n1, int16_t n2){
  if(((n1 + n2) & 0xff00) != 0) flag_c(true);
  else flag_c(false);
}
void Core::flag_c_noborrow(int16_t n1, int16_t n2){
  // todo: check
  return flag_c_carry(n1, n2);
}
void Core::trampoline(){
  //TODO: remove
  cout << "cpu clock: " << dec << clock << " drift: " << drift << endl;
  //
  uint16_t op = static_cast<uint16_t>(mmu->read(reg_pc++));
  if(op == 0xcb){
    op <<= 8;
    op |= mmu->read(reg_pc++);
  }
  tuple<string,void (*)(Core*, InterruptDispatcher*),size_t> instruction = instructions[op];
  if(get<1>(instruction)){
    cout << hex << setw(4) << setfill('0') << reg_pc-1 << ": [" << setw(4) << setfill('0') << op << "] " << get<0>(instruction) << endl;
    get<1>(instruction)(this, interrupt);
    advance_clock(get<2>(instruction));
  }else{
    cerr << "Invalid operation " << hex << setw(4) << setfill('0') << op << " at " << hex << setw(4) << setfill('0') << reg_pc-1 << endl;
    throw runtime_error("Invalid opcode");
  }
  if(interrupt->interrupt(this)) cout << "cpu interrupted" << endl;
}
void Core::shutdown(){
  cout << "Core stopped" << endl;
}
