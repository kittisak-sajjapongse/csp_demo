// This is for omitting the prefix of natsConnStatus
#define NATS_CONN_STATUS_NO_PREFIX

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <nats/nats.h>

// Function to get the current time as a string
const char* current_time() {
    static char time_buffer[20];
    time_t rawtime;
    struct tm* time_info;

    time(&rawtime);
    time_info = localtime(&rawtime);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", time_info);
    
    return time_buffer;
}

// Enhanced safe_call function with more detailed output
void safe_call(natsStatus status, const char* msg, const char* file, int line) {
    if (status != NATS_OK) {
        const char* err_text = natsStatus_GetText(status);  // Get NATS error description
        fprintf(stderr, "[%s] Error: %s (NATS Status: %s) [File: %s, Line: %d]\n", 
                current_time(), msg, err_text, file, line);
        exit(EXIT_FAILURE);
    }
}

#define SAFE_CALL(status, msg) safe_call(status, msg, __FILE__, __LINE__)

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
        usleep(100000);
    }
    printf("Connected\n");

    *r_opts = opts;
    *r_conn = conn;
}

void nats_finalize(natsOptions* opts, natsConnection* conn) {
    natsConnection_Destroy(conn);
    natsOptions_Destroy(opts);
}

void subscription_handler(natsConnection *nc, natsSubscription *sub, natsMsg *msg, void *closure) {
    printf("Received message: %s\n", natsMsg_GetData(msg));
    natsMsg_Destroy(msg);
}

int main(int argc, char** argv) {
    natsConnection *conn = NULL;
    natsOptions *opts = NULL;
    nats_connect(&opts, &conn, "nats://127.0.0.1:4222");


    natsSubscription* sub = NULL;
    SAFE_CALL(natsConnection_Subscribe(&sub, conn, "mySubject", subscription_handler, NULL), "Subscribe");
    while(true) {
        nats_Sleep(1000);
    }

    // SAFE_CALL(natsConnection_SubscribeSync(&sub, conn, "mySubject"), "Subscribe_Sync");
    // natsMsg* msg;
    // while (1) {
    //     SAFE_CALL(natsSubscription_NextMsg(&msg, sub, 10000000), "NextMsg");
    //     printf("Message received\n");
    // }

    
    natsSubscription_Destroy(sub);
    nats_finalize(opts, conn);

    return 0;
}
