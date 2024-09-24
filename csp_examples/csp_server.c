#include <csp/csp.h>
#include <csp/drivers/can_socketcan.h>

/* Server port, the port the server listens on for incoming connections from the client. */
#define MY_SERVER_PORT          10
#define CAN_ADDRESS		    1


int main(int argc, char** argv) {
    csp_init();

    csp_iface_t * default_iface = NULL;
    int error = csp_can_socketcan_open_and_add_interface("vcan1", CSP_IF_CAN_DEFAULT_NAME, CAN_ADDRESS, 1000000, true, &default_iface);
    default_iface->is_default = 1;

    // csp_print("Connection table\r\n");
    // csp_conn_print_table();
    csp_print("Interfaces\r\n");
    csp_iflist_print();

    csp_print("Server task started\n");

    /* Create socket with no specific socket options, e.g. accepts CRC32, HMAC, etc. if enabled during compilation */
    csp_socket_t sock = {0};

    /* Bind socket to all ports, e.g. all incoming connections will be handled here */
    csp_bind(&sock, CSP_ANY);

    /* Create a backlog of 10 connections, i.e. up to 10 new connections can be queued */
    csp_listen(&sock, 10);


    //KS: This was copied from ${LIBCSP_DIR}/examples/csp_server_client.c
    unsigned int server_received = 0;
    /* Wait for a new connection, 10000 mS timeout */
    while(1) {
        csp_conn_t *conn;
        if ((conn = csp_accept(&sock, 10000)) == NULL) {
            /* timeout */
            continue;
        }

        /* Read packets on connection, timout is 100 mS */
        csp_packet_t *packet;
        while ((packet = csp_read(conn, 50)) != NULL) {
            switch (csp_conn_dport(conn)) {
            case MY_SERVER_PORT:
                /* Process packet here */
                csp_print("Packet received on MY_SERVER_PORT: %s\n", (char *) packet->data);
                csp_buffer_free(packet);
                ++server_received;
                break;

            default:
                /* Call the default CSP service handler, handle pings, buffer use, etc. */
                csp_service_handler(packet);
                break;
            }
        }

        /* Close current connection */
        csp_close(conn);
    }

    return 0;
}
