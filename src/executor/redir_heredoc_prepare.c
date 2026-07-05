/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   redir_heredoc_prepare.c                            :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/05 18:00:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

void	close_heredoc_fds(t_cmd *cmd)
{
	t_redir	*r;

	while (cmd)
	{
		r = cmd->redirs;
		while (r)
		{
			if (r->type == TOK_HEREDOC && r->fd >= 0)
			{
				close(r->fd);
				r->fd = -1;
			}
			r = r->next;
		}
		cmd = cmd->next;
	}
}

int	prepare_heredocs(t_cmd *cmd, t_shell *shell)
{
	t_cmd	*start;
	t_redir	*r;

	start = cmd;
	while (cmd)
	{
		r = cmd->redirs;
		while (r)
		{
			if (r->type == TOK_HEREDOC && process_heredoc(r, shell) != 0)
			{
				close_heredoc_fds(start);
				return (0);
			}
			r = r->next;
		}
		cmd = cmd->next;
	}
	return (1);
}
