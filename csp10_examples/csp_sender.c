#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <csp/csp.h>
#include <csp/drivers/can_socketcan.h>

/* Server port, the port the server listens on for incoming connections from the client. */
#define MY_SERVER_PORT		10

int sender(const char* device, const uint8_t address, const uint8_t target_address) {
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

    // *** Required to connect properly ***
    csp_route_start_task(500, 0);

    csp_iface_t * default_iface = NULL;
    error = csp_can_socketcan_open_and_add_interface(device, CSP_IF_CAN_DEFAULT_NAME, 0, false, &default_iface);
    if (error != CSP_ERR_NONE) {
        csp_log_error("failed to add CAN interface [%s], error: %d", device, error);
        exit(1);
    }
    // *** Required to connect properly ***
    csp_rtable_set(CSP_DEFAULT_ROUTE, 0, default_iface, CSP_NO_VIA_ADDRESS);

    /* Send ping to server, timeout 1000 mS, ping size 100 bytes */
    int result = csp_ping(target_address, 1000, 100, CSP_O_NONE);
    csp_log_info("Ping address: %u, result %d [mS]", target_address, result);

    /* 1. Connect to host on 'target_address', port MY_SERVER_PORT with regular UDP-like protocol and 1000 ms timeout */
    csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, target_address, MY_SERVER_PORT, 1000, CSP_O_NONE);
    if (conn == NULL) {
        /* Connect failed */
        csp_log_error("Connection failed");
        return -1;
    }

    /* 2. Get packet buffer for message/data */
    csp_packet_t * packet = csp_buffer_get(100);
    if (packet == NULL) {
        /* Could not get buffer element */
        csp_log_error("Failed to get CSP buffer");
        return -1;
    }

    /* 3. Copy data to packet */
    snprintf((char *) packet->data, csp_buffer_data_size(), "Hello World");

    /* 4. Set packet length */
    packet->length = (strlen((char *) packet->data) + 1); /* include the 0 termination */

    /* 5. Send packet */
    if (!csp_send(conn, packet, 1000)) {
        /* Send failed */
        csp_log_error("Send failed");
        csp_buffer_free(packet);
    }

    /* 6. Close connection */
    csp_close(conn);


    csp_log_info("Success\n");
    return 0;
}

int main(int argc, char** argv) {
    int opt;
	int has_device = 0;
	char device[128] = "";
	int has_address = 0;
	uint8_t address = 0; // 0 is reserved for loop-back
    int has_target_address = 0;
	uint8_t target_address = 0; // 0 is reserved for loop-back

    // Parse the command-line options
    while ((opt = getopt(argc, argv, "a:d:ht:")) != -1) {
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
            case 't':
                target_address = atoi(optarg);
				has_target_address = 1;
                break;
            default:
                printf("Usage: ./program [-h] [-adt]\n");
                return 1;
        }
    }
	if ((!has_device) || (!has_address) || (!has_target_address)) {
		printf("Please specify 'device','address', and 'target address' using -a,-d, and -t options.\n");
		return -1;
	}
    return sender(device, address, target_address);
}