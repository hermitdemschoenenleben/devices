// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// delib_functions
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "malloc.h"

#ifndef __linux__
#include <windows.h>
#include "conio.h"


#include "\library\vc\ftdi\ftd2xx.h"

#include "delib_io_seriellCOM.h"
#include "delib_io_ftdi.h"

#include "d:\linux_work\library\vc\deditec_tcp_functions.h"

#include "..\includes\delib.h"
#include "delib_intern.h"

#include "delib_set_get_error.h"

#include "delib_io_tcp.h"

#define debug_commands 1

//extern FT_STATUS  set_renesas_reset_0(FT_HANDLE ftHandle);
//extern FT_STATUS  set_renesas_reset_1(FT_HANDLE ftHandle);
extern ULONG DapiRenesasReset(FT_HANDLE ftHandle);
extern DAPI_HANDLE*  __stdcall HandleMGR_get_handle_pointer(ULONG module, ULONG nr, ULONG subdevice);

#define USE_FTDI_DUAL 1
#define USE_FTDI 1
#define USE_SER 1
#define USE_ETH 1
#define USE_FTDI_SER 1

#else

#include "WinTypes.h"
//#define bool UCHAR

#ifdef USE_FTDI
#include "../usb/ftd2xx.h"
#endif

#include "delib.h"
#include "delib_intern.h"

struct timeval JS_Timerus_CounterStart;

#endif

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DapiInternGetBusType(ULONG moduleID)
{
	ULONG ret;

	switch(moduleID)
	{
		case USB_Interface8:
		case USB_CAN_STICK:
		case USB_LOGI_500:
		case RO_USB2:
		case USB_BITP_200:
		case USB_OPTOIN_8_RELAIS_8:
		case USB_OPTOIN_16_RELAIS_16:
		case USB_OPTOIN_32_RELAIS_32:
			ret = bus_FTDI_DUAL;
			break;

		case RO_USB1:
			ret = bus_SeriellFTDI;
			break;

		case RO_SER:
		case RO_CAN:
			ret = bus_SeriellCOM;
			break;

		case RO_ETH:
			ret = bus_ETH;
			break;

		case USB_WATCHDOG:
		case USB_MINI_STICK:
		case USB_OPTOIN_8:
		case USB_LOGI_18:
		case USB_SPI_MON:
		case USB_TTL_64:
			ret = bus_FTDI_SINGLE;
			break;

		default:	
			ret = 0;break;
	}

	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DapiCheckHandle(ULONG handle)
{
	DAPI_HANDLE *DapiHandle;

	char msg[200];




	if (handle == 0)
	{
		sprintf(msg,"|DELIB|********************* HANDLE ID = 0*************\n");
		debug_print(msg);
		return -1;
	}

	
	DapiHandle = (DAPI_HANDLE*) handle;

	
	if(DapiHandle->id != delib_id)
	{
		sprintf(msg,"|DELIB|********************* ILLEGAL Handle ID(0x%lx)*************\n", DapiHandle->id);
		debug_print(msg);
		return -1;
	}

	
	if((DapiHandle->BusType == bus_FTDI_DUAL) || (DapiHandle->BusType == bus_FTDI_SINGLE))
	{
	
		if(DapiHandle->ftHandle == 0)
		{
			sprintf(msg,"|DELIB|********************* ILLEGAL ftHandle (=0)*************\n");
			debug_print(msg);
			return -1;
		}
	}

	{
		return 0;
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiOpenModuleIntern(ULONG moduleID, ULONG moduleNr, ULONG subdevice, unsigned char* exbuffer)
{
	FT_HANDLE ftHandle;
//	FT_STATUS ftStatus;
	DAPI_HANDLE *DapiHandle;
	ULONG ret;
//	ULONG baudrate;

	char msg[200];


	if(subdevice==DapiModuleID_Device)
	{
		sprintf(msg,"|DELIB|--------------------DapiOpenModuleIntern(0x%lx, 0x%lx, 0x%lx, 0x%lx) Normal\n", moduleID, moduleNr, subdevice, (unsigned long) exbuffer);
		debug_print(msg);
	}
	else
	{
		sprintf(msg,"|DELIB|--------------------DapiOpenModuleIntern(0x%lx, 0x%lx, 0x%lx, 0x%lx) DEBUGDevice\n", moduleID, moduleNr, subdevice, (unsigned long) exbuffer);
		debug_print(msg);
	}


	// TEST
//	HandleMGR_show_handle_buffer();

	#ifndef __linux__
	DapiHandle = HandleMGR_get_handle_pointer(moduleID, moduleNr, subdevice);
	#else
	DapiHandle =  malloc(sizeof(DAPI_HANDLE));
	DapiHandle->ftHandle = (FT_HANDLE) -1;
	#endif


	#ifdef __linux__
	sprintf(msg,"|DELIB|DapiHandle = %x\n", (unsigned int) DapiHandle);
	debug_print(msg);
	sprintf(msg,"|DELIB|DapiHandle->id = %x\n", (unsigned int) DapiHandle->id);
	debug_print(msg);
	sprintf(msg,"|DELIB|DapiHandle->ftHandle=%x\n", (unsigned int) DapiHandle->ftHandle);
	debug_print(msg);
	sprintf(msg,"|DELIB|DapiHandle->subdevice=%x\n", (unsigned int) DapiHandle->SubDevice);
	debug_print(msg);
	#endif

	DapiHandle->BusType = DapiInternGetBusType(moduleID);

	DapiHandle->id = delib_id;


	// ----------------------------------------------------
	// FTDI_DUAL 
#ifdef USE_FTDI_DUAL

	if((DapiHandle->BusType == bus_FTDI_DUAL))
	{

		if((DapiHandle->ftHandle == (FT_HANDLE) -1) || (DapiHandle->id != delib_id))
		{
			ret = DapiOpenModuleGetFT_HANDLE(moduleID, moduleNr, DapiHandle->BusType, 1, subdevice, &ftHandle);	// Retry=1, subdevice

			sprintf(msg,"|DELIB|ret=%lx\n", ret);
			debug_print(msg);
			sprintf(msg,"|DELIB|ftHandle=%lx\n", (ULONG) ftHandle);
			debug_print(msg);
	
			if (ret==-1) 
			{
				sprintf(msg,"|DELIB|DapiOpenModuleIntern - return=0\n");
				debug_print(msg);

				DapiSetError(DAPI_ERR_DEVICE_NOT_FOUND, 0, NULL, __FILE__, __LINE__);					

				return 0;
			}

		}
		else
		{
			ftHandle = DapiHandle->ftHandle;
		}
	
		//	if(DapiHandle != 0)
			DapiHandle->id = delib_id;
			DapiHandle->moduleID = moduleID;
			DapiHandle->moduleNr = moduleNr;
			DapiHandle->ftHandle = ftHandle;
			DapiHandle->SubDevice = subdevice;
		
	//	}
		sprintf(msg,"|DELIB|DapiHandle->ftHandle=%lx\n", (unsigned long) DapiHandle->ftHandle);
		debug_print(msg);
	//	sprintf(msg,"|DELIB|DapiHandle->ftHandleProg=%x\n", DapiHandle->ftHandleProg);
	//	debug_print(msg);
		sprintf(msg,"|DELIB|DapiHandle D_API_id = %lx\n", DapiHandle->id);
		debug_print(msg);
		sprintf(msg,"|DELIB|DapiHandle D_API_module_ID = %ld  module_Nr=%lx\n", DapiHandle->moduleID, DapiHandle->moduleNr);
		debug_print(msg);
	}
#endif


	// ----------------------------------------------------
	// Seriell über die COM
#ifdef USE_SER
	if(DapiHandle->BusType == bus_SeriellCOM)
	{

		ret = DapiOpenModuleIntern_SeriellCOM((ULONG) DapiHandle, moduleID, moduleNr);
		if(ret!=0) 
		{
			sprintf(msg,"|DELIB|DapiOpenModuleIntern - return=0 - No module found\n");
			debug_print(msg);

			DapiSetError(DAPI_ERR_DEVICE_NOT_FOUND, 0, NULL, __FILE__, __LINE__);					

			return 0;	// Connection to Module failed
		}
	}
#endif

	// ----------------------------------------------------
	// Seriell über FTDI
#ifdef USE_FTDI
	if((DapiHandle->BusType == bus_SeriellFTDI)  || (DapiHandle->BusType == bus_FTDI_SINGLE))
	{

		#ifndef __linux__
		if((DapiHandle->ftHandle == (FT_HANDLE) -1) || (DapiHandle->id != delib_id))
		{
		#endif
			ret = DapiOpenModuleGetFT_HANDLE(moduleID, moduleNr, DapiHandle->BusType, 1, 2, &ftHandle);	// Retry=1, subdevice=2 (ohne A oder B)

			sprintf(msg,"|DELIB|ret=%lx\n", ret);
			debug_print(msg);
			sprintf(msg,"|DELIB|ftHandle=%lx\n", (ULONG) ftHandle);
			debug_print(msg);
	
			if (ret==-1)
			{
				sprintf(msg,"|DELIB|DapiOpenModuleIntern - return=0 - No module found\n");
				debug_print(msg);

				DapiSetError(DAPI_ERR_DEVICE_NOT_FOUND, 0, NULL, __FILE__, __LINE__);					

				return 0;
			}

		#ifndef __linux__
		}
		else
		{
			ftHandle = DapiHandle->ftHandle;
		}
		#endif	

		//	if(DapiHandle != 0)
			DapiHandle->id = delib_id;
			DapiHandle->moduleID = moduleID;
			DapiHandle->moduleNr = moduleNr;
			DapiHandle->ftHandle = ftHandle;
			DapiHandle->SubDevice = 2;//subdevice;
			DapiHandle->RS485_ModuleNr = 0;

		
	//	}
		sprintf(msg,"|DELIB|DapiHandle->ftHandle=%lx\n", (unsigned long) DapiHandle->ftHandle);
		debug_print(msg);
		sprintf(msg,"|DELIB|DapiHandle DT_API_id = %lx\n", DapiHandle->id);
		debug_print(msg);
		sprintf(msg,"|DELIB|DapiHandle DT_API_module_ID = %ld  module_Nr=%lx\n", DapiHandle->moduleID, DapiHandle->moduleNr);
		debug_print(msg);

		// ----------------------------
	}
#endif
	// ----------------------------------------------------
	// Ethernet
#ifdef USE_ETH

	if(DapiHandle->BusType == bus_ETH)
	{
		ret = DapiOpenModuleIntern_ETH((ULONG) DapiHandle, moduleID, moduleNr, exbuffer);
		if(ret!=0) 
		{
			sprintf(msg,"|DELIB|DapiOpenModuleIntern - return=0 - No module found\n");
			debug_print(msg);

			DapiSetError(DAPI_ERR_DEVICE_NOT_FOUND, 0, NULL, __FILE__, __LINE__);					

			return 0;	// Connection to Module failed
		}
	}
#endif
	// ----------------------------------------------------
	

	
	
	
	sprintf(msg,"|DELIB|DapiHandle->ftHandle=%lx\n", (unsigned long) DapiHandle->ftHandle);
	debug_print(msg);
	sprintf(msg,"|DELIB|DapiHandle DT_API_id = %lx\n", DapiHandle->id);
	debug_print(msg);
	sprintf(msg,"|DELIB|DapiHandle DT_API_module_ID = %ld  module_Nr=%lx\n", DapiHandle->moduleID, DapiHandle->moduleNr);
	debug_print(msg);

	// ----------------------------

	
	DapiClearLastError();




	sprintf(msg,"|DELIB|DapiHandle DapiOpenModuleIntern - ENDE\n");
	debug_print(msg);



	return (unsigned long) DapiHandle;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiOpenModule(ULONG moduleID, ULONG moduleNr)
{
	ULONG ret;
	char msg[200];
	sprintf(msg,"|DELIB|--------------------DapiOpenModule(0x%lx, 0x%lx)\n", moduleID, moduleNr);
	debug_print(msg);

	ret = DapiOpenModuleIntern(moduleID, moduleNr, DapiModuleID_Device, (unsigned char*) 0);

	sprintf(msg,"|DELIB|return = 0x%lx\n", ret);
	debug_print(msg);

	if(ret!=0) 
	{
		DapiClearLastError();
	}

	return (ret);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiOpenModuleEx(ULONG moduleID, ULONG moduleNr, unsigned char* exbuffer)
{
	ULONG ret;
	char msg[200];
	sprintf(msg,"|DELIB|--------------------DapiOpenModuleEx(0x%lx, 0x%lx, 0x%lx)\n", moduleID, moduleNr, (unsigned long) exbuffer );
	debug_print(msg);

	ret = DapiOpenModuleIntern(moduleID, moduleNr, DapiModuleID_Device, exbuffer);

	sprintf(msg,"|DELIB|return = 0x%lx\n", ret);
	debug_print(msg);

	if(ret!=0) 
	{
		DapiClearLastError();
	}

	return (ret);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiCloseModule(ULONG handle)
{
	DAPI_HANDLE *DapiHandle;

	if(handle==0) return -1;
	if(handle==-1) return -1;

	DapiHandle = (DAPI_HANDLE*) handle;

	char msg[200];

	sprintf(msg,"|DELIB|--------------------DapiCloseModule(0x%lx)\n", handle);
	debug_print(msg);

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) 
	{
		sprintf(msg,"|DELIB|DapiCloseModule - ERROR * DapiCheckHandle(handle) != 0\n");
		debug_print(msg);
		return 0;
	}


	// ----------------------------------------------------
	// FTDI
#ifdef USE_FTDI
	if((DapiHandle->BusType == bus_FTDI_DUAL) || (DapiHandle->BusType == bus_FTDI_SINGLE))
	{
		sprintf(msg,"|DELIB|FT_Close(0x%lx)  -Bus-Typ = bus_FTDI\n", (unsigned long) DapiHandle->ftHandle);
		debug_print(msg);

		FT_Close(DapiHandle->ftHandle);
		DapiHandle->ftHandle = (FT_HANDLE) -1;		// Clear Handle
	}
#endif


	// ----------------------------------------------------
	// FTDI
#ifdef USE_FTDI_SER
	if(DapiHandle->BusType == bus_SeriellCOM)
	{
		sprintf(msg,"|DELIB|FT_Close(0x%lx)  -Bus-Typ = bus_SeriellCOM\n", (unsigned long) DapiHandle->ftHandle);
		debug_print(msg);

	
		DapiSeriellCOMCloseComPort(DapiHandle->ComPort);
	}
#endif

	// ----------------------------------------------------
	// Seriell über FTDI
#ifdef USE_FTDI
	if(DapiHandle->BusType == bus_SeriellFTDI)
	{
		sprintf(msg,"|DELIB|FT_Close(0x%lx)  -Bus-Typ = bus_SeriellFTDI\n", (unsigned long) DapiHandle->ftHandle);
		debug_print(msg);

		FT_Close(DapiHandle->ftHandle);
		DapiHandle->ftHandle = (FT_HANDLE) -1;		// Clear Handle

	}
#endif
	// ----------------------------------------------------
	// Ethernet

#ifdef USE_ETH
	if(DapiHandle->BusType == bus_ETH)
	{
		sprintf(msg,"|DELIB|tcp_tx_close(0x%lx)  -Bus-Typ = bus_ETH\n", DapiHandle->ftHandle);
		debug_print(msg);

		tcp_tx_close(DapiHandle->TCPHandle);
	}
#endif
	// ----------------------------------------------------



	DapiClearLastError();

//	free (DapiHandle);
	
	return 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiGetDELIBVersion(ULONG mode, ULONG par)
{
if(mode==0) return (DELIB_VERSION);

return 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
