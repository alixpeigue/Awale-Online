OBJ-DIR = obj
SRC-DIR = src
INCLUDE-DIR = include
SRV-DIR = server
CLI-DIR = client
SRV-OBJ-DIR = $(SRV-DIR)/$(OBJ-DIR)
CLI-OBJ-DIR = $(CLI-DIR)/$(OBJ-DIR)
SRV-SRC-DIR = $(SRV-DIR)/$(SRC-DIR)
CLI-SRC-DIR = $(CLI-DIR)/$(SRC-DIR)
SRV-INCLUDE-DIR = $(SRV-DIR)/$(INCLUDE-DIR)
CLI-INCLUDE-DIR = $(SRV-DIR)/$(INCLUDE-DIR)

# File names
SRV-OUT = server
CLI-OUT = client
CLI-SRCS = $(wildcard $(CLI-SRC-DIR)/*.c)
SRV-SRCS = $(wildcard $(SRV-SRC-DIR)/*.c)
SRV-OBJS = $(SRV-SRCS:$(SRV-SRC-DIR)/%.c=$(SRV-OBJ-DIR)/%.o)
CLI-OBJS = $(CLI-SRCS:$(CLI-SRC-DIR)/%.c=$(CLI-OBJ-DIR)/%.o)

# Commands
CC = gcc
CCFLAGS = -ansi -Wall -Wextra -Dlinux
LDFLAGS = 
DEBUGFLAGS = -DMAP -g
RM = rm -f
ECHO = echo
LIBRAIRIES =

.PHONY: all clean fclean re run debug

all: $(SRV-OUT) $(CLI-OUT)

$(SRV-OUT): $(SRV-OBJS)
	$(CC) $(LDFLAGS) $(SRV-OBJS) -o $(SRV-OUT) $(LIBRAIRIES)

$(CLI-OUT): $(CLI-OBJS)
	$(CC) $(LDFLAGS) $(CLI-OBJS) -o $(CLI-OUT) $(LIBRAIRIES)

$(SRV-OBJ-DIR)/%.o: $(SRV-SRC-DIR)/%.c | $(SRV-OBJ-DIR)
	$(CC) $(CCFLAGS) -I$(SRV-INCLUDE-DIR) -c -o $@ $<

$(CLI-OBJ-DIR)/%.o: $(CLI-SRC-DIR)/%.c | $(CLI-OBJ-DIR)
	$(CC) $(CCFLAGS) -I$(CLI-INCLUDE-DIR) -c -o $@ $<

%/$(OBJ-DIR):
	mkdir -p $@

debug:
	make re CCFLAGS="$(CCFLAGS) $(DEBUGFLAGS)"

clean:
	$(RM) -r $(SRV-OBJ-DIR) $(CLI-OBJ-DIR) $(SRV-OUT) $(CLI-OUT)

re: clean all

run: $(NAME)
	./$(NAME)

-include $(OBJS:.o=.d)
