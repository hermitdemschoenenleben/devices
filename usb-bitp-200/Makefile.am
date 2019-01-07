ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS = $(DEPS_CFLAGS) -I@top_srcdir@/delib
AM_CXXFLAGS = -std=c++11

lib_LTLIBRARIES = src/libusb_bitp_200-@PACKAGE_VERSION@.la delib/libdelib-@PACKAGE_VERSION@.la 
src_libusb_bitp_200_@PACKAGE_VERSION@_la_SOURCES = src/usb_bitp_200.cc
delib_libdelib_@PACKAGE_VERSION@_la_SOURCES = delib/delib.cc delib/ftdi_base.cc

if DELIB_ORIG
delib_libdelib_@PACKAGE_VERSION@_la_SOURCES += delib/orig.cc delib/orig_delib/delib/delib_io_seriellFTDI.c delib/orig_delib/delib/delib_Dapi_di_do_ad_da_befehle-usb.c delib/orig_delib/delib/delib_set_get_error.c delib/orig_delib/delib/delib_Dapi_register_commands-usb.c delib/orig_delib/delib/delib_io_ftdi_linux.c delib/orig_delib/delib/delib_io_ftdi.c delib/orig_delib/delib/delib_open_close_linux-usb.c delib/orig_delib/delib/delib_open_close-usb.c delib/orig_delib/delib/convert_functions.c 
AM_CPPFLAGS += -DORIG_PRESENT
endif

bin_PROGRAMS = usb_bitp_200_demo
usb_bitp_200_demo_SOURCES = src/usb_bitp_200_demo.cc
usb_bitp_200_demo_LDADD = src/libusb_bitp_200-@PACKAGE_VERSION@.la delib/libdelib-@PACKAGE_VERSION@.la $(DEPS_LIBS)

dist_noinst_SCRIPTS = autogen.sh
