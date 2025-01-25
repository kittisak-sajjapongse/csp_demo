import time

import libcsp_py3 as libcsp

CLIENT_ADDRESS = 5
SERVER_ADDRESS = 9
SERVER_PORT = 10
BIT_RATE = 1000000

def main():
    libcsp.init("host", "model", "1.2.3")
    libcsp.can_socketcan_init("vcan1", CLIENT_ADDRESS, BIT_RATE, True)
    libcsp.route_start_task()
    libcsp.rtable_set(0, 0, "CAN")
    time.sleep(0.2)
    print("===== Connecitons =====")
    libcsp.print_connections()
    print("===== Interfaces =====")
    libcsp.print_interfaces()
    print("===== Routes =====")
    libcsp.print_routes()
    print("==================")

    conn = libcsp.connect(libcsp.CSP_PRIO_NORM, SERVER_ADDRESS, SERVER_PORT, 1000, libcsp.CSP_O_NONE)
    if conn is None:
        print("Cannot connect")
        return
    print("Connected")
    data = libcsp.buffer_get(0)
    libcsp.packet_set_data(data, b"Hello server! this is the message from the client")
    libcsp.send(conn, data)

    reply = libcsp.read(conn, 2000)
    if reply is None:
        print("No reply or timeout!")
    else:
        reply_data = libcsp.packet_get_data(reply)
        print("Client got reply:", reply_data)

    libcsp.close(conn)

if __name__ == "__main__":
    main()