extern void __stdcall DapiSetError(ULONG errorcode, ULONG suberror, char *errortext, char * filename, ULONG line);
//extern ULONG __stdcall DapiGetLastError();
//extern char* __stdcall DapiGetLastErrorText();
extern void __stdcall DapiClearLastError();
