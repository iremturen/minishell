#include "../../minishell.h"

int	is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>');
} //başka operatör gelemez mi?

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