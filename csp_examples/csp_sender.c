/*This code demonstrates a CSP (CubeSat Space Protocol) client 
that communicates over a CAN (Controller Area Network) interface using SocketCAN. 
The program initializes the CSP stack, sets up a CAN interface, 
connects to a server on the network, and sends a message.*/

#include <csp/csp.h> //Includes the CSP (CubeSat Space Protocol) library headers.
#include <csp/drivers/can_socketcan.h> //Includes the SocketCAN driver for using CAN (Controller Area Network) interfaces.
#include <stdlib.h> //Standard library for functions like exit()

#define DEVICE_NAME     "vcan2"  // Specifies the CAN interface name
#define SERVER_PORT		10   //Defines the port number on the server side where the client will connect (in this case, port 10).

/* Commandline options */
static uint8_t server_address = 1;
static uint8_t client_address = 2;

// static unsigned int successful_ping = 0;

int main(int argc, char** argv) {
    //KS: This was copied from ${LIBCSP_DIR}/examples/csp_server_client.c (Look at function client())
   
    /* Init CSP */
    /*Initializes the CSP stack. This prepares the system to use CSP functions and start networking.*/
    csp_init();

    /* Start router */
    // router_start();

    /* Add CAN interface(s) */
    /*Declares a pointer to a csp_iface_t structure, 
    which will hold the configuration for the default CAN interface.*/
    csp_iface_t * default_iface = NULL;

    int bitrate = 1000000; //The CAN bus bitrate is set to 1 Mbps (1,000,000 bits per second).
    // Opens the CAN interface (vcan2)
    int error = csp_can_socketcan_open_and_add_interface(DEVICE_NAME, CSP_IF_CAN_DEFAULT_NAME, client_address, bitrate, true, &default_iface);   
    if (error != CSP_ERR_NONE) {
			csp_print("failed to add CAN interface [%s], error: %d\n", DEVICE_NAME, error);
            exit(1); //If the interface setup fails, the program prints an error message and exits.
    }

    default_iface->is_default = 1; // Marks the CAN interface as the default interface by setting is_default = 1

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
    csp_conn_print_table(); // Prints the connection table, which contains active connections in the CSP stack.

    csp_print("Interfaces\r\n");
    csp_iflist_print(); //Prints the list of interfaces, including the CAN interface just added

    /* Start client work */
	// csp_print("Client started\n");

    /* Send ping to server, timeout 1000 mS, ping size 100 bytes */
    // int result = csp_ping(server_address, 1000, 100, CSP_O_NONE);
    // csp_print("Ping address: %u, result %d [mS]\n", server_address, result);
    // Increment successful_ping if ping was successful
    // if (result >= 0) {
    //     ++successful_ping;
    // }
	

    /* =========  Send data packet (string) to server ========= */

    /* 1. Connect to host on 'server_address', port SERVER_PORT with regular UDP-like protocol and 1000 ms timeout */
    /*csp_connect(): This function establishes a connection to the server.
        - CSP_PRIO_NORM: Sets normal priority for the connection.
        - server_address: The destination server address (set to 1).
        - SERVER_PORT: The port on the server the client will connect to (set to 10).
        - 1000: Timeout in milliseconds (1 second) for establishing the connection.
        - CSP_O_NONE: No special options are passed during connection establishment.*/
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
    /*Preparing the Packet*/
    /*csp_buffer_get(0): Allocates a buffer for a packet. 
    The argument 0 means the system will automatically assign an appropriate buffer size for the data.*/
	csp_packet_t * packet = csp_buffer_get(0);
    // if (packet == NULL) {
			/* Could not get buffer element */
			// csp_print("Failed to get CSP buffer\n");
			// ret = EXIT_FAILURE;
			// break;
		// }

    /* 3. Copy data to packet */

    char message_data[] = "Hello Sender HunterXHunter"; //The message string to be sent to the server
    int bytes = strlen(message_data); //Calculates the length of the message in bytes.
	memcpy(packet->data, message_data, bytes); //Copies the message data into the packet's data buffer.
    memset(packet->data + bytes, 0, 1); //Adds a null terminator (0) at the end of the message,

    /* 4. Set packet length */
    packet->length = (strlen((char *) packet->data) + 1); /* include the 0 termination */

    /* 5. Send packet */
    csp_send(conn, packet); //Sends the prepared packet over the established connection

    /* 6. Close connection */
    csp_close(conn); //Closes the connection after the data has been successfully sent.
    csp_print("Send data successed\n");
    return 0;
}


