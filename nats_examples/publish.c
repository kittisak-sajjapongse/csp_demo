#include "connection.c"

int main(int argc, char** argv) {
    natsOptions* opts;
    natsConnection* conn;
    nats_connect(&opts, &conn, "nats://127.0.0.1:4222");
    SAFE_CALL(natsConnection_PublishString(conn, "mySubject", "Hello"), "Publish");
    nats_finalize(opts, conn);

    return 0;
}