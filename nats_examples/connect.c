#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

int main(int argc, char** argv) {
    natsOptions* opts;
    SAFE_CALL(natsOptions_Create(&opts), "Create option");
    SAFE_CALL(natsOptions_SetURL(opts, "nats://127.0.0.1:4222"), "Set URL");
    SAFE_CALL(natsOptions_SetVerbose(opts, true), "Set verbose");
    SAFE_CALL(natsOptions_SetPingInterval(opts, 1000), "Set ping interval");
    SAFE_CALL(natsOptions_SetMaxPingsOut(opts, 5), "Set max ping out");
    SAFE_CALL(natsOptions_SetAllowReconnect(opts, true), "Set allow reconnect");
    SAFE_CALL(natsOptions_SetReconnectWait(opts, 1000), "Set reconnect wait");

    natsConnection* conn;
    SAFE_CALL(natsConnection_Connect(&conn, opts), "Connect");
    
    // Clean up and close connection
    natsConnection_Destroy(conn);
    natsOptions_Destroy(opts);

    return 0;
}
