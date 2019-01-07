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


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------






#ifdef __linux__
#include "../usb/ftd2xx.h"
#include "delib.h"
#include "delib_intern.h"
#include <stdio.h>
#include <string.h>
#include "../usb/ftdi.h"



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



ULONG second_call_prevent = 0;


//#define debug_print(x) printf("%s\n", x)

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


void debug_print2(char *msg1, char *msg2)
{
	char msgx[1024];

	strcpy (msgx,msg1);
	strcat (msgx, msg2);
	debug_print(msgx);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int IsFTDIError(FT_STATUS dStatus)
{
//	char ErrorMessage[256];

	if(dStatus != FT_OK)
	{
//		FTID_GetErrorCodeString("EN", dStatus, ErrorMessage, 256);
//		printf(ErrorMessage);
		return(TRUE);
	}
return(FALSE);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiFTDIErrorHandler(ULONG handle, FT_STATUS dStatus)
{
	FT_HANDLE ftHandle;
	DAPI_HANDLE *DapiHandle;
	char msg[200];
	ULONG ret;

	DapiHandle = (DAPI_HANDLE*) handle;


return;

	DapiCheckHandle(handle);

	do {
		sprintf(msg,"|DELIB|DapiFTDIErrorHandler - ErrorHandler - searching module...");
		debug_print(msg);

		if(DapiHandle->ftHandle!=0)
		{
			FT_Close(DapiHandle->ftHandle);
		}

		ret = DapiOpenModuleGetFT_HANDLE(DapiHandle->moduleID, DapiHandle->moduleNr, DapiHandle->BusType, 10, DapiHandle->SubDevice, &ftHandle);		// try to open / retry=10
		
		DapiHandle->ftHandle = ftHandle;	// Modul hat jetzt anderes Handle

	} while(ret!=0);

	
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG FTread_n_bytes_with_timeout(FT_HANDLE ftHandle, ULONG anz, unsigned char *buff, ULONG delay_max)
{
	//ULONG buff[1000];
	ULONG rx_pos;
	ULONG rx_count;
	FT_STATUS  dStatus;
	char msg[200];
	ULONG i;
	ULONG exit;

	rx_pos=0;

	exit=0;

//	sprintf(msg,"|DELIB|FTread_n_bytes_with_timeout - delay_max = %d\n", delay_max);
//	debug_print(msg);
//	printf("%s", msg);

	JS_Timerus_start();
	do 
	{
		dStatus = FT_Read(ftHandle, buff+rx_pos, anz-rx_pos, &rx_count);

		if (dStatus != FT_OK)
		{
			sprintf(msg,"|DELIB|FTread_n_bytes_with_timeout -  dStatus != FT_OK -> FT_Read");
			debug_print(msg);
			//return ProgDeviceErr_SyncError;
		}



		rx_pos+=rx_count;

	
/*	for(i=0;i!=rx_pos;++i) 
	{
		if(*(buff+i) == 13)
		{
			exit=1;
		}
		else 
	{
			exit=0;
		}
	}
*/


	} while((rx_pos<anz) && (JS_Timerus_diff()<delay_max) && (exit==0));
i=JS_Timerus_diff();

	//sprintf(msg,"|DELIB|FTread_n_bytes_with_timeout - delay = %d    anz=%d, rx_pos=%d\n", i, anz, rx_pos);
	//debug_print(msg);

if(rx_pos<anz) return rx_pos;		// received not enough bytes
else return anz;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DapiFTDISendCommand(ULONG handle, ULONG command, ULONG adress, ULONG be, UCHAR *data, ULONG data_length)
{
	DAPI_HANDLE *DapiHandle;

//	FTID_STATUS dStatus;
	FT_STATUS dStatus;

	char msg[2000];
	char msg2[200];
	unsigned char buff[10000];
	ULONG buff_cnt;
	ULONG buff_cnt_ret;

	ULONG i;

	ULONG cnt=0;
	ULONG get_char_cnt=0;
	ULONG error=0;
//	ULONG ret;

	ULONG retry_counter;

	if(second_call_prevent != 0 ) 
	{
		sprintf(msg,"|DELIB|FTDISendCommand - Second call in background detected - STOP it\n");
		debug_print(msg);
		return -1;
	}

	++second_call_prevent;	// prevent a background call 

	DapiHandle = (DAPI_HANDLE*) handle;

	if (handle ==0)
	{
		sprintf(msg,"|DELIB|FTDISendCommand - Invalid Handle (handle=0)\n");
		debug_print(msg);
		--second_call_prevent;	// prevent a background call 
		return -1;
	}

	if (DapiHandle->id != delib_id)
	{
		sprintf(msg,"|DELIB|FTDISendCommand - Invalid Handle-ID\n");
		debug_print(msg);
		--second_call_prevent;	// prevent a background call 
		return -1;
	}

	if(DapiCheckHandle(handle) != 0) 
	{
		--second_call_prevent;	// prevent a background call 
		return -1;
	}

	++DapiHandle->jobID;

	
//	sprintf(msg,"|DELIB|FTDISendCommand - Start with 50 retries DapiHandle->ftHandle=%x\n", DapiHandle->ftHandle);
//	debug_print(msg);

	retry_counter=5;	// retries

	do {
		error=0;
		DapiHandle->jobID = (DapiHandle->jobID)&0xff;

		buff_cnt=0;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;

	
		buff[buff_cnt++] = 0xff;
		buff[buff_cnt++] = (unsigned char) DapiHandle->jobID;
		buff[buff_cnt++] = (unsigned char) command;

		buff[buff_cnt++] = (unsigned char) ((adress      ) & 0xff);
		buff[buff_cnt++] = (unsigned char) ((adress >>  8) & 0xff);
		buff[buff_cnt++] = (unsigned char) ((adress >> 16) & 0xff);
		buff[buff_cnt++] = (unsigned char) ((adress >> 24) & 0xff);

		buff[buff_cnt++] = (unsigned char) be;
		// das wird IMMER gesendet

		if(command == cmd_send8byte)
		{
			// always send 8 bytes
			for(i=0;i!=8;++i)
			{
				//printf("Aufbereitung Byte %d\n",i);
				if(i<data_length) 
				{
					buff[buff_cnt++] = *(data+i);
				}
				else
				{
					buff[buff_cnt++] = 0;
				}
			}
		}


		do
		{
			dStatus = FT_Write(DapiHandle->ftHandle, buff, buff_cnt, &buff_cnt_ret);
			if(dStatus != FT_OK)
			{
				sprintf(msg,"|DELIB|FTDISendCommand - dStatus != FTID_SUCCESS   dStatus = %x\n", (unsigned int) dStatus);
				debug_print(msg);

				DapiFTDIErrorHandler(handle, dStatus);
			}
		} while(dStatus != FT_OK);
	
		if(command==cmd_ping) 
		{
			get_char_cnt=7;		// Start_char + job_id + 4bytes-data + end_char
		}
	
		if(command==cmd_send8byte) 
		{
			get_char_cnt=3;	// Start_char + job_id + 0bytes-data + end_char
		}

		if(command==cmd_get8byte) 
		{
			get_char_cnt=11;	// Start_char + job_id + 8bytes-data + end_char
		}

		buff_cnt_ret = FTread_n_bytes_with_timeout(DapiHandle->ftHandle, get_char_cnt, buff, 100*1000);	// max 100ms
		// buff_cnt_ret = n , es wurden "n" Bytes empfangen

		if(buff_cnt_ret != get_char_cnt) 
		{
			sprintf(msg,"|DELIB|FTDISendCommand - ft_send received char count = %d (sollte sein = %d)\n", (unsigned int) buff_cnt_ret, (unsigned int) get_char_cnt);
			debug_print(msg);
			++error;
		}
		else
		{
			for(i=0;i!=8;++i) 
			{
				if(i<data_length) *(data+i) = buff[i+2];
			}

			if(buff[0] != 0x1A) 
			{
				sprintf(msg,"|DELIB|FTDISendCommand - ft_send_buff[0]=%x  (sollte sein=%x)\n", (unsigned int) buff[0], 0x1a);
				debug_print(msg);
				++error;
			}
	
			if(buff[1] != DapiHandle->jobID) 
			{
				sprintf(msg,"|DELIB|FTDISendCommand - ft_send_buff[1]=%x sollte sein=%x\n", (unsigned int) buff[1], (unsigned int) DapiHandle->jobID);
				debug_print(msg);
				++error;
			}

			if(buff[get_char_cnt-1] != 0x4f) 
			{
				sprintf(msg,"|DELIB|FTDISendCommand - ft_send_buff[%d]=%x sollte sein=%x\n", (unsigned int) get_char_cnt, (unsigned int) buff[get_char_cnt-1], 0x4f);
				debug_print(msg);
				++error;
			}
		}


		if(error!=0)
		{
			sprintf(msg,"|DELIB|FTDISendCommand - received %d char: ", (unsigned int) buff_cnt_ret);
			if(buff_cnt_ret != 0)
			{
				for(i=0;i!=buff_cnt_ret;++i) 
				{
					sprintf(msg2,"%2x ", buff[i]);
					strcat(msg, msg2);
				}
			}
			debug_print(msg);

			sprintf(msg,"|DELIB|FTDISendCommand - Zeichen empfangen: 0x%x (anzahl return=%d dez)\n", (unsigned int) buff[0], (unsigned int) buff_cnt_ret);
			debug_print(msg);

			dStatus = FT_GetQueueStatus(DapiHandle->ftHandle, &buff_cnt_ret);
			if(IsFTDIError(dStatus)) 
			{
				--second_call_prevent;	// prevent a background call 
				return 0;
			}

			sprintf(msg,"|DELIB|FTDISendCommand - Rx Zeichen noch im buffer da: %d\n", (unsigned int) buff_cnt_ret);
			debug_print(msg);
		}


		/*dStatus = FT_GetQueueStatus(DapiHandle->ftHandle, &buff_cnt_ret);
		if(buff_cnt_ret!=0)
		{
			FT_Purge( DapiHandle->ftHandle, FT_PURGE_RX);
			if(IsFTDIError(dStatus)) 
			{
				sprintf(msg,"|DELIB|FTDISendCommand - ERROR FT_Purge\n");
				debug_print(msg);
	
				return 0;
			}
		}*/




		if(error!=0) 
		{
			sprintf(msg,"|DELIB|FTDISendCommand - ERROR aufgetreten-----------------------------------------\n");
			debug_print(msg);
		}

		if(error!=0) 
		{
			sprintf(msg,"|DELIB|DapiFTDISendCommand - RETRY RETRY RETRY RETRY = %d\n", (unsigned int) retry_counter);
			debug_print(msg);
		}

		--retry_counter;

	}while((error!=0) && (retry_counter>0));


	--second_call_prevent;	// prevent a background call 


	if(error==0) 
	{
		return TRUE;	// kein Fehler
	}
	else 
	{
		return FALSE;	// Fehler
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DapiFTDISendCommandMultipleBytes(ULONG handle, ULONG command, ULONG adress, ULONG adress_tiefe, ULONG wiederholungen, unsigned char* data, ULONG data_length)
{

	DAPI_HANDLE *DapiHandle;
	FT_STATUS dStatus;

	//unsigned char buff[8*65536];
	unsigned char * buff;
	ULONG buff_cnt;
	ULONG buff_cnt_ret;

	ULONG alloc_mem_size = 0;

	ULONG i;
	ULONG j;

	ULONG rd_offset;

	ULONG cnt=0;
	ULONG get_char_cnt=0;
	ULONG error=0;
	ULONG be=0;
	char msg[200];
	ULONG ret;

	ULONG retry_counter;

	if(second_call_prevent != 0 ) 
	{
		sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - Second call in background detected - STOP it\n");
		debug_print(msg);
		return -1;
	}

	DapiHandle = (DAPI_HANDLE*) handle;

	if(DapiCheckHandle(handle) != 0) return -1;

	if (DapiHandle->id != delib_id)
	{
		sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - Invalid Handle\n");
		debug_print(msg);
		return -1;
	}

	
	
	alloc_mem_size = (adress_tiefe+1)*wiederholungen + 100;

	sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - Allocating now memory (%d Bytes)\n", (unsigned int) alloc_mem_size);
	debug_print(msg);

	buff = (unsigned char *) malloc (alloc_mem_size);

	if(buff == 0)
	{
		sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - Allocating memory (%d Bytes) -> failure\n", (unsigned int) alloc_mem_size);
		debug_print(msg);
		return -1;
	}


	++second_call_prevent;	// prevent a background call 


	++DapiHandle->jobID;

	
	retry_counter=5;
	
	do {
		error=0;
		DapiHandle->jobID = (DapiHandle->jobID)&0xff;

		buff_cnt=0;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
		buff[buff_cnt++] = 0xfe;
	
		buff[buff_cnt++] = 0xff;
		buff[buff_cnt++] = (unsigned char) DapiHandle->jobID;
		buff[buff_cnt++] = (unsigned char) command;

		buff[buff_cnt++] = (unsigned char) ((adress      ) & 0xff);
		buff[buff_cnt++] = (unsigned char) ((adress >>  8) & 0xff);
		buff[buff_cnt++] = (unsigned char) ((adress >> 16) & 0xff);
		buff[buff_cnt++] = (unsigned char) ((adress >> 24) & 0xff);

		buff[buff_cnt++] = (unsigned char) be;
		// das wird IMMER gesendet

		buff[buff_cnt++] = (unsigned char)  adress_tiefe;				// Adressraum-Tiefe
		buff[buff_cnt++] = (unsigned char) (adress_tiefe>>8) & 0xff;	// Adressraum-Tiefe
		buff[buff_cnt++] = (unsigned char)  wiederholungen;				// Anzahl Wiederholungen
		buff[buff_cnt++] = (unsigned char) (wiederholungen>>8) & 0xff;	// Anzahl Wiederholungen

		//sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - send_char_cnt = %d\n", buff_cnt);
		//debug_print(msg);

		if(command == cmd_sendnbyte)
		{
			// 
			for(i=0;i!=wiederholungen;++i)
			{
				for(j=0;j!=adress_tiefe;++j)
				{
					rd_offset = i*(adress_tiefe) + j;

					if(rd_offset < data_length) 
					{
						buff[buff_cnt++] = *(data + rd_offset);
					}
					else
					{
						buff[buff_cnt++] = 0;

					}
				}
			}
		}


		sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - send_char_cnt = %d\n", (unsigned int) buff_cnt);
		debug_print(msg);

		ULONG wr_pos = 0;
		ULONG wr_size = 64*1024;			// Darf wg. Timeout nicht hoeher sein
		while(buff_cnt > 0)
		{
			if(buff_cnt > wr_size)
			{
				dStatus = FT_Write(DapiHandle->ftHandle, buff + wr_pos, wr_size, &buff_cnt_ret);
				sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - FT_Write( gibt zurück : buff_cnt_ret=%d\n", (unsigned int) buff_cnt_ret);
				debug_print(msg);
				wr_pos += wr_size;
				buff_cnt -= wr_size;
			}
			else
			{
				dStatus = FT_Write(DapiHandle->ftHandle, buff + wr_pos, buff_cnt, &buff_cnt_ret);
				sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - FT_Write( gibt zurück : buff_cnt_ret=%d\n", (unsigned int) buff_cnt_ret);
				debug_print(msg);
				buff_cnt = 0;
			}
		}
	
		if(dStatus != FT_OK)
		{
			DapiFTDIErrorHandler(handle, dStatus);
		}

		
	
		if(IsFTDIError(dStatus)) return 0;

//		sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - B\n");
//		debug_print(msg);

/*		if(command==cmd_ping) 
		{
			get_char_cnt=7;		// Start_char + job_id + 4bytes-data + end_char
		}

		if(command==cmd_send8byte) 
		{
			get_char_cnt=3;	// Start_char + job_id + 0bytes-data + end_char
		}
*/
		if(command==cmd_getnbyte) 
		{
			get_char_cnt=3 + ((adress_tiefe+1)*wiederholungen);	// Start_char + job_id + n*bytes-data + end_char
		}
		if(command==cmd_sendnbyte) 
		{
			get_char_cnt=3 + 2;	// Start_char + job_id + 2Bytes(wiederholungen) + end_char
		}

		sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - get_char_cnt = %d\n", (unsigned int) get_char_cnt);
		debug_print(msg);

		ret = FTread_n_bytes_with_timeout(DapiHandle->ftHandle, get_char_cnt, buff, (get_char_cnt*50) + 400*1000);	// max ..  + 400ms Reserve

	
		if(ret != get_char_cnt)
		{
			sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - ft_send received char count = %d, sollte sein = %d\n", (unsigned int) ret, (unsigned int) get_char_cnt);
			debug_print(msg);
			++error;
		}
		else
		{
			if(error!=0)
			{
				dStatus = FT_GetQueueStatus(DapiHandle->ftHandle, &buff_cnt_ret);
				if(IsFTDIError(dStatus)) return 0;
			}

			if(command==cmd_getnbyte) 
			{
				buff_cnt=0;
				for(j=0;j!=wiederholungen;++j) 
				{
					if(buff[j*(adress_tiefe+1) + 2] != (j&0xff))
					{
						sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - buff[j*(adress_tiefe+1)] = %x (sollte sein =%x)\n", (unsigned int) buff[j*(adress_tiefe+1) + 2], (unsigned int) j&0xff);
						debug_print(msg);
						++error;
					}
					else
					{
						for(i=0;i!=adress_tiefe;++i) 
						{
							if(buff_cnt<data_length) *(data+buff_cnt++) = buff[j*(adress_tiefe+1) + i + 1 + 2];	// 1 more (filter ID for repeats; 2 more (2 bytes header
						}
					}
				}
			}

			if(buff[0] != 0x1A) 
			{
				sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - ft_send_buff[0]=%x sollte sein=%x\n", (unsigned int) buff[0], 0x1a);
				debug_print(msg);
				++error;
			}
	
			if(buff[1] != DapiHandle->jobID) 
			{
				sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - ft_send_buff[1]=%x sollte sein=%x\n", (unsigned int) buff[1], (unsigned int) DapiHandle->jobID);
				debug_print(msg);
				++error;
			}

			if(buff[get_char_cnt-1] != 0x4f) 
			{
				sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - ft_send_buff[%d]=%x sollte sein=%x\n", (unsigned int) get_char_cnt, (unsigned int) buff[get_char_cnt-1], 0x4f);
				debug_print(msg);
				++error;
			}

			// ------------------------
			if(command==cmd_sendnbyte) 
			{
				if(buff[2] != (wiederholungen&0xff)) 
				{
					sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - ft_send_buff[2]=%x sollte sein=%x\n", (unsigned int) buff[2], (unsigned int) (wiederholungen&0xff));
					debug_print(msg);
					++error;
				}
				if(buff[3] != ((wiederholungen>>8)&0xff)) 
				{
					sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - ft_send_buff[3]=%x sollte sein=%x\n", (unsigned int) buff[3], (unsigned int) ((wiederholungen>>8)&0xff));
					debug_print(msg);
					++error;
				}
			}
			// ------------------------
		}

		if(error!=0) 
		{
			sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - ft_send_buff[%d]=%x sollte sein=%x\n", (unsigned int) get_char_cnt, (unsigned int) buff[get_char_cnt-1], (unsigned int) 0x4f);
			debug_print(msg);
		}

		if(error!=0) 
		{
			sprintf(msg,"|DELIB|DapiFTDISendCommandMultipleBytes - RETRY RETRY RETRY RETRY = %d\n", (unsigned int) retry_counter);
			debug_print(msg);
			//getch();
		}

		--retry_counter;

	}while((error!=0) && (retry_counter>0));


	--second_call_prevent;	// prevent a background call 


	free(buff);


	if(error==0) 
	{
		return TRUE;	// kein Fehler
	}
	else 
	{
		return FALSE;	// Fehler
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------









	struct ftdi_context global_ftdic;


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FT_STATUS WINAPI FT_Write(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesWritten)
{
	UCHAR* buff;
	char msg[200];

	buff = lpBuffer;
	ULONG i;
    int f;
	
	f = ftdi_write_data((struct ftdi_context *) ftHandle, buff, nBufferSize);
    if (f < 0)
    {
		sprintf(msg,"Error Writing %d bytes ", (int) nBufferSize);
		debug_print(msg);
//		sprintf(msg,write ftdic = 0x%x\n", ftHandle);
//		debug_print(msg);
		for(i=0;i!=nBufferSize;++i) {
			sprintf(msg,"%x ", (char) *(buff + i));
			debug_print(msg);
		}
        sprintf(msg,"write failed error %d (%s)\n",f, ftdi_get_error_string((struct ftdi_context *) ftHandle));
		debug_print(msg);
		*lpBytesWritten = 0;
		//exit -1;
		return -1;
    }

//printf("WRITTEN %d bytes\n", f);

	*lpBytesWritten = f;
	return FT_OK;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FTD2XX_API FT_STATUS WINAPI FT_Read(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesReturned)
{
	char msg[200];
	UCHAR* buff;
	int f;

	buff = lpBuffer;

	f=ftdi_read_data((struct ftdi_context *) ftHandle, buff, nBufferSize);
    if (f < 0)
    {
        sprintf(msg,"read failed error %d (%s)\n",f, ftdi_get_error_string((struct ftdi_context *) ftHandle));
		debug_print(msg);
		*lpBytesReturned = 0;
		exit;
    }

	*lpBytesReturned = f;
	return FT_OK;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FTD2XX_API FT_STATUS WINAPI FT_GetQueueStatus(FT_HANDLE ftHandle, DWORD *dwRxBytes)
{
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FTD2XX_API FT_STATUS WINAPI FT_Open(int deviceNumber, FT_HANDLE *pHandle)
{
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FTD2XX_API FT_STATUS WINAPI FT_Close(FT_HANDLE ftHandle)
{
    ftdi_usb_close((struct ftdi_context *) ftHandle);
    ftdi_deinit((struct ftdi_context *) ftHandle);

	return FT_OK;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FTD2XX_API FT_STATUS WINAPI FT_OpenEx(PVOID pArg1, DWORD Flags, FT_HANDLE *pHandle)
{
	char msg[200];
	unsigned long len;
	unsigned char *pSN;
	int f;

	pSN = (char*) pArg1;

    if (ftdi_init(&global_ftdic) < 0)
    {
        sprintf(msg,"ftdi_init failed\n");
		debug_print(msg);
        return -1;
    }



	if(Flags == FT_OPEN_BY_SERIAL_NUMBER)
	{
		len=strlen(pSN);
		if( *(pSN + len -1) == 'B')
		{
			*(pSN + len -1) = 0;
			// B abschneiden

			ftdi_set_interface(&global_ftdic, INTERFACE_B);
			f = ftdi_usb_open_desc_index(&global_ftdic, 0x0403, 0x6010, 0, pArg1, 0);	// FT2232 (DUAL)
		}
		else
		{
			f = ftdi_usb_open_desc_index(&global_ftdic, 0x0403, 0x6001, 0, pArg1, 0);	// FT232  (Single)
		}

	
	
	
		if (f < 0 && f != -5)
		{
			sprintf(msg,"unable to open ftdi device: %d (%s)\n", (unsigned int) f, (char*) ftdi_get_error_string(&global_ftdic));
			//printf("%s", msg);
			debug_print(msg);
			*pHandle = (FT_HANDLE) 0;
	*pHandle = (FT_HANDLE) &global_ftdic;
			return -1;
	    }
	}

	*pHandle = (FT_HANDLE) &global_ftdic;

	return FT_OK;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FTD2XX_API
FT_STATUS WINAPI FT_Purge(
    FT_HANDLE ftHandle,
	ULONG Mask
	)
{
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

