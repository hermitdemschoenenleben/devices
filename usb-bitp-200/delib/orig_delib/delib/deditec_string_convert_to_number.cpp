//#include <stdlib.h>
//#include <errno.h>
#include <malloc.h>
#include "deditec_string_convert_to_number.h"

//#define NULL 0

char *strextract (const char* str, const unsigned int len)
{
	char *extraction = (char*)malloc(len + 1);
	unsigned char i = 0;
	extraction[len] = 0;
	while (i < len)
	{
		extraction[i] = str[i];
		i++;
	}
	return extraction;
}

/*long str2l (const char* str)
{
	return 0;
}
unsigned long str2ul (const char* str)
{
	return 0;
}
float str2f (const char* str)
{
	return 0.0;
}*/

char dt_stringtochar (const char* str, const unsigned int len, long* end)
{
	long val;
int errno;
	val = dt_stringtolong(str, len, end);
	errno = 0;

	if (errno != 0 || val < -128 || val > 127)
	{
		errno = 1;
		return 0;
	}

	return (char)val;
}

unsigned char dt_stringtouchar (const char* str, const unsigned int len, long* end)
{
	long retval;
int errno;

	retval = dt_stringtoulong(str, len, end);
	errno = 0;

	if (errno != 0 || retval > 255 || retval < 0)
	{
		errno = 1;
		return 0;
	}

	return (unsigned char)retval;
}

int dt_stringtoint (const char* str, const unsigned int len, long* end)
{
	long retval;
int errno;

	retval = dt_stringtolong(str, len, end);
	errno = 0;

	if (errno != 0 || retval < -32768 || retval > 32767)
	{
		errno = 1;
		return 0;
	}

	return (int)retval;
}

unsigned int dt_stringtouint (const char* str, const unsigned int len, long* end)
{
	long retval;
int errno;

	retval = dt_stringtoulong(str, len, end);
	errno = 0;

	if (errno != 0 || retval > 65535 || retval < 0)
	{
		errno = 1;
		return 0;
	}

	return (unsigned int)retval;
}

long dt_stringtolong (const char* str, const unsigned int len, long* end)
{
	long i, jmp, neg;
	char *extraction;
	long retval;
int errno;

	*end = -1;
	jmp = 0;
	neg = 0;
	errno = 0;

	switch (str[0])
	{
		case '+':
			jmp = 1;
			break;
		case '-':
			neg = 1;
	}

	for (i = jmp + neg; i < (long)len; i++)
	{
		if (str[i] < '0' || str[i] > '9')
		{
			if (jmp == i)
			{
				return 0;
			}
			*end = i;
			break;
		}
	}
	*end = i;

	extraction = strextract(str + jmp, *end);
	retval = strtol(extraction, 0/*NULL*/, 10);
	free(extraction);
	if (errno != 0)
	{
		*end = -1;
		return 0;
	}
	return retval;
}

unsigned long dt_stringtoulong (const char* str, const unsigned int len, long* end)
{
	long i, jmp;
	char *extraction;
	unsigned long retval;
int errno;

	*end = -1;
	jmp = 0;
	errno = 0;

	if (str[0] == '+')
	{
	jmp++;
	}

	for (i = jmp; i < (long)len; i++)
	{
		if (str[i] < '0' || str[i] > '9')
		{
			if (jmp == i)
			{
				return 0;
			}
			*end = i;
			break;
		}
	}
	*end = i;

	extraction = strextract(str + jmp, *end);
	retval = strtoul(extraction, 0/*NULL*/, 10);
	free(extraction);
	if (errno != 0)
	{
		*end = -1;
		return 0;
	}

	return retval;
}

float dt_stringtofloat (const char* str, const unsigned int len, long* end)
{
	long i, jmp, neg;
	char *extraction;
	float retval;
	long dotpos;
int errno;

	*end = -1;
	jmp = 0;
	neg = 0;
	dotpos = -1;
	errno = 0;

	switch (str[0])
	{
		case '+':
			jmp = 1;
			break;
		case '-':
			neg = 1;
	}

	for (i = jmp + neg; i < (long)len; i++)
	{
		if (str[i] == '.' || str[i] == ',')
		{
			if (dotpos != -1)
			{
				*end = -1;
				return 0.0;
			}
			dotpos = i;
			continue;
		}
		if (str[i] < '0' || str[i] > '9')
		{
			if (jmp == i)
			{
				return 0;
			}
			*end = i;
			break;
		}
	}
	*end = i;

	extraction = strextract(str + jmp, *end);
	if (extraction[dotpos] == ',')
	{
		extraction[dotpos] = '.';
	}
	retval = strtod(extraction, NULL);
	free(extraction);
	if (errno != 0)
	{
		*end = -1;
		return 0.0;
	}
	return retval;
}

/*unsigned int str2ip (const char* str, const unsigned int len, long* end)
{
	unsigned char ip[4];


	char c, **parts = (char**)malloc(4 * sizeof(char*));
	unsigned int i, i2, pos = 0, retval = 0;
	bool error = false;
	errno = 0;

	for (i = 0; i < 4; i++)
	{
		parts[i] = (char*)malloc(4);
	}

	for (i = 0; i < 4; i++)
	{
		for (i2 = 0; i2 < 4; i2++)
		{
			c = str[pos + i2];
			if (c == '.')
			{
				parts[i][i2] = 0;
				pos += i2+1;
				break;
			}
			if (!isdigit(c))
			{
				if (i != 3)
				{
					error = true;
					break;
				}
				else
				{
					parts[i][i2] = 0;
					pos += i2+1;
					break;
				}
			}
			parts[i][i2] = c;
		}
		if (error)
		{
			break;
		}
	}

	for (i = 0; i < 4; i++)
	{
		if (!error)
		{
			unsigned short part = atoi(parts[i]) << (3 - i) * 8;
			if (part > 255) error = true;
			else retval |= part;
		}
		free(parts[i]);
	}
	free(parts);
	return retval;
}*/

unsigned long dt_hexstringtoulong (unsigned char* value, unsigned long value_length, unsigned long* end)
{
	unsigned long i;
	unsigned long retval = 0;
	unsigned char c;
	*end = -1;
	
	for (i = 0; i < value_length; i++)
	{
		c = value[i];
		if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
		{
			if (i == 0)
			{
				return 0;
			}
			*end = i;
		}
	}
	if (*end = -1) *end = value_length;
	for (i = 0; i < *end; i++)
	{
		c = value[*end - i - 1];
		if (c >= 'a' && c <= 'f')
		{
			c -= 'a' - 10;
		}
		else if (c >= 'A' && c <= 'F')
		{
			c -= 'A' - 10;
		}
		else
		{
			c -= '0';
		}
		retval += c << 4 * i;
	}
	return retval;
}
