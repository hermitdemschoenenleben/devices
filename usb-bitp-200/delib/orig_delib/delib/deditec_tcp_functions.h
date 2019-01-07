
#define TX_BUFFER_CP_LENGTH 1000

extern unsigned long tcp_tx_open         (unsigned char * host_name, unsigned long host_name_size, unsigned long port_no);
extern unsigned long tcp_tx_send_rec_data(unsigned long   handle, unsigned long job_id, unsigned long command_type,
                                          unsigned char * tx_buff,   unsigned long tx_buff_length,
                                          unsigned char * rx_buffer, unsigned long rx_buffer_length);
extern unsigned long tcp_tx_close        (unsigned long   handle);

//extern unsigned long tcp_keep_connected  (TCP_IO * tcp_params);
