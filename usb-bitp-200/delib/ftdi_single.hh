#ifndef FTDI_SINGLE_H
#define FTDI_SINGLE_H

#include "ftdi_base.hh"

namespace delib {
  class ftdi_single: public ftdi_base {
    public:
      ftdi_single (delib::module_id id, int nr = 0): ftdi_base(id, nr, A) {}
      virtual ~ftdi_single () {}
  };
}

#endif
