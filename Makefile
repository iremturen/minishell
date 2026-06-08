NAME		= minishell

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
CPPFLAGS	= -I. -I./Libft
LDFLAGS		= -L./Libft
LDLIBS		= -lft -lreadline

LIBFT		= Libft/libft.a

# ── Init ─────────────────────────────────────────────────
INIT_SRCS	= \
	src/init/init.c

# ── Lexer ────────────────────────────────────────────────
LEXER_SRCS	= \
	src/lexer/lexer.c \
	src/lexer/lexer_utils.c \
	src/lexer/lexer_util.c \
	src/lexer/split_inputs.c \
	src/lexer/error_handling.c

# ── Parser ───────────────────────────────────────────────
PARSER_SRCS	= \
	src/parser/parser.c

# ── Expander ─────────────────────────────────────────────
EXPANDER_SRCS	= \
	src/expander/expander.c \
	src/expander/quote_handler.c

# ── Executor ─────────────────────────────────────────────
EXECUTOR_SRCS	= \
	src/executor/executor.c \
	src/executor/execute_single.c \
	src/executor/execute_pipeline.c \
	src/executor/path_utils.c \
	src/executor/builtins/builtin_executor.c \
	src/executor/builtins/builtin_utils.c

# ── Tümü ─────────────────────────────────────────────────
SRCS	= \
	main.c \
	$(INIT_SRCS) \
	$(LEXER_SRCS) \
	$(PARSER_SRCS) \
	$(EXPANDER_SRCS) \
	$(EXECUTOR_SRCS)

OBJS	= $(SRCS:.c=.o)

all: $(LIBFT) $(NAME)

$(LIBFT):
	$(MAKE) -C Libft

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(NAME)

%.o: %.c minishell.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	$(MAKE) -C Libft clean
	rm -f $(OBJS)

fclean: clean
	$(MAKE) -C Libft fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
