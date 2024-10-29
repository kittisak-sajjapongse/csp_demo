#include "connection.h"

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
    while(1) {
        nats_Sleep(1000);
    }
    
    natsSubscription_Destroy(sub);
    nats_finalize(opts, conn);

    return 0;
}
