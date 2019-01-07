#ifndef FTDI_DUAL_H
#define FTDI_DUAL_H

#include "ftdi_base.hh"

namespace delib {
  class ftdi_dual: public ftdi_base {
    public:
      ftdi_dual (delib::module_id id, int nr = 0): ftdi_base(id, nr, B) {}
      virtual ~ftdi_dual () {}
  };
}

#endif
