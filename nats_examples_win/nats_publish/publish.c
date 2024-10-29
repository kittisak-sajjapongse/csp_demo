#include "connection.h"

int main(int argc, char** argv) {
    natsOptions* opts;
    natsConnection* conn;
    nats_connect(&opts, &conn, "nats://157.230.245.30:4222");
    SAFE_CALL(natsConnection_PublishString(conn, "mySubject", "Hello"), "Publish");
    nats_finalize(opts, conn);

    return 0;
}