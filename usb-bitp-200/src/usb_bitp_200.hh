#ifndef USB_BITP_200_H_
#define USB_BITP_200_H_
#include <memory>
#include "delib.hh"

class usb_bitp_200 {
  public:
    usb_bitp_200 (int device_nr);

    delib::value_t ping (delib::value_t count);

    void start ();
    void stop ();
    void pc_mode ();

    void write_ram (const delib::matrix_t &buff, size_t buffer_length);
    void read_ram (delib::matrix_t &buff, size_t buffer_length);
    void memory_test (size_t memory_lines);

    void reset_counter ();
    void counter (delib::value_t nr);
    void clock_divider (delib::value_t nr);	// T[ns] = (3 + clock_divider) * 2 * 6,6 ns
    void ram_begin (delib::address_t begin);
    void ram_end (delib::address_t end);

    delib::value_t counter ();
    delib::value_t clock_divider ();
    delib::address_t ram_begin ();
    delib::address_t ram_end ();
    
    delib::address_t address ();
    delib::value_t status ();
    delib::value_t ram_sm ();
    delib::value_t counter2 ();

  private:
    void set_cfg_bit (int bit);
    void clear_cfg_bit (int bit);
    void flip_10_cfg_bit (int bit);
    bool check_cfg_bit (int bit);

    delib::address_t ram_end_;

    delib::value_t cfg_reg_;
    std::unique_ptr<delib::base> io_;
};
#endif
