CC=gcc
CFLAGS=-Wall -Werror -Wextra -O2 -fsanitize=address -fno-omit-frame-pointer -g3
SRC_DIR = src
OBJ_DIR = obj

# c src files
SRCS = $(wildcard $(SRC_DIR)/*.c)
# c obj files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

HEADERS = $(wildcard $(SRC_DIR)/headers/*.h)

# executable
EXEC = duplicates

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJS)
	rmdir $(OBJ_DIR)

fullclean: clean
	rm -f $(EXEC)

.PHONY: check-leaks

DIRS ?= test1 test2

check-leaks: $(EXEC)
	valgrind -s --leak-check=full ./$(EXEC) $(DIRS) -a -r