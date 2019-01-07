// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// delib_io_tcp
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#ifdef __linux__

#include "project-include-eth.h"

#ifdef USE_FTDI
#include "../usb/ftd2xx.h"
#endif

#include "delib.h"
#include "delib_intern.h"
#include <stdio.h>

#define __stdcall

//#include "/library/vc/delib_extended_functions.h"
//#include "/library/vc/deditec_tcp_io.h"
//#include "/library/vc/dt_embedded_parameter.h"
//#include "/library/vc/deditec_tcp_functions.h"
//#include "delib_extended_functions.h"
#include "deditec_tcp_io.h"
#include "dt_embedded_parameter.h"
#include "deditec_tcp_functions.h"
#else


#include <stdio.h>
//#include "\library\vc\comport.h"


//#include "\library\vc\ftdi\ftd2xx.h"

#include "..\includes\delib.h"
#include "delib_intern.h"

#include "delib_set_get_error.h"

//#include "\library\vc\deditec_string_utils.h"

//#include "..\includes\delib_extended_functions.h"
//#include "e:\linux_work\library\vc\deditec_tcp_io.h"
//#include "e:\linux_work\library\vc\dt_embedded_parameter.h"

//#include "e:\linux_work\library\vc\deditec_tcp_functions.h"

#endif


//#include "convert_functions.h"

//#define debug_direkt_ausgabe 1


#define DapiTCPSendCommandRetryMax	5



extern void DapiTCPWriteByte(unsigned long handle, unsigned long adress, unsigned long value);


unsigned char global_ip_addr[IP_ADDR_SERVER_SIZE];


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


ULONG DapiOpenModuleIntern_ETH(ULONG handle, ULONG moduleID, ULONG moduleNr)
{
	unsigned char ip_addr[IP_ADDR_SERVER_SIZE];
	char msg[200];
	ULONG ret;
	ULONG port_no;
	ULONG enabled;
	char reg_port[30];
	char reg_hostname[70];
	char reg_enabled[30];
	ULONG i;

	DAPI_HANDLE *DapiHandle;


   	DapiHandle = (DAPI_HANDLE*) handle;

	if (handle ==0)
	{
		sprintf(msg,"|DELIB|DapiOpenModuleIntern_ETH - Invalid Handle (handle=0)\n");
		debug_print(msg);
		return FALSE;
	}

	if (DapiHandle->id != delib_id)
	{
		sprintf(msg,"|DELIB|DapiOpenModuleIntern_ETH - Invalid Handle-ID\n");
		debug_print(msg);
		return FALSE;
	}

	if(DapiCheckHandle(handle) != 0) return -1;


#ifndef __linux__
	sprintf(reg_hostname, "Hostname%ld", moduleNr);
	sprintf(reg_port, "Port%ld", moduleNr);
	sprintf(reg_enabled, "Enabled%ld", moduleNr);

	// Registry read Server Address (hostname) and Port-No
	ret = DapiInternCommand(DAPI_INTERN_CMD_GET_REGESTRY_STRING, (ULONG) "DELIB-CONFIG\\RO-ETH\\Config", (ULONG) reg_hostname, (ULONG ) &ip_addr, IP_ADDR_SERVER_SIZE, 0);
	ret = DapiInternCommand(DAPI_INTERN_CMD_GET_REGESTRY_DWORD,  (ULONG) "DELIB-CONFIG\\RO-ETH\\Config", (ULONG) reg_port, 0, 4, (ULONG *) &port_no);
	ret = DapiInternCommand(DAPI_INTERN_CMD_GET_REGESTRY_DWORD,  (ULONG) "DELIB-CONFIG\\RO-ETH\\Config", (ULONG) reg_enabled, 0, 4, (ULONG *) &enabled);
#else
	for(i=0;i!=IP_ADDR_SERVER_SIZE;++i) ip_addr[i]=global_ip_addr[i];
//	printf("ip-addr=%s\n", ip_addr);
	port_no=0;
	enabled=1;
#endif


	if(enabled != 1)
	{
		// Enabled in registry is not =1
		sprintf(msg,"|DELIB|DapiOpenModuleIntern_ETH - enabled !=1 -> return 0\n");;
		debug_print(msg);

		return -1;
	}
	else
	{

		if(port_no==0)
		{
			// Default Port
			port_no = 9912;//DEDITEC_DEFAULT_PORT_NO;
		}

		sprintf(msg,"|DELIB|DapiOpenModuleIntern_ETH - trying to connect to server %s:%ld\n", ip_addr, port_no);
		debug_print(msg);

		ret = tcp_tx_open(ip_addr, sizeof(ip_addr), port_no); //handle is false, it contains adr. of tcp_params

		sprintf(msg,"|DELIB|DapiOpenModuleIntern_ETH - tcp_tx_open returns %ld dez\n", ret);
		debug_print(msg);

		if (ret==-1 || ret==-2 || ret==-3 || ret==-4 || ret==-5|| ret==-6) // socket error
		{
			// Connect Error
			return -1;
		}
		else
		{
			DapiHandle->TCPHandle = ret;

			DapiHandle->moduleID = moduleID;
			DapiHandle->moduleNr = moduleNr;
	//		DapiHandle->ftHandle = moduleNr;		// == COM Schnittstelle
			DapiHandle->SubDevice = 2;
		}
	}
	return 0;	// -> ok

}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool DapiTCPSendCommand(ULONG handle, UCHAR tx[], unsigned long tx_length, UCHAR rx[], unsigned long rx_anzahl)
{
	DAPI_HANDLE *DapiHandle;

	unsigned long ret;
	unsigned long retry=DapiTCPSendCommandRetryMax;


	char buffer[100][128];

	unsigned long buff_cnt=0;

	unsigned char modul_nr = 0;

	TCP_IO * tcp_params;


	sprintf(buffer[buff_cnt++],"|DELIB|DapiTCPSendCommand(tx_length=%ld tx_buffer[0]=%c, tx_buffer[1]=%c rx_cnt=%ld\n", tx_length, tx[0],tx[1], rx_anzahl);
	#ifdef debug_direkt_ausgabe
		OutputDebugString(buffer[buff_cnt-1]);
	#endif

		
	DapiHandle = (DAPI_HANDLE*) handle;

	tcp_params = (TCP_IO *) DapiHandle->TCPHandle; // typecasting pointer handle to pointer (of structure) TCP_IO (see deditec_tcp_io.h)

	if (handle ==0)
	{
		sprintf(buffer[buff_cnt++],"|DELIB|TCPSendCommand - Invalid Handle (handle=0)\n");
		debug_print(buffer[buff_cnt-1]);
		return FALSE;
	}

	if (DapiHandle->id != delib_id)
	{
		sprintf(buffer[buff_cnt++],"|DELIB|TCPSendCommand - Invalid Handle-ID\n");
		debug_print(buffer[buff_cnt-1]);
		return FALSE;
	}

	if(DapiCheckHandle(handle) != 0) return FALSE;

	++DapiHandle->jobID;
		
	ret=tcp_tx_send_rec_data(tcp_params->tcp_handle_tx, DapiHandle->jobID, DEDITEC_TCP_PACKET_ID_2_CMD_RO_1, tx, tx_length, rx, rx_anzahl);


	return TRUE;
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool DapiTCPSendSpecialCommand(ULONG handle, UCHAR tx[], unsigned long tx_length, UCHAR rx[], unsigned long rx_anzahl)
{
	DAPI_HANDLE *DapiHandle;
	TCP_IO * tcp_params;

	unsigned long ret;
	unsigned long retry=DapiTCPSendCommandRetryMax;


	char buffer[100][128];

	unsigned long buff_cnt=0;

	unsigned char modul_nr = 0;

	sprintf(buffer[buff_cnt++],"|DELIB|DapiTCPSendCommand(tx_length=%ld tx_buffer[0]=%c, tx_buffer[1]=%c rx_cnt=%ld\n", tx_length, tx[0],tx[1], rx_anzahl);
	#ifdef debug_direkt_ausgabe
		OutputDebugString(buffer[buff_cnt-1]);
	#endif

		

		
	DapiHandle = (DAPI_HANDLE*) handle;

	tcp_params = (TCP_IO *) DapiHandle->TCPHandle; // typecasting pointer handle to pointer (of structure) TCP_IO (see deditec_tcp_io.h)

	if (handle ==0)
	{
		sprintf(buffer[buff_cnt++],"|DELIB|TCPSendCommand - Invalid Handle (handle=0)\n");
		debug_print(buffer[buff_cnt-1]);
		return FALSE;
	}

	if (DapiHandle->id != delib_id)
	{
		sprintf(buffer[buff_cnt++],"|DELIB|TCPSendCommand - Invalid Handle-ID\n");
		debug_print(buffer[buff_cnt-1]);
		return FALSE;
	}

	if(DapiCheckHandle(handle) != 0) return FALSE;

	++DapiHandle->jobID;
		
	ret=tcp_tx_send_rec_data(tcp_params->tcp_handle_tx, DapiHandle->jobID, DEDITEC_TCP_PACKET_ID_2_CMD_SPECIAL, tx, tx_length, rx, rx_anzahl);


	return TRUE;
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiTCPWriteByte(unsigned long handle, unsigned long adress, unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='B';		// BYTE
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

	tx_buffer[tx_cnt++]=(UCHAR) (value >> 0  ) & 0xff;

	if(!DapiTCPSendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiTCPWriteWord (unsigned long handle,unsigned long adress,unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='W';		// WORD
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;
	
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 8  ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 0  ) & 0xff;

	if(!DapiTCPSendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiTCPWriteLong (unsigned long handle,unsigned long adress,unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='L';		// Long
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

	tx_buffer[tx_cnt++]=(UCHAR) (value >> 24 ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 16 ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 8  ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 0  ) & 0xff;

	if(!DapiTCPSendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


void DapiTCPWriteLongLong (unsigned long handle,unsigned long adress, ULONGLONG value)
{
	
	UCHAR tx_buffer[256];
	unsigned long tx_cnt = 0;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='X';		// LongLong
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

	tx_buffer[tx_cnt++]=(UCHAR) (value >> 56 ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 48 ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 40 ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 32 ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 24 ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 16 ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 8  ) & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (value >> 0  ) & 0xff;

	if(!DapiTCPSendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


unsigned long DapiTCPReadByte (unsigned long handle,unsigned long adress)
{
	UCHAR tx_buffer[256];
	UCHAR rx[256];
	unsigned long data;
	unsigned long tx_cnt=0;
	unsigned long rx_cnt;

	tx_buffer[tx_cnt++]='R';		// Read
	tx_buffer[tx_cnt++]='B';		// BYTE

	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

	rx_cnt=1;		// 1 Byte soll empfangen werden

	if(DapiTCPSendCommand(handle, tx_buffer, tx_cnt, rx, rx_cnt)) 
	{
		data =((unsigned long) (rx[0]) << 0) & 0x00ff;
		return data;
	}
	else 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
		return(0L);
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

unsigned long DapiTCPReadWord (unsigned long handle,unsigned long adress)
{
	UCHAR tx_buffer[256];
	UCHAR rx[256];
	unsigned long data;
	unsigned long tx_cnt=0;
	unsigned long rx_cnt;


	tx_buffer[tx_cnt++]='R';		// Read
	tx_buffer[tx_cnt++]='W';		// WORD
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

	rx_cnt=2;		// 2 Bytes sollen empfangen werden

	if(DapiTCPSendCommand(handle, tx_buffer, tx_cnt, rx, rx_cnt)) 
	{
		data =((unsigned long) (rx[1]) << 8) & 0xff00;
		data|=((unsigned long) (rx[0]) << 0) & 0x00ff;
		return data;
	}
	else 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
		return(0L);
	}
}
	
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


unsigned long DapiTCPReadLong(unsigned long handle, unsigned long adress)
{
	UCHAR tx_buffer[256];
	UCHAR rx[256];
	unsigned long data;
	unsigned long tx_cnt=0;
	unsigned long rx_cnt;


	tx_buffer[tx_cnt++]='R';		// Read
	tx_buffer[tx_cnt++]='L';		// LONG
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

	rx_cnt=4;		// 4 Bytes sollen empfangen werden

	if(DapiTCPSendCommand(handle, tx_buffer, tx_cnt, rx, rx_cnt)) 
	{
		data =((unsigned long) (rx[3]) << 24) & 0xff000000;
		data|=((unsigned long) (rx[2]) << 16) & 0x00ff0000;
		data|=((unsigned long) (rx[1]) << 8 ) & 0x0000ff00;
		data|=((unsigned long) (rx[0]) << 0 ) & 0x000000ff;

		return data;
	}
	else 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
		return(0L);
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


ULONGLONG DapiTCPReadLongLong(unsigned long handle, unsigned long adress)
{
	UCHAR tx_buffer[256];
	UCHAR rx[256];
	ULONGLONG data;
	unsigned long tx_cnt = 0;
	unsigned long rx_cnt;

	tx_buffer[tx_cnt++]='R';		// Read
	tx_buffer[tx_cnt++]='X';		// LONGLONG
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

	rx_cnt=8;		// 8 Bytes sollen empfangen werden
	if(DapiTCPSendCommand(handle, tx_buffer, tx_cnt, rx, rx_cnt)) 
	{
#ifdef __linux__
		data =((ULONGLONG) (rx[7]) << 56) & 0xff00000000000000LL;
		data|=((ULONGLONG) (rx[6]) << 48) & 0x00ff000000000000LL;
		data|=((ULONGLONG) (rx[5]) << 40) & 0x0000ff0000000000LL;
		data|=((ULONGLONG) (rx[4]) << 32) & 0x000000ff00000000LL;
		data|=((ULONGLONG) (rx[3]) << 24) & 0x00000000ff000000LL;
		data|=((ULONGLONG) (rx[2]) << 16) & 0x0000000000ff0000LL;
		data|=((ULONGLONG) (rx[1]) << 8 ) & 0x000000000000ff00LL;
		data|=((ULONGLONG) (rx[0]) << 0 ) & 0x00000000000000ffLL;
#else
		data =((ULONGLONG) (rx[7]) << 56) & 0xff00000000000000;
		data|=((ULONGLONG) (rx[6]) << 48) & 0x00ff000000000000;
		data|=((ULONGLONG) (rx[5]) << 40) & 0x0000ff0000000000;
		data|=((ULONGLONG) (rx[4]) << 32) & 0x000000ff00000000;
		data|=((ULONGLONG) (rx[3]) << 24) & 0x00000000ff000000;
		data|=((ULONGLONG) (rx[2]) << 16) & 0x0000000000ff0000;
		data|=((ULONGLONG) (rx[1]) << 8 ) & 0x000000000000ff00;
		data|=((ULONGLONG) (rx[0]) << 0 ) & 0x00000000000000ff;
#endif
		return data;
	}
	else 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
		return(0L);
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*void DapiTCPWriteBitClear(unsigned long handle,unsigned long adress,unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

		tx_buffer[tx_cnt++]='C';		// ClearBit
		tx_buffer[tx_cnt++]='L';		// Long
		tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
		tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 28 ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 24 ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 20 ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 16 ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 12 ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 8  ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 4  ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value       ) & 0x0f);

		if(!DapiTCPSendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
		{
			DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
		}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiTCPWriteBitSet(unsigned long handle,unsigned long adress,unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

		tx_buffer[tx_cnt++]='S';		// SetBit
		tx_buffer[tx_cnt++]='L';		// Long
		tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
		tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 28 ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 24 ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 20 ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 16 ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 12 ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 8  ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 4  ) & 0x0f);
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value       ) & 0x0f);

		if(!DapiTCPSendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
		{
			DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
		}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiTCPWriteBitClearLONGLONG (unsigned long handle,unsigned long adress,ULONGLONG value)
{
	
	UCHAR tx_buffer[256];
	unsigned long tx_cnt = 0;

	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

		tx_buffer[tx_cnt++]='C';		// Clear
		tx_buffer[tx_cnt++]='X';		// LongLong
		tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
		tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 60 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 56 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 52 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 48 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 44 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 40 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 36 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 32 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 28 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 24 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 20 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 16 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 12 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 8  ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 4  ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value       ) & 0x0f));
		if(!DapiTCPSendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
		{
			DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
		}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiTCPWriteBitSetLONGLONG (unsigned long handle,unsigned long adress,ULONGLONG value)
{
	
	UCHAR tx_buffer[256];
	unsigned long tx_cnt = 0;

	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

		tx_buffer[tx_cnt++]='S';		// Clear
		tx_buffer[tx_cnt++]='X';		// LongLong
		tx_buffer[tx_cnt++]=(UCHAR) (adress >> 8)  & 0xff;
		tx_buffer[tx_cnt++]=(UCHAR) (adress >> 0 ) & 0xff;

		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 60 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 56 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 52 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 48 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 44 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 40 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 36 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 32 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 28 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 24 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 20 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 16 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 12 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 8  ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value >> 4  ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((value       ) & 0x0f));
		if(!DapiTCPSendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
		{
			DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
		}
}
*/
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiTCPSpecialCmd (unsigned long handle,unsigned long cmd)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='Z';		// SecialCmd
	tx_buffer[tx_cnt++]='C';		// SecialCmd

	tx_buffer[tx_cnt++]=(UCHAR) (cmd >> 8)  & 0xff;
	tx_buffer[tx_cnt++]=(UCHAR) (cmd >> 0 ) & 0xff;
	
	if(!DapiTCPSendSpecialCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
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


ULONG DapiTCPWriteMultipleBytes(ULONG handle, ULONG adress, ULONG adress_tiefe, ULONG wiederholungen, unsigned char* data, ULONG data_length)
{

	ULONG adr;
	ULONG anz;
	ULONG pos=0;
	ULONGLONG d64;

	for(anz=0;anz!=wiederholungen;++anz)
	{
		adr=adress;
		
		do 
		{
			if((adr+8) <= (adress+adress_tiefe))
			{
				// 8 Byte

				d64 =  (ULONGLONG) *(data+pos+0);
				d64|= ((ULONGLONG) *(data+pos+1)) << 8;
				d64|= ((ULONGLONG) *(data+pos+2)) << 16;
				d64|= ((ULONGLONG) *(data+pos+3)) << 24;
				d64|= ((ULONGLONG) *(data+pos+4)) << 32;
				d64|= ((ULONGLONG) *(data+pos+5)) << 40;
				d64|= ((ULONGLONG) *(data+pos+6)) << 48;
				d64|= ((ULONGLONG) *(data+pos+7)) << 56;

				DapiWriteLongLong(handle, adr, d64);
				adr+=8;
				pos+=8;
			}
			else
			{
				DapiWriteByte(handle, adr, *(data+pos));
				adr++;
				pos++;
			}
		} while(adr < (adress+adress_tiefe));
	}


/*

	UCHAR tx_buffer[256];
	unsigned long tx_cnt = 0;
	ULONG cnt;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='M';		// Multiple

	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);

	// adress_tiefe
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress_tiefe >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress_tiefe      ) & 0x0f);

	// wiederholungen
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((wiederholungen >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((wiederholungen      ) & 0x0f);

	//Data
	for(cnt=0;cnt!=data_length;++cnt)
	{
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((*(data+cnt) >> 4 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((*(data+cnt)      ) & 0x0f));
	}

	if(!DapiTCPSendSpecialCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}*/
	return 0;
}

