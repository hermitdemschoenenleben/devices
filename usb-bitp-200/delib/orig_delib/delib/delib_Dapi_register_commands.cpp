// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
//
// (c) DEDITEC GmbH, 2010
//
// web: http://www.deditec.de
//
// mail: vertrieb@deditec.de
//
//
//
// delib_functions
//
//
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************

#ifdef __linux__

#ifdef USE_FTDI
#include "../usb/ftd2xx.h"
#endif

#include "delib.h"
#include "delib_intern.h"
#include <stdio.h>

#else

#include <windows.h>
#include <stdio.h>
#include "conio.h"

#include "malloc.h"

#include "\library\vc\ftdi\ftd2xx.h"

#include "delib_io_seriellCOM.h"
#include "delib_io_seriellFTDI.h"
#include "delib_io_ftdi.h"
#include "delib_io_tcp.h"

#include "..\includes\delib.h"
#include "delib_intern.h"
#include "delib_set_get_error.h"

#define USE_FTDI	1
#define USE_ETH		1
#define USE_SER		1
#endif


//#define debug_commands 1


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiPing(ULONG handle, ULONG value)
{
	ULONG ret;
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return 0;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------DapiPing(0x%x, 0x%x)\n", handle, value);
	debug_print(msg);
	#endif
	
	switch(DapiHandle->BusType)
	{

		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			DapiSeriellFTDIWriteByte(handle, 0xffff, value);		// Save value at Adress 0xffff
			ret=DapiSeriellFTDIReadByte(handle, 0xffff);
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			ret = DapiFTDIPing(handle, value);
			break;
		#endif


		#ifdef USE_SER
		case bus_SeriellCOM:
			DapiSeriellCOMWriteByte(handle, 0xffff, value);		// Save value at Adress 0xffff
			ret=DapiSeriellCOMReadByte(handle, 0xffff);
			break;
		#endif

		#ifdef USE_ETH
		case bus_ETH:
			DapiTCPWriteByte(handle, 0xffff, value);		// Save value at Adress 0xffff
			ret=DapiTCPReadByte(handle, 0xffff);
			break;

		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}
	return ret;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiWriteByte(ULONG handle, ULONG adress, ULONG value)
{
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------DapiWriteByte adress=0x%lx value=0x%lx", adress, value);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			DapiSeriellFTDIWriteByte(handle, adress, value);
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			DapiFTDIWriteByte(handle, adress, value);
			break;
		#endif


		#ifdef USE_SER
		case bus_SeriellCOM:
			DapiSeriellCOMWriteByte(handle, adress, value);
			break;

		#endif

		#ifdef USE_ETH
		case bus_ETH:
			DapiTCPWriteByte(handle, adress, value);
			break;

		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiWriteWord(ULONG handle, ULONG adress, ULONG value)
{
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------Command=WriteWord adress=0x%lx value=0x%lx", adress, value);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			DapiSeriellFTDIWriteWord(handle, adress, value);
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			DapiFTDIWriteWord(handle, adress, value);
			break;
		#endif


		#ifdef USE_SER
		case bus_SeriellCOM:
			DapiSeriellCOMWriteWord(handle, adress, value);
			break;
		#endif

		#ifdef USE_ETH
		case bus_ETH:
			DapiTCPWriteWord(handle, adress, value);
			break;

		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiWriteLong(ULONG handle, ULONG adress, ULONG value)
{
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------Command=WriteLong adress=0x%lx value=0x%lx", adress, value);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			DapiSeriellFTDIWriteLong(handle, adress, value);
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			DapiFTDIWriteLong(handle, adress, value);
			break;
		#endif


		#ifdef USE_SER
		case bus_SeriellCOM:
			DapiSeriellCOMWriteLong(handle, adress, value);
			break;
		#endif


		#ifdef USE_ETH
		case bus_ETH:
			DapiTCPWriteLong(handle, adress, value);
			break;

		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiWriteLongLong(ULONG handle, ULONG adress, ULONGLONG value)
{
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------Command=WriteLongLong adress=0x%lx value1=0x%lx value2=0x%lx", adress, (unsigned long) value, (unsigned long) (value>>32));
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			DapiSeriellFTDIWriteLongLong(handle, adress, value);
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			DapiFTDIWriteLongLong(handle, adress, value);
			break;
		#endif


		#ifdef USE_SER
		case bus_SeriellCOM:
			DapiSeriellCOMWriteLongLong(handle, adress, value);
			break;
		#endif


		#ifdef USE_ETH
		case bus_ETH:
			DapiTCPWriteLongLong(handle, adress, value);
			break;

		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiReadByte(ULONG handle, ULONG adress)
{
	ULONG ret;
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return 0;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------Command=ReadByte adress=0x%lx\n", adress);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			ret = DapiSeriellFTDIReadByte(handle, adress);
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			ret = DapiFTDIReadByte(handle, adress);
			break;
		#endif


		#ifdef USE_SER
		case bus_SeriellCOM:
			ret = DapiSeriellCOMReadByte(handle, adress);
			break;
		#endif


		#ifdef USE_ETH
		case bus_ETH:
			ret = DapiTCPReadByte(handle, adress);
			break;

		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}

	#ifdef debug_commands
	sprintf(msg,"|DELIB|ReadByte adress=0x%lx data=0x%lx", adress, ret);
	debug_print(msg);
	#endif

	return ret;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiReadWord(ULONG handle, ULONG adress)
{
	ULONG ret;
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return 0;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------Command=ReadWord adress=0x%lx\n", adress);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			ret = DapiSeriellFTDIReadWord(handle, adress);
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			ret = DapiFTDIReadWord(handle, adress);
			break;
		#endif


		#ifdef USE_SER
		case bus_SeriellCOM:
			ret = DapiSeriellCOMReadWord(handle, adress);
			break;
		#endif


		#ifdef USE_ETH
		case bus_ETH:
			ret = DapiTCPReadWord(handle, adress);
			break;

		#endif
		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}

	#ifdef debug_commands
	sprintf(msg,"|DELIB|ReadWord adress=0x%lx data=0x%lx", adress, ret);
	debug_print(msg);
	#endif

	return ret;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiReadLong(ULONG handle, ULONG adress)
{
	ULONG ret;
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return 0;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------Command=ReadLong adress=0x%lx bus=%x\n", adress, DapiHandle->BusType);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			ret = DapiSeriellFTDIReadLong(handle, adress);
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			ret = DapiFTDIReadLong(handle, adress);
			break;
		#endif


		#ifdef USE_SER
		case bus_SeriellCOM:
			ret = DapiSeriellCOMReadLong(handle, adress);
			break;
		#endif


		#ifdef USE_ETH
		case bus_ETH:
			ret = DapiTCPReadLong(handle, adress);
			break;

		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}

	#ifdef debug_commands
	sprintf(msg,"|DELIB|ReadLong adress=0x%lx data=0x%lx ", adress, ret);
	debug_print(msg);
	#endif

return ret;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONGLONG DAPI_FUNCTION_PRE DapiReadLongLong(ULONG handle, ULONG adress)
{
	ULONGLONG ret;
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return 0;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------Command=ReadLongLong adress=0x%lx\n", adress);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			ret = DapiSeriellFTDIReadLongLong(handle, adress);
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			ret = DapiFTDIReadLongLong(handle, adress);
			break;
		#endif


		#ifdef USE_SER
		case bus_SeriellCOM:
			ret = DapiSeriellCOMReadLongLong(handle, adress);
			break;
		#endif


		#ifdef USE_ETH
		case bus_ETH:
			ret = DapiTCPReadLongLong(handle, adress);
			break;

		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}

	#ifdef debug_commands
	sprintf(msg,"|DELIB|ReadLongLong data1=0x%lx data2=0x%ld\n", ret, ret>>32);
	debug_print(msg);
	#endif

	return ret;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiWriteClearByte(ULONG handle, ULONG adress, ULONG value)
{
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------DapiWriteByte adress=0x%lx value=0x%lx", adress, value);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			DapiSeriellFTDIWriteClearByte(handle, adress, value);
			break;
		#endif


		#ifdef USE_SER
		//case bus_FTDI_DUAL:
	//	case bus_FTDI_SINGLE:
		//	DapiFTDIWriteByte(handle, adress, value);
	//		break;
	//	case bus_SeriellCOM:
		//	DapiSeriellCOMWriteByte(handle, adress, value);
	//		break;
		#endif


		#ifdef USE_ETH
	//	case bus_ETH:
	//		DapiTCPWriteByte(handle, adress, value);
	//		break;

		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiWriteSetByte(ULONG handle, ULONG adress, ULONG value)
{
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------DapiWriteByte adress=0x%lx value=0x%lx", adress, value);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			DapiSeriellFTDIWriteSetByte(handle, adress, value);
			break;
		#endif


		#ifdef USE_SER
	//	case bus_FTDI_DUAL:
	//	case bus_FTDI_SINGLE:
	//		DapiFTDIWriteByte(handle, adress, value);
	//		break;
	//	case bus_SeriellCOM:
	//		DapiSeriellCOMWriteByte(handle, adress, value);
	//		break;
		#endif


		#ifdef USE_ETH
	//	case bus_ETH:
	//		DapiTCPWriteByte(handle, adress, value);
	//		break;

		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiReadMultipleBytes(ULONG handle, ULONG adress, ULONG adress_depth, ULONG repeat, UCHAR * buff, ULONG buff_len)
{
	ULONG ret;
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return 0;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------Command=DapiReadMultipleBytes adress=0x%lx adress_depth=%d, incr=%d\n", adress, (unsigned int) adress_depth, (unsigned int) repeat);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
#ifndef __linux__
			DapiSeriellFTDIRead64BytesMax(handle, adress, buff, adress_depth);
#endif
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			ret = DapiFTDIReadMultipleBytes(handle, adress, adress_depth, repeat, buff, buff_len);
			break;
		#endif

		#ifdef USE_SER
		case bus_SeriellCOM:
			DapiSeriellCOMRead64BytesMax(handle, adress, buff, adress_depth);
			break;
		#endif


		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}
	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiWriteMultipleBytes(ULONG handle, ULONG adress, ULONG adress_depth, ULONG repeat, UCHAR * buff, ULONG buff_len)
{
	ULONG ret;
	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return 0;

	#ifdef debug_commands
	char msg[200];
	sprintf(msg,"|DELIB|--------------------Command=DapiWriteMultipleBytes adress=0x%lx adress_depth=%d, repeat=%d\n", adress, (unsigned int) adress_depth, (unsigned int) repeat);
	debug_print(msg);
	#endif

	switch(DapiHandle->BusType)
	{
		#ifdef USE_FTDI
		case bus_SeriellFTDI:
			DapiSeriellFTDIWrite64BytesMax(handle, adress, buff, adress_depth);
			break;
		case bus_FTDI_DUAL:
		case bus_FTDI_SINGLE:
			ret = DapiFTDIWriteMultipleBytes(handle, adress, adress_depth, repeat, buff, buff_len);
			break;
		#endif


		#ifdef USE_SER
		case bus_SeriellCOM:
			DapiSeriellCOMWrite64BytesMax(handle, adress, buff, adress_depth);
			break;
		#endif


		#ifdef USE_ETH
		case bus_ETH:
			ret = DapiTCPWriteMultipleBytes(handle, adress, adress_depth, repeat, buff, buff_len);
			break;
		#endif

		default:
			DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
			break;
	}

	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
