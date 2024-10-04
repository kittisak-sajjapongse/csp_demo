.PHONY: clean

# Define source files for each target
PUBLISH_SOURCES=connection.c publish.c
SUBSCRIBE_SOURCES=connection.c subscribe.c
SUBSCRIBE_SYNC_SOURCES=connection.c subscribe_sync.c

# Object files generated from the source files, stored in obj/ directory
PUBLISH_OBJECTS=$(patsubst %.c, obj/%.o, $(PUBLISH_SOURCES))
SUBSCRIBE_OBJECTS=$(patsubst %.c, obj/%.o, $(SUBSCRIBE_SOURCES))
SUBSCRIBE_SYNC_OBJECTS=$(patsubst %.c, obj/%.o, $(SUBSCRIBE_SYNC_SOURCES))

ALL_TARGETS=publish subscribe subscribe_sync

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

# Subscribe synchronous target
subscribe_sync: $(SUBSCRIBE_SYNC_OBJECTS)
	gcc $^ -o $@ -L /usr/local/lib -lnats
