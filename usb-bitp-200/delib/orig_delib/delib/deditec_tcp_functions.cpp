#include <stdio.h>
#include <string.h>  // bzero(), strlen(), strcpy(), bcopy()

#ifndef __linux__
   #include <winsock.h> ////////// added test
#else
   #include <sys/socket.h> // socket(), send(), connect()
   #include <netinet/in.h> // htons(), defines struct sockaddr_in
   #include <netdb.h>
   #include <sys/types.h>  // socket(), send(), connect()
#endif

#include <stdlib.h>  // exit()
#include "deditec_tcp_io.h"
#include "deditec_string_utils.h"
#include "deditec_tcp_functions.h"
#include "deditec_string_utils.h"

#include "dt_embedded_parameter.h"

//#include "/library/vc/js_timer.h"

//#include "conio.h"

#define BUFF_TCP_RECV_SIZE 16384  // better don't touch, used by read() and recv()

//#define debug_commands 1                     // if set, prints debug outputs


extern void debug_print2(char *msg1, char *msg2);

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// tcp_tx_open():
//
// synopsis: creates a tcp write/read handler and connects to a host
//
// description:
//
// This function is the first of 3 functions to communicate to a server. It
// prepares all the nessessary steps for the main function tcp_tx_send_rec_data().
// Data may be sent/received through function tcp_tx_send_rec_data().
// To terminate communication, use function tcp_tx_close().
//
// arguments:
//
// host_name:
// - under windows: host_name must be an IP-Addr and not a domain name
// (this is due to avoiding function gethostbyname() making trouble while using)
// - under linux: host_name may be either an IP-Addr or a domain name
//
// host_name_size: size of host_name array
//
// return errors:
//    tcp_params: no error, returns structure (defined in dedited.tcp_io.h, TCP_IO)
//    -1: loading winsock 2.0 error
//    -2: socket handler could not be created
//    -3: error, no such host, while resolving host_name (linux only, resolving name under windows not implemented)
//    -4: error connecting to socket, could not reach host
//    -5: error allocating memory, failed
//    -6: IP_ADDR_SERVER_SIZE (buffer) too small
///////////////////////////////////////////////////////////////////////////////

unsigned long tcp_tx_open(unsigned char * host_name, unsigned long host_name_size, unsigned long port_no)
{
   /// --- debug
	   char msg[200];
   #ifdef debug_commands
	   char msg_pre[200];

	   strcpy(msg_pre, "|DELIB|tcp_tx_open - ");
	   sprintf(msg,"function beginXXXXXXXXXXXX\n");
	   debug_print2(msg_pre, msg);
   #endif
   /// ---

   TCP_IO * tcp_params; // creates global pointer, pointing to a structure of type TCP_IO (see deditec_tcp_io.h)
   tcp_params = (TCP_IO *) malloc(sizeof(TCP_IO)); // allocate ram space for struct TCP_IO beginning at pointer
   if (tcp_params == 0)
   {
      /// --- debug
      #ifdef debug_commands
         sprintf(msg,"return value is %d dec \n", -5);
	      debug_print2(msg_pre, msg);
      #endif
      /// ---

      return -5; // error allocating memory, failed
   }
   unsigned long ret;

   if ( buffcpy_n(tcp_params->host_name, IP_ADDR_SERVER_SIZE, (unsigned char *) host_name, host_name_size) < 0) return -6; // dest_buffer too small
   tcp_params->host_name_size = host_name_size;
   tcp_params->port_no = port_no;

   /// --- debug
    sprintf(msg,"tcp_params = 0x%lx\n", (unsigned long) tcp_params);//Sonst gehts unter Linux nicht ....
   #ifdef debug_commands
      sprintf(msg,"tcp_params = 0x%x\n", (unsigned long) tcp_params);
	   debug_print2(msg_pre, msg);
      sprintf(msg,"using IP: %s\n", tcp_params->host_name);
	   debug_print2(msg_pre, msg);
      sprintf(msg,"using port: %d\n", tcp_params->port_no);
	   debug_print2(msg_pre, msg);
   #endif
   /// ---

   ret = tcp_client_connect(tcp_params, host_name, host_name_size, tcp_params->port_no, 1); // returns pointer to tcp_params or neg. error code

   if (ret==-1 || ret==-2 || ret==-3 || ret==-4 || ret==-5|| ret==-6) // socket error
   {
      /// ---
      #ifdef debug_commands
         sprintf(msg, "socket error - return is %d dec\n", ret);
         debug_print2(msg_pre, msg);
      #endif
      /// ---

      return ret;
   }

   /// --- debug
   #ifdef debug_commands
      sprintf(msg,"tcp_params->tcp_handle_tx = %d dez\n", tcp_params->tcp_handle_tx);
      debug_print2(msg_pre, msg);
      sprintf(msg,"function end - return value: 0x%x\n", ret);
      debug_print2(msg_pre, msg);
   #endif
   /// ---

   return (unsigned long) tcp_params; // returns pointer to tcp_params
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// tcp_tx_send_rec_data():
//
// synopsis: send data to host (server) and expect to receive a response
//
// description:
//
// Function is used to send messages to host (server) and expects to receive
// for each sent message a response from server.
// Each message begins with a 3 byte header. The first byte is the job ID, the
// next 2 bytes describes the length of data (non redundand data).
//
// For each message received, the server has to respond to it using the same job ID
//
// arguments:
//
// handle:    socket handler (file descriptor)
// tx_buffer: transmit message buffer (to be sent to server)
// rx_buffer: response message buffer (received from server)
//
// return values:
//  >=0: no error, amount bytes received
//   -1: socket error           // failed to receive message from server
//   -2: buffer of TX_BUFFER_CP_LENGTH too small
//   -3: failed to receive message from server
//   -4: buffer of TX_BUFFER_CP_LENGTH too small
//   -5: rx_buffer too small
// ---> test or debug purpose: sleep(1) in function (line 243)
///////////////////////////////////////////////////////////////////////////////////

unsigned long tcp_tx_send_rec_data(unsigned long   tcp_handle_tx, unsigned long job_id, unsigned long command_type,
                                   unsigned char * tx_buffer, unsigned long tx_buffer_length,
                                   unsigned char * rx_buffer, unsigned long rx_buffer_length)
{
	/// --- debug
	#ifdef debug_commands
	char msg[200];
	char msg2[200];
	char msg_pre[200];

	strcpy(msg_pre, "|DELIB|");
	sprintf(msg,"--------------------tcp_tx_send_rec_data(handle=0x%x, tx_buff_len=%ddez, rx_buff_len=%ddez)\n", tcp_handle_tx, tx_buffer_length, rx_buffer_length);
	debug_print2(msg_pre, msg);
	strcpy(msg_pre, "|DELIB|tcp_tx_send_rec_data - ");
	#endif
	/// ---

	unsigned char tx_buffer_cp[TX_BUFFER_CP_LENGTH];
	unsigned char recv_buffer[BUFF_TCP_RECV_SIZE];

	unsigned long amount_bytes_sent;
	signed   long amount_bytes_received;

	unsigned long tx_buffer_cnt;

	signed   long j; // counter; must be unsigned, reason: j is used to compare to return value too
	unsigned long i;

	unsigned long ret;
	//   unsigned long counter_ok;
	//   unsigned long counter_error;
	//signed   long connect_status; // must be signed, else if (arg) does not work !!!

	// -----------------------   init  -----------------------------------------
	//counter_ok = 0;
	//counter_error = 0;

	//tx_buffer_cnt = 0; das wird später auf null gesetzt !
	//tx_buffer_cp[0] = 0;  - WOZU ????
	amount_bytes_received = 0;







	//TCP_IO * tcp_params;
	//tcp_params = (TCP_IO *) handle; // typecasting pointer handle to pointer (of structure) TCP_IO (see deditec_tcp_io.h)
	
	//tcp_params = DapiHandle->TCPHandle;

	/// ---
	#ifdef debug_commands
	sprintf(msg,"tcp_handle_tx = %d dez\n", (unsigned long) tcp_handle_tx);
	debug_print2(msg_pre, msg);
	#endif
	/// ---  

//	if (tcp_params->tcp_handle_tx < 0) return -1; // return socket error
	if (tcp_handle_tx < 0) return -1; // return socket error

   //++tcp_params->tx_job_id; // increase job_id 

   // ---- creating message header (3 bytes for ID's, 1 byte for job_id, 2 bytes for message length-> 6 bytes total)
   tx_buffer_cnt = 0;

   // now 3 bytes for DEDITEC ID and the command-id
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) DEDITEC_TCP_PACKET_ID_0;
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) DEDITEC_TCP_PACKET_ID_1;
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) command_type;//DEDITEC_TCP_PACKET_ID_2_CMD_RO_1;

   // now first byte for job_id
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) job_id;

   // now 2 bytes for length of receive buffer
   tx_buffer_cp[tx_buffer_cnt++] = ((unsigned char) (tx_buffer_length+6) >> 8) & 255; // rotate by 8 bits and mask upper 8 bits
   tx_buffer_cp[tx_buffer_cnt++] =  (unsigned char) (tx_buffer_length+6) & 255;       // mask lower 8 bits

   ret = buffcpy_n( tx_buffer_cp + 6,  TX_BUFFER_CP_LENGTH - 6,
                    tx_buffer,         tx_buffer_length        );

 //  ret = buffcpy_n( tx_buffer,         tx_buffer_length  , 
//					tx_buffer_cp + 6,  TX_BUFFER_CP_LENGTH - 3);

   if (ret != 0)
   {
      /// ---
      #ifdef debug_commands
         sprintf(msg, "buffer too small return is %d dec\n", -2);
         debug_print2(msg_pre, msg);
      #endif
      /// ---  
      return -2;
   }

   tx_buffer_cnt += tx_buffer_length;
   // ----


   //sleep(0);


   amount_bytes_sent = tcp_tx(tcp_handle_tx, tx_buffer_cp, tx_buffer_cnt); // sending message

   #ifdef debug_commands
      sprintf(msg, "tcp_tx returns: %d dez bytes sent\n", amount_bytes_sent);
      debug_print2(msg_pre, msg);
      //for ( j = 0; j != amount_bytes_sent; j++) // filling up with bytes
      //{
       //  printf("0x%x ", tx_buffer_cp[j]);
      //}
      //printf("\n");
   #endif

   #ifdef __linux__
      amount_bytes_received = read(tcp_handle_tx, recv_buffer + amount_bytes_received, BUFF_TCP_RECV_SIZE);
   #else
      /*
      int recv ( SOCKET s, char FAR* buf, int len, int flags);

      -s:     Socket ï¿½ber den wir Daten empfangen wollen
      -buf:   Pointer zu einem Buffer in dem die empfangenen Daten gespeichert werden sollen
      -len:   Grï¿½sse des Buffers (oder wieviele Daten im Buffer gespeichert werden sollen)
      -flags: benï¿½tigen wir nicht
      */

      //amount_bytes_received = recv(tcp_params->tcp_handle_tx, (char *) rx_buffer + amount_bytes_received, BUFF_TCP_RECV_SIZE, 0);
      amount_bytes_received = recv(tcp_handle_tx, (char *) recv_buffer , BUFF_TCP_RECV_SIZE, 0);
   #endif


   #ifdef debug_commands
   sprintf(msg, "recv returns %d dez\n", amount_bytes_received);
   debug_print2(msg_pre, msg);


	sprintf(msg,"Received: ");
	for(i=0; (i < amount_bytes_received) && (i < 32); ++i)
	{
		sprintf(msg2, "%x ", * (recv_buffer + i));
		strcat(msg, msg2);
	}
	sprintf(msg2, "\n");
	strcat(msg, msg2);
    debug_print2(msg_pre, msg); // printout debug message


   #endif
 
   if (amount_bytes_received < 0) 
   {
      /// ---
		#ifdef debug_commands
		sprintf(msg, "recv return <0 (ret-value = %d) - function end - return is -1\n", amount_bytes_received);
		debug_print2(msg_pre, msg);
		#endif
      /// ---

		return -3; // failed to receive message from server
   }

   if (amount_bytes_received > 0)  // seems possible to receive an empty tcp-packet
   {

   #ifdef debug_commands
   sprintf(msg, "vor buff_cpy\n");
   debug_print2(msg_pre, msg);
   #endif
      ret = buffcpy_n(rx_buffer, rx_buffer_length, recv_buffer+7, amount_bytes_received-7); // -1 if rx_buffer too small
   #ifdef debug_commands
   sprintf(msg, "nach buff_cpy\n");
   debug_print2(msg_pre, msg);
   #endif
      if (ret < 0)
      {
         /// ---
         #ifdef debug_commands
            sprintf(msg, "rx_buffer too small return is %d dec\n", -5);
            debug_print2(msg_pre, msg);
         #endif
         /// ---  
         return -5;
      }

   }

   #ifdef debug_commands
      sprintf(msg, "bytes rcv (%d bytes): ", amount_bytes_received);
      debug_print2(msg_pre, msg);
      for ( j = 0; j != amount_bytes_received; j++) // filling up with bytes
      {
         //printf("0x%x ", rx_buffer[j]);
      }
      //printf("\n");
   #endif

   // to debug: print full sent and received message
   /*
   if (strcmp(tx_buffer_cp, rx_buffer) == 0) counter_ok = 1;
   else counter_error = 1;

   printf("badly recv echo msg: %d\n", counter_error);
   printf("ident recv messages: %d\n", counter_ok);

   counter_ok = 0;
   counter_error = 0;
   if (tx_buffer_cp[0] == rx_buffer[0]) ++counter_ok;
   else ++counter_error;

   printf("badly recv job_id: %d\n", counter_error);
   printf("ident recv job_id: %d\n", counter_ok);
   */

   // after job_id and length check, remove header in message
/*   ret = buffcpy_n( rx_buffer,        rx_buffer_length,
                   (recv_buffer + 3), TX_BUFFER_CP_LENGTH - 3 );
   if (ret != 0)
   {
      /// ---
      #ifdef debug_commands
         sprintf(msg, "buffer too small return is %d dec\n", -2);
         debug_print2(msg_pre, msg);
      #endif
      /// ---  
      return -4;
   }
*/
 
   /// ---
   #ifdef debug_commands
      sprintf(msg, "return is %d dec\n", amount_bytes_received -3);
      debug_print2(msg_pre, msg);
   #endif
   /// ---

   return amount_bytes_received - 7;
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// tcp_tx_close()
//
// synopsis: closes handle and returns result of it
//
// return value:
//   0: no error
//  -1: closing handle error
///////////////////////////////////////////////////////////////////////////////////
unsigned long tcp_tx_close(unsigned long handle)
{
   /// -- debug
   #ifdef debug_commands 
      char msg[200];
      char msg_pre[200];
      strcpy(msg_pre, "|DELIB|tcp_tx_close - ");

      sprintf(msg,"function start\n");
      debug_print2(msg_pre, msg);
   #endif
   /// ---
   
   TCP_IO * tcp_params;
   unsigned int ret;

   tcp_params = (TCP_IO *) handle; // creates pointer, pointing to a structure of type TCP_IO (see tcp_io.h)

   ret = tcp_disconnect(tcp_params);

   free(tcp_params);

   /// -- debug
   #ifdef debug_commands 
      sprintf(msg,"function end - return is %d dec\n", ret);
      debug_print2(msg_pre, msg);
   #endif
   /// ---

   return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// tcp_rx_close_session()    // server only
//
// synopsis: closes handle and returns result of it
//
// return value:
//   0: no error
//  -1: closing handle error
///////////////////////////////////////////////////////////////////////////////////

#ifdef __linux__
   unsigned long tcp_rx_close_session(unsigned long socket_file_descr) //closing server session
   {
      //unsigned int ret;

      return close(socket_file_descr);

      //return ret;
   }
#endif






unsigned long tcp_tx_send_rec_data_big(unsigned long   handle,
                                   unsigned char * tx_buffer, unsigned long tx_buffer_length,
                                   unsigned char * rx_buffer, unsigned long rx_buffer_length)
{
   /// --- debug
   #ifdef debug_commands
	   char msg[200];
	   char msg2[200];
	   char msg_pre[200];

	   strcpy(msg_pre, "|DELIB|");
	   sprintf(msg,"--------------------tcp_tx_send_rec_data(handle=0x%x, tx_buff_len=%ddez, rx_buff_len=%ddez)\n", handle, tx_buffer_length, rx_buffer_length);
	   debug_print2(msg_pre, msg);
	   strcpy(msg_pre, "|DELIB|tcp_tx_send_rec_data - ");
   #endif
   /// ---

	   printf("BIG\n");
	   unsigned char *buffer;
   unsigned char tx_buffer_cp[TX_BUFFER_CP_LENGTH];
   unsigned char recv_buffer[BUFF_TCP_RECV_SIZE];	// wir für LINUX benötigt !!!

   unsigned long amount_bytes_sent;
   signed   long amount_bytes_received;

   unsigned long tx_buffer_cnt;

//   signed   long j; // counter; must be unsigned, reason: j is used to compare to return value too
	unsigned long i;

//   unsigned long ret;
   unsigned long b;
   unsigned long cnt;
//   unsigned long counter_ok;
//   unsigned long counter_error;
   //signed   long connect_status; // must be signed, else if (arg) does not work !!!

   // -----------------------   init  -----------------------------------------
   //counter_ok = 0;
   //counter_error = 0;

   //tx_buffer_cnt = 0; das wird später auf null gesetzt !
   //tx_buffer_cp[0] = 0;  - WOZU ????
   amount_bytes_received = 0;

   TCP_IO * tcp_params;
   tcp_params = (TCP_IO *) handle; // typecasting pointer handle to pointer (of structure) TCP_IO (see deditec_tcp_io.h)

   /// ---
   #ifdef debug_commands
      sprintf(msg,"tcp_params->tcp_handle_tx = %d dez\n", tcp_params->tcp_handle_tx);
      debug_print2(msg_pre, msg);
   #endif
   /// ---  

if (tcp_params->tcp_handle_tx < 0) return -1; // return socket error

   ++tcp_params->tx_job_id; // increase job_id 


   // ---- creating message header (3 bytes for ID's, 1 byte for job_id, 2 bytes for message length-> 6 bytes total)
   tx_buffer_cnt = 0;

   // now 3 bytes for DEDITEC ID and the command-id
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) DEDITEC_TCP_PACKET_ID_0;
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) DEDITEC_TCP_PACKET_ID_1;
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) DEDITEC_TCP_PACKET_ID_2_CMD_TESTDATA;

   // now first byte for job_id
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) tcp_params->tx_job_id;
#define BIG_BUFFER 100*1024*1024
   // now 4 bytes for length of receive buffer
   rx_buffer_length = BIG_BUFFER;
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) ((rx_buffer_length >> 24) & 255);
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) ((rx_buffer_length >> 16) & 255);
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) ((rx_buffer_length >> 8 ) & 255);
   tx_buffer_cp[tx_buffer_cnt++] = (unsigned char) ((rx_buffer_length      ) & 255);

//   ret = buffcpy_n( tx_buffer_cp + 6,  TX_BUFFER_CP_LENGTH - 6,
  //                  tx_buffer,         tx_buffer_length        );

 //  ret = buffcpy_n( tx_buffer,         tx_buffer_length  , 
//					tx_buffer_cp + 6,  TX_BUFFER_CP_LENGTH - 3);

  /* if (ret != 0)
   {
      /// ---
      #ifdef debug_commands
         sprintf(msg, "buffer too small return is %d dec\n", -2);
         debug_print2(msg_pre, msg);
      #endif
      /// ---  
      return -2;
   }*/

   //tx_buffer_cnt += tx_buffer_length;
   // ----


   //sleep(0);
printf("Sende %ld bytes\n",tx_buffer_cnt);

   amount_bytes_sent = tcp_tx(tcp_params->tcp_handle_tx, tx_buffer_cp, tx_buffer_cnt); // sending message

   #ifdef debug_commands
      sprintf(msg, "tcp_tx returns: %d dez bytes sent\n", amount_bytes_sent);
      debug_print2(msg_pre, msg);
      //for ( j = 0; j != amount_bytes_sent; j++) // filling up with bytes
      //{
       //  printf("0x%x ", tx_buffer_cp[j]);
      //}
      //printf("\n");
   #endif


	buffer = (unsigned char*) malloc(1024*1024);

	if(buffer==0) printf("MALLOC ERROR\n");

   #ifdef __linux__
      amount_bytes_received = read(tcp_params->tcp_handle_tx, recv_buffer + amount_bytes_received, BUFF_TCP_RECV_SIZE);
   #else
      /*
      int recv ( SOCKET s, char FAR* buf, int len, int flags);

      -s:     Socket ï¿½ber den wir Daten empfangen wollen
      -buf:   Pointer zu einem Buffer in dem die empfangenen Daten gespeichert werden sollen
      -len:   Grï¿½sse des Buffers (oder wieviele Daten im Buffer gespeichert werden sollen)
      -flags: benï¿½tigen wir nicht
      */

      //amount_bytes_received = recv(tcp_params->tcp_handle_tx, (char *) rx_buffer + amount_bytes_received, BUFF_TCP_RECV_SIZE, 0);
printf("Starte Daten empfangen\n");



      amount_bytes_received = recv(tcp_params->tcp_handle_tx, (char *) buffer , 1024*1024, 0);
	  printf("bytes received = %d\n", amount_bytes_received );

	  ULONG anz=0;
	  JS_Timerus_start();
	  ULONG ppos=0;
	do 
	{
		amount_bytes_received = recv(tcp_params->tcp_handle_tx, (char *) buffer , 1024*1024, 0);
		anz+=amount_bytes_received;
		printf("bytes received = %d anz=%d\n", amount_bytes_received , anz);


		// Check Test-Data
		for(i=0;i!=(unsigned int) amount_bytes_received;++i)
		{
			cnt=ppos>>2;
			if((ppos&3)==0) b=(cnt>>0)&255;
			if((ppos&3)==1) b=(cnt>>8)&255;
			if((ppos&3)==2) b=(cnt>>16)&255;
			if((ppos&3)==3) b=(cnt>>24)&255;


			if(b != *(buffer + i)) 
			{
				printf("Data not equal: ppos=%d  b=%x  *buffer=%x\n", ppos, b, *(buffer+i));
				//getch();
			}
			++ppos;
		}



	} while (anz<BIG_BUFFER);


	ULONG xx=JS_Timerus_diff();

	printf("Timer=%d   sekunden=%f\n", xx, ((float) xx/1000.0F/1000.0F));


   #endif


   #ifdef debug_commands
   sprintf(msg, "recv returns %d dez\n", amount_bytes_received);
   debug_print2(msg_pre, msg);


	sprintf(msg,"Received: ");
	for(i=0; (i < amount_bytes_received) && (i < 32); ++i)
	{
		sprintf(msg2, "%x ", * (recv_buffer + i));
		strcat(msg, msg2);
	}
	sprintf(msg2, "\n");
	strcat(msg, msg2);
    debug_print2(msg_pre, msg); // printout debug message


   #endif
 
   if (amount_bytes_received < 0) 
   {
      /// ---
		#ifdef debug_commands
		sprintf(msg, "recv return <0 (ret-value = %d) - function end - return is -1\n", amount_bytes_received);
		debug_print2(msg_pre, msg);
		#endif
      /// ---

		return -3; // failed to receive message from server
   }

/*   if (amount_bytes_received > 0)  // seems possible to receive an empty tcp-packet
   {

   #ifdef debug_commands
   sprintf(msg, "vor buff_cpy\n");
   debug_print2(msg_pre, msg);
   #endif
      ret = buffcpy_n(rx_buffer, rx_buffer_length, recv_buffer+7, amount_bytes_received-7); // -1 if rx_buffer too small
   #ifdef debug_commands
   sprintf(msg, "nach buff_cpy\n");
   debug_print2(msg_pre, msg);
   #endif
      if (ret < 0)
      {
         /// ---
         #ifdef debug_commands
            sprintf(msg, "rx_buffer too small return is %d dec\n", -5);
            debug_print2(msg_pre, msg);
         #endif
         /// ---  
         return -5;
      }

   }

   #ifdef debug_commands
      sprintf(msg, "bytes rcv (%d bytes): ", amount_bytes_received);
      debug_print2(msg_pre, msg);
      for ( j = 0; j != amount_bytes_received; j++) // filling up with bytes
      {
         //printf("0x%x ", rx_buffer[j]);
      }
      //printf("\n");
   #endif
*/
   // to debug: print full sent and received message
   /*
   if (strcmp(tx_buffer_cp, rx_buffer) == 0) counter_ok = 1;
   else counter_error = 1;

   printf("badly recv echo msg: %d\n", counter_error);
   printf("ident recv messages: %d\n", counter_ok);

   counter_ok = 0;
   counter_error = 0;
   if (tx_buffer_cp[0] == rx_buffer[0]) ++counter_ok;
   else ++counter_error;

   printf("badly recv job_id: %d\n", counter_error);
   printf("ident recv job_id: %d\n", counter_ok);
   */

   // after job_id and length check, remove header in message
/*   ret = buffcpy_n( rx_buffer,        rx_buffer_length,
                   (recv_buffer + 3), TX_BUFFER_CP_LENGTH - 3 );
   if (ret != 0)
   {
      /// ---
      #ifdef debug_commands
         sprintf(msg, "buffer too small return is %d dec\n", -2);
         debug_print2(msg_pre, msg);
      #endif
      /// ---  
      return -4;
   }
*/
 
   /// ---
   #ifdef debug_commands
      sprintf(msg, "return is %d dec\n", amount_bytes_received -3);
      debug_print2(msg_pre, msg);
   #endif
   /// ---

   return amount_bytes_received - 7;
}

