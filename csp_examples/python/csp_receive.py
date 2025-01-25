import time
import threading

import libcsp_py3 as libcsp

SERVER_ADDRESS = 9
SERVER_PORT = 10
BIT_RATE = 500000

def handle_client_connection(conn):
    """
    Handles data reception (and reply logic) for one connection.
    This runs in its own thread so that the main server loop can
    keep accepting new connections.
    """

    while True:
        # Try to read a packet with a 100 ms timeout
        packet = libcsp.read(conn, 100)
        if packet is None:
            # Connection closed or no packet received in time
            break

        # If the destination port is our 'service' port
        if libcsp.conn_dport(conn) == SERVER_PORT:
            data = bytearray(libcsp.packet_get_data(packet))
            length = libcsp.packet_get_length(packet)
            # print("Received packet, len = {}, data = {}".format(
            #       length, ''.join('{:02x}'.format(x) for x in data)))
            print(f"data={data}")
            reply = libcsp.buffer_get(0)
            libcsp.packet_set_data(reply, b"Thanks, I got your message")
            libcsp.send(conn, reply)
        else:
            # If it’s some other port, let CSP’s service_handler handle it
            libcsp.service_handler(conn, packet)
    

def csp_server():
    sock = libcsp.socket()
    libcsp.bind(sock, libcsp.CSP_ANY)
    libcsp.listen(sock, 5)
    while True:
        conn = libcsp.accept(sock, 100)
        if not conn:
            continue

        print ("connection: source=%i:%i, dest=%i:%i" % (libcsp.conn_src(conn),
                                                         libcsp.conn_sport(conn),
                                                         libcsp.conn_dst(conn),
                                                         libcsp.conn_dport(conn)))

        t = threading.Thread(target=handle_client_connection, args=(conn,))
        t.daemon = True   # Optional: daemon thread will close if main thread exits
        t.start()

def main():
    libcsp.init("host", "model", "1.2.3")
    libcsp.can_socketcan_init("vcan0", SERVER_ADDRESS, BIT_RATE)
    libcsp.rtable_set(0, 0, "CAN")
    libcsp.route_start_task()
    time.sleep(0.2)
    libcsp.print_connections()
    libcsp.print_interfaces()
    csp_server()

if __name__ == "__main__":
    main()