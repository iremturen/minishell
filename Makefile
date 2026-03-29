NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror
CPPFLAGS = -I. -I./Libft
LDFLAGS =
LDLIBS = -lreadline

SRCS = \
	main.c \
	src/lexer/split_inputs.c \
	src/lexer/lexer_util.c \
	src/lexer/error_handling.c \
	Libft/ft_split.c \
	Libft/ft_substr.c \
	Libft/ft_strlen.c \
	Libft/ft_strncmp.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(NAME)

%.o: %.c minishell.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
