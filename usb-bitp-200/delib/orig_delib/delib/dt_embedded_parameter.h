


#define ACTUALIZE_ETH0_AND_AUTOSTARTUP		"/deditec/bin/ip_address_set_from_parameter_io > /deditec_logs/output_from_ip_address_set_from_parameter_io"
#define ACTUALIZE_FIRMWARE_DEFAULT			"/firmware/arm9_upd_fw_default.sh > /deditec_logs/output_from_firmware_default"
#define ACTUALIZE_FIRMWARE_UPDATE			"/firmware/arm9_upd_fw_update.sh > //deditec_logs/output_from_firmware_update"


// ----------------------------------------------------------------------------
#define DIRNAME_ROOT_DIR_HTML				"/deditec/www/"          // don't forget to add a '/' at end

// ----------------------------------------------------------------------------
#define FILENAME_DEFAULT_OPEN_HTML			"index.html"
#define FILENAME_PARAMETER_IO				"/deditec/deditec.cfg"
#define FILENAME_LOGFILE					"/deditec_logs/deditec.log"
#define FILENAME_LOG_WEBCMD_POST			"/deditec_logs/webcmd_post.log"
#define FILENAME_LOG_WEBCMD_POST_EXTRACT	"/deditec_logs/webcmd_post_extract.log"
#define FILENAME_LOG_WEBCMD_SEND			"/deditec_logs/webcmd_send_to_browser.log"

#define FILENAME_LOG_WEBUPD_POST			"/deditec_logs/webupd_post.log"
#define FILENAME_LOG_WEBUPD_POST_EXTRACT	"/deditec_logs/webupd_post_extract.log"
#define FILENAME_LOG_WEBUPD_SEND			"/deditec_logs/webupd_send_to_browser.log"

#define FILENAME_FIRMWARE_UPDATE_FILE		"/firmware/update/UPDATE_FIRMWARE.tar"
#define FILENAME_FIRMWARE_UPDATE_FILE_PC	"ro_cpu_eth_fw.dfw"

#define FILE_LOG_MAX_SIZE					1024*1024


// ----------------------------------------------------------------------------
#define DEDITEC_EMBEDDED_BC_PORT			9912
#define DEDITEC_DEFAULT_TCP_PORT_NO			9912


// ----------------------------------------------------------------------------
// TCP TX and RX ID's
#define DEDITEC_TCP_PACKET_ID_0							0x63
#define DEDITEC_TCP_PACKET_ID_1							0x9a
#define DEDITEC_TCP_PACKET_ID_2_CMD_RO_1				0x01
#define DEDITEC_TCP_PACKET_ID_2_CMD_TESTDATA			0x02
#define DEDITEC_TCP_PACKET_ID_2_CMD_SEND_FILE			0x03
#define DEDITEC_TCP_PACKET_ID_2_CMD_SPECIAL				0x04


#define DEDITEC_TCP_PACKET_ID_2_ANSWER_RO_1				0x81
#define DEDITEC_TCP_PACKET_ID_2_ANSWER_TESTDATA			0x82
#define DEDITEC_TCP_PACKET_ID_2_ANSWER_CMD_SEND_FILE	0x83
#define DEDITEC_TCP_PACKET_ID_2_ANSWER_CMD_SPECIAL		0x84

// ----------------------------------------------------------------------------
#define CONNECTION_TIMEOUT_TO_CLIENT 5  // timeout value in seconds; server closes connection to client after x seconds




// ----------------------------------------------------------------------------
// Broadcast TX and RX ID's

#define DEDITEC_BC_PACKET_TX_ID_0 0x12
#define DEDITEC_BC_PACKET_TX_ID_1 0x47
#define DEDITEC_BC_PACKET_TX_ID_2 0x22
#define DEDITEC_BC_PACKET_TX_ID_3 0x01

#define DEDITEC_BC_PACKET_RX_ID_0 0x13
#define DEDITEC_BC_PACKET_RX_ID_1 0x47
#define DEDITEC_BC_PACKET_RX_ID_2 0x22
#define DEDITEC_BC_PACKET_RX_ID_3 0x01

#define DEDITEC_BC_PACKET_CMD_GLOBAL_CALL		0x01
#define DEDITEC_BC_PACKET_CMD_PARAM_GET			0x02
#define DEDITEC_BC_PACKET_CMD_PARAM_SET			0x03
#define DEDITEC_BC_PACKET_CMD_ETH0_CONFIGURE	0x04

#define DEDITEC_BC_PACKET_PARAM_BOARD_NAME		0x01
#define DEDITEC_BC_PACKET_PARAM_IP_ADDR			0x02
#define DEDITEC_BC_PACKET_PARAM_NETMASK			0x03
#define DEDITEC_BC_PACKET_PARAM_STDGATEWAY		0x04
#define DEDITEC_BC_PACKET_PARAM_DNS1			0x05
#define DEDITEC_BC_PACKET_PARAM_DHCP			0x06

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// PARAMETER_IO
// PARAMETER_IO
// PARAMETER_IO
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


//CONFIG-Seite

// General
#define DEDITEC_PARAM_BOARD_NAME		"board_name"
#define DEDITEC_PARAM_PRODUCT_NAME		"prodcut_name"

//PRODUCT_CONFIGURATION
#define DEDITEC_PARAM_PRODUCT_CONFIG_DI			"product_cfg_di"
#define DEDITEC_PARAM_PRODUCT_CONFIG_DO			"product_cfg_do"
#define DEDITEC_PARAM_PRODUCT_CONFIG_DI_CNT		"product_cfg_di_cnt"
#define DEDITEC_PARAM_PRODUCT_CONFIG_AI			"product_cfg_ai"
#define DEDITEC_PARAM_PRODUCT_CONFIG_AO			"product_cfg_ao"
#define DEDITEC_PARAM_PRODUCT_CONFIG_TEMP		"product_cfg_temp"
#define DEDITEC_PARAM_PRODUCT_CONFIG_STEPP		"product_cfg_stepp"


// FW-Revisions
#define DEDITEC_PARAM_FW_REV_KEY				"fw_rev_key_handler"
#define DEDITEC_PARAM_FW_REV_ROETHFW			"fw_rev_ro_eth_fw"
#define DEDITEC_PARAM_FW_REV_BCSERVER			"fw_rev_bc_server"
#define DEDITEC_PARAM_FW_REV_WEBCMD				"fw_rev_webcmd"
#define DEDITEC_PARAM_FW_REV_WEBDAPIEXECUTE		"fw_rev_webdapiexecute"



// Network
#define DEDITEC_PARAM_IP_IP				"config-ip-ip"
#define DEDITEC_PARAM_IP_NETMASK		"config-ip-netmask"
#define DEDITEC_PARAM_IP_STDGATEWAY		"config-ip-stdgateway"
#define DEDITEC_PARAM_IP_DNS1			"config-ip-dns1"


// Mail-Server
#define DEDITEC_PARAM_MAIL_SMTP_SERVER_HOSTNAME                    "mail-smtp-server-hostname"
#define DEDITEC_PARAM_MAIL_SMTP_SERVER_AUTHENTIFICATE              "mail-smtp-server-authentificate"
#define DEDITEC_PARAM_MAIL_SMTP_SERVER_AUTHENTIFICATE_USER         "mail-smtp-server-authentificate-user"
#define DEDITEC_PARAM_MAIL_SMTP_SERVER_AUTHENTIFICATE_PWD          "mail-smtp-server-authentificate-pwd"


// a number will be appended to each string
#define DEDITEC_PARAM_MAIL_SENDER                                  "mail-sender-"
#define DEDITEC_PARAM_MAIL_SENDER_NAME                             "mail-sender-name-"
#define DEDITEC_PARAM_MAIL_RECIPIENT                               "mail-recipient-"
#define DEDITEC_PARAM_MAIL_SUBJECT                                 "mail-subject-"
#define DEDITEC_PARAM_MAIL_MESSAGE                                 "mail-message-"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Default Values for handler_for_ip_set_to_default
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define DEFAULT_IP                        "192.168.1.1"
#define DEFAULT_NETMASK                   "255.255.255.0"
#define DEFAULT_STDGATEWAY                   "192.168.1.254"

