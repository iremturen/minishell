/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   error_handling.c                                   :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/05 18:00:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static int	token_is(const char *token, const char *expected)
{
	if (!token || !expected)
		return (0);
	return (ft_strncmp(token, expected, ft_strlen(expected) + 1) == 0);
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

static int	check_args_syntax(char **args, int i)
{
	if (token_is(args[i], "|"))
	{
		if (!args[i + 1] || token_is(args[i + 1], "|"))
		{
			if (args[i + 1] && token_is(args[i + 1], "|"))
				return (print_syntax_error("||"));
			return (print_syntax_error(args[i]));
		}
	}
	if (is_redir_token(args[i])
		&& (!args[i + 1] || token_is(args[i + 1], "|")
			|| is_redir_token(args[i + 1])))
		return (print_syntax_error(args[i + 1]));
	return (0);
}

int	validate_lexer_syntax(char **args)
{
	int	i;
	int	err;

	if (!args || !args[0])
		return (0);
	if (token_is(args[0], "|"))
	{
		if (args[1] && token_is(args[1], "|"))
			return (print_syntax_error("||"));
		return (print_syntax_error(args[0]));
	}
	i = 0;
	while (args[i])
	{
		err = check_args_syntax(args, i);
		if (err)
			return (err);
		i++;
	}
	if (token_is(args[i - 1], "|"))
		return (print_syntax_error(args[i - 1]));
	return (0);
}
