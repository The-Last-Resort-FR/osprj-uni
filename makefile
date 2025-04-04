NAME=test.run
CC=gcc
CFLAGS= -Wall -Wextra -g
LDFLAGS=

OBJ_DIR = obj
SRC_DIR = src
INC_DIR = -Iinclude
BIN_DIR = bin

SRCS	= $(wildcard $(SRC_DIR)/*.c)
OBJS	= ${SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o}

LIBS= -lgpiod


all: $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(NAME) $(OBJS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INC_DIR) -c -o $@ $<


.PHONY: clear
clear:
	@rm -rf $(OBJ_DIR)/* $(BIN_DIR)/$(NAME)