/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   split_inputs.c                                     :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static int	handle_op(char *line, char *res, int *j, int i)
{
	if (line[i] == '>' && line[i + 1] == '>')
	{
		res[(*j)++] = ' ';
		res[(*j)++] = '>';
		res[(*j)++] = '>';
		res[(*j)++] = ' ';
		return (2);
	}
	if (line[i] == '<' && line[i + 1] == '<')
	{
		res[(*j)++] = ' ';
		res[(*j)++] = '<';
		res[(*j)++] = '<';
		res[(*j)++] = ' ';
		return (2);
	}
	res[(*j)++] = ' ';
	res[(*j)++] = line[i];
	res[(*j)++] = ' ';
	return (1);
}

static int	update_quote(char c, int q)
{
	if (c == '\'' && q == Q_NONE)
		return (Q_SINGLE);
	if (c == '\'' && q == Q_SINGLE)
		return (Q_NONE);
	if (c == '"' && q == Q_NONE)
		return (Q_DOUBLE);
	if (c == '"' && q == Q_DOUBLE)
		return (Q_NONE);
	return (q);
}

static char	*add_spaces(char *line)
{
	char	*res;
	int		q;
	int		i;
	int		j;

	q = Q_NONE;
	res = malloc(get_spaced_len(line));
	if (!res)
		return (NULL);
	i = 0;
	j = 0;
	while (line[i])
	{
		q = update_quote(line[i], q);
		if (q == Q_NONE && is_operator(line[i]))
			i += handle_op(line, res, &j, i);
		else
			res[j++] = line[i++];
	}
	res[j] = '\0';
	return (res);
}

static char	**split_q(char *s)
{
	char	**arr;
	int		n;
	int		i;
	int		w;

	n = count_words_q(s);
	arr = ft_calloc(n + 1, sizeof(char *));
	if (!arr)
		return (NULL);
	i = 0;
	w = 0;
	while (s[i] && w < n)
	{
		while (s[i] == ' ')
			i++;
		arr[w] = ft_substr(s, i, word_end_q(s, i) - i);
		if (!arr[w])
		{
			free_array(arr);
			return (NULL);
		}
		i = word_end_q(s, i);
		w++;
	}
	return (arr);
}

char	**split_inputs(char *line)
{
	char	*tmp;
	char	**args;

	tmp = add_spaces(line);
	if (!tmp)
		return (NULL);
	args = split_q(tmp);
	free(tmp);
	return (args);
}
