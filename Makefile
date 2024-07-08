CC=gcc
CFLAGS=-g -Wall -Werror

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

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fullclean: clean
	rm -f $(EXEC)

.PHONY: check-leaks

DIRS ?= test1 test2

check-leaks: $(EXEC)
	valgrind -s --leak-check=full ./$(EXEC) $(DIRS)