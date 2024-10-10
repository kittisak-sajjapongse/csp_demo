.PHONY: clean

# Define source files for each target
PUBLISH_SOURCES=connection.c publish.c
SUBSCRIBE_SOURCES=connection.c subscribe.c
SUBSCRIBE_SYNC_SOURCES=connection.c subscribe_sync.c
SERVER_RESPOND_SOURCES=connection.c server_respond.c
CLIENT_REQUEST_SOURCES=connection.c client_request.c

# Object files generated from the source files, stored in obj/ directory
PUBLISH_OBJECTS=$(patsubst %.c, obj/%.o, $(PUBLISH_SOURCES))
SUBSCRIBE_OBJECTS=$(patsubst %.c, obj/%.o, $(SUBSCRIBE_SOURCES))
SUBSCRIBE_SYNC_OBJECTS=$(patsubst %.c, obj/%.o, $(SUBSCRIBE_SYNC_SOURCES))
SERVER_RESPOND_OBJECTS=$(patsubst %.c, obj/%.o, $(SERVER_RESPOND_SOURCES))
CLIENT_REQUEST_OBJECTS=$(patsubst %.c, obj/%.o, $(CLIENT_REQUEST_SOURCES))

ALL_TARGETS=publish subscribe subscribe_sync server_respond client_request

all: ${ALL_TARGETS}
	@echo "NATS examples Built successfully"

# Create the obj directory if it doesn't exist
obj/%.o: %.c
	@mkdir -p obj
	gcc -c $< -o $@

clean:
	rm -f ${ALL_TARGETS} obj/*.o

# Publish target
publish: $(PUBLISH_OBJECTS)
	gcc $^ -o $@ -L /usr/local/lib -lnats

# Subscribe target
subscribe: $(SUBSCRIBE_OBJECTS)
	gcc $^ -o $@ -L /usr/local/lib -lnats

server_respond: $(SERVER_RESPOND_OBJECTS)
	gcc $^ -o $@ -L /usr/local/lib -lnats

client_request: $(CLIENT_REQUEST_OBJECTS)
	gcc $^ -o $@ -L /usr/local/lib -lnats

# Subscribe synchronous target
subscribe_sync: $(SUBSCRIBE_SYNC_OBJECTS)
	gcc $^ -o $@ -L /usr/local/lib -lnats
