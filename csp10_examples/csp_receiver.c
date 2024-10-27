#include <stdlib.h>
#include <unistd.h> // For parsing arguments

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/drivers/can_socketcan.h>

/* Server port, the port the server listens on for incoming connections from the client. */
#define MY_SERVER_PORT		10

CSP_DEFINE_TASK(task_server) {
    csp_log_info("Server task started");

    /* Create socket with no specific socket options, e.g. accepts CRC32, HMAC, XTEA, etc. if enabled during compilation */
	csp_socket_t *sock = csp_socket(CSP_SO_NONE);

	/* Bind socket to all ports, e.g. all incoming connections will be handled here */
	csp_bind(sock, CSP_ANY);

	/* Create a backlog of 10 connections, i.e. up to 10 new connections can be queued */
	csp_listen(sock, 10);

    /* Read packets on connection, timout is 100 mS */
    while (1) {
		csp_conn_t *conn;
		if ((conn = csp_accept(sock, 10000)) == NULL) {
			/* timeout */
			continue;
		}

        csp_packet_t *packet;
        while ((packet = csp_read(conn, 50)) != NULL) {
            switch (csp_conn_dport(conn)) {
            case MY_SERVER_PORT:
                /* Process packet here */
                csp_log_info("Packet received on MY_SERVER_PORT: %s", (char *) packet->data);
                csp_buffer_free(packet);
                break;

            default:
                /* Call the default CSP service handler, handle pings, buffer use, etc. */
                csp_service_handler(conn, packet);
                break;
            }
        }

        csp_close(conn);
    }

    return CSP_TASK_RETURN;
}

int receiver(const char* device, const uint8_t address) {
    csp_debug_level_t debug_level = CSP_INFO;
    /* enable/disable debug levels */
    for (csp_debug_level_t i = 0; i <= CSP_LOCK; ++i) {
        csp_debug_set_level(i, (i <= debug_level) ? true : false);
    }

    /* Init CSP with address and default settings */
    csp_conf_t csp_conf;
    csp_conf_get_defaults(&csp_conf);
    csp_conf.address = address;
    int error = csp_init(&csp_conf);
    if (error != CSP_ERR_NONE) {
        csp_log_error("csp_init() failed, error: %d", error);
        exit(1);
    }

    csp_route_start_task(500, 0);

    csp_iface_t * default_iface = NULL;
    error = csp_can_socketcan_open_and_add_interface(device, CSP_IF_CAN_DEFAULT_NAME, 0, false, &default_iface);
    if (error != CSP_ERR_NONE) {
        csp_log_error("failed to add CAN interface [%s], error: %d", device, error);
        exit(1);
    }
    csp_rtable_set(CSP_DEFAULT_ROUTE, 0, default_iface, CSP_NO_VIA_ADDRESS);

    csp_thread_create(task_server, "SERVER", 1000, NULL, 0, NULL);
    while(1) {
        csp_sleep_ms(3000);
    }
    return 0;
}

int main(int argc, char** argv) {
    int opt;
    int has_device = 0;
	char device[128] = "";
	int has_address = 0;
	uint8_t address = 0; // 0 is reserved for loop-back

    // Parse the command-line options
    while ((opt = getopt(argc, argv, "a:d:h")) != -1) {
        switch (opt) {
            case 'h':
                csp_log_info("Usage: ./%s [-h] [-ad]\n", argv[0]);
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
                csp_log_info("Usage: ./program [-h] [-ad]\n");
                return 1;
        }
    }
	if ((!has_device) || (!has_address)) {
		csp_log_info("Please specify both 'device' and 'address' using -a and -d options.\n");
		return -1;
	}
    receiver(device, address);
}