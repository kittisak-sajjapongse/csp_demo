#include "connection.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char data[128];
    sprintf(data, "%s", argv[1]);
    natsConnection *conn = NULL;
    natsOptions *opts = NULL;
    nats_connect(&opts, &conn, "nats://127.0.0.1:4222");
    
    while (1) {
        natsMsg* reply = NULL;
        SAFE_CALL(
            natsConnection_Request(&reply, conn, "send.tc", data, strlen(data), 2000),
            "Cannot request"
        );
        printf("Received: %s", natsMsg_GetData(reply));
        natsMsg_Destroy(reply);
    }

    nats_finalize(opts, conn);

    return 0;
}