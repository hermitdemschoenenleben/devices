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
// dtapi_io_SeriellFTDI
//
//
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************

#ifdef __linux__
#define bool unsigned long

#include "../usb/ftd2xx.h"
#include "delib.h"
#include "delib_intern.h"
#include <stdio.h>




#else

#include <windows.h>
#include <stdio.h>

#include "\library\vc\ftdi\ftd2xx.h"

#include "delib.h"
#include "delib_intern.h"

#include "delib_set_get_error.h"

#include "delib_io_ftdi.h"

#endif


#include "convert_functions.h"

#define debug_direkt_ausgabe 1


#define DapiSeriellFTDISendCommandRetryMax	5


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool DapiSeriellFTDISendCommand(ULONG handle, UCHAR tx[], unsigned long tx_length, UCHAR rx[], unsigned long rx_anzahl)
{
	DAPI_HANDLE *DapiHandle;
	FT_STATUS ftStatus;

	unsigned long i;
	unsigned long ret;
	unsigned long retry=DapiSeriellFTDISendCommandRetryMax;

	unsigned char checksum;
	unsigned char rx_count;

	unsigned char rx_data[100];
	UCHAR rx_pos;
	unsigned long ok;
	UCHAR tx_buffer[200];
	UCHAR tx_buffer_cnt;

	char buffer[100][128];
	unsigned char send_char;
	unsigned long buff_cnt=0;

	unsigned char modul_nr;// = 0;


		
	DapiHandle = (DAPI_HANDLE*) handle;

	if (handle ==0)
	{
		sprintf(buffer[buff_cnt++],"|DELIB|FTDISendCommand - Invalid Handle (handle=0)\n");
		debug_print(buffer[buff_cnt-1]);
		return FALSE;
	}

	if (DapiHandle->id != delib_id)
	{
		sprintf(buffer[buff_cnt++],"|DELIB|FTDISendCommand - Invalid Handle-ID\n");
		debug_print(buffer[buff_cnt-1]);
		return FALSE;
	}

	if(DapiCheckHandle(handle) != 0) return FALSE;


	modul_nr = (unsigned char) DapiHandle->RS485_ModuleNr;

	sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand(tx_length=%d tx_buffer[0]=%c, tx_buffer[1]=%c rx_cnt=%d - rs485_mod_nr=%d\n", (unsigned int) tx_length, (unsigned int) tx[0],(unsigned int) tx[1], (unsigned int) rx_anzahl, (unsigned int) modul_nr );
	#ifdef debug_direkt_ausgabe
		OutputDebugString(buffer[buff_cnt-1]);
	#endif
	

	++DapiHandle->jobID;
		
	
		
		
		do {

		if(retry!=DapiSeriellFTDISendCommandRetryMax)
		{
			// Empfangsbuffer flushen
//			do 
//			{
			//	ret=comport->read((unsigned char*) &rx_data, 1);	// 1*Zeichen
//////////////////////
//////////////////////
//////////////////////
//////////////////////



	ret=FTread_n_bytes_with_timeout(DapiHandle->ftHandle, 200, rx_data, 1);	// Read x Byte (max 1ms->zum flushen)
//FT_Purge( DapiHandle->ftHandle, FT_PURGE_RX);
//FT_Purge( DapiHandle->ftHandle, FT_PURGE_RX | FT_PURGE_TX);

//			} while(ret==TRUE);
		}

		// String senden

		//FLUSH
		//ret=comport->flush();
		//////////////////////
//////////////////////
//////////////////////
//////////////////////

		ok=0;
	
		// -----------------------------------------------------------
		// Ab hier wird die Checksumme berechnet
	
		checksum=0;
	
		tx_buffer_cnt=0;
		tx_buffer[tx_buffer_cnt++] = 0x01;		// SOH ASCII=StartOfHeader
		checksum+= 0x01;


		// Sende Modul-Nr - H
		send_char = (UCHAR) convert_hex_to_ascii((modul_nr >> 4) & 0x0f);
		tx_buffer[tx_buffer_cnt++] = send_char;
		checksum += send_char;

		// Sende Modul-Nr - L
		send_char = (UCHAR) convert_hex_to_ascii((modul_nr >> 0) & 0x0f);
		tx_buffer[tx_buffer_cnt++] = send_char;
		checksum += send_char;

		// Sende Job-ID - H
		send_char = (UCHAR) convert_hex_to_ascii((DapiHandle->jobID >> 4) & 0x0f);
		tx_buffer[tx_buffer_cnt++] = send_char;
		checksum += send_char;

		// Sende Job-ID - L
		send_char = (UCHAR) convert_hex_to_ascii((DapiHandle->jobID >> 0) & 0x0f);
		tx_buffer[tx_buffer_cnt++] = send_char;
		checksum += send_char;


		// Zeichen aus Sendestring dazu

		for(i=0;i<tx_length;++i)
		{
			checksum+=tx[i];
			tx_buffer[tx_buffer_cnt++] = tx[i];
		}
		// Checksumme dazu
		tx_buffer[tx_buffer_cnt++]=(UCHAR) convert_hex_to_ascii((checksum >> 4) & 0x0f);
		tx_buffer[tx_buffer_cnt++]=(UCHAR) convert_hex_to_ascii((checksum     ) & 0x0f);
	
		tx_buffer[tx_buffer_cnt++] = 13;		// Abschlusszeichen


	
		sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand * jetzt senden");
		#ifdef debug_direkt_ausgabe
			OutputDebugString(buffer[buff_cnt-1]);
		#endif


			
		tx_buffer[tx_buffer_cnt]=0;		// Terminate String

		sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand * Sende string : (length=%d)= string=-->%s<--\r\n", tx_buffer_cnt, tx_buffer);
		#ifdef debug_direkt_ausgabe
		OutputDebugString(buffer[buff_cnt-1]);
		#endif

		// Alles an FTDI-Com senden
		ftStatus = FT_Write(DapiHandle->ftHandle, tx_buffer, tx_buffer_cnt, &ret);
	
		sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand * send:ret=%d",(unsigned int) ret);
		#ifdef debug_direkt_ausgabe
		OutputDebugString(buffer[buff_cnt-1]);
		#endif

	
		sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand * Senden fertig (retry=%d)\r\n",(unsigned int) retry);
		#ifdef debug_direkt_ausgabe
		OutputDebugString(buffer[buff_cnt-1]);
		#endif

		
		// -----------------------------------------------------------
		// Empfange jetzt einen Antwortstring (bis einschl. CR)
//		rx_count=0;
/*		do
		{
//			ret=comport->read((unsigned char*) &rx_data[rx_count], 1);	// 1*Zeichen

//////////////////////
//////////////////////
//////////////////////
//////////////////////
			ret=FTread_n_bytes_with_timeout(DapiHandle->ftHandle, 1, &rx_data[rx_count], 30000);	// Read 2 Byte (max 30ms)

			
			
			++rx_count;
		} while ((ret==1) && (rx_data[rx_count-1]!=13));		// Solange was kommt und kein CR den String terminiert !

*/


/*
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
*/







	ULONG	rx_nr = rx_anzahl + 6; // 'D' + 2*Job_id + 2* Checksum + CR -> 6






		rx_count = (unsigned char) FTread_n_bytes_with_timeout(DapiHandle->ftHandle, rx_nr, rx_data, 100000);	// Read 2 Byte (max 30ms)



		rx_data[rx_count]=0;		// Terminate String

		sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand *  Antwort (length=%d)= string=-->%s<--\r\n", rx_count, rx_data);
		#ifdef debug_direkt_ausgabe
		OutputDebugString(buffer[buff_cnt-1]);
		#endif


		if(rx_count!=0)
		{
			if(rx_data[0]=='E')	// 'E' = Error
			{
				// Error
				if(rx_count>1)
				{
					sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand * Received-Error Code = %c\r\n", rx_data[1]);
					#ifdef debug_direkt_ausgabe
					OutputDebugString(buffer[buff_cnt-1]);
					#endif
				}
			}
			if((rx_data[0]=='D') || (rx_data[0]=='O'))	// 'D' jetzt kommen Daten / 'O' Alles ok !
			{
				rx_pos=1;
				checksum=rx_data[0];

				// Job ID ok ??
				if(((UCHAR) convert_hex_to_ascii((DapiHandle->jobID >> 4) & 0x0f) == rx_data[rx_pos++]) && \
				   ((UCHAR) convert_hex_to_ascii((DapiHandle->jobID >> 0) & 0x0f) == rx_data[rx_pos++]))
				{

					// Job-ID ist ok !
					checksum+=rx_data[1];
					checksum+=rx_data[2];

					// Soll auch empfangen werden ???
					if(rx_anzahl!=0)	
					{
						
						for(i=0;i!=rx_anzahl;++i)
						{
							rx[i]=rx_data[rx_pos];	// Copy Data
							checksum+=rx_data[rx_pos];	// Add checksum

							sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand * Daten Empfang: rx=0x%x  rx_pos=%d rx_cnt=%d\r\n", (unsigned int) rx_data[rx_pos], (unsigned int) i, (unsigned int) rx_anzahl);
							#ifdef debug_direkt_ausgabe
							OutputDebugString(buffer[buff_cnt-1]);
							#endif

							++rx_pos;
						}
					}// if(rx_anzahl!=0)

					// Checksum ok ??
					if(((UCHAR) convert_hex_to_ascii((checksum >> 4) & 0x0f) == rx_data[rx_pos]) && \
					   ((UCHAR) convert_hex_to_ascii((checksum >> 0) & 0x0f) == rx_data[rx_pos+1]))
					{
						ok=1;
					}
					else
					{
						rx_pos+=2;
						sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand * Daten Empfang: checksum nicht ok !  calc_checksum = %x , rx_checksum = %c %c \n", checksum, rx_data[rx_pos-2], rx_data[rx_pos-1]);
						#ifdef debug_direkt_ausgabe
						OutputDebugString(buffer[buff_cnt-1]);
						#endif
						ok=0;
					}
				}
			}
		}//if(rx_count!=0)
		if(ok!=1)
		{
			// Nicht ok
			--retry;
		}

	} while((ok!=1) && (retry!=0));

	// all ok !
//	serFTDI_job_id++;			// NEW JOB-ID


	sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand * retry = %d -----------\r\n", (unsigned int) retry);
	#ifdef debug_direkt_ausgabe
	OutputDebugString(buffer[buff_cnt-1]);
	#endif

//	sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand * ok=%d\n", ok);
//	OutputDebugString(buffer[buff_cnt-1]);

	

	if(ok==1) 
	{
		return TRUE;
	}
	else
	{
	sprintf(buffer[buff_cnt++],"|DELIB|DapiSeriellFTDISendCommand * ok!=1   = %d\n", (unsigned int) ok);
	OutputDebugString(buffer[buff_cnt-1]);
		// ----------------
		//#ifdef debug_direkt_ausgabe
		for(i=0;i!=buff_cnt;++i)
		{
			OutputDebugString(buffer[i]);
		}
		//#endif
		// ----------------
		return FALSE;
	}
	}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiSeriellFTDIWriteByte(unsigned long handle, unsigned long adress, unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='B';		// BYTE
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 4  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value       ) & 0x0f);

	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiSeriellFTDIWriteWord (unsigned long handle,unsigned long adress,unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='W';		// WORD
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 12 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 8  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 4  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value       ) & 0x0f);
	
	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiSeriellFTDIWriteLong (unsigned long handle,unsigned long adress,unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='L';		// Long
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 28 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 24 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 20 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 16 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 12 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 8  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 4  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value       ) & 0x0f);

	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


void DapiSeriellFTDIWriteLongLong (unsigned long handle,unsigned long adress, ULONGLONG value)
{
	
	UCHAR tx_buffer[256];
	unsigned long tx_cnt = 0;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='X';		// LongLong
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);

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

	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


void DapiSeriellFTDIWrite64BytesMax (unsigned long handle,unsigned long adress, unsigned char *data, ULONG data_length)
{
	
	UCHAR tx_buffer[256];
	unsigned long tx_cnt = 0;
	ULONG cnt;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='M';		// Multiple
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);

	// Length
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((data_length >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((data_length      ) & 0x0f);

	//Data
	for(cnt=0;cnt!=data_length;++cnt)
	{
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((*(data+cnt) >> 4 ) & 0x0f));
		tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((unsigned long) ((*(data+cnt)      ) & 0x0f));
	}

	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


unsigned long DapiSeriellFTDIReadByte (unsigned long handle,unsigned long adress)
{
	UCHAR tx_buffer[256];
	UCHAR rx[256];
	unsigned long data;
	unsigned long tx_cnt=0;
	unsigned long rx_cnt;

	tx_buffer[tx_cnt++]='R';		// Read
	tx_buffer[tx_cnt++]='B';		// BYTE
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);

	rx_cnt=2;		// 1 Byte soll empfangen werden

	if(DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, rx, rx_cnt)) 
	{
		data =((unsigned long) convert_ascii_to_hex(rx[0])<<4) & 0x00f0;
		data|=((unsigned long) convert_ascii_to_hex(rx[1])   ) & 0x000f;
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

unsigned long DapiSeriellFTDIReadWord (unsigned long handle,unsigned long adress)
{
	UCHAR tx_buffer[256];
	UCHAR rx[256];
	unsigned long data;
	unsigned long tx_cnt=0;
	unsigned long rx_cnt;

	tx_buffer[tx_cnt++]='R';		// Read
	tx_buffer[tx_cnt++]='W';		// WORD
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);

	rx_cnt=4;		// 2 Bytes sollen empfangen werden

	if(DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, rx, rx_cnt)) 
	{
		data =((unsigned long) convert_ascii_to_hex(rx[0])<<12) & 0xf000;
		data|=((unsigned long) convert_ascii_to_hex(rx[1])<<8 ) & 0x0f00;
		data|=((unsigned long) convert_ascii_to_hex(rx[2])<<4 ) & 0x00f0;
		data|=((unsigned long) convert_ascii_to_hex(rx[3])    ) & 0x000f;
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


unsigned long DapiSeriellFTDIReadLong(unsigned long handle, unsigned long adress)
{
	UCHAR tx_buffer[256];
	UCHAR rx[256];
	unsigned long data;
	unsigned long tx_cnt=0;
	unsigned long rx_cnt;


	tx_buffer[tx_cnt++]='R';		// Read
	tx_buffer[tx_cnt++]='L';		// LONG
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);

	rx_cnt=8;		// 4 Bytes sollen empfangen werden

	if(DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, rx, rx_cnt)) 
	{
		data =((unsigned long) convert_ascii_to_hex(rx[0])<<28) & 0xf0000000;
		data|=((unsigned long) convert_ascii_to_hex(rx[1])<<24) & 0x0f000000;
		data|=((unsigned long) convert_ascii_to_hex(rx[2])<<20) & 0x00f00000;
		data|=((unsigned long) convert_ascii_to_hex(rx[3])<<16) & 0x000f0000;
		data|=((unsigned long) convert_ascii_to_hex(rx[4])<<12) & 0x0000f000;
		data|=((unsigned long) convert_ascii_to_hex(rx[5])<<8 ) & 0x00000f00;
		data|=((unsigned long) convert_ascii_to_hex(rx[6])<<4 ) & 0x000000f0;
		data|=((unsigned long) convert_ascii_to_hex(rx[7])    ) & 0x0000000f;

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


ULONGLONG DapiSeriellFTDIReadLongLong(unsigned long handle, unsigned long adress)
{
	UCHAR tx_buffer[256];
	UCHAR rx[256];
	ULONGLONG data;
	unsigned long tx_cnt = 0;
	unsigned long rx_cnt;

	tx_buffer[tx_cnt++]='R';		// Read
	tx_buffer[tx_cnt++]='X';		// LONGLONG
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);

	rx_cnt=16;		// 8 Bytes sollen empfangen werden
	if(DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, rx, rx_cnt)) 
	{
#ifdef __linux__
		data =(((ULONGLONG) convert_ascii_to_hex(rx[0] ))<<60) & 0xf000000000000000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[1] ))<<56) & 0x0f00000000000000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[2] ))<<52) & 0x00f0000000000000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[3] ))<<48) & 0x000f000000000000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[4] ))<<44) & 0x0000f00000000000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[5] ))<<40) & 0x00000f0000000000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[6] ))<<36) & 0x000000f000000000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[7] ))<<32) & 0x0000000f00000000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[8] ))<<28) & 0x00000000f0000000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[9] ))<<24) & 0x000000000f000000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[10]))<<20) & 0x0000000000f00000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[11]))<<16) & 0x00000000000f0000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[12]))<<12) & 0x000000000000f000LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[13]))<<8 ) & 0x0000000000000f00LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[14]))<<4 ) & 0x00000000000000f0LL;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[15]))    ) & 0x000000000000000fLL;
#else
		data =(((ULONGLONG) convert_ascii_to_hex(rx[0] ))<<60) & 0xf000000000000000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[1] ))<<56) & 0x0f00000000000000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[2] ))<<52) & 0x00f0000000000000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[3] ))<<48) & 0x000f000000000000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[4] ))<<44) & 0x0000f00000000000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[5] ))<<40) & 0x00000f0000000000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[6] ))<<36) & 0x000000f000000000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[7] ))<<32) & 0x0000000f00000000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[8] ))<<28) & 0x00000000f0000000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[9] ))<<24) & 0x000000000f000000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[10]))<<20) & 0x0000000000f00000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[11]))<<16) & 0x00000000000f0000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[12]))<<12) & 0x000000000000f000;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[13]))<<8 ) & 0x0000000000000f00;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[14]))<<4 ) & 0x00000000000000f0;
		data|=(((ULONGLONG) convert_ascii_to_hex(rx[15]))    ) & 0x000000000000000f;
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

void DapiSeriellFTDIWriteClearByte(unsigned long handle, unsigned long adress, unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='C';		// CLEAR
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 4  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value       ) & 0x0f);

	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiSeriellFTDIWriteSetByte(unsigned long handle, unsigned long adress, unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='W';		// Write
	tx_buffer[tx_cnt++]='S';		// Set
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 4  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value       ) & 0x0f);

	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*void DapiSeriellFTDIWriteBitClear(unsigned long handle,unsigned long adress,unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='C';		// ClearBit
	tx_buffer[tx_cnt++]='L';		// Long
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 28 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 24 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 20 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 16 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 12 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 8  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 4  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value       ) & 0x0f);

	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiSeriellFTDIWriteBitSet(unsigned long handle,unsigned long adress,unsigned long value)
{
	UCHAR tx_buffer[256];
	unsigned long tx_cnt=0;

	tx_buffer[tx_cnt++]='S';		// SetBit
	tx_buffer[tx_cnt++]='L';		// Long
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 28 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 24 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 20 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 16 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 12 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 8  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value >> 4  ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((value       ) & 0x0f);

	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiSeriellFTDIWriteBitClearLONGLONG (unsigned long handle,unsigned long adress,ULONGLONG value)
{
	
	UCHAR tx_buffer[256];
	unsigned long tx_cnt = 0;

	tx_buffer[tx_cnt++]='C';		// Clear
	tx_buffer[tx_cnt++]='X';		// LongLong
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);

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

	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
	{
		DapiSetError(DAPI_ERR_COMMUNICATION_ERROR, 0, NULL, __FILE__, __LINE__);					
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DapiSeriellFTDIWriteBitSetLONGLONG (unsigned long handle,unsigned long adress,ULONGLONG value)
{
	
	UCHAR tx_buffer[256];
	unsigned long tx_cnt = 0;

	tx_buffer[tx_cnt++]='S';		// Clear
	tx_buffer[tx_cnt++]='X';		// LongLong
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 12) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 8 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress >> 4 ) & 0x0f);
	tx_buffer[tx_cnt++]=(UCHAR) convert_hex_to_ascii((adress      ) & 0x0f);

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
	if(!DapiSeriellFTDISendCommand(handle, tx_buffer, tx_cnt, 0, 0)) 
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
