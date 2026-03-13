# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = ./includes
SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:src/%.c=obj/%.o)
NAME = redic

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean all