// deditec_tcp_io.h

#define IP_ADDR_SERVER_SIZE 200


typedef struct XXTCP_IO
{
      unsigned long     tcp_handle_tx;
      unsigned long     tcp_handle_server_bind;
      unsigned long     tcp_handle_server_session;

      unsigned char     host_name[IP_ADDR_SERVER_SIZE]; // server name / IP
      unsigned char     rx_host[IP_ADDR_SERVER_SIZE];   // client name / IP
      unsigned long     host_name_size;
      unsigned long     port_no;

      unsigned long     tx_job_id;

      //unsigned char     rx_host[IP_ADDR_SERVER_SIZE];
      unsigned long     rx_port;

} TCP_IO;

extern unsigned long tcp_client_connect(TCP_IO        * tcp_params, unsigned char * host_name, unsigned long host_name_size,
                                        unsigned long   port_no,    unsigned long keep_alive);
extern unsigned long tcp_tx            (unsigned long tcp_handle/*TCP_IO        * tcp_params*/, unsigned char * tx_message, unsigned long tx_message_size);
extern unsigned long tcp_tx_echo       (TCP_IO        * tcp_params);
extern unsigned long tcp_disconnect    (TCP_IO        * tcp_params);

#ifdef __linux__  // server only functions
   extern unsigned long tcp_server_connect(TCP_IO  * tcp_params, unsigned char * host_name,  unsigned long port_no);
   extern unsigned long tcp_server_accept (TCP_IO  * tcp_params, unsigned char * rx_buffer,  unsigned int  rx_buffer_size);
   extern unsigned long tcp_tx_server     (TCP_IO  * tcp_params, unsigned char * tx_message, unsigned long tx_message_size);

   extern unsigned long get_eth_mac( unsigned char * buffer, unsigned long buffer_length, unsigned long eth_no);
#endif

