extern bool DapiTCPOpenComPort(unsigned long nComPort, unsigned long BaudRate, unsigned long Timeout);
extern BOOL DapiTCPCloseComPort(unsigned long nComPort);
extern bool DapiTCPSendCommand(ULONG handle, UCHAR tx[], unsigned long tx_length, UCHAR rx[], unsigned long rx_anzahl);

extern unsigned long DapiOpenModuleIntern_ETH(unsigned long handle, unsigned long moduleID, unsigned long moduleNr);

extern ULONG DapiTCPWriteMultipleBytes(ULONG handle, ULONG adress, ULONG adress_tiefe, ULONG wiederholungen, unsigned char* data, ULONG data_length);


extern ULONG DapiTCPPing(ULONG handle, ULONG value);

extern void DapiTCPWriteByte(unsigned long handle, unsigned long adress, unsigned long value);
extern void DapiTCPWriteWord (unsigned long handle,unsigned long adress,unsigned long value);
extern void DapiTCPWriteLong (unsigned long handle,unsigned long adress,unsigned long value);
extern void DapiTCPWriteLongLong (unsigned long handle,unsigned long adress,ULONGLONG value);

extern unsigned long DapiTCPReadByte (unsigned long handle,unsigned long adress);
extern unsigned long DapiTCPReadWord (unsigned long handle,unsigned long adress);
extern unsigned long DapiTCPReadLong(unsigned long handle, unsigned long adress);
extern ULONGLONG DapiTCPReadLongLong(unsigned long handle, unsigned long adress);

extern void DapiTCPWriteBitClear(unsigned long handle,unsigned long adress,unsigned long value);
extern void DapiTCPWriteBitSet(unsigned long handle,unsigned long adress,unsigned long value);

extern void DapiTCPWriteBitClearLONGLONG (unsigned long handle,unsigned long adress,ULONGLONG value);
extern void DapiTCPWriteBitSetLONGLONG (unsigned long handle,unsigned long adress,ULONGLONG value);

extern void DapiTCPSpecialCmd (unsigned long handle,unsigned long cmd);
