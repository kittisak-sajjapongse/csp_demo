#include "connection.h"

int main(int argc, char** argv) {
    natsConnection *conn = NULL;
    natsOptions *opts = NULL;
    nats_connect(&opts, &conn, "nats://127.0.0.1:4222");


    natsSubscription* sub = NULL;
    SAFE_CALL(natsConnection_SubscribeSync(&sub, conn, "mySubject"), "Subscribe_Sync");
    natsMsg* msg;
    while (1) {
        SAFE_CALL(natsSubscription_NextMsg(&msg, sub, 10000000), "NextMsg");
        printf("Received message (sync): %s\n", natsMsg_GetData(msg));
    }

    
    natsSubscription_Destroy(sub);
    nats_finalize(opts, conn);

    return 0;
}
