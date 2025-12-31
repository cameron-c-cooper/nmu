CC := gcc
PROJ_ROOT := $(patsubst %/,%,$(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
SRC_DIR := $(PROJ_ROOT)/src
INC_DIR := $(PROJ_ROOT)/include
BUILD_DIR := $(PROJ_ROOT)/build

SRC := $(shell find $(SRC_DIR) -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
BIN := $(BUILD_DIR)/nmu

CWARNINGS		:= -Wall -Wextra -Wpedantic -pedantic-errors -Werror -Waggregate-return \
				   -Wbad-function-cast -Wcast-align -Wcast-qual \
				   -Wfloat-equal -Wformat=2 -Wlogical-op \
				   -Wmissing-declarations -Wmissing-include-dirs -Wmissing-prototypes \
				   -Wnested-externs -Wpointer-arith -Wredundant-decls -Wsequence-point \
				   -Wshadow -Wstrict-prototypes -Wswitch -Wundef -Wunreachable-code \
				   -Wunused-but-set-parameter -Wwrite-strings \
				   -Wno-unused-function -Wno-error=unused-variable
CFLAGS := $(CWARNINGS) -g -I$(INC_DIR)

$(BIN): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)/*	
