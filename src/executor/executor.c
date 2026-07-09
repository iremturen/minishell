/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static void	execute_redir_only(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
	{
		setup_signals_child();
		if (!apply_redirs(cmd->redirs, shell))
			child_exit(1, shell);
		child_exit(0, shell);
	}
	if (pid == -1)
	{
		perror("fork");
		return ;
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		shell->last_exit = WEXITSTATUS(status);
}

static int	has_heredoc(t_cmd *cmd)
{
	t_redir	*r;

	while (cmd)
	{
		r = cmd->redirs;
		while (r)
		{
			if (r->type == TOK_HEREDOC)
				return (1);
			r = r->next;
		}
		cmd = cmd->next;
	}
	return (0);
}

void	execute_cmd(t_cmd *cmd, t_shell *shell)
{
	if (!cmd)
		return ;
	if (has_heredoc(cmd) && !prepare_heredocs(cmd, shell))
		return ;
	if (!cmd->argv || !cmd->argv[0])
	{
		if (cmd->redirs)
			execute_redir_only(cmd, shell);
		close_heredoc_fds(cmd);
		return ;
	}
	if (cmd->next)
	{
		execute_pipeline(cmd, shell);
		close_heredoc_fds(cmd);
		return ;
	}
	if (is_builtin(cmd->argv[0]))
	{
		execute_builtin(cmd, shell);
		close_heredoc_fds(cmd);
		return ;
	}
	execute_single(cmd, shell);
	close_heredoc_fds(cmd);
}
