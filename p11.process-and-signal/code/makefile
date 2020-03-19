BUILD_DIR = ./build/

CC = gcc

BUILD_CMD = $(CC) $? -o $(BUILD_DIR)/$@

all: make_dir sigint_test_1 executable

make_dir:
	mkdir -p $(BUILD_DIR)

executable:
	chmod +x -R $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

sigint_test_1: sigint_test.v1.c
	$(BUILD_CMD)

sigint_test_2: sigint_test.v2.c
	$(BUILD_CMD)

sigint_test_3: sigint_test.v3.c
	$(BUILD_CMD)

sigint_test_4: sigint_test.v4.c
	$(BUILD_CMD)

sigint_test_5: sigint_test.v5.c
	$(BUILD_CMD)

sigint_test_6: sigint_test.v6.c
	$(BUILD_CMD)