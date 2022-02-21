BUILD_DIR = ./build/

CC = gcc

BUILD_CMD = $(CC) $? -o $(BUILD_DIR)/$@

all: buildDir executable

buildDir:
	mkdir -p $(BUILD_DIR)

executable:
	chmod +x -R $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

servers: buildDir inet_server inet_server_fork inet_server_select inet_server_poll inet_server_sock_shutdown inet_client_read_notifyserver

inet_server: inet_server.c
	$(BUILD_CMD)

inet_server_sock_shutdown: inet_server.c
	$(BUILD_CMD) -D_SOCK_SHUTDOWN

inet_server_fork: inet_server_fork.c
	$(BUILD_CMD)

inet_server_select: inet_server_select.c
	$(BUILD_CMD)

inet_server_poll: inet_server_poll.c
	$(BUILD_CMD)

inet_server_epoll: inet_server_epoll.c
	$(BUILD_CMD)

clients: buildDir inet_client inet_client_read inet_client_read_keepalive inet_client_read_noblock inet_client_read_notifyserver inet_client_recv

inet_client_recv: inet_client.c
	$(BUILD_CMD)

inet_client_read: inet_client.c
	$(BUILD_CMD) -D_OP_READ -D_OP_READ

inet_client_read_keepalive:inet_client.c
	$(BUILD_CMD) -D_OP_READ -D_SET_KEEPALIVE

inet_client_read_notifyserver:inet_client.c
	$(BUILD_CMD) -D_OP_READ -D_NOTIFY_SERVER

inet_client_read_noblock: inet_client.c
	$(BUILD_CMD) -D_OP_READ -D_NON_BLOCK

thundering_herd_accept: thundering_herd_accept.c
	$(BUILD_CMD)

thundering_herd_select: thundering_herd_select.c
	$(BUILD_CMD)
