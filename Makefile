CC := gcc

NAME := interpreter
SRC_DIR := src
BIN_DIR := bin
BUILD_DIR := build

OBJS := main.o virtualmachine.o virtualmachine_goto.o

$(NAME): dir $(OBJS)
	$(CC) -o $(BIN_DIR)/$(NAME) $(foreach file,$(OBJS),$(BUILD_DIR)/$(file))

%.o: dir $(SRC_DIR)/%.c
	$(CC) -o $(BUILD_DIR)/$*.o -c $(SRC_DIR)/$*.c

dir:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

clean:
	@rd $(BUILD_DIR) -r -fo
	@rd $(BIN_DIR) -r -fo

.PHONY: dir clean

