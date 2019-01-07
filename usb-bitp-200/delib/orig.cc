#include <stdexcept>
#include "orig.hh"

using namespace delib;

orig::orig (int id, int nr) { 
  handle_ = DapiOpenModule (id, nr); 

  if (!handle_) throw std::runtime_error ("module not found");

}

orig::~orig () { DapiCloseModule (handle_); }

delib::value_t orig::ping_ (value_t count) { return DapiPing (handle_, count); }

void orig::write_ (data_width dw, address_t address, value_t value) { 
  switch (dw) {
    case d1:
      DapiWriteByte (handle_, address, value);
      break;
    case d2:
      DapiWriteWord (handle_, address, value);
      break;
    case d4:
      DapiWriteLong (handle_, address, value);
      break;
    case d8:
      DapiWriteLongLong (handle_, address, value);
      break;
  }
}

void orig::write_ (address_t address, const matrix_t &matrix, address_t columns, address_t rows) {
  DapiWriteMultipleBytes (handle_, address, columns, rows, (unsigned char*)(&matrix[0]), rows * columns);
}

delib::value_t orig::read_ (data_width dw, address_t address) { 
  switch (dw) {
    case d1:
      return DapiReadByte (handle_, address);
      break;
    case d2:
      return DapiReadWord (handle_, address);
      break;
    case d4:
      return DapiReadLong (handle_, address);
      break;
    case d8:
      return DapiReadLongLong (handle_, address);
      break;
  }
  return 0;
}

void orig::read_ (address_t address, matrix_t &matrix, address_t columns, address_t rows) {
  DapiReadMultipleBytes (handle_, address, columns, rows, (unsigned char*)(&matrix[0]), rows * columns);
}
