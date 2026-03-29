#include "../../minishell.h"

char	*add_spaces(char *line)
{
	char	*res;
	int		i;
	int		j;

	i = 0;
	j = 0;
	if (!line)
		return (NULL);
	res = malloc(get_spaced_len(line));
	if (!res)
		return (NULL);
	while (line[i])
	{
		if (line[i] == '>' && line[i + 1] == '>')
		{
			res[j++] = ' ';
			res[j++] = '>';
			res[j++] = '>';
			res[j++] = ' ';
			i += 2;
		}
		else if (line[i] == '<' && line[i + 1] == '<')
		{
			res[j++] = ' ';
			res[j++] = '<';
			res[j++] = '<';
			res[j++] = ' ';
			i += 2;
		}
		else if (is_operator(line[i]))
		{
			res[j++] = ' ';
			res[j++] = line[i];
			res[j++] = ' ';
			i++;
		}
		else
			res[j++] = line[i++];
	}
	res[j] = '\0';
	return (res);
}

char	**split_inputs(char *line)
{
	char	*tmp;
	char	**args;

	tmp = add_spaces(line);
	if (!tmp)
		return (NULL);
	args = ft_split(tmp, ' ');
	free(tmp);
	return (args);
}
