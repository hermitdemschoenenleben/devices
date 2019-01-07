#include <algorithm>
#include <sstream>
#include "usb_bitp_200.hh"
#if ORIG_PRESENT
#include "orig.hh"
#endif
#include "ftdi_dual.hh"

using namespace delib;

usb_bitp_200::usb_bitp_200 (int nr): cfg_reg_(0), ram_end_(0) {
  //io_ = new orig (USB_BITP_200, nr);
  io_ = std::unique_ptr<base>(new ftdi_dual (USB_BITP_200, 0));
}

void usb_bitp_200::set_cfg_bit (int bit) {
  cfg_reg_ |= (1 << bit);
  io_->write (delib::d4, 0, cfg_reg_);
}

void usb_bitp_200::clear_cfg_bit (int bit) {
  cfg_reg_ &= ~(1 << bit);
  io_->write (delib::d4, 0, cfg_reg_);
}

void usb_bitp_200::flip_10_cfg_bit (int bit) {
  set_cfg_bit (bit);
  clear_cfg_bit (bit);
}

bool usb_bitp_200::check_cfg_bit (int bit) { return io_->read (delib::d4, 0) & (1 << bit); }


delib::value_t usb_bitp_200::ping (delib::value_t count) { return io_->ping (count); }

void usb_bitp_200::start () { flip_10_cfg_bit (0); }

void usb_bitp_200::stop () { flip_10_cfg_bit (1); }

void usb_bitp_200::pc_mode () { flip_10_cfg_bit (2); }

void usb_bitp_200::reset_counter () { flip_10_cfg_bit (5); }

void usb_bitp_200::counter (delib::value_t nr) {
  if (!nr) set_cfg_bit (3);
  else {
    io_->write (delib::d4, 0x18, nr);
    clear_cfg_bit (3);
  }
}

void usb_bitp_200::clock_divider (delib::value_t nr) {
  if (!nr) {
    set_cfg_bit (4);
    if (ram_end_) ram_end (ram_end_);
  } else {
    clear_cfg_bit (4);
    io_->write (delib::d4, 0x1C, nr << 1);
  }
}  

void usb_bitp_200::ram_begin (delib::address_t begin) {
  io_->write (delib::d4, 0x10, begin >> 1);
}

void usb_bitp_200::ram_end (delib::address_t end) {
  ram_end_ = end;
  if (!clock_divider ()) end += 2;
  io_->write (delib::d4, 0x14, end >> 1);
}

delib::value_t usb_bitp_200::counter () { return check_cfg_bit (3) ? 0 : io_->read (delib::d4, 0x18); }

delib::value_t usb_bitp_200::clock_divider () { return check_cfg_bit (4) ? 0 : io_->read (delib::d4, 0x1C) >> 1; }

delib::address_t usb_bitp_200::ram_begin () { return io_->read (delib::d4, 0x10) << 1; }

delib::address_t usb_bitp_200::ram_end () { return (io_->read (delib::d4, 0x14) << 1) + (clock_divider () ? 0 : -2); }

delib::value_t usb_bitp_200::status () { return 0xf & io_->read (delib::d4, 0xe8); }

delib::address_t usb_bitp_200::address () { return 0xf0 & io_->read (delib::d4, 0xf0); }

delib::value_t usb_bitp_200::ram_sm () { return 0xff & io_->read (delib::d4, 0xe9); }

delib::value_t usb_bitp_200::counter2 () { return io_->read (delib::d4, 0xd8); }


void usb_bitp_200::write_ram (const delib::matrix_t &buff, size_t buffer_length) {
  stop ();

  reset_counter ();

  io_->write (0x60, buff, 10, buffer_length / 10);
}



void usb_bitp_200::read_ram (delib::matrix_t &buff, size_t buffer_length) {
  reset_counter ();

  io_->read (delib::d4, 0x68);

  io_->read (0x60, buff, 10, buffer_length / 10);
}

void usb_bitp_200::memory_test (size_t memory_lines) {
  stop ();
  matrix_t buff(memory_lines * 5);
  matrix_t buff2(memory_lines * 5);

  for (size_t i = 0; i < memory_lines; i++) {
    for (size_t j = 0; j < 5; j++) buff[i * 5 + j] = (i & 0xff) + 1 + j;
    buff [i * 5 + 4] &= 0xf;
  }

  write_ram (buff, memory_lines * 5);

  read_ram (buff2, memory_lines * 5);

  if (!std::equal (buff.begin (), buff.begin () + memory_lines * 5, buff2.begin ())) {
    auto x = std::mismatch (buff.begin (), buff.begin () + memory_lines * 5, buff2.begin ());
    std::ostringstream m;
    m << "memory mismatch at position " << x.first - buff.begin () << " values " << int(*x.first) << "!=" << int(*x.second);
    throw std::runtime_error (m.str ());
  }
}
