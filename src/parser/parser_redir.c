#include "../../minishell.h"

// yeni bir yonlendirme dugumu olusturuyor
t_redir	*new_redir(t_token_type type, char *file)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->file = ft_strdup(file);
	redir->delimiter = NULL;
	redir->fd = -1;
	redir->next = NULL;
	if (!redir->file)
	{
		free(redir);
		return (NULL);
	}
	return (redir);
}

// komutun redir listesinin sonuna yeni redir ekliyor
int	cmd_add_redir(t_cmd *cmd, t_token_type type, char *file)
{
	t_redir	*redir;
	t_redir	*cur;

	redir = new_redir(type, file);
	if (!redir)
		return (0);
	if (!cmd->redirs)
	{
		cmd->redirs = redir;
		return (1);
	}
	cur = cmd->redirs;
	while (cur->next)
		cur = cur->next;
	cur->next = redir;
	return (1);
}
