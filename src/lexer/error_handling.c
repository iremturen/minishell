#include "../../minishell.h"

static int	token_is(const char *token, const char *expected)
{
	if (!token || !expected)
		return (0);
	return (ft_strncmp(token, expected, ft_strlen(expected) + 1) == 0);
}

static int	is_pipe_token(const char *token)
{
	return (token_is(token, "|"));
}

static int	is_redir_token(const char *token)
{
	if (token_is(token, ">"))
		return (1);
	if (token_is(token, "<"))
		return (1);
	if (token_is(token, ">>"))
		return (1);
	if (token_is(token, "<<"))
		return (1);
	return (0);
}

int	print_syntax_error(const char *token)
{
	if (!token)
		token = "newline";
	write(2, "minishell: syntax error near unexpected token `", 46);
	write(2, token, ft_strlen(token));
	write(2, "'\n", 2);
	return (2);
}

int	validate_lexer_syntax(char **args)
{
	int	i;

	if (!args || !args[0])
		return (0);
	if (is_pipe_token(args[0]))
		return (print_syntax_error(args[0]));
	i = 0;
	while (args[i])
	{
		if (is_pipe_token(args[i]) && (!args[i + 1] || is_pipe_token(args[i + 1])))
			return (print_syntax_error(args[i]));
		if (is_redir_token(args[i])
			&& (!args[i + 1] || is_pipe_token(args[i + 1])
				|| is_redir_token(args[i + 1])))
			return (print_syntax_error(args[i + 1]));
		i++;
	}
	if (is_pipe_token(args[i - 1]))
		return (print_syntax_error(args[i - 1]));
	return (0);
}
