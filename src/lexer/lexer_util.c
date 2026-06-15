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

// tirnaklarin kapanip kapanmadigini kontrol ediyor, acik kalirsa 1 doner
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