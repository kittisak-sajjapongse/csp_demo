.phony: clean

ALL_TARGETS=csp_server csp_client csp_receiver csp_sender

all: ${ALL_TARGETS}
	@echo "CSP examples Built successfully"

clean:
	rm -f ${ALL_TARGETS}

%: %.c
	gcc $< -I ../libcsp/include -I ../libcsp/build/include -o $@ -L ../libcsp/build -static -lcsp -lsocketcan
