#ifndef DELIB_H
#define DELIB_H
#include <stdint.h>
#include <stddef.h>
#include <mutex>
#include <vector>

namespace delib {
    enum data_width { d1 = sizeof (uint8_t), d2 = sizeof (uint16_t), d4 = sizeof (uint32_t), d8 = sizeof (uint64_t) };
#ifndef ORIG_PRESENT
    enum module_id {
      USB_Interface8 = 1,
      USB_CAN_STICK = 2,
      USB_LOGI_500 = 3,
      RO_USB2 = 4,
      RO_SER = 5,
      USB_BITP_200 = 6,
      RO_USB1 = 7,
      RO_USB = 7,
      RO_ETH = 8,
      USB_MINI_STICK = 9,
      USB_LOGI_18 = 10,
      RO_CAN = 11,
      USB_SPI_MON = 12,
      USB_WATCHDOG = 13,
      USB_OPTOIN_8 = 14,
      USB_RELAIS_8 = 14,
      USB_OPTOIN_8_RELAIS_8 = 15,
      USB_OPTOIN_16_RELAIS_16 = 16,
      USB_OPTOIN_32 = 16,
      USB_RELAIS_32 = 16,
      USB_OPTOIN_32_RELAIS_32 = 17,
      USB_OPTOIN_64 = 17,
      USB_RELAIS_64 = 17,
      USB_TTL_32 = 18,
      USB_TTL_64 = 18,
    };
#else
    typedef int module_id;
#endif
    typedef uint32_t address_t;
    typedef uint64_t value_t;
    typedef uint8_t byte_t;
    typedef std::vector<byte_t> matrix_t;

    class base {
      public:
	base (): job_id_(1) {}
	virtual ~base () {}

	value_t ping (value_t count);

	void write (data_width dw, address_t address, value_t value);
	void write (address_t address, const matrix_t &matrix, address_t columns, address_t rows);

	value_t read (data_width dw, address_t address);
	void read (address_t address, matrix_t &matrix, address_t columns, address_t rows);

	int job_id () const { return job_id_; }
      private:
	virtual value_t ping_ (value_t count) = 0;
	virtual void write_ (data_width dw, address_t address, value_t value) = 0;
	virtual void write_ (address_t address, const matrix_t &matrix, address_t columns, address_t rows) = 0;
	virtual value_t read_ (data_width dw, address_t address) = 0;
	virtual void read_ (address_t address, matrix_t &matrix, address_t columns, address_t rows) = 0;

	int job_id_;
	std::mutex mutex_;

      protected:
	int job_id_inc () { return job_id_++; }

	enum opcode {
	  oc_ping = 0x12,
	  oc_send = 0x23,
	  oc_recv = 0x34,
	  oc_send_block = 0x45,
	  oc_recv_block = 0x56,
	};
    };
}

#endif
