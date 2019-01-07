#ifndef dt_string_convert_to_number
#define dt_string_convert_to_number

extern char           dt_stringtochar (const char* str, const unsigned int len, long* end);
extern unsigned char  dt_stringtouchar (const char* str, const unsigned int len, long* end);
extern int            dt_stringtoint (const char* str, const unsigned int len, long* end);
extern unsigned int   dt_stringtouint (const char* str, const unsigned int len, long* end);
extern long           dt_stringtolong (const char* str, const unsigned int len, long* end);
extern unsigned long  dt_stringtoulong (const char* str, const unsigned int len, long* end);
extern float          dt_stringtofloat (const char* str, const unsigned int len, long* end);
// extern unsigned int   str2ip (const char* str, const unsigned int len, long* end);
extern unsigned long  dt_hexstringtoulong (unsigned char* value, unsigned long value_length, unsigned long* end);

#endif
