#include "../../minishell.h"

// t_cmd bagli listesini baslindan sonuna kadar temizliyor
void	free_cmds(t_cmd *head)
{
	t_cmd	*next;
	int		i;

	while (head)
	{
		next = head->next;
		if (head->argv)
		{
			i = 0;
			while (head->argv[i])
				free(head->argv[i++]);
			free(head->argv);
		}
		free(head->cmd_path);
		free(head);
		head = next;
	}
}

// henuz parse yok, stub olarak null donuyor
t_cmd	*parse(t_token *tokens)
{
	(void)tokens;
	return (NULL);
}
