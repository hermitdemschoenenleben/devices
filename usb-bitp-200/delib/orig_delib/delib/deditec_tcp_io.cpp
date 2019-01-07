#include <stdio.h>
#include <string.h>        // bzero(), strlen(), strcpy(), bcopy()

#ifndef __linux__
   #include <winsock.h>    // socket(), send(), recv(), connect()
   //#include <io.h>
#else
   #include <strings.h>    // recv()
   #include <sys/socket.h> // socket(), send(), connect()
   #include <sys/types.h>  // socket(), send(), connect()
   #include <netinet/in.h> // htons(), defines struct sockaddr_in
   #include <netdb.h>
   #include <sys/ioctl.h>
   #include <net/if.h>
   #include <net/if.h>
	

#endif

#include <stdlib.h>        // exit()
#include <errno.h>
#include <signal.h>        // struct sigaction, SIGALARM

#include "deditec_tcp_io.h"
#include "deditec_tcp_functions.h"  // here, only for server needed
#include "deditec_string_utils.h"


#ifndef __linux__
   // LINKER: Add wsock32.lib
   #pragma comment(lib, "wsock32.lib ")
#endif

//#define debug_commands 1                     // if set, prints debug outputs
#define debug_print(x)  printf("%s", x)                  // belongs to upper line

//#define KEEP_ALIVE 1                         // switch tcp-mode to keepalive

///////////////////////////////////////////////////////////////////////////////////
// The aim of this base library is to communicate (send/receive bytes) over TCP/IP
//
// Most, if not all of the following functions require a structure of type TCP_IO
// as defined in deditec_tcp_io.h. This structure contains the parameters
// nessessary to configure your TCP/IP connection. Create a pointer in your main()
// (or wherever else you want to have it) pointing to this parameter structure.
// This will be used as argument for the functions.
//
//
// A Speed measurement is listed at the end of this file
//
// How to use those functions:
//
// As client:
//
// a) use tcp_client_connect() to create connection to server
// b) use tcp_tx() to send data to server/host
// c) use tcp_disconnect() to disconnect from socket
//
///////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// debug_print2():
//
// synopsis: helps to print debug outputs on stdout (not for customers)
//
// description:
//
// Function may be manually activated using the define constant debug_commands
// (see on top of this file). Where used, it will print out the function name
// (inserted manually) and a message like a simple print as debug message
///////////////////////////////////////////////////////////////////////////////

#ifndef DELIB_COMPILE
void debug_print2(char * msg1, char * msg2)
{
   char msgx[1024];

   strcpy (msgx, msg1);
   strcat (msgx, msg2);
   debug_print (msgx);
}
#else
extern void debug_print2(char *msg1, char *msg2);
#endif

///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// tcp_client_connect():
//
// synopsis: creates a handler and uses it to connect to a host. Data may be
//           sent After a successfull connection,  (with another function)
//
// description:
//
// Fuction creates a socket and connects to it. The aim is to use this function
// as client to connect to a server.
//
// arguments:
//
// tcp_params: - structure type TCP_IO (see deditec_tcp_io.h)
// host_name:  - under windows: host_name must be an IP-Addr and not a domain name
//               (this is due to avoiding function gethostbyname() making trouble while using)
//             - under linux: host_name may be either an IP-addr. or a domain name
// port_no:      port number of server to connect to
// keep_alive: - if set to 1, activates keep_alive connection, if set to 0, then ignored
//               (socket option; socket sends message in regular intervals to keep connection
//                up; it works on it's own, no send or receive function is called)
//
// Hint:
//  - a connection is to be terminated with function tcp_disconnect()
//  - tcp_client_connect() will only create one socket, no other socket can
//    be created until the first one is closed
//
// return errors:
//     0: ok, socket created and connected to it
//    -1: loading winsock 2.0 error (windows only)
//    -2: socket handler (file descriptor) could not be created (linux only)
//    -3: error, no such host, while resolving host_name (linux only)
//    -4: error connecting to socket, could not reach host
//    //-5: could not set desired keep_alive socket option
//        (not a fatal error, socket may still be used, better change this to pos. value !)
/////////////////////////////////////////////////////////////////////////////

unsigned long tcp_client_connect(TCP_IO        * tcp_params,
                                 unsigned char * host_name, unsigned long host_name_size,
                                 unsigned long   port_no,   unsigned long keep_alive)
{
   // --- debug
   #ifdef debug_commands
      char msg[200];
      char msg_pre[200];

      strcpy(msg_pre, "|DELIB|tcp_client_connect - ");

      sprintf(msg,"function begin\n");
      debug_print2(msg_pre, msg);
   #endif
   // ---

//	ULONG on;

   struct sockaddr_in serv_addr; // struct 'sockaddr_in' is defined in <netinet/in.h>  (see also below)
   #ifdef __linux__
      struct hostent *server; // used if compiled under linux
   #endif

   unsigned char host_name_cp[IP_ADDR_SERVER_SIZE];

   // -------------------------------------- init ----------------------------------
   #ifndef __linux__

      // hints about win socket library: see botom of file
      WSADATA wsaData;    // Structure for WinSock setup communication

      if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) // Load Winsock 2.0 DLL
      //if (WSAStartup(MAKEWORD(1, 2), &wsaData) != 0) // Load Winsock 1.2 DLL ???
      {
         /// --- debug
         #ifdef debug_commands
            sprintf(msg,"return is %d dec, loading windows socket (winsock) failed\n", -1);
            debug_print2(msg_pre, msg);
         #endif
         /// ---
         return (-1); // "WSAStartup() failed
      }
   #endif


   tcp_params->tcp_handle_tx = 0; // setting this to zero, grants tcp_client_connect() to create socket-handler

   strcpy_n(host_name_cp, IP_ADDR_SERVER_SIZE, host_name, host_name_size ); // const defined in header



   // -------------------------------------- begin ----------------------------------
   if (tcp_params->tcp_handle_tx == 0) // if zero, grants creating a socket, else not (then already created)
                                       // (permits only one socket during run time)
   {
      tcp_params->tcp_handle_tx = socket (AF_INET, SOCK_STREAM, 0); // create file descriptor (socket handle), on error, gives -1
//      tcp_params->tcp_handle_tx = socket (AF_LOCAL, SOCK_STREAM, 0); // create file descriptor (socket handle), on error, gives -1
      // arguments of socket(): AF_INET --> IPv4, SOCK_STREAM --> TCP (full-duplex byte stream), 0 = protocol
      if (tcp_params->tcp_handle_tx < 0)
      {
		  //printf("ERROR creating socket\n");
         /// --- debug
         #ifdef debug_commands
            sprintf(msg,"return is %d dec, creating linux-socket failed\n", -2);
            debug_print2(msg_pre, msg);
         #endif
         /// ---
         return -2; // could not create socket
      }

      // see end of file for a small description
      memset(&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero)); // fill sin_zero with NULLs; obligatory, see struct sockaddr_in
      serv_addr.sin_family = AF_INET;
//      serv_addr.sin_family = AF_LOCAL;
      serv_addr.sin_port = (unsigned short) htons((unsigned short) port_no); // set port no (host-byte-order to network-byte-order)

      #ifndef __linux__
         // unsigned long inet_addr(char* cp );
         serv_addr.sin_addr.s_addr = inet_addr((const char *) host_name_cp);   // works under windows
      #else
         server = gethostbyname((const char *) host_name_cp); // resolve IP from hostname (IP may also be given)
         if (server == NULL)
         {
		//	printf("ERROR creating socket\n");

            /// --- debug
            #ifdef debug_commands
               sprintf(msg,"return is %d dec, could not resolve host name\n", -3); // should not happen, if host_name_cp is IP-Addr.
               debug_print2(msg_pre, msg);
            #endif
            /// ---
            return (-3); // error, no such host
         }

         strcpy_n((unsigned char *)&serv_addr.sin_addr.s_addr, serv_addr.sin_addr.s_addr, // copy 32 bit IP-address
                  (unsigned char *)server->h_addr, server->h_length);
      #endif
   }

   // connecting to socket
   if (connect(tcp_params->tcp_handle_tx, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0)
   {
		printf("ERROR connect\n");

      /// --- debug
      #ifdef debug_commands
         sprintf(msg,"return is %d dec, failed connecting to server\n", -4);
         debug_print2(msg_pre, msg);
      #endif
      /// ---
      return (-4); // server not accessible, not running or is not responding, (timeout or busy ?)
   }
/*

  #ifdef KEEP_ALIVE


   // switch keepalive on
   int on = 1; //set keep_alive to on
   int optval;
   int optlen = sizeof(optval);

       printf("HELLO\n");
       printf("HELLO\n");
       printf("HELLO\n");
       printf("HELLO\n");

   if(getsockopt(tcp_params->tcp_handle_tx, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, &optlen) < 0)
    {
       printf("could not get socket options\n");
       //getchar();
    }
    printf("SO_KEEPALIVE value: %d\n", optval);
//    getchar();
//    // preventing socket disconnection due to inactivity using SO_KEEPALIVE
    if(setsockopt(tcp_params->tcp_handle_tx,SOL_SOCKET, SO_KEEPALIVE, (const char *) &on, sizeof(on)) < 0)
    {
       // error
       printf("could not set socket to KEEPALIVE mode\n");
    }
    if(getsockopt(tcp_params->tcp_handle_tx, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, &optlen) < 0)
    {
       printf("could not get socket options\n");
       //getchar(); 
    }
    printf("SO_KEEPALIVE value: %d\n", optval);
//    getchar();
   #endif
*/
   // --- debug
   #ifdef debug_commands
      sprintf(msg,"function end - return is %d dec\n", 0);
      debug_print2(msg_pre, msg);
   #endif
   // ---

   return 0; // ok
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// get_eth_mac():
//
// synopsis:
//
// function, that querries MAC-addr of given ethernet card no and writes
// it to given buffer; buffer must be at least 6 bytes long
//
//
// arguments:
//
// buffer_length: at least 6 char long (size of MAC-addr.)
// range of eth_no: 0..99 (no of ethernet card)
//
// return values:
//
//  0: on success
// -1: if error or eth-network card not found
// ----------------------------------------------------------------------------

#ifdef __linux__

unsigned long get_eth_mac( unsigned char * buffer, unsigned long buffer_length, unsigned long eth_no)
{
   int s;
   struct ifreq buffer_mac;
   unsigned char eth_string[6];
   unsigned char eth_no_string[3];
   unsigned long mac_addr_found;

//    // --- init
//    memset(buffer, 0x00, buffer_length);
//    memset(&buffer_mac, 0x00, sizeof(buffer_mac));
//    // ---

   if (buffer_length < 6 || eth_no > 99) return -1; // buffer to small or ethernet number out of range


   s = socket(PF_INET, SOCK_DGRAM, 0);
   if (s == -1) return -1;

    // --- create string "eth0", "eth1" ..."eth99"
   strcpy_n(eth_string, sizeof(eth_string), "eth", 3);
   snprintf(eth_no_string, sizeof(eth_no_string), "%d", (int) eth_no); // same as sprintf, but does not write outside of eth_no_string
   strcpy_n(eth_string + 3, sizeof(eth_string -3), eth_no_string, 3);
   // ---

   //printf("%s\n", eth_string);
   strcpy(buffer_mac.ifr_name, eth_string);

   ioctl(s, SIOCGIFHWADDR, &buffer_mac);

   close(s);

   mac_addr_found = 0;

   for( s = 0; s < 6; s++ )
   {
      //printf("%.2x ", (unsigned char) buffer_mac.ifr_hwaddr.sa_data[s]); // %.2x forces to display always 2 digits of a hex value (0F instead of F)
      *(buffer + s) = *(buffer_mac.ifr_hwaddr.sa_data +s); // charwise copy MAC-addr

      if ( *(buffer + s) != 0) mac_addr_found = 1;
      //printf(" -->%.2x ", buffer[s]);
   }

   //printf("\n");

   if (mac_addr_found == 1) return 0;
   else return -1;
}
#endif

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// tcp_tx():
//
// synopsis: send bytes over a TCP/IP connection
//
// arguments:
//
// tcp_params:      structure of type TCP_IO (see deditec_tcp_io.h)
// tx_buffer:      buffer of message to send
// tx_buffer_size: size of message to send
//
//
// return values:
//   0: no error, amount of bytes sent >= 0 (zero == empty message)
//  -1: mesage could not be sent, local error occured (and errno is set)
//
// Hint: a TCP connection to a server or client must exist before sending data
///////////////////////////////////////////////////////////////////////////////

// unsigned long tcp_tx(TCP_IO * tcp_params, unsigned char * tx_buffer, unsigned long tx_buffer_size)
// {
//    unsigned long i;
//    /// --- debug
//    #ifdef debug_commands
//       char msg[200];
//       char msg2[200];
//       char msg_pre[200];
//       strcpy(msg_pre, "|DELIB|tcp_tx - ");
//
//       sprintf(msg,"function start\n");
//       debug_print2(msg_pre, msg);
//    #endif
//    /// ---
//
//    signed long bytes_sent;
//    unsigned long ret = -1;
//
//    /// --- debug
//    #ifdef debug_commands
//       sprintf(msg, "now sending %d bytes to server\n", tx_buffer_size);
//       debug_print2(msg_pre, msg);
//       //sprintf(msg, "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \n", *(tx_buffer+0), *(tx_buffer+1), *(tx_buffer+2), *(tx_buffer+3), *(tx_buffer+4), *(tx_buffer+5), *(tx_buffer+6), *(tx_buffer+7), *(tx_buffer+8), *(tx_buffer+9), *(tx_buffer+10), *(tx_buffer+11), *(tx_buffer+12), *(tx_buffer+13), *(tx_buffer+14), *(tx_buffer+15));
// //      debug_print2(msg_pre, msg);
// 	sprintf(msg,"Sending: ");
// 	for(i=0;(i<tx_buffer_size) && (i<32);++i)
// 	{
// 		sprintf(msg2, "%x ", *(tx_buffer+i));
// 		strcat(msg, msg2);
// 	}
// 	sprintf(msg2, "\n");
// 	strcat(msg, msg2);
//     debug_print2(msg_pre, msg); // printout debug message
//
//    #endif
//    /// ---
//
//    //bytes_sent = send(tcp_params->tcp_handle_tx, "", 0, 0); // send zero bytes
//    bytes_sent = send(tcp_params->tcp_handle_tx, (const char *) tx_buffer, tx_buffer_size, 0);
//    // returns amount of sent bytes if ok, else -1 (errno is set apropriately)
//
//    ret = bytes_sent;
//
//    if (bytes_sent < 0) // return value is -1; error, message could not be sent
//    {
//       /// --- debug
//       #ifdef debug_commands
//          sprintf(msg,"return is: %d dec, message could not be sent\n", ret);
//          debug_print2(msg_pre, msg);
//       #endif
//       /// ---
//
//       return ret;
//    }
//
//    else
//    {
//       /// --- debug
//       #ifdef debug_commands
//          sprintf(msg,"function end - return is: %d dec\n", ret);
//          debug_print2(msg_pre, msg);
//       #endif
//       /// ---
//
//       return bytes_sent;
//    }
// }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// tcp_tx_echo(): is used to hold connection up and running !!! NOT USED !!!
//
// synopsis: hold socket connection to host up and running
//
// details: tcp_tx_zero() sends zero byte long tcp data to host, this will keep
//          the connection alive
//
//
// return values:
//     0: could reach host to send it zero bytes
//    -1: could not reach host, local error
///////////////////////////////////////////////////////////////////////////////
/*unsigned long tcp_tx_echo(TCP_IO * tcp_params)
{
   unsigned char bulk[1];    // function send expects an address, even if zero in length
   signed   long bytes_sent;

   bytes_sent = send(tcp_params->tcp_handle_tx, (const char *) &bulk, 0, 0); // send zero bytes

   if (bytes_sent < 0) return -1;
   return bytes_sent;
}
*/

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// tcp_disconnect():
//
// synopsis: closes file descriptor (TCP-handler)
//
// argument:
//
// tcp_params: structure of type TCP_IO (defined in deditec_tcp_io.h) containing
//             handle to be closed
//
// return values:
//   0: disconnection from handle successfull
//  -1: error while disconnecting (errno is set appropriately)
//////////////////////////////////////////////////////////////////////////////////

// unsigned long tcp_disconnect(TCP_IO * tcp_params)
// {
//    /// -- debug
//    #ifdef debug_commands
//       char msg[200];
//       char msg_pre[200];
//       strcpy(msg_pre, "|DELIB|tcp_tx - ");
//
//       sprintf(msg,"function start\n");
//       debug_print2(msg_pre, msg);
//    #endif
//    /// ---
//
//
//    unsigned long ret;
//    ret = 0;
//    if (tcp_params->tcp_handle_tx > 0) // TCP-handler exists, close it
//    {
//       #ifdef __linux__
//          ret = close(tcp_params->tcp_handle_tx);
//       #else // windows
//           ret = closesocket(tcp_params->tcp_handle_tx);
//           WSACleanup();  // Cleanup Winsock
//       #endif
//
//       if (ret == 0)
//       {
//          tcp_params->tcp_handle_tx = 0; // indicates beeing disconnected and ready for a new connect
//       }
//    }
//
//    /// -- debug
//    #ifdef debug_commands
//       sprintf(msg,"function end - return is %d dec\n", ret);
//       debug_print2(msg_pre, msg);
//    #endif
//    /// ---
//
//    return ret;
// }



// ---------------------------------- Apendix ------------------------------------
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

/*
   comments to understand better functionality of socket
   (source: http://www.pug.org/mediawiki/index.php/Einf%C3%BChrung_in_die_Netzwerkprogrammierung#struct_sockaddr_in)

   excerpt from <netinet/in.h>

   struct sockaddr_in {
   sa_familiy_t sin_familiy;  // AF_INET
   in_port_t    sin_port;     // 16-bit TCP or UDP port number
                             // network byte ordered
   struct       sin_addr;     // 32-bit IPv4 address
                             // network byte ordered
   char         sin_zero[8];
};
   */

//////////////////////////////////////////////////////////////////////////////////
// rough speed measurements:
//
// measure 1.)
// - Intel pentium 4 CPU, 1,7 Ghz, Ubunutu 8.04, localy (loopback device)
//
// - size of packet/loop: 100 bytes
// - time elapsed to echo 100 bytes back doing 20000 loops: 30 sec
// - result: 150 us for 100 bytes
//
//
// measure 2.)
// - Intel pentium 4 CPU, 1,7 Ghz, Ubunutu 8.04, localy (loopback device)
//
// - size of packet/loop: 10 bytes
// - time elapsed to echo 10 bytes back doing 20000 loops:  sec
// - result: 30 us for 10 bytes
//
//////////////////////////////////////////////////////////////////////////////////



/*hints about win socket library:

   16 bit win: use winsock.lib
   32 bit win and winsock 1.1 functionality: use wsock32.lib
   32 bit win and winsock 2   functionality: use ws2_32.lib

*/
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// tcp_disconnect():
//
// synopsis: closes file descriptor (TCP-handler)
//
// argument:
//
// tcp_params: structure of type TCP_IO (defined in deditec_tcp_io.h) containing
//             handle to be closed
//
// return values:
//   0: disconnection from handle successfull
//  -1: error while disconnecting (errno is set appropriately)
//////////////////////////////////////////////////////////////////////////////////

unsigned long tcp_disconnect(TCP_IO * tcp_params)
{
   /// -- debug
/*   #ifdef debug_commands 
      char msg[200];
      sprintf(msg,"tcp_tx - function start\n");
      log_write(msg, LOG_WRITE_ENABLE); // printout debug message
   #endif
*/   /// ---


   unsigned long ret;
   ret = 0;
   if (tcp_params->tcp_handle_tx > 0) // TCP-handler exists, close it
   {
      #ifdef __linux__
         ret = close(tcp_params->tcp_handle_tx);
      #else // windows
          ret = closesocket(tcp_params->tcp_handle_tx);
          WSACleanup();  // Cleanup Winsock
      #endif

      if (ret == 0)
      {
         tcp_params->tcp_handle_tx = 0; // indicates beeing disconnected and ready for a new connect
      }
   }

   /// -- debug
/*   #ifdef debug_commands 
      sprintf(msg,"function end - return is %d dec\n", ret);
      log_write(msg, LOG_WRITE_ENABLE); // printout debug message
   #endif
*/   /// ---

   return ret;
}



// ---------------------------------- Apendix ------------------------------------
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

/*
   comments to understand better functionality of socket
   (source: http://www.pug.org/mediawiki/index.php/Einf%C3%BChrung_in_die_Netzwerkprogrammierung#struct_sockaddr_in)

   excerpt from <netinet/in.h>

   struct sockaddr_in {
   sa_familiy_t sin_familiy;  // AF_INET
   in_port_t    sin_port;     // 16-bit TCP or UDP port number
                             // network byte ordered
   struct       sin_addr;     // 32-bit IPv4 address
                             // network byte ordered
   char         sin_zero[8];
};
   */

//////////////////////////////////////////////////////////////////////////////////
// rough speed measurements:
//
// measure 1.)
// - Intel pentium 4 CPU, 1,7 Ghz, Ubunutu 8.04, localy (loopback device)
//
// - size of packet/loop: 100 bytes
// - time elapsed to echo 100 bytes back doing 20000 loops: 30 sec
// - result: 150 us for 100 bytes
//
//
// measure 2.)
// - Intel pentium 4 CPU, 1,7 Ghz, Ubunutu 8.04, localy (loopback device)
//
// - size of packet/loop: 10 bytes
// - time elapsed to echo 10 bytes back doing 20000 loops:  sec
// - result: 30 us for 10 bytes
//
//////////////////////////////////////////////////////////////////////////////////



/*hints about win socket library:

   16 bit win: use winsock.lib
   32 bit win and winsock 1.1 functionality: use wsock32.lib
   32 bit win and winsock 2   functionality: use ws2_32.lib

*/


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// tcp_tx():
//
// synopsis: send bytes over a TCP/IP connection
//
// arguments:
//
// tcp_params:      structure of type TCP_IO (see deditec_tcp_io.h)
// tx_buffer:      buffer of message to send
// tx_buffer_size: size of message to send
//
//
// return values:
//   0: no error, amount of bytes sent >= 0 (zero == empty message)
//  -1: mesage could not be sent, local error occured (and errno is set)
//
// Hint: a TCP connection to a server or client must exist before sending data
///////////////////////////////////////////////////////////////////////////////

unsigned long tcp_tx(unsigned long tcp_handle/*TCP_IO * tcp_params*/, unsigned char * tx_buffer, unsigned long tx_buffer_size)
{
//   unsigned long i;
   /// --- debug
//      char msg[200];
//      char msg2[200];
/*   #ifdef debug_commands 

      sprintf(msg,"tcp_tx - function start\n");
      log_write(msg, LOG_WRITE_ENABLE); // printout debug message
   #endif
   /// ---
*/
   signed long bytes_sent;
   unsigned long ret = -1;

   /// --- debug
   #ifdef debug_commands
      //sprintf(msg, "now sending %d bytes to server\n", tx_buffer_size);
      //log_write(msg, LOG_WRITE_ENABLE); // printout debug message
      //sprintf(msg, "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \n", *(tx_buffer+0), *(tx_buffer+1), *(tx_buffer+2), *(tx_buffer+3), *(tx_buffer+4), *(tx_buffer+5), *(tx_buffer+6), *(tx_buffer+7), *(tx_buffer+8), *(tx_buffer+9), *(tx_buffer+10), *(tx_buffer+11), *(tx_buffer+12), *(tx_buffer+13), *(tx_buffer+14), *(tx_buffer+15));
		//debug_print2(msg_pre, msg);
	sprintf(msg,"Sending: ");
	for(i=0;(i<tx_buffer_size) && (i<32);++i)
	{
		sprintf(msg2, "%x ", *(tx_buffer+i));
		strcat(msg, msg2);
	}
	//sprintf(msg2, "\n");
	//strcat(msg, msg2);
    //log_write(msg, LOG_WRITE_ENABLE); // printout debug message

   #endif
   /// ---

   //bytes_sent = send(tcp_params->tcp_handle_tx, "", 0, 0); // send zero bytes
   bytes_sent = send(tcp_handle/*tcp_params->tcp_handle_tx*/, (const char *) tx_buffer, tx_buffer_size, 0);
   // returns amount of sent bytes if ok, else -1 (errno is set apropriately)

   ret = bytes_sent;

   if (bytes_sent < 0) // return value is -1; error, message could not be sent
   {
      /// --- debug
      /*#ifdef debug_commands 
         sprintf(msg,"return is: %d dec, message could not be sent\n", ret);
         log_write(msg, LOG_WRITE_ENABLE); // printout debug message
      #endif*/
      /// ---

      return ret;
   }

   else
   {
      /// --- debug
/*      #ifdef debug_commands 
         sprintf(msg,"function end - return is: %d dec\n", ret);
         log_write(msg, LOG_WRITE_ENABLE); // printout debug message
      #endif
  */    /// ---

      return bytes_sent;
   }   
}


