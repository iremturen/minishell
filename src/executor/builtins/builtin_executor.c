/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   builtin_executor.c                                 :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../../minishell.h"

static int	save_and_redir(t_cmd *cmd, int *sin, int *sout, t_shell *shell)
{
	*sin = dup(STDIN_FILENO);
	*sout = dup(STDOUT_FILENO);
	if (*sin == -1 || *sout == -1)
	{
		if (*sin != -1)
			close(*sin);
		if (*sout != -1)
			close(*sout);
		return (0);
	}
	if (cmd->redirs && !apply_redirs(cmd->redirs, shell))
	{
		dup2(*sin, STDIN_FILENO);
		dup2(*sout, STDOUT_FILENO);
		close(*sin);
		close(*sout);
		return (0);
	}
	return (1);
}

static void	restore_fds(int sin, int sout)
{
	dup2(sin, STDIN_FILENO);
	dup2(sout, STDOUT_FILENO);
	close(sin);
	close(sout);
}

void	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	int	sin;
	int	sout;

	if (!save_and_redir(cmd, &sin, &sout, shell))
	{
		shell->last_exit = 1;
		return ;
	}
	if (!ft_strncmp(cmd->argv[0], "exit", 5))
		builtin_exit(cmd, shell);
	else
	{
		shell->last_exit = 0;
		if (!ft_strncmp(cmd->argv[0], "echo", 5))
			builtin_echo(cmd->argv);
		else if (!ft_strncmp(cmd->argv[0], "pwd", 4))
			builtin_pwd();
		else if (!ft_strncmp(cmd->argv[0], "env", 4))
			builtin_env(shell->envp);
		else if (!ft_strncmp(cmd->argv[0], "cd", 3))
			builtin_cd(cmd, shell);
		else if (!ft_strncmp(cmd->argv[0], "unset", 6))
			builtin_unset(cmd, shell);
		else if (!ft_strncmp(cmd->argv[0], "export", 7))
			builtin_export(cmd, shell);
	}
	restore_fds(sin, sout);
}
