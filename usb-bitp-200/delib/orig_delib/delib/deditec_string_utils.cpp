#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define ERROR 100

#define LINE_LENGTH 1000


/////////////////////////////////////////////////////////////////////////////////////////
// removes '\n' at end of string
// removes leading and trailing spaces
/////////////////////////////////////////////////////////////////////////////////////////

unsigned long make_line_clean(unsigned char *string, unsigned long string_size)
{
   unsigned long i;
   unsigned long i_destination;
   unsigned long ret;

   ret = 0; //string_not_modified

   // removing trailing new_lines or spaces
   i = 0;
   while (string[i] != 0 && i <= string_size - 1) // get amount of chars (not counting NULL)
   {
      i++;
   }
   i--;
   while (string[i] == 10  || string[i] == 13  || string[i] == ' ')
   {
      string[i] = 0;
      i--;
      ret = 1; //string_modified
   }

   // removing beginnig spaces
   i = 0;
   while(string[i] == ' ')
   {
      i++;
   }
   i_destination = 0;
   if (i > 0) // move all chars one by one overwriting leading space
   {
      while (i <= string_size - 1)
      {
         string[i_destination] = string[i];
         i++;
         i_destination++;
         ret = 1; //string_modified
      }
   }
   return ret;
}


////////////////////////////////////////////////////////////////////////////////////
// charwise copy from source_buffer to dest_buffer until line feed (<LF>) or NULL
//
// - if during copy LF found, will add LF followed by NULL to dest_buffer
// - if during copy no LF but NULL found, than NULL will be added to dest_buffer
// - function returns amount of copied chars (inclusive LF, but exclusive NULL)
////////////////////////////////////////////////////////////////////////////////////

unsigned long buffer_line_copy(unsigned char *dest_buffer, unsigned char * source_buffer)
{
	unsigned long counter;

	counter = 0;

	while ((*(source_buffer + counter) != 10) && ((*(source_buffer + counter) != 0))) // ASCII 10 is line feed (<LF>)
	{
//      printf("buffer_line_copy char: %x####\n", *(source_buffer + counter) );
		*(dest_buffer + counter) = *(source_buffer + counter);
		++counter;
	}
	if (*(source_buffer + counter) == 0x0a) // if <LF>
	{
		*(dest_buffer + counter) = 0x0a;
		++counter;
		*(dest_buffer + counter) = 0x00;
	}
	else
	{
		*(dest_buffer + counter) = 0x00;
	}
//   getchar();
	return counter;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// strcpy_n():
//
// synopsis: copy a buffer to another buffer with forced NULL termination
//
// descripion:
//
// copy up to amount_of_chars charwise from source_buffer to dest_buffer or
// until dest_buffer full (indicated with dest_bufer_size);
//
// If during the copy process a NULL in source_buffer is found, it will be
// copied too, but the copy-process will stop at this point
//
// (string-appending is possible through increasing dest_buffer pointing to
//  the position of NULL, thus overwriting it)
//
// If last char to be copied is not NULL, a NULL will be written to dest_buffer
// instead
//
//
// Hint: not recommended for binary copy. because if NULL in string, function
//       will copy until this NULL (include copying it). Fuction will then end
//       returning a value; for binary copy, use buffcpy_n instead
//
// return values:
//
// >=1: returns amount of chars copied
////////////////////////////////////////////////////////////////////////////////////
unsigned long strcpy_n( unsigned char * dest_buffer,   unsigned long dest_buffer_size,
                        unsigned char * source_buffer, unsigned long amount_of_chars)
{

	unsigned long pos_source_buffer;
   unsigned long amount_copied_chars;

   pos_source_buffer = 0;
   amount_copied_chars = 0;

   while (   pos_source_buffer < amount_of_chars
          && pos_source_buffer < dest_buffer_size - 1
          && *(source_buffer + pos_source_buffer) != 0)
   {
	//for (pos_source_buffer = 0; pos_source_buffer < amount_of_chars; pos_source_buffer++)
	//{
//		printf("content of pos_source_buffer: %c\n", *(source_buffer + pos_source_buffer));
//		getch();
      *(dest_buffer + pos_source_buffer) = *(source_buffer + pos_source_buffer);
      pos_source_buffer++;
      amount_copied_chars++;
      //printf("%c", *(source_buffer + pos_source_buffer));
	}
	*(dest_buffer + pos_source_buffer) = 0;
   return amount_copied_chars;
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// buffcpy_n():
//
// synopsis: copy a buffer to another buffer (binary copy)
//
// decripion:
//
// copy amount_of_chars bytewise from source_buffer to dest_buffer;

// If a source-char is NULL, function will stop by copying this last NULL char;
// If no NULL apears
// amount_of_chars reached
// or dest_buffer full. dest_buffer will be NULL-terminated.
// until dest_buffer full, all chars copied or char is NULL
// (append is possible through increasing dest_buffer where NULL is found)
//
// Hint: not recommended for binary copy. because if NULL in string, function
//       will copy until this NULL (still copying it). Fuction will then end
//       returning a value
//
// return values:
//
//  0: ok; desired amount_of_chars copied
// -1: error, destination buffer too small
////////////////////////////////////////////////////////////////////////////////////
unsigned long buffcpy_n( unsigned char * dest_buffer,   unsigned long dest_buffer_size,
                         unsigned char * source_buffer, unsigned long amount_of_bytes)
{
	unsigned long pos_source_buffer;
   unsigned long amount_copied_bytes;

   // init
   pos_source_buffer = 0;
   amount_copied_bytes = 0;

   // begin
   if (dest_buffer_size < amount_of_bytes) return -1; // destination buffer too small

   while (pos_source_buffer < amount_of_bytes)
   {
		*(dest_buffer + pos_source_buffer) = *(source_buffer + pos_source_buffer);
      pos_source_buffer++;
      amount_copied_bytes++;
      //printf("0x%x ", *(source_buffer + pos_source_buffer));
	}
   return 0;
}


////////////////////////////////////////////////////////////////////////////////////
// UNFINISHED !!! appends content of source_buffer to dest_buffer
// (copies charwise from source_buffer to dest_buffer)
//
// - returns amount of chars copied
////////////////////////////////////////////////////////////////////////////////////

unsigned long strcat_max (unsigned char *dest_buffer,   unsigned long dest_buffer_size,
                          unsigned char *source_buffer, unsigned long amount_of_chars)
{
   unsigned long ret;
   unsigned long pos_dest_buffer;


   //init
   ret = 0;
   pos_dest_buffer = 0;

   while(*(dest_buffer + pos_dest_buffer) != 0 && pos_dest_buffer < dest_buffer_size - 1)
   {
      pos_dest_buffer++;

   }

   return ret;
}


////////////////////////////////////////////////////////////////////////////////////
// DEBUG purpose only !!!
//
// prints charwise a buffer to stdout until end of buffer reached or NULL
// returns amount of chars
////////////////////////////////////////////////////////////////////////////////////

unsigned long printf_n (unsigned char *buffer, unsigned long size_of_buffer)
{
   unsigned long buffer_pos;

   buffer_pos = 0;

   while (*(buffer + buffer_pos) != 0 && *(buffer + buffer_pos) < size_of_buffer)
   {
      printf ("%c at buffer_pos: %d\n", * (buffer + buffer_pos), (int) buffer_pos);
      buffer_pos++;
   }
   return --buffer_pos;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// ascii to hex
/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char asc2hex (char z)
{
    unsigned char ret;

    if(z >= '0' && z <= '9') ret = z - '0';
    else if(z >= 'a' && z <= 'f') ret = z - 'a' + 10;
    else if(z >= 'A' && z <= 'F') ret = z - 'A' + 10;
    else ret = 0; // error, char not expected

    return ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// reverse string with (even some or none) escaped chars into string with unescaped chars
// returns FALSE if after a '%' two expected hex values not found
/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char unescape_str(unsigned char *escape,	 unsigned long escape_length,
						         unsigned char *unescape, unsigned long unescape_length)
{
//	unsigned char unescape[254];
//	unsigned char escape_length;
	unsigned long pos_escape;
	unsigned long pos_unescape;
	unsigned char asc_p1;
	unsigned char asc_p2;

//	printf("--escape: %s, escape_length: %d\n", escape, escape_length);

	//seek for escape chars
	pos_unescape = 0;
	*(unescape + pos_unescape) = 0;


	for (pos_escape = 0; ((*(escape + pos_escape) != 0) && (pos_escape < escape_length)); pos_escape++)
	{
		//printf("char is: %c\n", *(escape + pos_escape));

		if (*(escape + pos_escape) == '%')	// an escaped char found
		{
			if (pos_escape + 2 >= escape_length) // escaped char should have 2 chars
			{
				*(unescape + pos_unescape) = 0;
				return FALSE;
			}
			++pos_escape;

			asc_p1 = *(escape + pos_escape++);
			asc_p2 = *(escape + pos_escape);


			if (!((asc_p1 >= '0' && asc_p1 <= '9') ||  // if illegal char found, end here
				  (asc_p1 >= 'A' && asc_p1 <= 'F') ||
				  (asc_p1 >= 'a' && asc_p1 <= 'f')) )
			{
				*(unescape + pos_unescape) = 0;
				return FALSE;
			}

			if (!((asc_p2 >= '0' && asc_p2 <= '9') ||  // if illegal char found, end here
				  (asc_p2 >= 'A' && asc_p2 <= 'F') ||
				  (asc_p2 >= 'a' && asc_p2 <= 'f')) )
			{
				*(unescape + pos_unescape) = 0;
				return FALSE;
			}

			*(unescape + pos_unescape)  = (asc2hex(asc_p1) << 4) | asc2hex(asc_p2);

//			printf("asc2hex(asc_p1): %x\n", asc2hex(asc_p1));
//			printf("asc2hex(asc_p2): %x\n", asc2hex(asc_p2));
//			printf("*(unescape + pos_escape): %x\n",*(unescape + pos_unescape));

			//printf ("unescaped is: %c\n", *(unescape + pos_unescape));
//getch();
			pos_unescape++;
		}
      else if (*(escape + pos_escape) == '+') // a space char is found (+ is encoded as space)
      {
         *(unescape + pos_unescape) = ' ';
         pos_unescape++;
      }
      else
		{
			*(unescape + pos_unescape) = *(escape + pos_escape);
			pos_unescape++;
		}
	}

//getch();

	*(unescape + pos_unescape) = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
// function searchs for a given pattern inside right_string.
//
// - returns (#defined) TRUE, if pattern is found, else (#defined) FALSE; in case an
//   error occurs, (#defined) ERROR is returned
//  REM: pattern MUST be NULL-terminated !!!
//////////////////////////////////////////////////////////////////////////////////////



unsigned long cutontext(unsigned char *right_string, unsigned long right_string_size,
					      	unsigned char *left_string,  unsigned long left_string_size,
					      	unsigned char *pattern,		  unsigned long pattern_length)
{
	unsigned long pos_right_string;
	unsigned long pos_left_string;
	unsigned long pos_pattern;
	unsigned long found;

	//init
	pos_right_string = 0;
	pos_left_string = 0;
	pos_pattern = 0;
	found = FALSE;



	while (	pos_right_string < right_string_size &&
			pos_right_string < strlen((const char*) right_string) )
	{
		if ( *(right_string + pos_right_string) == *(pattern + pos_pattern) )	// if char n of pattern found
		{
//      printf("debug msg cutontext, found matching char is: %c\n", *(right_string + pos_right_string));

//			printf("char $ found\n");
			pos_right_string++;
			pos_pattern++;														//  n + 1

			if (pos_pattern == pattern_length)	// if all chars of pattern found
			{
//				printf("all chars of pattern found\n");
				found = TRUE;

				if (left_string_size - 1 < pos_right_string - pos_pattern)
				{
					return ERROR; // left_string_size not big enough
				}
				// copy left side of pattern to left_string
				strcpy_n(left_string, left_string_size, right_string, pos_right_string - pos_pattern);
//            printf("<br />right_string: -->%s<--\n<br />", right_string);
//				printf("<br />left_string: -->%s<--\n<br />", left_string);
//				printf(">>>>strlen(right_string): %d\n<br />", strlen(right_string));
//				printf(">>>>pos_pattern: %d\n<br />", pos_pattern);
//				printf(">>>>pos_right_string :%d\n<br />", pos_right_string);
//				printf(">>>>strlen(right_string) + 1 - pos_pattern - pos_right_string): %d\n<br />", strlen(right_string) + 1 - pos_pattern - pos_right_string);
				// copy right side of pattern to right_string
            //write_to_log("cutontext: 2", strlen("cutontext: 2"));
           //write_to_log(pattern, strlen(pattern));
           //write_to_log(right_string, strlen(right_string));
           strcpy_n(right_string, right_string_size, &right_string[pos_right_string], strlen((const char *) right_string) - pos_right_string );
				//strcpy_n(right_string, &right_string[pos_right_string], strlen(right_string) + 2 - pos_pattern - pos_right_string);

    //        strcpy_n(unsigned char *dest_buffer, unsigned char *source_buffer, unsigned long amount_of_chars)

            //write_to_log("cutontext: 3", strlen("cutontext: 3"));
//				strcpy_n(right_string, &right_string[pos_right_string], pos_pattern + 1 - strlen(right_string));
//				printf("right_string: -->%s<--\n", right_string);

				return found;
			}
		}
		else
		{
			pos_pattern = 0;		// reset in case the first n chars of pattern where found, but n+1 wasn't

			// without following if, the pattern 'some' would not be found if right_string is 'somsome'
			if ( *(right_string + pos_right_string) == *(pattern + pos_pattern) )	// if char 0 of pattern found
			{
				pos_pattern++;
			}
			pos_right_string++;
		}
	}
	return found;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// splits a string (while parsing from left to right) at a certain char in two parts.
// the left string-chunk (seen from the found char) will be stored in left_string,
// while the right string-chunk will be stored in right_string, which
// is also the source (where the original string was and so, will be overwritten)
//
// If the char is to be found more then once in string, then cutting will take place at the leftmost char
// - returns TRUE if split took place
// - returns FALSE if no split (char c was not found)
//
//
// return values:
//
//  1: ok, cutting done
//  0: ok, nothing cutted, char 'c' not found
// -1:
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long cuttext(	unsigned char *right_string, unsigned long right_string_size,
					      	unsigned char *left_string,  unsigned long left_string_size,
                        unsigned char c)
{
	unsigned long pos_right_string;
	unsigned long right_string_length;
	unsigned long i;

	//init
	pos_right_string = 0;
   right_string_length = 0;

   //same as: right_string_length = strlen(right_string);
   while (* (right_string + pos_right_string) != 0 && pos_right_string < right_string_size)
   {
      right_string_length++;
      pos_right_string++;
      //printf("right_string_length: %d\n", right_string_length);
   }

   pos_right_string = 0; // reset again for while loop


//printf("char is :%c<br />", c);
//	printf("-->right_string: %s\n", right_string);


	while(*(right_string + pos_right_string) != c  &&
			pos_right_string < right_string_length &&
			pos_right_string < right_string_size)
	{

		++pos_right_string;
		//if (pos_right_string + 1 >= right_string_size) {return 0;}
	}
	// char not found
	if (pos_right_string >= right_string_length)
	{
//printf("char %c not found, right_string_length: %d\n", c, right_string_length);
		// return complete string (don't change anything)
		for (i = 0; i < right_string_length; i++)
		{
//printf("char %c\n", *(right_string + i));
			if (i + 1 >= left_string_size) {return ERROR;}
			*(left_string + i) = *(right_string + i) ;
		}
		*(left_string + i) = 0;

		return FALSE; // no split

	// char found
	}
	else
	{

		for (i = 0; i < pos_right_string; i++)
		{
			if (i + 1 >= left_string_size) {return ERROR;}
			*(left_string + i) = *(right_string + i);
		}
		*(left_string + i) = 0;

		for (i = 0; (i + pos_right_string) < right_string_length; i++)
		{
			*(right_string + i) = *(right_string + i + pos_right_string + 1);
			//printf("*(right_string + %d): %c\n", i, *(right_string + i));
		}
		*(right_string + i) = 0;

		//printf("right_string_length: %d\n", right_string_length);
		//printf("pos_right_string: %d\n", pos_right_string);
	}
	return 1; //  splitting done
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// retrieves content of file (as include) and writes it to given pointer
// - returns amount of chars if everything went fine, else -1 (error)
//
//////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long copy_file_to_buffer (unsigned char *file_name,
                                   unsigned char *destination_buffer, unsigned long destination_buffer_size)
{
   FILE *include_file_ptr;

   unsigned char line[LINE_LENGTH];
   signed long ret;

   unsigned long pos_destination_buffer;
   unsigned long pos_line;


	// --- debug
	#ifdef debug_commands
		char msg[200];
		sprintf(msg,"copy_file_to_buffer - function begin\n");
		log_write(msg, LOG_WRITE_ENABLE);
	#endif
	// ---

   //init
   ret = -1; // -1 is error
   pos_destination_buffer = 0;

   include_file_ptr = fopen(file_name, "r");
   if (include_file_ptr != 0) // if file exists and readable
   {
      while (fgets(line, LINE_LENGTH, include_file_ptr) != NULL)
      {
         // append line to include_buffer

			// --- debug
			#ifdef debug_commands
				sprintf(msg,"line is: %s\n", line);
				log_write(msg, LOG_WRITE_ENABLE);
			#endif
			// ---

         pos_line = 0;
         while (pos_line < LINE_LENGTH && line[pos_line] != 0)
         {
            if(strlen(line) + pos_line >  destination_buffer_size) // aborting, file too big, does not fit in buffer
            {
               fclose (include_file_ptr);
               return ret;
            }
            *(destination_buffer + pos_destination_buffer) = *(line + pos_line);
            pos_destination_buffer++;
            pos_line++;
         }
         *(destination_buffer + pos_destination_buffer) = 0;

        // for(pos_line = 0; pos_line <; pos_line++)
      }
      fclose (include_file_ptr);

		// --- debug
		#ifdef debug_commands
			sprintf(msg,"Include found,  destination_buffer is: %s\n", destination_buffer);
			log_write(msg, LOG_WRITE_ENABLE);
		#endif
		// ---

      ret = pos_destination_buffer - 1;
   }
   return ret;
}
