#include "main.h"

class Graphics: public Clockable {
  public:
    Graphics(MMU* m){
      mmu = m;
    }
    void trampoline(){
      //cout << "gpu clock: " << dec << clock << " drift: " << drift << endl;

      clock++;
      drift--;
    }
  private:
    MMU* mmu = nullptr;
};

template <typename t> void create_hardware(vector<pair<thread*,Clockable*>>& list, t component){
  thread* thr = new thread(&Clockable::run, component);
  list.push_back(make_pair(thr, component));
}

vector<pair<thread*,Clockable*>> components;
Clock* emu_clock = nullptr;

void handle_sigint(int s){
  cout << endl;
  emu_clock->stop();
  for(auto hw : components) hw.second->stop();
}

void register_signal(int sig, void(handler)(int)){
  struct sigaction signal_s;
  signal_s.sa_handler = handler;
  sigemptyset(&signal_s.sa_mask);
  signal_s.sa_flags = 0;
  sigaction(sig, &signal_s, NULL);
}

int main(int argc, char** argv){
  register_signal(SIGINT, handle_sigint);

  Bootrom* bootrom = new Bootrom("DMG_ROM.bin");

  if(argc <= 1){
    cerr << "No cartridge specified" << endl;
    return EXIT_FAILURE;
  }
  Cartridge* cart = new Cartridge(argv[1]);
  cout << endl;

  emu_clock = new Clock("Main");

  //debug
  //emu_clock->change_clock(60);

  MMU* mmu = new MMU();
  cart->connect(mmu);
  cart->load();

  TimerControl* timer = new TimerControl(mmu);
  InterruptDispatcher* interrupt = new InterruptDispatcher(mmu);
  SerialIO* serial = new SerialIO(mmu);

  Core* cpu = new Core(mmu, interrupt);
  create_hardware(components, cpu);
  emu_clock->add_device(cpu);

  //set when bootrom is skipped
  cpu->reg_pc = 0x100;
  //bootrom->load_to(mmu);

  Graphics* gpu = new Graphics(mmu);
  create_hardware(components, gpu);
  emu_clock->add_device(gpu);

  cout << "Hardware init done" << endl;

  thread clock_thread(&Clock::run, emu_clock);
  clock_thread.join();
  delete emu_clock;

  cout << "Shutdown" << endl;
  for(auto hw : components){
    hw.first->join();
    delete hw.first;
    cout << "Component stopped" << endl;
    delete hw.second;
  }
  delete timer;
  delete interrupt;
  delete serial;
  delete mmu;
  delete cart;

  cout << "Cleanup done" << endl;
  return EXIT_SUCCESS;
}
