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
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "conio.h"


#include "\library\vc\ftdi\ftd2xx.h"

#include "..\includes\delib.h"
#include "delib_intern.h"
#include "..\includes\delib_extended_functions.h"
#include "delib_set_get_error.h"

#include "delib_io_ftdi.h"
#endif



extern unsigned long tcp_tx_send_rec_data_big(unsigned long   handle,
                                   unsigned char * tx_buffer, unsigned long tx_buffer_length,
                                   unsigned char * rx_buffer, unsigned long rx_buffer_length);



// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiDOSet1_WithTimer(ULONG handle, ULONG ch, ULONG data, ULONG time_ms)
{
	DapiWriteLong(handle, 0x200, time_ms);
	DapiWriteWord(handle, 0x204, (ch << 8) | data);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiDOSet1(ULONG handle, ULONG ch, ULONG data)
{
	unsigned long xx;//, xxalt;
	unsigned long mask;


	xx = DapiDOReadback32(handle, ch&0xffe0);

	mask = 1 << (ch&31);


	if(data == 0)
	{
		// clear Bit
		xx = xx & (~mask);
	}
	else
	{
		// set Bit
		xx = xx | mask;
	}
	
//	printf("DOSet1 ch=%d data = %d  ch&0xffe0=%x mask=%x  xxalt=%x xxneu=%x\n", ch,data,ch&0xffe0, mask,xxalt,xx);


	DapiDOSet32(handle, ch&0xffe0, xx);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiDOSet8(ULONG handle, ULONG ch, ULONG data)
{
	//DapiSetError(DAPI_ERR_FUNCTION_NOT_DEFINED, 0, NULL, __FILE__, __LINE__);					
	DapiWriteByte(handle, (ch>>3) & 0x1f, data);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiDOSet16(ULONG handle, ULONG ch, ULONG data)
{
	DapiWriteWord(handle, (ch>>3) & (~1) & 0x1f, data);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiDOSet32(ULONG handle, ULONG ch, ULONG data)
{
	DapiWriteLong(handle, (ch>>3) & (~3) & 0x1f, data);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiDOSet64(ULONG handle, ULONG ch, ULONGLONG data)
{
	DapiWriteLongLong(handle, (ch>>3) & (~7) & 0x1f, data);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiDIGet1(ULONG handle, ULONG ch)
{
	unsigned char xx;
	unsigned char mask;

	xx = (unsigned char) DapiDIGet8(handle, ch&0xfff8);

	mask = 1 << (ch&7);
	if((xx&mask) == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiDIGet8(ULONG handle, ULONG ch)
{
	ULONG ret;

	ret = DapiReadByte(handle, 0x20 | ((ch>>3) & 0x1f));
	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiDIGet16(ULONG handle, ULONG ch)
{
	ULONG ret;

	ret = DapiReadWord(handle, 0x20 | ((ch>>3) & 0x1f));

//	printf("read DI 16 ch=%d  ch>>3 = %d   ret=%d\n", ch, ch>>3, ret);
	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiDIGet32(ULONG handle, ULONG ch)
{
	ULONG ret;

	ret = DapiReadLong(handle, 0x20 | ((ch>>3) & 0x1f));
	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONGLONG DAPI_FUNCTION_PRE DapiDIGet64(ULONG handle, ULONG ch)
{
	ULONGLONG ret;

	ret = DapiReadLongLong(handle, 0x20 | ((ch>>3) & 0x1f));
	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiDIGetFF32(ULONG handle, ULONG ch)
{
	ULONG ret;

	ret = DapiReadLong(handle, 0x40 | ((ch>>3) & 0x1f));
	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiDIGetCounter(ULONG handle, ULONG ch, ULONG mode)
{
	ULONG adr;
	ULONG ret;

	adr = 0x100 + (ch&0x1f)*2;
	if(mode == DAPI_CNT_MODE_READ_WITH_RESET)
	{
		adr+=0x100;
	}
	else if(mode == DAPI_CNT_MODE_READ_LATCHED)
	{
		adr+=0x200;
	}

	ret = DapiReadWord(handle, adr);
	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiDOReadback32(ULONG handle, ULONG ch)
{
	ULONG ret;

	ret = DapiReadLong(handle, (ch>>3) & 0x1f);
	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONGLONG DAPI_FUNCTION_PRE DapiDOReadback64(ULONG handle, ULONG ch)
{
	ULONGLONG ret;

	ret = DapiReadLongLong(handle, (ch>>3) & 0x1f);
	return ret;
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

void DAPI_FUNCTION_PRE DapiADSetMode(ULONG handle, ULONG ch, ULONG mode)
{
	DapiWriteByte(handle, 0x1000 + ch*4 + 3, mode);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiADGetMode(ULONG handle, ULONG ch)
{
	return DapiReadByte(handle, 0x1000 + ch*4 + 3);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------



unsigned char global_ad_buffer[64*4];		// Max 64 chan





ULONG DAPI_FUNCTION_PRE DapiADGet(ULONG handle, ULONG ch)
{
	unsigned long ret;

	if ((ch & 0x8000) == 0)
	{
		return DapiReadWord(handle, 0x1000 + ch*4);
	}
	else
	{
		ret = ((unsigned long) global_ad_buffer[((ch&63) * 4) + 0]);
		ret|= ((unsigned long) global_ad_buffer[((ch&63) * 4) + 1]) << 8;
		return ret;
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

float DAPI_FUNCTION_PRE DapiADGetVolt(ULONG handle, ULONG ch)
{
	ULONG data;
	ULONG mode;
	float value=0;
	char msg[80];

	if ((ch & 0x8000) == 0)
	{
		data = DapiReadLong(handle, 0x1000 + ch*4);
	}
	else
	{
		data = ((unsigned long) global_ad_buffer[((ch&63) * 4) + 0]);
		data|= ((unsigned long) global_ad_buffer[((ch&63) * 4) + 1]) << 8;
		data|= ((unsigned long) global_ad_buffer[((ch&63) * 4) + 2]) << 16;
		data|= ((unsigned long) global_ad_buffer[((ch&63) * 4) + 3]) << 24;
	}

	mode = (data >> 24) & 0xff;

	switch(mode)
	{
		case ADDA_MODE_UNIPOL_10V:
			// 0-10V
			value = (((float) (data&0xffff)) *10.0F / 65536.0F);
			break;

		case ADDA_MODE_UNIPOL_5V:
			// 0-5V
			value = (((float) (data&0xffff)) *5.0F / 65536.0F);
			break;

		case ADDA_MODE_BIPOL_10V:
			// +-10V
			value = (((float) (data&0xffff)) *20.0F / 65536.0F) - 10.0F;
			break;

		case ADDA_MODE_BIPOL_5V:
			// +-5V
			value = (((float) (data&0xffff)) *10.0F / 65536.0F) - 5.0F;
			break;

		default:
			sprintf(msg, "Illegal A/D Mode detected (mode=0x%x)", (unsigned int) mode);
			DapiSetError(DAPI_ERR_ILLEGAL_MODE, 0, msg, __FILE__, __LINE__);					
			break;

	}

	
	return value;
}




//				printf("A/D Ch %d = 0x%4x  Wert=%f modus=%x\n", i, data_ad, (((float) data_ad) *20.0 / 65536) - 10, modus_ad);

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

float DAPI_FUNCTION_PRE DapiADGetmA(ULONG handle, ULONG ch)
{
	ULONG data;
	ULONG mode;
	float value=0;
	char msg[80];

	if ((ch & 0x8000) == 0)
	{
		data = DapiReadLong(handle, 0x1000 + ch*4);
	}
	else
	{
		data = global_ad_buffer[ch&63];
	}


	mode = data >> 24;

	switch(mode)
	{
		case ADDA_MODE_0_50mA:
			// 0-5V entspricht 0-50mA (100 Ohm)
			value = (((float) (data&0xffff)) *50.0F / 65536.0F);
			break;

		case ADDA_MODE_0_24mA:
		case ADDA_MODE_0_20mA:
		case ADDA_MODE_4_20mA:
			// 0-5V entspricht 0-25mA (100 Ohm) und Spannungsverdopplung !
			value = (((float) (data&0xffff)) *25.0F / 65536.0F);
			break;

		default:
			sprintf(msg, "Illegal A/D Mode detected (mode=0x%x)", (unsigned int) mode);
			DapiSetError(DAPI_ERR_ILLEGAL_MODE, 0, msg, __FILE__, __LINE__);					
			break;

	}

	
	return value;
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

void DAPI_FUNCTION_PRE DapiDASetMode(ULONG handle, ULONG ch, ULONG mode)
{
	char msg[200];

	DapiWriteByte(handle, 0x2000 + ch*8 + 2, mode&255);

	if((mode & ADDA_MODE_DA_DISABLE) == ADDA_MODE_DA_DISABLE)
	{
		DapiWriteByte(handle, 0x2000 + ch*8 + 3, 1);	// Disable D/A Channel
		sprintf(msg,"|DELIB|DapiDASetMode(ch=%d, mode=0x%x - disable D/A\n", (unsigned int) ch, (unsigned int) mode);
		debug_print(msg);
	}
	if((mode & ADDA_MODE_DA_ENABLE) == ADDA_MODE_DA_ENABLE)
	{
		DapiWriteByte(handle, 0x2000 + ch*8 + 3, 0);	// Enable D/A Channel
		sprintf(msg,"|DELIB|DapiDASetMode(ch=%d, mode=0x%x - disable D/A\n", (unsigned int) ch, (unsigned int) mode);
		debug_print(msg);
	}

}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiDAGetMode(ULONG handle, ULONG ch)
{
	return DapiReadByte(handle, 0x2000 + ch*8 + 2);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiDASet(ULONG handle, ULONG ch, ULONG data)
{
	DapiWriteWord(handle, 0x2000 + ch*8, data);

}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiDASetVolt(ULONG handle, ULONG ch, float volt)
{
	ULONG mode;
	ULONG value;
	char msg[80];

	mode = 	DapiDAGetMode(handle, ch);

	switch(mode)
	{
		case ADDA_MODE_UNIPOL_10V:
			value = (ULONG) (3276.8 *2.0 * volt);
			if(value > 0xffff) value = 0xffff;
			DapiDASet(handle, ch, value );
			break;

		case ADDA_MODE_UNIPOL_5V:
			value = (ULONG) (3276.8 * 4.0 * volt);
			if(value > 0xffff) value = 0xffff;
			DapiDASet(handle, ch, value );
			break;
		
		case ADDA_MODE_UNIPOL_2V5:
			value = (ULONG) (3276.8 * 8.0 * volt);
			if(value > 0xffff) value = 0xffff;
			DapiDASet(handle, ch, value );
			break;
		
		case ADDA_MODE_BIPOL_10V:
			value = (ULONG) (32768.0 + 3276.8 * volt);
			if(value > 0xffff) value = 0xffff;
			DapiDASet(handle, ch, value );
			break;
		
		case ADDA_MODE_BIPOL_5V:
			value = (ULONG) (32768.0 + 3276.8 * 2.0 * volt);
			if(value > 0xffff) value = 0xffff;
			DapiDASet(handle, ch, value );
			break;

		case ADDA_MODE_BIPOL_2V5:
			value = (ULONG) (32768.0 + 3276.8 * 4.0 * volt);
			if(value > 0xffff) value = 0xffff;
			DapiDASet(handle, ch, value );
			break;

		default:
			sprintf(msg, "Illegal D/A Mode detected (chan=%d, mode=0x%x)", (unsigned int) ch, (unsigned int) mode);
			DapiSetError(DAPI_ERR_ILLEGAL_MODE, 0, msg, __FILE__, __LINE__);					
			break;
	}
//		case ADDA_MODE_0_20mA			0x80
//		case ADDA_MODE_4_20mA			0x81
//		case ADDA_MODE_0_24mA			0x82
	

}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiDASetmA(ULONG handle, ULONG ch, float data)
{
	ULONG mode;
	ULONG value;
	char msg[80];


	mode = 	DapiDAGetMode(handle, ch);

	switch(mode)
	{
		case ADDA_MODE_0_20mA:
			if(data<0) data=0;
			value = (ULONG) (3276.8 * data);
			if(value > 0xffff) value = 0xffff;
			DapiDASet(handle, ch, value );
			break;

		case ADDA_MODE_4_20mA:
			if(data<4) data=4;
			value = (ULONG) (4096 * (data-4.0));
			if(value > 0xffff) value = 0xffff;
			DapiDASet(handle, ch, value );
			break;
		
		case ADDA_MODE_0_24mA:
			if(data<4) data=4;
			value = (ULONG) (65536/24 * data);
			if(value > 0xffff) value = 0xffff;
			DapiDASet(handle, ch, value );
			break;

		default:
			sprintf(msg, "Illegal D/A Mode detected (chan=%dm mode=0x%x)", (unsigned int) ch, (unsigned int) mode);
			DapiSetError(DAPI_ERR_ILLEGAL_MODE, 0, msg, __FILE__, __LINE__);					
			break;
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
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

float DAPI_FUNCTION_PRE DapiTempGet(ULONG handle, ULONG ch)
{
	float f;
	unsigned long d;

	if((ch&0x8000) == 0)
	{
		// Temperatur soll gelesen werden
		d = DapiReadLong(handle, 0x4000 + ch*8);

		switch((d>>16) & 0xff)
		{
			case 1:	f = ((float) (d&0x7fff)) / 10;break;				// Faktor 10
			case 2:	f = ((float) (d&0x7fff)) / 100;break;				// Faktor 100
			default: f=0;
		}

		if(((d>>15)&1) != 0) f=-f;			// Negative Temp
	}
	else
	{
		// Der Widerstandswert soll gelesen werden
		d = DapiReadWord(handle, 0x4000 + ch*8 + 6);				// Widerstandswert lesen

		f = ((float) (d&0x7fff)) / 100;								// Faktor 100
		if(((d>>15)&1) != 0) f=-f;									// Negative
	}

	return f;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void   DAPI_FUNCTION_PRE DapiCnt48ModeSet(ULONG handle, ULONG ch, ULONG mode)
{
	DapiWriteWord(handle, 0x5000 + ch*8 + 6, mode);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG  DAPI_FUNCTION_PRE DapiCnt48ModeGet(ULONG handle, ULONG ch)
{	
	return DapiReadWord(handle, 0x5000 + ch*8 + 6);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG  DAPI_FUNCTION_PRE DapiCnt48CounterGet32(ULONG handle, ULONG ch)
{
	return DapiReadLong(handle, 0x5000 + ch*8);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONGLONG  DAPI_FUNCTION_PRE DapiCnt48CounterGet48(ULONG handle, ULONG ch)
{
	return DapiReadLongLong(handle, 0x5000 + ch*8);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiPulseGenSet(ULONG handle, ULONG ch, ULONG mode, ULONG par0, ULONG par1, ULONG par2)
{
	unsigned long ret;
	char msg[200];

	sprintf(msg,"|DELIB|--------------------DapiPulseGenSet Reg A = %x", (unsigned int) mode);
	debug_print(msg);

	sprintf(msg,"|DELIB|--------------------DapiPulseGenSet Reg B = %x", (unsigned int) par0);
	debug_print(msg);

	sprintf(msg,"|DELIB|--------------------DapiPulseGenSet Reg C = %x", (unsigned int) par1);
	debug_print(msg);

	sprintf(msg,"|DELIB|--------------------DapiPulseGenSet Reg D = %x", (unsigned int) par2);
	debug_print(msg);

	DapiWriteLong(handle, 0x5800 + ch*16 + 0 , mode);
	DapiWriteLong(handle, 0x5800 + ch*16 + 4 , par0);
	DapiWriteLong(handle, 0x5800 + ch*16 + 8 , par1);
	DapiWriteLong(handle, 0x5800 + ch*16 + 12, par2);

	ret = DapiReadLong(handle, 0x5800 + ch*16 + 0);
	sprintf(msg,"|DELIB|--------------------DapiPulseGenSet Reg A (readback) = %x", (unsigned int) ret);
	debug_print(msg);

	ret = DapiReadLong(handle, 0x5800 + ch*16 + 4);
	sprintf(msg,"|DELIB|--------------------DapiPulseGenSet Reg B (readback) = %x", (unsigned int) ret);
	debug_print(msg);

	ret = DapiReadLong(handle, 0x5800 + ch*16 + 8);
	sprintf(msg,"|DELIB|--------------------DapiPulseGenSet Reg C (readback) = %x", (unsigned int) ret);
	debug_print(msg);

	ret = DapiReadLong(handle, 0x5800 + ch*16 + 12);
	sprintf(msg,"|DELIB|--------------------DapiPulseGenSet Reg D (readback) = %x", (unsigned int) ret);
	debug_print(msg);



/*	printf("Mode  readback = %x\n", DapiReadLong(handle, 0x5800 + chan*16 + 0));IsError();
	printf("ModeA readback = %x\n", DapiReadLong(handle, 0x5800 + chan*16 + 4));IsError();
	printf("ModeB readback = %x\n", DapiReadLong(handle, 0x5800 + chan*16 + 8));IsError();
	printf("ModeC readback = %x\n", DapiReadLong(handle, 0x5800 + chan*16 + 12));IsError();
*/
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiSpecialCommand(ULONG handle, ULONG cmd, ULONG par1, ULONG par2, ULONG par3)
{
	ULONG ret=0;
	ULONG i;
	ULONG bank;
	unsigned char *buff;
	ULONG pos;
	char msg[200];
	ULONG result;
	ULONG cnt;

	ULONGLONG d64;

	DAPI_HANDLE *DapiHandle;
	DapiHandle = (DAPI_HANDLE*) handle;

	sprintf(msg,"|DELIB|--------------------DapiSpecialCommand(handle=0x%x, cmd=0x%x, par1=0x%x, par2=0x%x, par3=0x%x\n", (unsigned int) handle, (unsigned int) cmd, (unsigned int) par1, (unsigned int) par2, (unsigned int) par3);
	debug_print(msg);

	// check if handle is correct
	if(DapiCheckHandle(handle) != 0) return DAPI_ERR_ILLEGAL_HANDLE;

	ret = DAPI_ERR_NONE;

	switch(cmd)
	{


		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_DA:
			switch(par1)
			{
				// ----------------------------------------
				case DAPI_SPECIAL_DA_PAR_DA_LOAD_DEFAULT:
					DapiWriteByte(handle, 0x2000 + par2*8 + 7, 0x12);		// Auslieferungszustand laden

					cnt=100;
					do
					{
						--cnt;
					} while((DapiReadByte(handle, 0x2000 + par2*8 + 7) != 0xff) && (cnt!=0));

					break;
				// ----------------------------------------
				case DAPI_SPECIAL_DA_PAR_DA_SAVE_EEPROM_CONFIG:
					DapiWriteByte(handle, 0x2000 + par2*8 + 7, 0x10);		// Ins EEPROM schreiben

					cnt=100;
					do
					{
						--cnt;
					} while((DapiReadByte(handle, 0x2000 + par2*8 + 7) != 0xff) && (cnt!=0));

					break;
				// ----------------------------------------
				case DAPI_SPECIAL_DA_PAR_DA_LOAD_EEPROM_CONFIG:
					DapiWriteByte(handle, 0x2000 + par2*8 + 7, 0x11);		// Aus EEPROM laden

					cnt=100;
					do
					{
						--cnt;
					} while((DapiReadByte(handle, 0x2000 + par2*8 + 7) != 0xff) && (cnt!=0));

					break;
				// ----------------------------------------




				//DapiWriteByte(handle, 0x2000 + i*8 + 7, 0x10);		// Diesen Zustand ins EEPROM
			}
			break;

		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_AD:
			switch(par1)
			{
				// ----------------------------------------
				case DAPI_SPECIAL_AD_READ_MULTIPLE_AD:
					
					if(par2>64) return -1;
					if(par3>64) return -1;
					if(par2>par3) return -1;
					ret=DapiReadMultipleBytes(handle, 0x1000, (par3-par2+1)*4, 1, ((unsigned char*) &global_ad_buffer) + par2*4, (par3-par2)*4);
					break;
				// ----------------------------------------
			}
			break;

		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------


					
					
		// ------------------------------------------------
		case DAPI_SPECIAL_CMD_SET_DIR_DX_8:
			DapiWriteLong(handle, 0x100 + par1/64, par2);
			break;

		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_SET_DIR_DX_1:
			DapiWriteLong(handle, 0x120 + par1/8, par2);
			break;

		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_GET_MODULE_CONFIG:
			switch(par1)
			{
			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DO:
				ret = DapiReadByte(handle, 0xff00);
				break;

			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DI:
				ret = DapiReadByte(handle, 0xff02);
				break;

			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DI_FF:
				ret = DapiReadByte(handle, 0xff0c);
				break;

			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DI_COUNTER:
				ret = DapiReadByte(handle, 0xff0e);
				break;

			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DX:
				ret = DapiReadByte(handle, 0xff04);
				break;

			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_AD:
				ret = DapiReadByte(handle, 0xff08);
				break;

			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DA:
				ret = DapiReadByte(handle, 0xff06);
				break;

			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_STEPPER:
				ret = DapiReadByte(handle, 0xff0a);
				break;

			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_TEMP:
				ret = DapiReadByte(handle, 0xff10);
				break;

			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_CNT48:
				ret = DapiReadByte(handle, 0xff12);
				break;

			case DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_PULSE_GEN:
				ret = DapiReadByte(handle, 0xff14);
				break;

			}
			break;
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_GET_MODULE_VERSION:
			switch(par1)
			{
			case DAPI_SPECIAL_GET_MODULE_PAR_VERSION_0:
				ret = DapiReadByte(handle, 0xfff4) - '0';
				break;

			case DAPI_SPECIAL_GET_MODULE_PAR_VERSION_1:
				ret = DapiReadByte(handle, 0xfff5) - '0';
				break;

			case DAPI_SPECIAL_GET_MODULE_PAR_VERSION_2:
				ret = DapiReadByte(handle, 0xfff6) - '0';
				break;

			case DAPI_SPECIAL_GET_MODULE_PAR_VERSION_3:
				ret = DapiReadByte(handle, 0xfff7) - '0';
				break;
			}
			break;
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_TIMEOUT:
			switch(par1)
			{
			case DAPI_SPECIAL_TIMEOUT_SET_VALUE_SEC:
				DapiWriteWord(handle, 0xfd02, par2*10+ (par3%10));
				break;

			case DAPI_SPECIAL_TIMEOUT_ACTIVATE:
				DapiWriteByte(handle, 0xfd00,1);
				break;

			case DAPI_SPECIAL_TIMEOUT_DEACTIVATE:
				DapiWriteByte(handle, 0xfd00,0);
				break;

			case DAPI_SPECIAL_TIMEOUT_GET_STATUS:
				ret = DapiReadByte(handle, 0xfd01);
				break;
			}
			break;
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_DI:
			switch(par1)
			{
			case DAPI_SPECIAL_DI_FF_FILTER_VALUE_SET:
				DapiWriteByte(handle, 0xfd10, par2);
				break;
			case DAPI_SPECIAL_DI_FF_FILTER_VALUE_GET:
				ret = DapiReadByte(handle, 0xfd10);
				break;
			}
			break;
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_WATCHDOG:
			switch(par1)
			{
			case DAPI_SPECIAL_WATCHDOG_SET_TIMEOUT_MSEC:
				DapiWriteLong(handle, 0xe004, par2);
				break;

			case DAPI_SPECIAL_WATCHDOG_GET_TIMEOUT_MSEC:
				ret = DapiReadLong(handle, 0xe004);
				break;

			case DAPI_SPECIAL_WATCHDOG_GET_STATUS:
				ret=DapiReadByte(handle, 0xe000);
				break;

			case DAPI_SPECIAL_WATCHDOG_GET_WD_COUNTER_MSEC:
				ret = DapiReadLong(handle, 0xe008);
				break;

			case DAPI_SPECIAL_WATCHDOG_GET_TIMEOUT_RELAIS_COUNTER_MSEC:
				ret = DapiReadLong(handle, 0xe00c);
				break;

			case DAPI_SPECIAL_WATCHDOG_SET_TIMEOUT_REL1_COUNTER_MSEC:
				DapiWriteLong(handle, 0xe008, par2);
				break;

			case DAPI_SPECIAL_WATCHDOG_SET_TIMEOUT_REL2_COUNTER_MSEC:
				DapiWriteLong(handle, 0xe00c, par2);
				break;

			
			}
			break;
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_COUNTER:
			switch(par1)
			{
			case DAPI_SPECIAL_COUNTER_LATCH_ALL:
				DapiWriteByte(handle, 0xfe12, 0x19);					// Latch all Counter
				break;
			case DAPI_SPECIAL_COUNTER_LATCH_ALL_WITH_RESET:
				DapiWriteByte(handle, 0xfe12, 0x1a);					// Latch all Counter WITH RESET
				break;
			}
			break;

		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_CNT48:
			switch(par1)
			{
			case DAPI_SPECIAL_CNT48_RESET_SINGLE:
				DapiWriteByte(handle, 0x5000 + par2*8, 0x00);			// Reset Counter Nr. "par2"
				break;
			case DAPI_SPECIAL_CNT48_RESET_GROUP8:
				DapiWriteByte(handle, 0x5003 + (par2/8)*0x40, 0x00);	// Reset Counter Group Nr. "par2"
				break;
			case DAPI_SPECIAL_CNT48_LATCH_GROUP8:
				DapiWriteByte(handle, 0x5002 + (par2/8)*0x40, 0x00);	// Latch Counter Group Nr. "par2"
				break;
			}
			break;

		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		#ifndef __linux__

		case DAPI_SPECIAL_CMD_MODULE_CONFIG:
			switch(par1)
			{
			case DAPI_SPECIAL_MODULECFG_MODULE_ID_SET:
				DapiFTDISetModuleAdress(DapiHandle->ftHandle, DapiHandle->moduleID, par2);
				break;
			}
			break;

			
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------

		case DAPI_SPECIAL_CMD_ROCPU_EEPROM_ERASE:
			for(bank=0;bank<=1;++bank)
			{
				if((par1&(bank+1))!=0)	
				{
					DapiWriteByte(handle, 0xfdf0, bank);			// EEPROM Erase Bank 1 oder 2

					sprintf(msg,"|DELIB|DAPI_SPECIAL_CMD_ROCPU_EEPROM_ERASE: Erase State 1\n");
					debug_print(msg);

					DapiWriteByte(handle, 0xfdf1, 0xfe);		// EEPROM Erase (1k)
				
					sprintf(msg,"|DELIB|DAPI_SPECIAL_CMD_ROCPU_EEPROM_ERASE: Erase State 2\n");
					debug_print(msg);

					DapiWriteByte(handle, 0xfdf1, 0x04);		// Verify Erase
					result = DapiReadByte(handle, 0xfdf2);		// Get EEPROM Status

					sprintf(msg,"|DELIB|DAPI_SPECIAL_CMD_ROCPU_EEPROM_ERASE: Erase State 3 = result=%d bank=%d\n", result, bank);
					debug_print(msg);

					if(result==0)
					{
						ret = 0;
					}
					else
					{
						ret = -1;
						return ret;
					}
				}

			}

/*			if((par1&2)!=0)	
			{
				DapiWriteByte(handle, 0xfdf0, 1);			// EEPROM Erase Bank 2
				DapiWriteByte(handle, 0xfdf1, 0xfe);		// EEPROM Erase (1k)
				
				sprintf(msg,"|DELIB|DAPI_SPECIAL_CMD_ROCPU_EEPROM_ERASE: Erase State 3\n");
				debug_print(msg);

				DapiWriteByte(handle, 0xfdf1, 0x04);		// Verify Erase
				result = DapiReadByte(handle, 0xfdf2);		// Get EEPROM Status

				sprintf(msg,"|DELIB|DAPI_SPECIAL_CMD_ROCPU_EEPROM_ERASE: Erase State 4 = result=%d\n", result);
				debug_print(msg);

				if(result==0)
				{
					ret = 0;
				}
				else
				{
					ret = -1;
					return ret;
				}
			}
*/
			break;
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_ROCPU_EEPROM_WRITE:
			// write 1k/2k to EEPROM
			// par1 = 1=first 1k, 2=second1k, 3 = 2K EEPROM
			// par3 = Buffer

			buff = (unsigned char*) par3;
			pos=0;	// Start at beginning of the buffer

			for(bank=0;bank<=1;++bank)
			{
				if((par1 & (bank+1))!=0) // &1  oder &2
				{
					//write first 1k
					ULONG bank_start = bank*4;
					ULONG bank_end = (bank+1)*4;


/*					if (DapiHandle->moduleID == USB_WATCHDOG)
					{
						// Wir haben nur 256 Bytes pro EEPROM Bank
						bank_start = bank;
						bank_end = (bank+1);
					}
*/
					for (ULONG j=bank_start;j!=(bank_end);++j)	// 0-3 oder 4-7
					{
						DapiWriteMultipleBytes(handle, 0xf000 , 64, 1, buff+pos , 64);pos+=64;
						DapiWriteMultipleBytes(handle, 0xf040 , 64, 1, buff+pos , 64);pos+=64;
						DapiWriteMultipleBytes(handle, 0xf080 , 64, 1, buff+pos , 64);pos+=64;
						DapiWriteMultipleBytes(handle, 0xf0c0 , 64, 1, buff+pos , 64);pos+=64;
					
						sprintf(msg,"|DELIB|DAPI_SPECIAL_CMD_ROCPU_EEPROM_WRITE: Write State 1\n");
						debug_print(msg);

						// write 256 Byte to EEPROM
						DapiWriteWord(handle, 0xfdf0, (j)&7 | 0x02<<8);		// EEPROM Write

						sprintf(msg,"|DELIB|DAPI_SPECIAL_CMD_ROCPU_EEPROM_WRITE: Write State 2\n");
						debug_print(msg);

						// Verify Data
						DapiWriteByte(handle, 0xfdf1, 0x03);		// Verify Write
						result = DapiReadByte(handle, 0xfdf2);		// Get Status
					
						sprintf(msg,"|DELIB|DAPI_SPECIAL_CMD_ROCPU_EEPROM_WRITE: Write State 3-result=%d bank=%d j=%d\n",result,bank,j);
						debug_print(msg);

						if(result==0)
						{
							ret = 0;
						}
						else
						{
							ret = -1;
							return ret;
						}
					}
				}
			}
/*
			if((par1 & 2)!=0)
			{
				//write second 1k
				for (ULONG j=4;j!=8;++j)
				{
					DapiWriteMultipleBytes(handle, 0xf000 , 64, 1, buff+pos , 64);pos+=64;
					DapiWriteMultipleBytes(handle, 0xf040 , 64, 1, buff+pos , 64);pos+=64;
					DapiWriteMultipleBytes(handle, 0xf080 , 64, 1, buff+pos , 64);pos+=64;
					DapiWriteMultipleBytes(handle, 0xf0c0 , 64, 1, buff+pos , 64);pos+=64;

					// write 256 Byte to EEPROM
					DapiWriteWord(handle, 0xfdf0, (j)&7 | 0x02<<8);		// EEPROM Write

					// Verify Data
					DapiWriteByte(handle, 0xfdf1, 0x03);		// Verify Erase
					result = DapiReadByte(handle, 0xfdf2);		// Get Status
					
					if(result==0)
					{
						ret = 0;
					}
					else
					{
						ret = -1;
						return ret;
					}
				}
			}
*/
			break;
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_ROCPU_EEPROM_READ:
			// read 1k/2k from EEPROM
			// par1 = 1=first 1k, 2=second1k, 3 = 2K EEPROM
			// par3 = Buffer

			buff = (unsigned char*) par3;
			pos=0;


			for(bank=0;bank<=1;++bank)
			{
				if((par1 & (bank+1))!=0)
				{
					//read first 1k
					for (ULONG j=(bank*4);j!=((bank+1)*4);++j)
					{
						// read 256 Byte from EEPROM
						DapiWriteWord(handle, 0xfdf0, (j)&7 | 0x01<<8);		// EEPROM Read
						if(DapiGetLastError() != 0) return 0;

						for(i=0;i!=256;i+=8)
						{
							/*ret=DapiReadByte(handle, 0xf000+i);
							if(DapiGetLastError() != 0) return 0;
							*(buff+pos) = (unsigned char) ret;
							++pos;*/

							d64=DapiReadLongLong(handle, 0xf000+i);
							if(DapiGetLastError() != 0) return 0;
							*(buff+pos+0) = (unsigned char)   d64 & 0x00000000000000ff;
							*(buff+pos+1) = (unsigned char) ((d64 & 0x000000000000ff00) >> 8);
							*(buff+pos+2) = (unsigned char) ((d64 & 0x0000000000ff0000) >> 16);
							*(buff+pos+3) = (unsigned char) ((d64 & 0x00000000ff000000) >> 24);
							*(buff+pos+4) = (unsigned char) ((d64 & 0x000000ff00000000) >> 32);
							*(buff+pos+5) = (unsigned char) ((d64 & 0x0000ff0000000000) >> 40);
							*(buff+pos+6) = (unsigned char) ((d64 & 0x00ff000000000000) >> 48);
							*(buff+pos+7) = (unsigned char) ((d64 & 0xff00000000000000) >> 56);
							pos+=8;
						}
					}
				}
			}

			break;
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_RS485_MODULE_NR_SET:

			DapiHandle->RS485_ModuleNr = par1;

			sprintf(msg,"|DELIB|DapiSpecialCommand(DAPI_SPECIAL_CMD_RS485_MODULE_NR_SET = %d\n", par1);
			debug_print(msg);


			break;	
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		case DAPI_SPECIAL_CMD_TCP_TX_RX_TESTDATA:
			DAPI_HANDLE *DapiHandle;
			DapiHandle = (DAPI_HANDLE*) handle;
			tcp_tx_send_rec_data_big(DapiHandle->TCPHandle, 0,0,0,0);
			break;
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------



		#endif 
		// Nicht für Linux

	
	}

return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#ifndef __linux__

ULONG DAPI_FUNCTION_PRE DapiScanModule(ULONG moduleID, ULONG cmd)
{
	ULONG ret=0;
	ULONG BusType;
	ULONG i;
	ULONG handle;

	if(cmd==DAPI_SCANMODULE_GET_MODULES_AVAILABLE)
	{

		BusType = DapiInternGetBusType(moduleID);
		if((BusType == bus_FTDI_DUAL) || (BusType==bus_SeriellFTDI)	|| (BusType == bus_FTDI_SINGLE))
		{
			ret = DapiScanFTDIModulesGetModulesAvailable(moduleID);
		}
		else
		{
			for(i=0;i!=8;++i)
			{
				handle = DapiOpenModule(moduleID, i);
				if(handle != 0)
				{
					ret |= (1<<i);
					DapiCloseModule(handle);
				}
			}
		}
	}
	return ret;
}

#endif
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

ULONG DAPI_FUNCTION_PRE DapiStepperCommand(ULONG handle, ULONG motor, ULONG cmd, ULONG par1, ULONG par2, ULONG par3, ULONG par4)
{
	ULONG ret;
	ULONG adr_base = 0x3000 | motor*0x40;
	ret=0;
	char msg[200];

	unsigned char buff[20];
	ULONG pos;

	pos = 0;

	buff[pos++] = (UCHAR) ((par3)     & 0xff);
	buff[pos++] = (UCHAR) ((par3>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((par3>>16) & 0xff);
	buff[pos++] = (UCHAR) ((par3>>24) & 0xff);

	buff[pos++] = (UCHAR) ((par2)     & 0xff);
	buff[pos++] = (UCHAR) ((par2>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((par2>>16) & 0xff);
	buff[pos++] = (UCHAR) ((par2>>24) & 0xff);

	buff[pos++] = (UCHAR) ((par1)     & 0xff);
	buff[pos++] = (UCHAR) ((par1>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((par1>>16) & 0xff);
	buff[pos++] = (UCHAR) ((par1>>24) & 0xff);

	buff[pos++] = (UCHAR) ((cmd)     & 0xff);
	buff[pos++] = (UCHAR) ((cmd>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((cmd>>16) & 0xff);
	buff[pos++] = (UCHAR) ((cmd>>24) & 0xff);

	DapiWriteMultipleBytes(handle, adr_base + 0x10 , pos, 1, buff , pos);


	sprintf(msg,"|DELIB|DapiStepperCommand(motor=0x%x, cmd=0x%x, par1=0x%x, par2=0x%x, par3=0x%x\n", (unsigned int) motor, (unsigned int) cmd, (unsigned int) par1, (unsigned int) par2, (unsigned int) par3);
	debug_print(msg);

	switch (cmd & 0xc0000000)
	{
		case DAPI_STEPPER_RETURN_0_BYTES:
			ret=0;
			break;

		case DAPI_STEPPER_RETURN_1_BYTES:
			ret=DapiReadByte(handle, adr_base | 0x20);;
			break;
		
		case DAPI_STEPPER_RETURN_2_BYTES:
			ret=DapiReadWord(handle, adr_base | 0x20);;
			break;

		case DAPI_STEPPER_RETURN_4_BYTES:
			ret=DapiReadLong(handle, adr_base | 0x20);;
			break;
	}

	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiStepperCommandEx(ULONG handle, ULONG motor, ULONG cmd, ULONG par1, ULONG par2, ULONG par3, ULONG par4, ULONG par5, ULONG par6, ULONG par7)
{
	ULONG ret;
	ULONG adr_base = 0x3000 | motor*0x40;
	ret=0;



	unsigned char buff[40];
	ULONG pos;

	pos = 0;

	buff[pos++] = (UCHAR) ((par7)     & 0xff);
	buff[pos++] = (UCHAR) ((par7>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((par7>>16) & 0xff);
	buff[pos++] = (UCHAR) ((par7>>24) & 0xff);

	buff[pos++] = (UCHAR) ((par6)     & 0xff);
	buff[pos++] = (UCHAR) ((par6>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((par6>>16) & 0xff);
	buff[pos++] = (UCHAR) ((par6>>24) & 0xff);

	buff[pos++] = (UCHAR) ((par5)     & 0xff);
	buff[pos++] = (UCHAR) ((par5>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((par5>>16) & 0xff);
	buff[pos++] = (UCHAR) ((par5>>24) & 0xff);

	buff[pos++] = (UCHAR) ((par4)     & 0xff);
	buff[pos++] = (UCHAR) ((par4>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((par4>>16) & 0xff);
	buff[pos++] = (UCHAR) ((par4>>24) & 0xff);

	buff[pos++] = (UCHAR) ((par3)     & 0xff);
	buff[pos++] = (UCHAR) ((par3>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((par3>>16) & 0xff);
	buff[pos++] = (UCHAR) ((par3>>24) & 0xff);

	buff[pos++] = (UCHAR) ((par2)     & 0xff);
	buff[pos++] = (UCHAR) ((par2>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((par2>>16) & 0xff);
	buff[pos++] = (UCHAR) ((par2>>24) & 0xff);

	buff[pos++] = (UCHAR) ((par1)     & 0xff);
	buff[pos++] = (UCHAR) ((par1>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((par1>>16) & 0xff);
	buff[pos++] = (UCHAR) ((par1>>24) & 0xff);

	buff[pos++] = (UCHAR) ((cmd)     & 0xff);
	buff[pos++] = (UCHAR) ((cmd>>8)  & 0xff);
	buff[pos++] = (UCHAR) ((cmd>>16) & 0xff);
	buff[pos++] = (UCHAR) ((cmd>>24) & 0xff);

	DapiWriteMultipleBytes(handle, adr_base + 0x00 , pos, 1, buff , pos);


	switch (cmd & 0xc0000000)
	{
		case DAPI_STEPPER_RETURN_0_BYTES:
			ret=0;
			break;

		case DAPI_STEPPER_RETURN_1_BYTES:
			ret=DapiReadByte(handle, adr_base | 0x20);;
			break;
		
		case DAPI_STEPPER_RETURN_2_BYTES:
			ret=DapiReadWord(handle, adr_base | 0x20);;
			break;

		case DAPI_STEPPER_RETURN_4_BYTES:
			ret=DapiReadLong(handle, adr_base | 0x20);;
			break;
	}

	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiStepperGetStatus(ULONG handle, ULONG motor, ULONG cmd)
{
	ULONG ret;
	ULONG adr_base = 0x3000 | motor*0x40;
	ret=0;

	switch(cmd)
	{
		case DAPI_STEPPER_STATUS_GET_POSITION:
			// Get Motorposition
			ret = DapiReadLong(handle, adr_base | 0x28);
			break;

		case DAPI_STEPPER_STATUS_GET_SWITCH:
			// Get Endswitches
			ret = DapiReadByte(handle, adr_base | 0x2f);
			break;

		case DAPI_STEPPER_STATUS_GET_ACTIVITY:
			// Get Endswitches
			ret = DapiReadByte(handle, adr_base | 0x2e);
			break;
	}

	return ret;
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

void DAPI_FUNCTION_PRE DapiWatchdogEnable(ULONG handle)
{
	DapiWriteByte(handle, 0xe000, 0x23);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiWatchdogDisable(ULONG handle)
{
	DapiWriteByte(handle, 0xe000, 0x12);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiWatchdogRetrigger(ULONG handle)
{
	DapiWriteByte(handle, 0xe001, 0x34);
}
