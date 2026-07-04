/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   quote_handler.c                                    :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static int	handle_quote_char(char c, int *q)
{
	if (c == '\'' && *q != Q_DOUBLE)
	{
		if (*q == Q_SINGLE)
			*q = Q_NONE;
		else
			*q = Q_SINGLE;
		return (1);
	}
	if (c == '"' && *q != Q_SINGLE)
	{
		if (*q == Q_DOUBLE)
			*q = Q_NONE;
		else
			*q = Q_DOUBLE;
		return (1);
	}
	return (0);
}

// tirnak karakterlerini silerek temiz bir string donduruyor
static char	*strip_one(char *str)
{
	char	*res;
	int		q;
	int		i;
	int		j;

	res = malloc(ft_strlen(str) + 1);
	if (!res)
		return (NULL);
	q = Q_NONE;
	i = 0;
	j = 0;
	while (str[i])
	{
		if (handle_quote_char(str[i], &q))
			i++;
		else
			res[j++] = str[i++];
	}
	res[j] = '\0';
	return (res);
}

// her tok_word degerinden tirnak karakterlerini temizliyor
void	handle_quotes(t_token *head)
{
	char	*stripped;

	while (head)
	{
		if (head->type == TOK_WORD)
		{
			stripped = strip_one(head->value);
			if (stripped)
			{
				free(head->value);
				head->value = stripped;
			}
		}
		head = head->next;
	}
}
