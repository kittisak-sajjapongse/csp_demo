.phony: clean

ALL_TARGETS=connect

all: ${ALL_TARGETS}
	@echo "NATS examples Built successfully"

clean:
	rm -f ${ALL_TARGETS}

%: %.c
	gcc $< -o $@ -L /usr/local/lib -lnats
