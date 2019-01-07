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
// delib_functions ONLY FOR LINUX
//
//
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************

#include "stdio.h"

#include "WinTypes.h"
//#define bool UCHAR

#ifdef USE_FTDI
#include "../usb/ftd2xx.h"
#endif

#include "delib.h"
#include "delib_intern.h"

struct timeval JS_Timerus_CounterStart;

#include "malloc.h"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------



OutputDebugString(msg)
{
//printf("%s", (char*) msg);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BOOL JS_Timerus_start()
{
    if (gettimeofday(&JS_Timerus_CounterStart, NULL)) 
        return FALSE;
    else
        return TRUE;
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG JS_Timerus_diff()
{
    struct timeval now;
    ULONG tdiff_us;

    if (gettimeofday(&now, NULL)) 
	{
        /* error handling ? */
        tdiff_us = 0;
    } else 
	{
        tdiff_us = (now.tv_sec - JS_Timerus_CounterStart.tv_sec) * (1000 * 1000) + (now.tv_usec - JS_Timerus_CounterStart.tv_usec);
    }

    return tdiff_us;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#ifdef USE_FTDI

ULONG DapiOpenModuleGetFT_HANDLE(ULONG moduleID, ULONG moduleNr, ULONG BusType, ULONG retry_max, ULONG subdevice, FT_HANDLE* handle)
{
	unsigned long NumDevices = 0;
	ULONG LocID = 0, ChipID = 0;
	FT_STATUS dStatus;
//	FT_HANDLE ftHandle;
	FT_STATUS ftStatus;
	DWORD iNumDevs;
	long dwLoc;
	DWORD EEUA_Size;
	ULONG sn;

	char text_dev_nr[10];
	unsigned char buffer[64];
	DWORD BytesRead;
	ULONG i;

	FT_DEVICE_LIST_INFO_NODE *devInfo;
	ULONG check_device;
	char msg[200];

	DWORD dwDriverVer;

	FT_STATUS status;

	ULONG ModuleLocation;


	ULONG baudrate;

	ULONG ok;


	if(subdevice==0)
	{
		sprintf(text_dev_nr , "DT00%4dA", (int) moduleID);
	}
	else if(subdevice==1)
	{
		sprintf(text_dev_nr , "DT00%4dB", (int) moduleID);
	}
	else
	{
		// Ohne A oder B
		sprintf(text_dev_nr , "DT00%4d", (int) moduleID);
	}

	// Linux KEIN A oder B
	//sprintf(text_dev_nr , "DT00%4d", (int) moduleID);


	sprintf(msg,"|DELIB|---------------OpenModuleGetFT_HANDLE(0x%x, 0x%x, 0x%x, 0x%x) Normal\n", (unsigned int) moduleID, (unsigned int) moduleNr, (unsigned int) retry_max, (unsigned int) subdevice);
	debug_print(msg);

	for(i=4;i!=8;++i)
	{
		if(text_dev_nr[i]<'0' || text_dev_nr[i]>'9') text_dev_nr[i]='0';
	}
	text_dev_nr[9]=0;

	sprintf(msg, "|DELIB|OpenModuleGetFT_HANDLE - Searching for the following S/N = %s\n", text_dev_nr);
	debug_print(msg);
	
	// ----------------------------------------------------

	// 6.12.2011 Geändert auf -1
	ModuleLocation = -1;
//	ModuleLocation = 0;



	ftStatus = FT_OpenEx(text_dev_nr,FT_OPEN_BY_SERIAL_NUMBER, handle);
	if (ftStatus == FT_OK)
	{

		sprintf(msg, "|DELIB|OpenModuleGetFT_HANDLE - FTDI-Handle = %x\n", (unsigned int) *handle);
		debug_print(msg);

	}
	else
	{
		sprintf(msg, "|DELIB|OpenModuleGetFT_HANDLE - FTDI OPEN ERROR \n");
		debug_print(msg);

		*handle = (FT_HANDLE) -1;
	}




	if(*handle != ((FT_HANDLE) -1))
	{
		if((BusType == bus_SeriellFTDI)  || (BusType == bus_FTDI_SINGLE))
		{

		
		
		
//		dStatus = FT_SetTimeouts(ftHandle, 100*5, 100*5);

		sprintf(msg,"|DELIB|OpenModuleGetFT_HANDLE - Timeout ok\n");
		debug_print(msg);

	
		if((moduleID != USB_MINI_STICK) && (moduleID != USB_LOGI_18) && (moduleID != USB_SPI_MON) && (moduleID != USB_WATCHDOG) && (moduleID != USB_OPTOIN_8))
		{
			baudrate = 115200;
		}
		else
		{
			baudrate = (unsigned long) (250000/1.666);					// 150 KBaud beim R8C1A


		}


//		printf("write ftdic = 0x%x\n", handle);



//		ftStatus=FT_SetBaudRate(ftHandle, baudrate);
		if(ftdi_set_baudrate (*handle, baudrate) != 0)
//		if (ftStatus != FT_OK)
		{
			sprintf(msg,"|DELIB|DapiHandle ftStatus != FT_OK -> FT_SetBaudRate");
			debug_print(msg);
			//return -1;
		}
		else
		{
			sprintf(msg,"|DELIB|DapiHandle * Serial Baudrate = %d", (unsigned int) baudrate);
			debug_print(msg);
		}

//printf("XX\n");
		// ----------------------------
//		ftStatus=FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
		if (ftStatus != FT_OK)
		{
			sprintf(msg,"|DELIB|DapiHandle ftStatus != FT_OK -> FT_SetDataCharacteristics");
			debug_print(msg);
			//return -1;
		}

		// ----------------------------
//		ftStatus=FT_SetTimeouts(ftHandle,1,1); 
		if (ftStatus != FT_OK)
		{
			sprintf(msg,"|DELIB|DapiHandle ftStatus != FT_OK -> FT_SetTimeouts");
			debug_print(msg);
			//return -1;
		}
		// ----------------------------
//		ftStatus=FT_SetLatencyTimer(ftHandle,2); 
		if(ftdi_set_latency_timer(*handle, 2) != 0)
		//if (ftStatus != FT_OK)
		{
			sprintf(msg,"|DELIB|DapiHandle ftStatus != FT_OK -> FT_SetLatencyTimer");
			debug_print(msg);
			//return -1;
		}
		// ----------------------------
	
	}
	}
	

	return (ULONG) *handle;
}
#endif


