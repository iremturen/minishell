/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   execute_single.c                                   :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

// komut yok ya da izinsiz: 127/126 ile hata yazdirir
static void	cmd_not_found(t_cmd *cmd, t_shell *shell)
{
	write(2, "minishell: ", 11);
	write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
	if (ft_strchr(cmd->argv[0], '/') && access(cmd->argv[0], F_OK) == 0)
	{
		write(2, ": Permission denied\n", 20);
		shell->last_exit = 126;
	}
	else
	{
		write(2, ": command not found\n", 20);
		shell->last_exit = 127;
	}
}

// child bitince exit durumunu last_exit e yazar
static void	update_exit_status(int status, t_shell *shell)
{
	if (WIFEXITED(status))
		shell->last_exit = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		shell->last_exit = 128 + WTERMSIG(status);
		if (WTERMSIG(status) == SIGQUIT)
			write(2, "Quit (core dumped)\n", 19);
		else if (WTERMSIG(status) == SIGINT)
			write(1, "\n", 1);
	}
}

static void	exec_single_child(t_cmd *cmd, t_shell *shell)
{
	setup_signals_child();
	if (!apply_redirs(cmd->redirs, shell))
		exit(1);
	exec_or_exit(cmd, shell->envp);
}

// tek bir dis komutu fork ile calistiriyor, last_exit i gunceller
void	execute_single(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	if (!cmd || !cmd->argv)
		return ;
	cmd->cmd_path = resolve_path(cmd->argv[0], shell->envp);
	if (!cmd->cmd_path)
	{
		cmd_not_found(cmd, shell);
		return ;
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(cmd->cmd_path);
		cmd->cmd_path = NULL;
		return ;
	}
	if (pid == 0)
		exec_single_child(cmd, shell);
	waitpid(pid, &status, 0);
	free(cmd->cmd_path);
	cmd->cmd_path = NULL;
	update_exit_status(status, shell);
}
