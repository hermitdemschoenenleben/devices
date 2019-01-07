#ifndef FTDI_BASE_H
#define FTDI_BASE_H
#include "delib.hh"
#include <ftdi.h>

namespace delib {
  class ftdi_base: public base {
    public:
      virtual ~ftdi_base ();

      void set_baudrate (int baudrate);
      void set_line_property (ftdi_bits_type bits, ftdi_stopbits_type sbit, ftdi_parity_type parity);
      void set_latency_timer (uint8_t latency);

    private:
      value_t ping_ (value_t count) override;
      void write_ (data_width dw, address_t address, value_t value) override;
      void write_ (address_t address, const matrix_t &matrix, address_t columns, address_t rows) override;
      value_t read_ (data_width dw, address_t address) override;
      void read_ (address_t address, matrix_t &matrix, address_t columns, address_t rows) override;

      value_t cmd_ (opcode cmd, address_t address, address_t modifier, value_t data);
      void cmd_ (opcode cmd, address_t address, matrix_t &matrix, address_t columns, address_t rows);

      ftdi_context handle_;

    protected:
      enum link_type { A, B, other };
      ftdi_base (delib::module_id id, int nr, link_type link);
  };
}

#endif
