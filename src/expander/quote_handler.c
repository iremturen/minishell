#include "../../minishell.h"

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
		if (str[i] == '\'' && q != Q_DOUBLE)
		{
			if (q == Q_SINGLE)
				q = Q_NONE;
			else
				q = Q_SINGLE;
			i++;
			continue ;
		}
		if (str[i] == '"' && q != Q_SINGLE)
		{
			if (q == Q_DOUBLE)
				q = Q_NONE;
			else
				q = Q_DOUBLE;
			i++;
			continue ;
		}
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
