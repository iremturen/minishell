/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:02:56 by azkaraka          #+#    #+#             */
/*   Updated: 2025/07/01 04:27:54 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(const char *s, char c)
{
	int	words;

	words = 0;
	while (*s)
	{
		while (*s == c)
			s++;
		if (*s)
		{
			words++;
			while (*s && *s != c)
				s++;
		}
	}
	return (words);
}

static void	free_all(char **res, int filled)
{
	int	i;

	i = 0;
	while (i < filled)
		free(res[i++]);
	free(res);
}

static char	**split_words(char **res, char const *s, char c, int word_count)
{
	int	i;
	int	word_len;

	i = 0;
	while (*s && i < word_count)
	{
		while (*s && *s == c)
			s++;
		word_len = 0;
		while (s[word_len] && s[word_len] != c)
			word_len++;
		if (word_len > 0)
		{
			res[i] = ft_substr(s, 0, word_len);
			if (!res[i])
			{
				free_all(res, i);
				return (NULL);
			}
			i++;
			s += word_len;
		}
	}
	res[i] = NULL;
	return (res);
}

char	**ft_split(char const *s, char c)
{
	int		word_count;
	char	**res;

	if (s == 0)
		return (0);
	word_count = count_words(s, c);
	res = (char **)malloc(sizeof(char *) * (word_count + 1));
	if (!res)
		return (0);
	if (!split_words(res, s, c, word_count))
		return (NULL);
	return (res);
}
