#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "Libft/libft.h"

int		is_operator(char c);
char	**split_inputs(char *line);
size_t	get_spaced_len(char *line);
int		print_syntax_error(const char *token);
int		validate_lexer_syntax(char **args);

#endif
