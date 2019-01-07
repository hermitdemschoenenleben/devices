// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
//
//
// delib.h
//
//
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
//
//
#define DELIB_VERSION											0x0140		// Actual DELIB-Version


// all Modul-ID's
#define USB_Interface8											1			// USB-Controller8/USB-TTL-IN8-OUT8
#define USB_CAN_STICK											2			// USB-CAN-Stick
#define USB_LOGI_500											3			// USB-LOGI-500/USB-LOGI-250
#define RO_USB2													4			// RO-CPU2 / 480 MBit/sec
#define RO_SER													5			// RO-SER-Serie
#define USB_BITP_200											6			// USB-BITP-200
#define RO_USB1													7			// RO-USB-Serie
#define RO_USB													7			// RO-USB-Serie
#define RO_ETH													8			// RO-ETH-Serie
#define USB_MINI_STICK											9			// USB-MINI-Stick-Serie
#define USB_LOGI_18												10			// USB-LOGI-100
#define RO_CAN													11			// RO-CAN-Serie
#define USB_SPI_MON												12			// USB_SPI_MON
#define USB_WATCHDOG											13			// USB_Watchdog
#define USB_OPTOIN_8											14			// USB-OPTOIN8 / USB-RELAIS-8
#define USB_RELAIS_8											14			// USB-OPTOIN8 / USB-RELAIS-8
#define USB_OPTOIN_8_RELAIS_8									15			// USB-OPTOIN-8-RELAIS-8
#define USB_OPTOIN_16_RELAIS_16									16			// USB-OPTOIN-16-RELAIS-16
#define USB_OPTOIN_32											16			// USB-OPTOIN-16-RELAIS-16
#define USB_RELAIS_32											16			// USB-OPTOIN-16-RELAIS-16
#define USB_OPTOIN_32_RELAIS_32									17			// USB-OPTOIN-32-RELAIS-32
#define USB_OPTOIN_64											17			// USB-OPTOIN-32-RELAIS-32
#define USB_RELAIS_64											17			// USB-OPTOIN-32-RELAIS-32
#define USB_TTL_32												18			// USB-TTL-32
#define USB_TTL_64												18			// USB-TTL-64

#define MAX_NR_OF_MODULES 18


// ----------------------------------------------------------------------------
// Declarations


#ifndef _MSC_VER
#define UCHAR unsigned char
#define ULONG unsigned long
#define ULONGLONG unsigned long long
#endif



#ifdef __linux__
#define DAPI_FUNCTION_PRE	


#else
#define DAPI_FUNCTION_PRE	__stdcall

#endif

// ----------------------------------------------------------------------------
// ERROR Codes
#define DAPI_ERR_NONE											0
#define DAPI_ERR_DEVICE_NOT_FOUND								-1
#define DAPI_ERR_COMMUNICATION_ERROR							-2
#define DAPI_ERR_ILLEGAL_HANDLE									-10
#define DAPI_ERR_FUNCTION_NOT_DEFINED							-11
#define DAPI_ERR_ILLEGAL_COM_HANDLE								-12
#define DAPI_ERR_ILLEGAL_MODE									-13

// ----------------------------------------------------------------------------
// Special Function-Codes
#define DAPI_SPECIAL_CMD_GET_MODULE_CONFIG						1
#define DAPI_SPECIAL_CMD_TIMEOUT								2
#define DAPI_SPECIAL_CMD_DI										10
#define DAPI_SPECIAL_CMD_SET_DIR_DX_1							3
#define DAPI_SPECIAL_CMD_SET_DIR_DX_8							4
#define DAPI_SPECIAL_CMD_GET_MODULE_VERSION						5
#define DAPI_SPECIAL_CMD_DA										6
#define DAPI_SPECIAL_CMD_WATCHDOG								7
#define DAPI_SPECIAL_CMD_COUNTER								8
#define DAPI_SPECIAL_CMD_AD										9
#define DAPI_SPECIAL_CMD_CNT48									11

// values for PAR1
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DI					1
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DI_FF				7
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DI_COUNTER			8
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DO					2
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DX					3
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_AD					4
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_DA					5
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_TEMP					9
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_STEPPER				6
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_CNT48				10
#define DAPI_SPECIAL_GET_MODULE_CONFIG_PAR_PULSE_GEN			11
//
#define DAPI_SPECIAL_GET_MODULE_PAR_VERSION_0					0
#define DAPI_SPECIAL_GET_MODULE_PAR_VERSION_1					1
#define DAPI_SPECIAL_GET_MODULE_PAR_VERSION_2					2
#define DAPI_SPECIAL_GET_MODULE_PAR_VERSION_3					3
//
#define DAPI_SPECIAL_TIMEOUT_SET_VALUE_SEC						1
#define DAPI_SPECIAL_TIMEOUT_ACTIVATE							2
#define DAPI_SPECIAL_TIMEOUT_DEACTIVATE							3
#define DAPI_SPECIAL_TIMEOUT_GET_STATUS							4

#define DAPI_SPECIAL_DI_FF_FILTER_VALUE_SET						1
#define DAPI_SPECIAL_DI_FF_FILTER_VALUE_GET						2

#define DAPI_SPECIAL_AD_READ_MULTIPLE_AD						1

#define DAPI_SPECIAL_DA_PAR_DA_LOAD_DEFAULT						1
#define DAPI_SPECIAL_DA_PAR_DA_SAVE_EEPROM_CONFIG				2
#define DAPI_SPECIAL_DA_PAR_DA_LOAD_EEPROM_CONFIG				3

#define DAPI_SPECIAL_WATCHDOG_SET_TIMEOUT_MSEC					1
#define DAPI_SPECIAL_WATCHDOG_GET_TIMEOUT_MSEC					2
#define DAPI_SPECIAL_WATCHDOG_GET_STATUS						3
#define DAPI_SPECIAL_WATCHDOG_GET_WD_COUNTER_MSEC				4
#define DAPI_SPECIAL_WATCHDOG_GET_TIMEOUT_RELAIS_COUNTER_MSEC	5
#define DAPI_SPECIAL_WATCHDOG_SET_TIMEOUT_REL1_COUNTER_MSEC		6
#define DAPI_SPECIAL_WATCHDOG_SET_TIMEOUT_REL2_COUNTER_MSEC		7

#define DAPI_SPECIAL_COUNTER_LATCH_ALL							1
#define DAPI_SPECIAL_COUNTER_LATCH_ALL_WITH_RESET				2

#define DAPI_SPECIAL_CNT48_RESET_SINGLE							1
#define DAPI_SPECIAL_CNT48_RESET_GROUP8							2
#define DAPI_SPECIAL_CNT48_LATCH_GROUP8							3


// ----------------------------------------------------------------------------
// DapiScanModules-Codes
#define DAPI_SCANMODULE_GET_MODULES_AVAILABLE					1


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#ifdef __BORLANDC__
	#pragma message	("DELIB.H * BORLAND C Compiler used")
#endif

#ifdef _MSC_VER
	#pragma message	("DELIB.H * Microsoft C Compiler used")
#endif

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Prototypes for DELIB-Functions
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#ifdef __cplusplus
	extern "C"
	{
	//#pragma message	("DELIB.H Mode for C++ used")

#else
	/*#pragma message	("DELIB.H - Mode for C used")*/
#endif

// ----------------------------------------------------------------------------
// 
extern ULONG DAPI_FUNCTION_PRE DapiOpenModule(ULONG moduleID, ULONG nr);
extern ULONG DAPI_FUNCTION_PRE DapiOpenModuleEx(ULONG moduleID, ULONG nr, unsigned char* exbuffer);
extern ULONG DAPI_FUNCTION_PRE DapiCloseModule(ULONG handle);

extern ULONG DAPI_FUNCTION_PRE DapiScanModule(ULONG moduleID, ULONG cmd);

extern ULONG DAPI_FUNCTION_PRE DapiGetDELIBVersion(ULONG mode, ULONG par);

extern ULONG DAPI_FUNCTION_PRE DapiPing(ULONG handle, ULONG value);

// ----------------------------------------------------------------------------
// Register Access
extern void DAPI_FUNCTION_PRE DapiWriteByte(ULONG handle, ULONG adress, ULONG value);
extern void DAPI_FUNCTION_PRE DapiWriteWord(ULONG handle, ULONG adress, ULONG value);
extern void DAPI_FUNCTION_PRE DapiWriteLong(ULONG handle, ULONG adress, ULONG value);
extern void DAPI_FUNCTION_PRE DapiWriteLongLong(ULONG handle, ULONG adress, ULONGLONG value);

extern ULONG DAPI_FUNCTION_PRE DapiReadByte(ULONG handle, ULONG adress);
extern ULONG DAPI_FUNCTION_PRE DapiReadWord(ULONG handle, ULONG adress);
extern ULONG DAPI_FUNCTION_PRE DapiReadLong(ULONG handle, ULONG adress);
extern ULONGLONG DAPI_FUNCTION_PRE DapiReadLongLong(ULONG handle, ULONG adress);

extern void DAPI_FUNCTION_PRE DapiWriteSetByte(ULONG handle, ULONG adress, ULONG value);
extern void DAPI_FUNCTION_PRE DapiWriteClearByte(ULONG handle, ULONG adress, ULONG value);

// ----------------------------------------------------------------------------
// Error Handling
extern ULONG DAPI_FUNCTION_PRE DapiGetLastError();
extern ULONG DAPI_FUNCTION_PRE DapiGetLastErrorText(UCHAR * msg, ULONG msg_length);
extern void  DAPI_FUNCTION_PRE DapiClearLastError(void);

// ----------------------------------------------------------------------------
// Digital Inputs
extern ULONG DAPI_FUNCTION_PRE DapiDIGet1(ULONG handle, ULONG ch);
extern ULONG DAPI_FUNCTION_PRE DapiDIGet8(ULONG handle, ULONG ch);
extern ULONG DAPI_FUNCTION_PRE DapiDIGet16(ULONG handle, ULONG ch);
extern ULONG DAPI_FUNCTION_PRE DapiDIGet32(ULONG handle, ULONG ch);
extern ULONGLONG DAPI_FUNCTION_PRE DapiDIGet64(ULONG handle, ULONG ch);

extern ULONG DAPI_FUNCTION_PRE DapiDIGetFF32(ULONG handle, ULONG ch);

extern ULONG DAPI_FUNCTION_PRE DapiDIGetCounter(ULONG handle, ULONG ch, ULONG mode);

// ----------------------------------------------------------------------------
// Digital Outputs
extern void DAPI_FUNCTION_PRE DapiDOSet1(ULONG handle, ULONG ch, ULONG data);
extern void DAPI_FUNCTION_PRE DapiDOSet8(ULONG handle, ULONG ch, ULONG data);
extern void DAPI_FUNCTION_PRE DapiDOSet16(ULONG handle, ULONG ch, ULONG data);
extern void DAPI_FUNCTION_PRE DapiDOSet32(ULONG handle, ULONG ch, ULONG data);
extern void DAPI_FUNCTION_PRE DapiDOSet64(ULONG handle, ULONG ch, ULONGLONG data);

extern ULONG     DAPI_FUNCTION_PRE DapiDOReadback32(ULONG handle, ULONG ch);
extern ULONGLONG DAPI_FUNCTION_PRE DapiDOReadback64(ULONG handle, ULONG ch);

extern void DAPI_FUNCTION_PRE DapiDOSet1_WithTimer(ULONG handle, ULONG ch, ULONG data, ULONG time_ms);

// ----------------------------------------------------------------------------
// Analog Inputs
extern ULONG DAPI_FUNCTION_PRE DapiADGet(ULONG handle, ULONG ch);
extern float DAPI_FUNCTION_PRE DapiADGetVolt(ULONG handle, ULONG ch);
extern float DAPI_FUNCTION_PRE DapiADGetmA(ULONG handle, ULONG ch);
extern void  DAPI_FUNCTION_PRE DapiADSetMode(ULONG handle, ULONG ch, ULONG mode);
extern ULONG DAPI_FUNCTION_PRE DapiADGetMode(ULONG handle, ULONG ch);

// ----------------------------------------------------------------------------
// Analog Outputs
extern void DAPI_FUNCTION_PRE DapiDASet(ULONG handle, ULONG ch, ULONG data);
extern void DAPI_FUNCTION_PRE DapiDASetVolt(ULONG handle, ULONG ch, float data);
extern void DAPI_FUNCTION_PRE DapiDASetmA(ULONG handle, ULONG ch, float data);
extern void  DAPI_FUNCTION_PRE DapiDASetMode(ULONG handle, ULONG ch, ULONG mode);
extern ULONG DAPI_FUNCTION_PRE DapiDAGetMode(ULONG handle, ULONG ch);

// ----------------------------------------------------------------------------
// Temperature Inputs
extern float DAPI_FUNCTION_PRE DapiTempGet(ULONG handle, ULONG ch);

// ----------------------------------------------------------------------------
// Counter48 Inputs
extern void   DAPI_FUNCTION_PRE DapiCnt48ModeSet(ULONG handle, ULONG ch, ULONG mode);
extern ULONG  DAPI_FUNCTION_PRE DapiCnt48ModeGet(ULONG handle, ULONG ch);
extern ULONG  DAPI_FUNCTION_PRE DapiCnt48CounterGet32(ULONG handle, ULONG ch);
extern ULONGLONG  DAPI_FUNCTION_PRE DapiCnt48CounterGet48(ULONG handle, ULONG ch);

// ----------------------------------------------------------------------------
// Pulse-Generator Outputs
extern void DAPI_FUNCTION_PRE DapiPulseGenSet(ULONG handle, ULONG ch, ULONG mode, ULONG par0, ULONG par1, ULONG par2);

// ----------------------------------------------------------------------------
// Stepper
extern ULONG DAPI_FUNCTION_PRE DapiStepperCommand(ULONG handle, ULONG motor, ULONG cmd, ULONG par1, ULONG par2, ULONG par3, ULONG par4);
extern ULONG DAPI_FUNCTION_PRE DapiStepperCommandEx(ULONG handle, ULONG motor, ULONG cmd, ULONG par1, ULONG par2, ULONG par3, ULONG par4, ULONG par5, ULONG par6, ULONG par7);
extern ULONG DAPI_FUNCTION_PRE DapiStepperGetStatus(ULONG handle, ULONG motor, ULONG cmd);

// ----------------------------------------------------------------------------
// Watchdog
extern void DAPI_FUNCTION_PRE DapiWatchdogEnable(ULONG handle);
extern void DAPI_FUNCTION_PRE DapiWatchdogDisable(ULONG handle);
extern void DAPI_FUNCTION_PRE DapiWatchdogRetrigger(ULONG handle);

// ----------------------------------------------------------------------------
// CAN
typedef struct
{
	ULONG address;
	ULONG timestamp;
	ULONG use_extended_ids;
	ULONG data_count;
	UCHAR data[8];
} DAPI_CAN_MESSAGE_STRUCT;

extern ULONG DAPI_FUNCTION_PRE DapiCANCommand(ULONG handle, ULONG cmd, ULONG par1, ULONG par2, ULONG par3);
extern ULONG DAPI_FUNCTION_PRE DapiCANGetPacket(ULONG handle, ULONG message_box_nr, DAPI_CAN_MESSAGE_STRUCT *can_msg);
extern void DAPI_FUNCTION_PRE DapiCANSendPacket(ULONG handle, DAPI_CAN_MESSAGE_STRUCT *can_msg);


// ----------------------------------------------------------------------------
// Special
extern ULONG DAPI_FUNCTION_PRE DapiSpecialCommand(ULONG handle, ULONG cmd, ULONG par1, ULONG par2, ULONG par3);

extern ULONG DAPI_FUNCTION_PRE DapiReadMultipleBytes(ULONG handle, ULONG adress, ULONG adress_depth, ULONG increments, UCHAR * buff, ULONG buff_len);
extern ULONG DAPI_FUNCTION_PRE DapiWriteMultipleBytes(ULONG handle, ULONG adress, ULONG adress_depth, ULONG increments, UCHAR * buff, ULONG buff_len);

// ----------------------------------------------------------------------------


#ifdef __cplusplus
	}
#endif




// ----------------------------------------------------------------------------
// DI - Counter Mode

#define DAPI_CNT_MODE_READ_WITH_RESET							0x01
#define DAPI_CNT_MODE_READ_LATCHED								0x02

// ----------------------------------------------------------------------------
// A/D and D/A Modes

#define ADDA_MODE_UNIPOL_10V									0x00
#define ADDA_MODE_UNIPOL_5V										0x01
#define ADDA_MODE_UNIPOL_2V5									0x02

#define ADDA_MODE_BIPOL_10V										0x40
#define ADDA_MODE_BIPOL_5V										0x41
#define ADDA_MODE_BIPOL_2V5										0x42

#define ADDA_MODE_0_20mA										0x80
#define ADDA_MODE_4_20mA										0x81
#define ADDA_MODE_0_24mA										0x82
//#define ADDA_MODE_0_25mA										0x83
#define ADDA_MODE_0_50mA										0x84


#define ADDA_MODE_DA_DISABLE									0x100
#define ADDA_MODE_DA_ENABLE										0x200

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Stepper-Defines

// ------------------------------------
// ERROR Codes

#define DAPI_STEPPER_ERR_NONE									0		// es liegt kein Fehler vor 
#define DAPI_STEPPER_ERR_PARAMETER								1		// Parameter hat falschen Wertebereich 
#define DAPI_STEPPER_ERR_MOTOR_MOVE								2		// Kommando abgelehnt, da sich der Motor dreht
#define DAPI_STEPPER_ERR_DISABLE_MOVE							3		// Kommando abgehelnt, da Motorbewegung disabled ist
#define DAPI_STEPPER_ERR_DEVICE_NOT_FOUND						-1		// es liegt kein Fehler vor 

// ------------------------------------
// Special Stepper Function-Codes

#define DAPI_STEPPER_RETURN_0_BYTES 							0x00000000		// Kommando schickt 0 Byte als Antwort
#define DAPI_STEPPER_RETURN_1_BYTES 							0x40000000		// Kommando schickt 1 Byte als Antwort
#define DAPI_STEPPER_RETURN_2_BYTES 							0x80000000		// Kommando schickt 2 Byte als Antwort
#define DAPI_STEPPER_RETURN_4_BYTES 							0xc0000000		// Kommando schickt 4 Byte als Antwort


#define DAPI_STEPPER_CMD_SET_MOTORCHARACTERISTIC                ( 0x00000001 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_GET_MOTORCHARACTERISTIC                ( 0x00000002 + DAPI_STEPPER_RETURN_4_BYTES )  
#define DAPI_STEPPER_CMD_SET_POSITION                           ( 0x00000003 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_GO_POSITION                            ( 0x00000004 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_GET_POSITION                           ( 0x00000005 + DAPI_STEPPER_RETURN_4_BYTES )  
#define DAPI_STEPPER_CMD_SET_FREQUENCY                          ( 0x00000006 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_SET_FREQUENCY_DIRECTLY                 ( 0x00000007 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_GET_FREQUENCY                          ( 0x00000008 + DAPI_STEPPER_RETURN_2_BYTES )  
#define DAPI_STEPPER_CMD_FULLSTOP                               ( 0x00000009 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_STOP                                   ( 0x00000010 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_GO_REFSWITCH                           ( 0x00000011 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_DISABLE                                ( 0x00000014 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_MOTORCHARACTERISTIC_LOAD_DEFAULT		( 0x00000015 + DAPI_STEPPER_RETURN_0_BYTES )
#define DAPI_STEPPER_CMD_MOTORCHARACTERISTIC_EEPROM_SAVE		( 0x00000016 + DAPI_STEPPER_RETURN_0_BYTES )
#define DAPI_STEPPER_CMD_MOTORCHARACTERISTIC_EEPROM_LOAD		( 0x00000017 + DAPI_STEPPER_RETURN_0_BYTES )
#define DAPI_STEPPER_CMD_GET_CPU_TEMP							( 0x00000018 + DAPI_STEPPER_RETURN_1_BYTES )
#define DAPI_STEPPER_CMD_GET_MOTOR_SUPPLY_VOLTAGE				( 0x00000019 + DAPI_STEPPER_RETURN_2_BYTES )
#define DAPI_STEPPER_CMD_GO_POSITION_RELATIVE                   ( 0x00000020 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_EEPROM_ERASE							( 0x00000021 + DAPI_STEPPER_RETURN_0_BYTES )  
#define DAPI_STEPPER_CMD_SET_VECTORMODE                         ( 0x00000040 + DAPI_STEPPER_RETURN_0_BYTES )  

//#define DAPI_STEPPER_CMD_GET_STATUS                             ( 0x00000015 + DAPI_STEPPER_RETURN_1_BYTES )


// ------------------------------------
// values for PAR1 for DAPI_STEPPER_CMD_SET_MOTORCHARACTERISTIC

#define DAPI_STEPPER_MOTORCHAR_PAR_STEPMODE						1					// Schrittmode (Voll-, Halb-, Viertel-, Achtel-, Sechszehntelschritt 
#define DAPI_STEPPER_MOTORCHAR_PAR_GOFREQUENCY					2					// Schrittfrequenz bei GoPosition [Vollschritt / s]
#define DAPI_STEPPER_MOTORCHAR_PAR_STARTFREQUENCY				3					// Startfrequenz [Vollschritt / s]
#define DAPI_STEPPER_MOTORCHAR_PAR_STOPFREQUENCY				4					// Stopfrequenz [Vollschritt / s]
#define DAPI_STEPPER_MOTORCHAR_PAR_MAXFREQUENCY					5					// maximale Frequenz [Vollschritt / s]
#define DAPI_STEPPER_MOTORCHAR_PAR_ACCELERATIONSLOPE			6					// Beschleunigung in [Vollschritten / ms]
#define DAPI_STEPPER_MOTORCHAR_PAR_DECELERATIONSLOPE			7					// Bremsung in [Vollschritten / ms]
#define DAPI_STEPPER_MOTORCHAR_PAR_PHASECURRENT					8					// Phasenstrom [mA]
#define DAPI_STEPPER_MOTORCHAR_PAR_HOLDPHASECURRENT				9					// Phasenstrom bei Motorstillstand [mA]
#define DAPI_STEPPER_MOTORCHAR_PAR_HOLDTIME 					10					// Zeit in der der Haltestrom flieﬂt nach Motorstop [s]
#define DAPI_STEPPER_MOTORCHAR_PAR_STATUSLEDMODE				11					// Betriebsart der Status-LED
#define DAPI_STEPPER_MOTORCHAR_PAR_INVERT_ENDSW1				12					// invertiere Funktion des Endschalter1  
#define DAPI_STEPPER_MOTORCHAR_PAR_INVERT_ENDSW2				13					// invertiere Funktion des Endschalter12 
#define DAPI_STEPPER_MOTORCHAR_PAR_INVERT_REFSW1				14					// invertiere Funktion des Referenzschalterschalter1
#define DAPI_STEPPER_MOTORCHAR_PAR_INVERT_REFSW2				15					// invertiere Funktion des Referenzschalterschalter2
#define DAPI_STEPPER_MOTORCHAR_PAR_INVERT_DIRECTION 			16					// invertiere alle Richtungsangaben
#define DAPI_STEPPER_MOTORCHAR_PAR_ENDSWITCH_STOPMODE			17					// Bei Endschalter soll (0=full stop/1=stop mit rampe)
#define DAPI_STEPPER_MOTORCHAR_PAR_GOREFERENCEFREQUENCY_TOENDSWITCH		18			// Motor Frequency for GoReferenceCommand
#define DAPI_STEPPER_MOTORCHAR_PAR_GOREFERENCEFREQUENCY_AFTERENDSWITCH	19			// Motor Frequency for GoReferenceCommand
#define DAPI_STEPPER_MOTORCHAR_PAR_GOREFERENCEFREQUENCY_TOOFFSET 20					// Motor Frequency for GoReferenceCommand

// ----------------------------------------------------------------------------
// values for PAR1 for DAPI_STEPPER_CMD_GO_REFSWITCH

#define DAPI_STEPPER_GO_REFSWITCH_PAR_REF1						1
#define DAPI_STEPPER_GO_REFSWITCH_PAR_REF2						2
#define DAPI_STEPPER_GO_REFSWITCH_PAR_REF_LEFT					4
#define DAPI_STEPPER_GO_REFSWITCH_PAR_REF_RIGHT					8
#define DAPI_STEPPER_GO_REFSWITCH_PAR_REF_GO_POSITIVE			16
#define DAPI_STEPPER_GO_REFSWITCH_PAR_REF_GO_NEGATIVE			32
#define DAPI_STEPPER_GO_REFSWITCH_PAR_SET_POS_0					64

// ------------------------------------
// Stepper Read Status
#define DAPI_STEPPER_STATUS_GET_POSITION						0x01
#define DAPI_STEPPER_STATUS_GET_SWITCH							0x02
#define DAPI_STEPPER_STATUS_GET_ACTIVITY						0x03


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// CAN-Defines



// ------------------------------------
// CAN Commands

#define DAPI_CAN_CMD_SET_BITRATE								1
#define DAPI_CAN_CMD_SET_MASK0									2
#define DAPI_CAN_CMD_SET_RX_ADDRESS								3
#define DAPI_CAN_CMD_CLR_TIMESTAMP								4

#define DAPI_CAN_CMD_TEST_GEN_RX_PACK							0xfffffff0


#define DAPI_PAR_CAN_MESSAGE_BOX_0								0
#define DAPI_PAR_CAN_MESSAGE_BOX_1								1
#define DAPI_PAR_CAN_MESSAGE_BOX_2								2
#define DAPI_PAR_CAN_MESSAGE_BOX_3								3

#define DAPI_CAN_BITRATE_10000									10000
#define DAPI_CAN_BITRATE_20000									20000
#define DAPI_CAN_BITRATE_50000									50000
#define DAPI_CAN_BITRATE_100000									100000
#define DAPI_CAN_BITRATE_125000									125000
#define DAPI_CAN_BITRATE_250000									250000
#define DAPI_CAN_BITRATE_500000									500000
#define DAPI_CAN_BITRATE_1000000 								1000000

#define DAPI_CAN_MASK_SINGLE									0xffffffff
#define DAPI_CAN_MASK_ALL										0x0

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// CNT48 Commands


#define DAPI_CNT48_FILTER_20ns								0x0000
#define DAPI_CNT48_FILTER_100ns								0x1000
#define DAPI_CNT48_FILTER_250ns								0x2000
#define DAPI_CNT48_FILTER_500ns								0x3000
#define DAPI_CNT48_FILTER_1us								0x4000
#define DAPI_CNT48_FILTER_2_5us								0x5000
#define DAPI_CNT48_FILTER_5us								0x6000
#define DAPI_CNT48_FILTER_10us								0x7000
#define DAPI_CNT48_FILTER_25us								0x8000
#define DAPI_CNT48_FILTER_50us								0x9000
#define DAPI_CNT48_FILTER_100us								0xA000
#define DAPI_CNT48_FILTER_250us								0xB000
#define DAPI_CNT48_FILTER_500us								0xC000
#define DAPI_CNT48_FILTER_1ms								0xD000
#define DAPI_CNT48_FILTER_2_5ms								0xE000
#define DAPI_CNT48_FILTER_5ms								0xF000

#define DAPI_CNT48_MODE_COUNT_RISING_EDGE					0x0000
#define DAPI_CNT48_MODE_T									0x000D
#define DAPI_CNT48_MODE_FREQUENCY							0x000E
#define DAPI_CNT48_MODE_PWM									0x000F

//#define DAPI_CNT48_MODE_COUNT_RISING_EDGE_WITH_READ_RESET	0x0001


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Structs

typedef struct
{
	unsigned char address[256];
	ULONG portno;
} DAPI_OPENMODULEEX_STRUCT;
