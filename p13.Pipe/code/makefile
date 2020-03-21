BUILD_DIR = ./build/

CC = gcc

BUILD_CMD = $(CC) $? -o $(BUILD_DIR)/$@

all: make_dir echo data executable

make_dir:
	mkdir -p $(BUILD_DIR)

executable:
	chmod +x -R $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

pipe_read: pipe_read.c
	$(BUILD_CMD)

pipe_write: pipe_write.c
	$(BUILD_CMD)

sys_pipe_test: sys_pipe_test.c
	$(BUILD_CMD)

progress_fd_test: progress_fd_test.c
	$(BUILD_CMD)