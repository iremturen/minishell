#include "../../minishell.h"

// yeni bos bir komut dugumu olusturuyor
static t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->cmd_path = NULL;
	cmd->is_builtin = 0;
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

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

// argv dizisini bir buyuk yeni dizi olarak kopyalayip donduruyor
static char	**grow_argv(char **old, int len)
{
	char	**tmp;
	int		i;

	tmp = malloc(sizeof(char *) * (len + 2));
	if (!tmp)
		return (NULL);
	i = 0;
	while (i < len)
	{
		tmp[i] = old[i];
		i++;
	}
	tmp[len] = NULL;
	tmp[len + 1] = NULL;
	return (tmp);
}

// komuta yeni arguman ekliyor, eski argv yi serbest birakiyor
static int	cmd_add_arg(t_cmd *cmd, char *word)
{
	int		len;
	char	**tmp;

	len = 0;
	while (cmd->argv && cmd->argv[len])
		len++;
	tmp = grow_argv(cmd->argv, len);
	if (!tmp)
		return (0);
	tmp[len] = ft_strdup(word);
	if (!tmp[len])
	{
		free(tmp);
		return (0);
	}
	free(cmd->argv);
	cmd->argv = tmp;
	return (1);
}

// tok_word tokenlarini alip cmd->argv ye ekliyor
// pipe ve redir buraya sonra eklenecek
t_cmd	*parse(t_token *tokens)
{
	t_cmd	*cmd;
	t_token	*cur;

	if (!tokens)
		return (NULL);
	cmd = new_cmd();
	if (!cmd)
		return (NULL);
	cur = tokens;
	while (cur)
	{
		if (cur->type == TOK_WORD)
		{
			if (!cmd_add_arg(cmd, cur->value))
			{
				free_cmds(cmd);
				return (NULL);
			}
		}
		cur = cur->next;
	}
	if (!cmd->argv)
	{
		free_cmds(cmd);
		return (NULL);
	}
	return (cmd);
}
