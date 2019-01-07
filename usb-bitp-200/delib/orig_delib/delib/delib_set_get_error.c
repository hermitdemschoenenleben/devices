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

#define debug_commands 1



#ifdef __linux__


#ifdef USE_FTDI
#include "../usb/ftd2xx.h"
#endif

#include "delib.h"
#include <string.h>
#include <stdio.h>

debug_print(char *t)
{
#ifdef debug_commands
	printf("%s", t);
#endif
}
#else
#include <windows.h>
#include <stdio.h>
#include "conio.h"

#include "\library\vc\ftdi\ftd2xx.h"

#include "..\includes\delib.h"
#include "delib_debug.h"

#endif




// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

typedef struct xxDAPI_ERROR
{
	ULONG errorcode;
	ULONG lasterror;
	char errortext[1000];
	char filename[255];
	ULONG suberror;
	ULONG line;
} DAPI_ERROR;



DAPI_ERROR lasterror;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiSetError(ULONG errorcode, ULONG suberror, char *errortext, char * filename, ULONG line)
{
	char msg[1000];

	char text[500];
//	if(lasterror==0)
	{
		lasterror.errorcode = errorcode;
		lasterror.lasterror = errorcode;
		//memcpy(lasterror.errortext, errortext, 255);

	//	sprintf(msg,"|DELIB|DapiSetErrorA  (%d, %d) errtext=%s, filename=%s, line = %d\n", errorcode, suberror, errortext, filename, line);
	//	debug_print(msg);
		if(errortext==NULL)
		{
	//	sprintf(msg,"|DELIB|DapiSetErrorB (%d, %d) errtext=%s, filename=%s, line = %d\n", errorcode, suberror, errortext, filename, line);
	//	debug_print(msg);
			switch(errorcode)
			{
				case DAPI_ERR_DEVICE_NOT_FOUND:
					sprintf(text, "(auto text) Device not found");
					break;

				case DAPI_ERR_COMMUNICATION_ERROR:	
					sprintf(text, "(auto text) Communication Error");
					break;

				case DAPI_ERR_ILLEGAL_HANDLE:
					sprintf(text, "(auto text) Illegal Handle");
					break;

				case DAPI_ERR_FUNCTION_NOT_DEFINED:
					sprintf(text, "(auto text) Function not defined");
					break;

				case DAPI_ERR_ILLEGAL_COM_HANDLE:
					sprintf(text, "(auto text) Illegal COM-Handle");
					break;

				default:
					sprintf(text, "(auto text) unknown errorcode");
					break;

			}
		}
		else
		{
			sprintf(lasterror.errortext, "DELIB Error  = %s", errortext);
		}
		sprintf(lasterror.errortext, "%s * errorcode = %d.%d * filename = %s * line = %d", text, (unsigned int) errorcode, (unsigned int) suberror, filename, (unsigned int) line);

		
		
		sprintf(msg,"|DELIB|DapiSetError(%d, %d) errtext=%s, filename=%s, line = %d\n", (unsigned int) errorcode, (unsigned int) suberror, errortext, filename, (unsigned int) line);
		debug_print(msg);
		sprintf(msg,"|DELIB|DapiSetError lasterror.errortext=%s\n", lasterror.errortext);
		debug_print(msg);
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DAPI_FUNCTION_PRE DapiClearLastError()
{
	lasterror.lasterror=0;
	lasterror.errorcode=0;
	return;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiGetLastErrorText(unsigned char * msg_ret, unsigned long msg_length)
{
	ULONG i;
	char msg[1000];

	sprintf(msg,"|DELIB|DapiGetLastErrorText (msg_length=%d)\n", (unsigned int) msg_length);
	debug_print(msg);


	sprintf(msg,"|DELIB|DapiGetLastErrorText lasterror.errortext=%s * strlen=%d\n",  (char*) lasterror.errortext, strlen(lasterror.errortext));
	debug_print(msg);

	
	for(i=0;i!=strlen(lasterror.errortext);++i)
	{
		if(i<msg_length)
		{
			*(msg_ret + i) = *(lasterror.errortext + i);
		}
	}

	if(i<msg_length) *(msg_ret + i) = 0;

	return 0;
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ULONG DAPI_FUNCTION_PRE DapiGetLastError()
{
	ULONG ret;

	ret=lasterror.lasterror;
//	lasterror.lasterror=0;
	return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
