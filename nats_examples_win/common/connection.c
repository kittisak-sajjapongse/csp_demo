#include "connection.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
//#include <unistd.h>

// Enhanced safe_call function with more detailed output
void safe_call(natsStatus status, const char* msg, const char* file, int line) {
    if (status != NATS_OK) {
        const char* err_text = natsStatus_GetText(status);  // Get NATS error description
        fprintf(stderr, "Error: %s (NATS Status: %s) [File: %s, Line: %d]\n", 
                msg, err_text, file, line);
        exit(EXIT_FAILURE);
    }
}

void nats_connect(natsOptions** r_opts, natsConnection** r_conn, const char* url) {
    natsOptions* opts;
    SAFE_CALL(natsOptions_Create(&opts), "Create option");
    SAFE_CALL(natsOptions_SetURL(opts, url), "Set URL");
    SAFE_CALL(natsOptions_SetVerbose(opts, true), "Set verbose");
    SAFE_CALL(natsOptions_SetPingInterval(opts, 1000), "Set ping interval");
    SAFE_CALL(natsOptions_SetMaxPingsOut(opts, 5), "Set max ping out");
    SAFE_CALL(natsOptions_SetAllowReconnect(opts, true), "Set allow reconnect");
    SAFE_CALL(natsOptions_SetReconnectWait(opts, 1000), "Set reconnect wait");

    natsConnection* conn;
    SAFE_CALL(natsConnection_Connect(&conn, opts), "Connect");
    natsConnStatus conn_status = DISCONNECTED;
    while (conn_status == DISCONNECTED) {
        conn_status = natsConnection_Status(conn);
        if ((conn_status != CONNECTED) && (conn_status != CONNECTING)) {
            printf("Error: Connecting failed\n");
            exit(EXIT_FAILURE);
        }
        Sleep(1000);
    }
    printf("Connected\n");

    *r_opts = opts;
    *r_conn = conn;
}

void nats_finalize(natsOptions* opts, natsConnection* conn) {
    natsConnection_Destroy(conn);
    natsOptions_Destroy(opts);
}
