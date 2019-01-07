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
#include "../usb/ftd2xx.h"
#include "delib.h"
#include "delib_intern.h"

#else

#include <windows.h>
#include <stdio.h>
#include "conio.h"

#include "malloc.h"

#include "\library\vc\ftdi\ftd2xx.h"

#include "..\includes\delib.h"
#include "delib_intern.h"

#include "delib_debug.h"
#include "delib_set_get_error.h"

#include "delib_io_ftdi.h"

#include "\library\vc\js_timer.h"

#endif



//#define debug_commands 1





// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DapiFTDIPing(ULONG handle, ULONG value)
{
	unsigned char buff[8];
	ULONG data;

	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	if(!DapiFTDISendCommand(handle, cmd_ping, value, 0, (unsigned char*) &buff, 4))
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
		return -1;
	}
	else
	{
		data=0;
		data |= (buff[0] & 0xff);
		data |= (buff[1] & 0xff) << 8;
		data |= (buff[2] & 0xff) << 16;
		data |= (buff[3] & 0xff) << 24;
		return data;
	}
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiFTDIWriteByte(ULONG handle, ULONG adress, ULONG value)
{
	ULONG be;
	ULONG i;
	unsigned char buff[8];

	for(i=0;i!=8;++i) buff[i]=0;

	buff[adress&7] = (unsigned char) (value&0xff);

	be = 1 << (adress&7);
	
	if(!DapiFTDISendCommand(handle, cmd_send8byte, adress, be, (unsigned char*) &buff, 8))
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
	}
	return;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiFTDIWriteWord(ULONG handle, ULONG adress, ULONG value)
{
	ULONG be;
	ULONG i;
	unsigned char buff[8];

	for(i=0;i!=8;++i) buff[i]=0;

	buff[(adress&6)+0] = (unsigned char) ( value     & 0xff);
	buff[(adress&6)+1] = (unsigned char) ((value>>8) & 0xff);

	be = 3 << (adress&6);
	
	if(!DapiFTDISendCommand(handle, cmd_send8byte, adress, be, (unsigned char*) &buff, 8))
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
	}
	return;

}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiFTDIWriteLong(ULONG handle, ULONG adress, ULONG value)
{
	ULONG be;
	ULONG i;
	unsigned char buff[8];

	for(i=0;i!=8;++i) buff[i]=0;

	buff[(adress&4)+0] = (unsigned char) ( value     & 0xff);
	buff[(adress&4)+1] = (unsigned char) ((value>>8) & 0xff);
	buff[(adress&4)+2] = (unsigned char) ((value>>16) & 0xff);
	buff[(adress&4)+3] = (unsigned char) ((value>>24) & 0xff);

	be = 15 << (adress&4);
	
	if(!DapiFTDISendCommand(handle, cmd_send8byte, adress, be, (unsigned char*) &buff, 8))
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
	}
	return;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiFTDIWriteLongLong(ULONG handle, ULONG adress, ULONGLONG value)
{
	ULONG be;

	be = 255;
	
	if(!DapiFTDISendCommand(handle, cmd_send8byte, adress, be, (unsigned char*) &value, 8))
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
	}
	return;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DapiFTDIReadByte(ULONG handle, ULONG adress)
{
	ULONG data;
	ULONG be;
	unsigned char buff[8];

	be = 1 << (adress&7);

	if(!DapiFTDISendCommand(handle, cmd_get8byte, adress, be, (unsigned char*) &buff, 8))
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
	}
	else
	{
		data=0;
		data |= (buff[adress&7] & 0xff);
	}
	return data;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DapiFTDIReadWord(ULONG handle, ULONG adress)
{
	ULONG data;
	ULONG be;
	unsigned char buff[8];

	be = 3 << (adress&6);

	if(!DapiFTDISendCommand(handle, cmd_get8byte, adress, be, (unsigned char*) &buff, 8))
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
	}
	else
	{
		data=0;
		data |= (buff[(adress&6)+0] & 0xff);
		data |= (buff[(adress&6)+1] & 0xff) << 8;
	}
	return data;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DapiFTDIReadLong(ULONG handle, ULONG adress)
{
	ULONG data;
	ULONG be;
	unsigned char buff[8];

	be = 15 << (adress&4);

	if(!DapiFTDISendCommand(handle, cmd_get8byte, adress, be, (unsigned char*) &buff, 8))
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
	}
	else
	{
		data=0;
		data |= (buff[(adress&4)+0]&0xff);
		data |= (buff[(adress&4)+1]&0xff) << 8;
		data |= (buff[(adress&4)+2]&0xff) << 16;
		data |= (buff[(adress&4)+3]&0xff) << 24;
	}
	return data;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONGLONG DapiFTDIReadLongLong(ULONG handle, ULONG adress)
{
	ULONGLONG data;
	ULONG be;

	be = 255;

	if(!DapiFTDISendCommand(handle, cmd_get8byte, adress, be, (unsigned char*) &data, 8))
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
	}

	return data;
}
		
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DapiFTDIReadMultipleBytes(ULONG handle, ULONG adress, ULONG adress_depth, ULONG increments, UCHAR * buff, ULONG buff_len)
{

	ULONG pos=0;
	ULONG incr=increments;
	ULONG send_incr;
	ULONG buff_offset = 0;

	while(incr>0)
	{
		if(incr>32768)
		{
			send_incr = 32768;
		}
		else
		{
			send_incr = incr;
		}
		
		if(!DapiFTDISendCommandMultipleBytes(handle, cmd_getnbyte, adress, adress_depth, send_incr, buff + buff_offset, buff_len-buff_offset))
		{
			DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
			return 0;
		}

		incr -= send_incr;
		buff_offset += send_incr*adress_depth;
	}

	return 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DapiFTDIWriteMultipleBytes(ULONG handle, ULONG adress, ULONG adress_depth, ULONG increments, UCHAR * buff, ULONG buff_len)
{
	ULONG pos=0;
	ULONG incr=increments;
	ULONG send_incr;
	ULONG buff_offset = 0;

	while(incr>0)
	{
		if(incr>32768)
		{
			send_incr = 32768;
		}
		else
		{
			send_incr = incr;
		}
		
		if(!DapiFTDISendCommandMultipleBytes(handle, cmd_sendnbyte, adress, adress_depth, send_incr, buff + buff_offset, buff_len-buff_offset))
		{
			DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);
			return 0;
		}

		incr -= send_incr;
		buff_offset += send_incr*adress_depth;
	}

	return 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
