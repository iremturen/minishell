/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   lexer_util.c                                       :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

int	is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

size_t	get_spaced_len(char *line)
{
	size_t	i;
	size_t	len;

	i = 0;
	len = 0;
	while (line[i])
	{
		if ((line[i] == '>' && line[i + 1] == '>')
			|| (line[i] == '<' && line[i + 1] == '<'))
		{
			len += 4;
			i += 2;
		}
		else if (is_operator(line[i]))
		{
			len += 3;
			i++;
		}
		else
		{
			len++;
			i++;
		}
	}
	return (len + 1);
}

int	has_unclosed_quotes(char *line)
{
	int	q;
	int	i;

	q = Q_NONE;
	i = 0;
	while (line[i])
	{
		if (line[i] == '\'' && q == Q_NONE)
			q = Q_SINGLE;
		else if (line[i] == '\'' && q == Q_SINGLE)
			q = Q_NONE;
		else if (line[i] == '"' && q == Q_NONE)
			q = Q_DOUBLE;
		else if (line[i] == '"' && q == Q_DOUBLE)
			q = Q_NONE;
		i++;
	}
	return (q != Q_NONE);
}

int	word_end_q(char *s, int i)
{
	int	q;

	q = Q_NONE;
	while (s[i])
	{
		if (s[i] == '\'' && q == Q_NONE)
			q = Q_SINGLE;
		else if (s[i] == '\'' && q == Q_SINGLE)
			q = Q_NONE;
		else if (s[i] == '"' && q == Q_NONE)
			q = Q_DOUBLE;
		else if (s[i] == '"' && q == Q_DOUBLE)
			q = Q_NONE;
		else if (s[i] == ' ' && q == Q_NONE)
			break ;
		i++;
	}
	return (i);
}

int	count_words_q(char *s)
{
	int	i;
	int	n;

	i = 0;
	n = 0;
	while (s[i])
	{
		while (s[i] == ' ')
			i++;
		if (s[i])
		{
			n++;
			i = word_end_q(s, i);
		}
	}
	return (n);
}
