/* This code is for a simple CSP (CubeSat Space Protocol) server that operates on 
a CAN (Controller Area Network) interface using the SocketCAN driver. 
It initializes the CSP library, sets up a router task to handle routing, 
opens a CAN interface, and starts a server that listens for connections and processes incoming packets. */

#include <csp/csp.h>  // This header provides the CubeSat Space Protocol (CSP) API.
#include <csp/drivers/can_socketcan.h>  // This header includes the CAN interface driver for SocketCAN.
#include <stdlib.h>  //  Provides standard library functions, such as memory management and process control.
#include <pthread.h> // Provides the POSIX threads library to create and manage threads.
#include <unistd.h> // For parsing arguments
#include <stdio.h> // For printing
#include <string.h>

#define SERVER_PORT		10  // The port number on which the server will listen for incoming connections.

/* Thread Creation for Router */
/* This function creates a new POSIX thread. */
static int csp_pthread_create(void * (*routine)(void *)) {

	pthread_attr_t attributes;
	pthread_t handle;
	int ret;

	/*Initializes a 'pthread_attr_t' object, which contains attributes for the thread.*/
	if (pthread_attr_init(&attributes) != 0) {
		return CSP_ERR_NOMEM;  // If initialization fails, it returns an error code 'CSP_ERR_NOMEM'.
	}
	/*'pthread_create': Creates a thread with the provided routine (a function pointer), 
	passing NULL as the argument.*/
	ret = pthread_create(&handle, &attributes, routine, NULL);
	pthread_attr_destroy(&attributes); // Destroys the attributes after the thread is created.

	if (ret != 0) {
		return ret; // If the thread creation fails, it returns the error code.
	}
	// Otherwise, it returns CSP_ERR_NONE (success).
	return CSP_ERR_NONE;
}
/*Router Task (Thread)*/
/*This is the function that runs in the router thread. 
It continuously calls csp_route_work() 
to perform routing tasks in the CSP stack.*/
static void * task_router(void * param) {

	/* Here there be routing */
	while (1) {
		csp_route_work();
	}

	return NULL;
}

/*This function starts the router by creating a new thread 
that runs the 'task_router' function.*/
int router_start(void) {
	return csp_pthread_create(task_router);
}

/**
 * @param device_name - The name of the CAN interface (here it's vcan0, which is a virtual CAN interface used for testing).
 * @param server_address - The address of the server on the CAN network. (Chanel)
 *                         Note: Address "0" is reserved for loop-back.
 */ 
int receiver(const char* device_name, const uint8_t server_address) {
    csp_print("Initialising CSP\n");

    /* Init CSP */
    csp_init();

    /* Start router */
    router_start(); // Starts the router thread, which will handle packet routing.

    /* Add CAN interface(s) */
    csp_iface_t * default_iface = NULL; // csp_iface_t: CSP interface structure.

	/*csp_can_socketcan_open_and_add_interface: 
	This function opens the CAN interface (vcan0) using SocketCAN, 
	assigns the interface name CSP_IF_CAN_DEFAULT_NAME, 
	sets the server address to 1, 
	and the bitrate to 1 Mbps (1000000 bps). 
	The true argument enables promiscuous mode, 
	and the created interface is stored in default_iface.*/

	/*Ex: csp_can_socketcan_open_and_add_interface(const char * device, const char * ifname, 
		unsigned int node_id, int bitrate, bool promisc, csp_iface_t ** return_iface)
		in src/drivers/can/can_socketcan.c*/
    int error = csp_can_socketcan_open_and_add_interface(device_name, CSP_IF_CAN_DEFAULT_NAME, server_address, 1000000, true, &default_iface);
        if (error != CSP_ERR_NONE) {
            csp_print("failed to add CAN interface [%s], error: %d\n", device_name, error);
            exit(1); // If the interface can't be opened, it prints an error message and exits the program.
        }
    
    default_iface->is_default = 1; // Marks the CAN interface as the default interface by setting is_default = 1.

	/*Printing Tables and Interfaces*/
    csp_print("Connection table\r\n");
    csp_conn_print_table(); //Prints the current connection table (csp_conn_print_table()) 

    csp_print("Interfaces\r\n");
    csp_iflist_print(); //the list of available interfaces (csp_iflist_print()).

    /* Start server thread */
    csp_print("Server task started\n");

	/* Create socket with no specific socket options, e.g. accepts CRC32, HMAC, etc. if enabled during compilation */
	csp_socket_t sock = {0}; //Initializes a CSP socket, which will be used for communication.

	/* Bind socket to all ports, e.g. all incoming connections will be handled here */
	csp_bind(&sock, CSP_ANY); //Binds the socket to all ports (CSP_ANY), meaning it will handle any incoming connection.

	/* Create a backlog of 10 connections, i.e. up to 10 new connections can be queued */
	csp_listen(&sock, 10); //The socket listens for incoming connections

	/*Accepting and Processing Connections*/
    /* Wait for connections and then process packets on the connection */
	while (1) {
        /* Wait for a new connection, 10000 mS (10 second) timeout */
		csp_conn_t *conn;
		if ((conn = csp_accept(&sock, 10000)) == NULL) {
			/* timeout */
			continue;
		} // If no connection is received within the timeout, it continues waiting.

        /* Read packets on connection, timout is 100 mS */
		csp_packet_t *packet;
        while ((packet = csp_read(conn, 50)) != NULL) { // Once a connection is accepted, it reads incoming packets with a 50 ms timeout
			/* Checks if the packet is sent to SERVER_PORT. 
			If so, it prints the data received in the packet 
			and then frees the packet buffer (csp_buffer_free).*/
			switch (csp_conn_dport(conn)) {
			case SERVER_PORT: 
				/* Process packet here */
				csp_print("Packet received on SERVER_PORT: %s\n", (char *) packet->data);
				csp_buffer_free(packet);
				// ++server_received;
				break;

			default:
				/* Call the default CSP service handler, handle pings, buffer use, etc. */
				/*If the packet isn't for the SERVER_PORT, 
				it calls 'csp_service_handler' to handle standard CSP services like pings, etc.*/
				csp_service_handler(packet);
				break;
			}
		}

		/* Close current connection */
		/*After processing packets on the connection, 
		it closes the connection using csp_close() and goes back to waiting for new connections.*/
		csp_close(conn);
    }

    return 0;
}

int main(int argc, char * argv[]) {
	int opt;
	int has_device = 0;
	char device[128] = "";
	int has_address = 0;
	uint8_t address = 0; // 0 is reserved for loop-back

    // Parse the command-line options
    while ((opt = getopt(argc, argv, "a:d:h")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: ./%s [-h] [-ad]\n", argv[0]);
				return -1;
                break;
            case 'a':
                address = atoi(optarg);
				has_address = 1;
                break;
			case 'd':
                strcpy(device, optarg);
				has_device = 1;
                break;
            default:
                printf("Usage: ./program [-h] [-ad]\n");
                return 1;
        }
    }
	if ((!has_device) || (!has_address)) {
		printf("Please specify both 'device' and 'address' using -a and -d options.\n");
		return -1;
	}
	return receiver(device, address);
}