#include <algorithm>
#include <sstream>
#include <iomanip>
#include "ftdi_base.hh"
#include <iostream>

using namespace delib;

ftdi_base::ftdi_base (delib::module_id id, int nr, link_type link) { 
  if (ftdi_init (&handle_) < 0) throw std::runtime_error ("ftdi_init failed");

  std::ostringstream serial;
  serial << "DT00" << std::setw (4) << std::setfill ('0') << int(id);

  if (link == A) serial << "A";
  else if (link == B) ftdi_set_interface (&handle_, INTERFACE_B);

  int ret = ftdi_usb_open_desc_index (&handle_, 0x0403, 0x6010, 0, serial.str ().c_str (), nr);
  if (ret < 0) throw std::runtime_error ("ftdi_usb_open_desc_index failed");

  set_baudrate (115200);

  set_latency_timer (2);
}

ftdi_base::~ftdi_base () {
  int ret = ftdi_usb_close (&handle_);
  if (ret < 0) throw std::runtime_error ("ftdi_usb_close failed");

  ftdi_deinit (&handle_);
}

delib::value_t ftdi_base::ping_ (value_t count) { 
  value_t data = cmd_ (oc_ping, count, 0, 0);

  value_t mask = (value_t(1) << (int(d4) * 8)) - 1;
  return data & mask;
}

void ftdi_base::write_ (data_width dw, address_t address, value_t value) { 
  address_t shift = address & (8 - int(dw));
  address_t modifier = ((1 << int(dw)) - 1) << shift;
  value_t data = value << (shift * 8);
  
  cmd_ (oc_send, address, modifier, data);
}

delib::value_t ftdi_base::read_ (data_width dw, address_t address) { 
  address_t shift = address & (8 - int(dw));
  address_t modifier = ((1 << int(dw)) - 1) << shift;

  value_t data = cmd_ (oc_recv, address, modifier, 0);

  value_t mask = (value_t(1) << (int(dw) * 8)) - 1;
  return (data >> (8 * shift)) & mask;
}


void ftdi_base::write_ (address_t address, const matrix_t &matrix, address_t columns, address_t rows) {
  matrix_t tmp = matrix;
  cmd_ (oc_send_block, address, tmp, columns, rows);
}


void ftdi_base::read_ (address_t address, matrix_t &matrix, address_t columns, address_t rows) {
  cmd_ (oc_recv_block, address, matrix, columns, rows);
}

void ftdi_base::set_baudrate (int baudrate) {
  int ret = ftdi_set_baudrate (&handle_, baudrate);
  if (ret < 0) throw std::runtime_error ("ftdi_set_baudrate failed");
}
void ftdi_base::set_line_property (ftdi_bits_type bits, ftdi_stopbits_type sbit, ftdi_parity_type parity) {
  int ret = ftdi_set_line_property (&handle_, bits, sbit, parity);
  if (ret < 0) throw std::runtime_error ("ftdi_set_line_property failed");
}
void ftdi_base::set_latency_timer (uint8_t latency) {
  int ret = ftdi_set_latency_timer  (&handle_, latency);
  if (ret < 0) throw std::runtime_error ("ftdi_set_latency_timer failed");
}


delib::value_t ftdi_base::cmd_ (opcode cmd, address_t address, address_t modifier, value_t data) {
  uint8_t job_id = job_id_inc ();

  if (cmd != oc_send) data = 0;
  uint8_t payload[24];

  std::fill_n (payload, 8, 0xfe);
  payload[8] = 0xff;
  payload[9] = job_id;
  payload[10] = cmd;
  std::copy (reinterpret_cast<uint8_t *>(&address), reinterpret_cast<uint8_t *>(&address) + 4, payload + 11);
  payload[15] = modifier;
  std::copy (reinterpret_cast<uint8_t *>(&data), reinterpret_cast<uint8_t *>(&data) + 8, payload + 16);
  address_t send_cnt = ((cmd == oc_send) ? 24 : 16);

  address_t ret = ftdi_write_data (&handle_, payload, send_cnt);
  if (ret < 0) throw std::runtime_error ("ftdi_write_data");

  address_t read_cnt = 3;
  if (cmd == oc_ping) read_cnt = 7;
  else if (cmd == oc_recv) read_cnt = 11;

  address_t done_cnt = 0;
  for (int i = 0; i < 20; i++) {
    ret = ftdi_read_data (&handle_, payload + done_cnt, read_cnt - done_cnt);
    if (ret < 0) throw std::runtime_error ("ftdi_read_data");

    if (ret == 0) continue;
    done_cnt += ret;

    if (done_cnt >= read_cnt) break;
  }
  if (done_cnt != read_cnt) throw std::runtime_error ("comunication error");

  if (payload[0] != 0x1A || payload[1] != job_id || payload[read_cnt - 1] != 0x4f) throw std::runtime_error ("malformed payload");

  std::copy (payload + 2, payload + 2 + sizeof (value_t), reinterpret_cast<uint8_t *>(&data));

  return data;
}


void ftdi_base::cmd_ (opcode cmd, address_t address, matrix_t &matrix, address_t columns, address_t rows) {
  uint8_t job_id = job_id_inc ();

  std::vector<uint8_t> payload(2 * (columns + 1) * rows + 20, 0);
  
  std::fill_n (payload.begin (), 8, 0xfe);
  payload[8] = 0xff;
  payload[9] = job_id;
  payload[10] = cmd;
  std::copy (reinterpret_cast<uint8_t *>(&address), reinterpret_cast<uint8_t *>(&address) + 4, payload.begin () + 11);
  payload[15] = 0;
  std::copy (reinterpret_cast<uint8_t *>(&columns), reinterpret_cast<uint8_t *>(&columns) + 2, payload.begin () + 16);
  std::copy (reinterpret_cast<uint8_t *>(&rows), reinterpret_cast<uint8_t *>(&rows) + 2, payload.begin () + 18);
  address_t send_cnt = 20;

  if (cmd == oc_send_block) {
    std::copy (matrix.begin (), matrix.begin () + columns * rows, payload.begin () + send_cnt);
    send_cnt += columns * rows;
  }

  address_t ret = ftdi_write_data (&handle_, &payload[0], send_cnt);
  if (ret < 0) throw std::runtime_error ("ftdi_write_data");

  address_t read_cnt = (cmd == oc_send_block) ? 5 : ((columns + 1) * rows) + 3;

  address_t done_cnt = 0;
  for (int i = 0; i < 20; i++) {
    ret = ftdi_read_data (&handle_, &payload[0] + done_cnt, read_cnt - done_cnt);
    if (ret < 0) throw std::runtime_error ("ftdi_read_data");

    if (ret == 0) continue;
    done_cnt += ret;

    if (done_cnt >= read_cnt) break;
  }
  if (done_cnt != read_cnt) throw std::runtime_error ("comunication error");

  if (payload[0] != 0x1A || payload[1] != job_id || payload[read_cnt - 1] != 0x4f) throw std::runtime_error ("malformed payload");

  if (cmd == oc_send_block && (address_t(payload[3]) << 8 | payload[2]) != rows) throw std::runtime_error ("malformed answer");

  for (address_t i = 0; i < rows && cmd == oc_recv_block; i++) {
    if (payload[i * (columns + 1) + 2] != (i & 0xff)) throw std::runtime_error ("malformed answer");
    std::copy (payload.begin () + i * (columns + 1) + 3 , payload.begin () + (i + 1) * (columns + 1) + 3 - 1, matrix.begin () + i * columns);
  }
}

