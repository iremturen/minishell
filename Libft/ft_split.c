
#include "libft.h"

static char	**free_all(char **res, int j)
{
	while (j >= 0)
	{
		free(res[j]);
		j--;
	}
	free(res);
	return (NULL);
}

static int	word_count(char const *s, char c)
{
	int	count;
	int	i;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] && s[i] == c)
			i++;
		else
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
	}
	return (count);
}

static int	add_word(char **res, char const *s, int len, int *j)
{
	res[*j] = ft_substr(s, 0, len);
	if (!res[*j])
	{
		free_all(res, *j - 1);
		return (0);
	}
	(*j)++;
	return (1);
}

char	**ft_split(char const *s, char c)
{
	char	**res;
	int		i;
	int		j;
	int		start;

	res = malloc(sizeof(char *) * (word_count(s, c) + 1));
	j = 0;
	i = 0;
	if (!s || !res)
		return (NULL);
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		start = i;
		while (s[i] && s[i] != c)
			i++;
		if (i > start)
		{
			if (!add_word(res, s + start, i - start, &j))
				return (NULL);
		}
	}
	res[j] = NULL;
	return (res);
}
