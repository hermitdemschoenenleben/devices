//dt_ftapi_dll.h




#define BOOL int
#define TRUE 1
#define FALSE 0

#define true TRUE
#define false FALSE

//#define ULONG unsigned long




#define MAX_NO_OF_SAME_MODULES 8



//#define debug_commands 1
//#define debug_commands_printf 1

extern void debug_print2(char *msg1, char *msg2);


//ftapi_hdlmgr.h

extern ULONG DapiCheckHandle(ULONG handle);

//extern  "C" FT_STATUS __cdecl FT_Close(FT_HANDLE ftHandle);




#define MAX_NO_OF_SAME_MODULES 8


#define delib_id 0x12345678

#define DapiModuleID_Device 1
#define DapiModuleID_DeviceProg 0

//#define	debug_print(msg)	{printf(msg);OutputDebugString(msg);}
#define	debug_print(msg)	OutputDebugString(msg)

typedef struct xxDAPI_HANDLE
{
	ULONG id;
	FT_HANDLE ftHandle;
	ULONG TCPHandle;
//	FT_HANDLE ftHandleProg;
	ULONG moduleID;
	ULONG moduleNr;
	ULONG SubDevice;
	ULONG jobID;
	ULONG BusType;
	ULONG ComPort;
	ULONG RS485_ModuleNr;
} DAPI_HANDLE;


#define cmd_ping		0x12
#define cmd_send8byte	0x23
#define cmd_get8byte	0x34
#define cmd_sendnbyte	0x45
#define cmd_getnbyte	0x56



#define bus_FTDI_DUAL	1
#define bus_SeriellCOM	2
#define bus_SeriellFTDI	3
#define bus_ETH			4
#define bus_FTDI_SINGLE	5


extern ULONG DapiInternGetBusType(ULONG moduleID);
extern void DapiInternCloseAllSubmodules(ULONG moduleID);
