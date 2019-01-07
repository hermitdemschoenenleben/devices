extern bool DapiSeriellFTDIOpenComPort(unsigned long nComPort, unsigned long BaudRate, unsigned long Timeout);
extern BOOL DapiSeriellFTDICloseComPort(unsigned long nComPort);

extern ULONG DapiSeriellFTDIPing(ULONG handle, ULONG value);

extern void DapiSeriellFTDIWriteByte(unsigned long handle, unsigned long adress, unsigned long value);
extern void DapiSeriellFTDIWriteWord (unsigned long handle,unsigned long adress,unsigned long value);
extern void DapiSeriellFTDIWriteLong (unsigned long handle,unsigned long adress,unsigned long value);
extern void DapiSeriellFTDIWriteLongLong (unsigned long handle,unsigned long adress,ULONGLONG value);

extern unsigned long DapiSeriellFTDIReadByte (unsigned long handle,unsigned long adress);
extern unsigned long DapiSeriellFTDIReadWord (unsigned long handle,unsigned long adress);
extern unsigned long DapiSeriellFTDIReadLong(unsigned long handle, unsigned long adress);
extern ULONGLONG DapiSeriellFTDIReadLongLong(unsigned long handle, unsigned long adress);

extern void DapiSeriellFTDIWriteBitClear(unsigned long handle,unsigned long adress,unsigned long value);
extern void DapiSeriellFTDIWriteBitSet(unsigned long handle,unsigned long adress,unsigned long value);

extern void DapiSeriellFTDIWriteBitClearLONGLONG (unsigned long handle,unsigned long adress,ULONGLONG value);
extern void DapiSeriellFTDIWriteBitSetLONGLONG (unsigned long handle,unsigned long adress,ULONGLONG value);

extern void DapiSeriellFTDIWrite64BytesMax (unsigned long handle,unsigned long adress, unsigned char *data, ULONG data_length);



extern void DapiSeriellFTDIWriteSetByte(unsigned long handle, unsigned long adress, unsigned long value);
extern void DapiSeriellFTDIWriteClearByte(unsigned long handle, unsigned long adress, unsigned long value);
