OBJ=delib/delib_io_seriellFTDI.o delib/delib_Dapi_di_do_ad_da_befehle-usb.o delib/delib_set_get_error.o delib/delib_Dapi_register_commands-usb.o delib/delib_io_ftdi_linux.o delib/delib_io_ftdi.o delib/delib_open_close_linux-usb.o delib/delib_open_close-usb.o delib/convert_functions.o usb/ftdi.o usb/usb.o usb/linux.o usb/error.o usb/descriptors.o

CFLAGS := -g

liborigdelib.a: $(OBJ)
	ar rv $@ $(OBJ)
	ranlib $@

clean:
	rm -f $(OBJ) liborigdelib.a

