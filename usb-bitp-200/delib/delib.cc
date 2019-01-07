#include "delib.hh"

#ifdef DEBUG_DELIB
#include <iostream>
#include <iomanip>
#endif

using namespace delib;

delib::value_t base::ping (value_t count) {
  std::lock_guard<std::mutex> lk(mutex_);
  auto x = ping_ (count);
#ifdef DEBUG_DELIB
  std::cout << "base::ping(" << std::hex << count << "): " << x << std::endl;
#endif
  return x;
}

void base::write (data_width dw, address_t address, value_t value) {
  std::lock_guard<std::mutex> lk(mutex_);
  write_ (dw, address, value); 
#ifdef DEBUG_DELIB
  std::cout << "base::write(" << dw << "," << std::hex << address << "," << std::hex << value << ")" << std::endl;
#endif
}

void base::write (address_t address, const matrix_t &matrix, address_t columns, address_t rows) {
  std::lock_guard<std::mutex> lk(mutex_);
  write_ (address, matrix, columns, rows); 
#ifdef DEBUG_DELIB
  std::cout << "base::write(" << std::hex << address << "," << std::hex << "," columns << "," << rows << ")" << std::endl;
  for (int i = 0; i < buff_len; i++) std::cout << std::hex << int(matrix[i]) << " ";
  std::cout << std::endl;
#endif
}

delib::value_t base::read (data_width dw, address_t address) { 
  std::lock_guard<std::mutex> lk(mutex_);
  auto x = read_ (dw, address); 
#ifdef DEBUG_DELIB
  std::cout << "base::read(" << dw << "," << std::hex << address << "): " << x << std::endl;
#endif
  return x;
}

void base::read (address_t address, matrix_t &matrix, address_t columns, address_t rows) {
  if (matrix.size () < columns * rows) matrix.resize (columns * rows);
  std::lock_guard<std::mutex> lk(mutex_);
  read_ (address, matrix, columns, rows);
#ifdef DEBUG_DELIB
  std::cout << "base::read(" << std::hex << address << "," << std::hex << "," columns << "," << rows << ")" << std::endl;
  for (int i = 0; i < buff_len; i++) std::cout << std::hex << int(matrix[i]) << " ";
  std::cout << std::endl;
#endif
}
