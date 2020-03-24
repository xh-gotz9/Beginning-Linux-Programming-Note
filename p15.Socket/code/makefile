BUILD_DIR = ./build/

CC = gcc

BUILD_CMD = $(CC) $? -o $(BUILD_DIR)/$@

all: make_dir executable

make_dir:
	mkdir -p $(BUILD_DIR)

executable:
	chmod +x -R $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

inet_server: inet_server.c
	$(BUILD_CMD)

inet_server_sock_shutdown: inet_server.c
	$(BUILD_CMD) -D_SOCK_SHUTDOWN

inet_client_recv: inet_client.c
	$(BUILD_CMD)

inet_client_read: inet_client.c
	$(BUILD_CMD) -D_OP_READ

inet_client_read_keepalive:inet_client.c
	$(BUILD_CMD) -D_OP_READ -D_SET_KEEPALIVE
