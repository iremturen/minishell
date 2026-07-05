/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

t_redir	*new_redir(t_token_type type, char *file, int heredoc_expand)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->file = ft_strdup(file);
	redir->delimiter = NULL;
	redir->fd = -1;
	redir->heredoc_expand = heredoc_expand;
	redir->next = NULL;
	if (!redir->file)
	{
		free(redir);
		return (NULL);
	}
	return (redir);
}

int	cmd_add_redir(t_cmd *cmd, t_token_type type, char *file,
		int heredoc_expand)
{
	t_redir	*redir;
	t_redir	*cur;

	redir = new_redir(type, file, heredoc_expand);
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

t_cmd	*new_cmd(void)
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

static void	free_single_cmd(t_cmd *cmd)
{
	t_redir	*redir;
	t_redir	*rnext;
	int		i;

	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
			free(cmd->argv[i++]);
		free(cmd->argv);
	}
	redir = cmd->redirs;
	while (redir)
	{
		rnext = redir->next;
		free(redir->file);
		free(redir->delimiter);
		free(redir);
		redir = rnext;
	}
	free(cmd->cmd_path);
	free(cmd);
}

void	free_cmds(t_cmd *head)
{
	t_cmd	*next;

	while (head)
	{
		next = head->next;
		free_single_cmd(head);
		head = next;
	}
}
