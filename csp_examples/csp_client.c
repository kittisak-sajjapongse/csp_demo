#include <csp/csp.h>
#include <csp/drivers/can_socketcan.h>
#include <stdlib.h>

#define DEVICE_NAME                 "vcan2" 
// #define CAN_CLIENT_ADDRESS		    1
// #define SERVER_ADDRESS              2

// Server Port
#define SERVER_PORT		10

/* Commandline options */
static uint8_t server_address = 1;
static uint8_t client_address = 2;

static unsigned int successful_ping = 0;

int main(int argc, char** argv) {
    //KS: This was copied from ${LIBCSP_DIR}/examples/csp_server_client.c (Look at function client())
    // int ret = EXIT_SUCCESS;
    csp_print("Initialising CSP\n");
    
    /* Init CSP */
    csp_init();

    /* Start router */
    // router_start();

    /* Add CAN interface(s) */
    csp_iface_t * default_iface = NULL;
    int error = csp_can_socketcan_open_and_add_interface(DEVICE_NAME, CSP_IF_CAN_DEFAULT_NAME, client_address, 1000000, true, &default_iface);   
    if (error != CSP_ERR_NONE) {
			csp_print("failed to add CAN interface [%s], error: %d\n", DEVICE_NAME, error);
            exit(1);
        } else {
            csp_print("Open and Add CAN interface: %d\n", error)
        }
    default_iface->is_default = 1;

    /* Setup routing table */
	// if (CSP_USE_RTABLE) {
	// 	if (rtable) {
	// 		int error = csp_rtable_load(rtable);
	// 		if (error < 1) {
	// 			csp_print("csp_rtable_load(%s) failed, error: %d\n", rtable, error);
	// 			exit(1);
	// 		}
	// 	} else if (default_iface) {
	// 		csp_rtable_set(0, 0, default_iface, CSP_NO_VIA_ADDRESS);
	// 	}
	// }

    csp_print("Connection table\r\n");
    csp_conn_print_table();

    csp_print("Interfaces\r\n");
    csp_iflist_print();

    /* Start client work */
	// csp_print("Client started\n");

    /* Send ping to server, timeout 1000 mS, ping size 100 bytes */
    int result = csp_ping(server_address, 1000, 100, CSP_O_NONE);
    csp_print("Ping address: %u, result %d [mS]\n", server_address, result);
    // Increment successful_ping if ping was successful
    if (result >= 0) {
        ++successful_ping;
    }
	

    /* =========  Send data packet (string) to server ========= */

    /* 1. Connect to host on 'server_address', port SERVER_PORT with regular UDP-like protocol and 1000 ms timeout */
	csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, server_address, SERVER_PORT, 1000, CSP_O_NONE);
    if (conn == NULL) {
			/* Connect failed */
			csp_print("Connection failed\n");
			// ret = EXIT_FAILURE;
			// break;
		} else {
            csp_print("Connect to Server....\n")
        }
    /* 2. Get packet buffer for message/data */
	csp_packet_t * packet = csp_buffer_get(0);
    if (packet == NULL) {
			/* Could not get buffer element */
			csp_print("Failed to get CSP buffer\n");
			// ret = EXIT_FAILURE;
			// break;
		}

    /* 3. Copy data to packet */
	memcpy(packet->data, "Hello Client", 12);
    memset(packet->data + 12, 0, 1);

    /* 4. Set packet length */
    packet->length = (strlen((char *) packet->data) + 1); /* include the 0 termination */

    /* 5. Send packet */
    csp_send(conn, packet);

    /* 6. Close connection */
    csp_close(conn);
    csp_print("Send data successed\n");
    return 0;
}


