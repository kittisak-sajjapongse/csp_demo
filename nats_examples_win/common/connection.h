// This is for omitting the prefix of natsConnStatus
#define NATS_CONN_STATUS_NO_PREFIX
#include <nats/nats.h>

#define SAFE_CALL(status, msg) safe_call(status, msg, __FILE__, __LINE__)

void safe_call(natsStatus status, const char* msg, const char* file, int line);
void nats_connect(natsOptions** r_opts, natsConnection** r_conn, const char* url);
void nats_finalize(natsOptions* opts, natsConnection* conn);