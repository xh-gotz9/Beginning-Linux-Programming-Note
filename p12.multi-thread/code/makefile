BUILD_DIR = ./build/

CC = gcc

BUILD_CMD = $(CC) $? -o $(BUILD_DIR)/$@

BUILD_CMD_THREAD = $(BUILD_CMD) -lpthread

all: make_dir executable

make_dir:
	mkdir -p $(BUILD_DIR)

executable:
	chmod +x -R $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

thread_test_1: thread_test_1.c
	$(BUILD_CMD_THREAD)