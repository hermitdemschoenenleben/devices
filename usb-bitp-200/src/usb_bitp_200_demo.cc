#include <stdexcept>
#include <iostream>
#include "usb_bitp_200.hh"

// first bit: channel 0-8, memory 0
// 2nd bit: channel 1-9, memory 0
// ...
// ...
// fifth bit??
// sixth bit: channel 0-8, memory 1
// ...

int main(int argc, char* argv[]) try {
  if (argc < 7) {
    std::cout << "not enough arguments" << std::endl;
    return 1;
  }

  //long target_freq = 7000;
  int counter = std::stoi(argv[1]);
  double target_freq = std::stof(argv[2]);
  long base_freq;
  int decimation;
  if (target_freq >= 1500) {
    base_freq = 152000000;
    decimation = 0;
  } else if (target_freq >= 200) {
    base_freq = 19000000;
    decimation = 1;
  } else if (target_freq >= 10) {
    base_freq = 19000000 / 20.2;
    decimation = 76;
  } else if (target_freq >= 0.5) {
    base_freq = 19000000 / 1100;
    decimation = 0x1111;
  } else {
    base_freq = 19000000 / 18000;
    decimation = 0x11111;
  }
  long length = base_freq / target_freq;
  std::cout << "base freq" << base_freq << std::endl;
  std::cout << "length" << length << std::endl;

  /*float cooler_percentage = 0.5;
  float repumper_percentage = 0.5;
  float cooler_start = 0;
  float repumper_start = 0;*/
  float cooler_start = std::stof(argv[3]);
  float cooler_percentage = std::stof(argv[4]);
  float repumper_start = std::stof(argv[5]);
  float repumper_percentage = std::stof(argv[6]);

  std::cout << "frequency: " << (base_freq / length) << "Hz; cooler=" << cooler_start << "+" << cooler_percentage << "; repumper=" << repumper_start << "+" << repumper_percentage << std::endl;

  int memory_lines = length;
  int channels = 6;
  delib::matrix_t buff(memory_lines * channels, 0);

  long cooler_start_idx = length * cooler_start;
  long repumper_start_idx = length * repumper_start;

  long cooler_length_idx = length * cooler_percentage;
  long repumper_length_idx = length * repumper_percentage;

  for (int i=cooler_start_idx; i<cooler_start_idx + cooler_length_idx; i++) {
    buff[(i * 5) + 0] = buff[(i * 5) + 0] | 1;
  }

  // repumper is inverse: fill everything, then set to 0 where repumper should be
  for (int i=0; i<length; i++) {
    buff[(i * 5) + 0] = buff[(i * 5) + 0] | 2;
  }
  for (int i=repumper_start_idx; i<repumper_start_idx + repumper_length_idx; i++) {
    buff[(i * 5) + 0] = buff[(i * 5) + 0] ^ 2;
  }

  usb_bitp_200 bitp(0);

  /*std::cout << "PING" << std::endl;
  for (delib::value_t i = 0; i < 100; i++) std::cout << ((bitp.ping (i) == i) ? "." : "E");
  std::cout << std::endl;*/

  bitp.write_ram (buff, memory_lines * channels);

  //std::cout << "Configuring" << std::endl;
  bitp.stop ();
  bitp.counter (counter);
  bitp.ram_begin (0);
  bitp.ram_end(length);
  bitp.clock_divider (decimation);

  //std::cout << "running " << bitp.counter () << " loops from " << bitp.ram_begin () << " to " << bitp.ram_end () << " with divider set to " << bitp.clock_divider () << std::endl;
  bitp.start ();

  //std::cout << "Status: " << bitp.status () << std::endl;
} catch (std::exception& ex) {
  std::cout << "exception: " << ex.what() << std::endl;
}