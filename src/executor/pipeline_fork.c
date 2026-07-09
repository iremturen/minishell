/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   pipeline_fork.c                                    :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static void	exec_builtin_child(t_cmd *cmd, t_shell *shell)
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
	else if (!ft_strncmp(cmd->argv[0], "exit", 5))
		builtin_exit(cmd, shell);
	child_exit(shell->last_exit, shell);
}

static void	exec_child(t_cmd *cmd, t_fork_data *data)
{
	setup_signals_child();
	if (data->in_fd != -1)
	{
		dup2(data->in_fd, STDIN_FILENO);
		close(data->in_fd);
	}
	if (data->out_fd != -1)
	{
		dup2(data->out_fd, STDOUT_FILENO);
		close(data->out_fd);
	}
	if (!apply_redirs(cmd->redirs, data->shell))
		child_exit(1, data->shell);
	if (is_builtin(cmd->argv[0]))
		exec_builtin_child(cmd, data->shell);
	cmd->cmd_path = resolve_path(cmd->argv[0], data->shell->envp);
	if (!cmd->cmd_path)
	{
		write(2, "minishell: ", 11);
		write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
		write(2, ": command not found\n", 20);
		child_exit(127, data->shell);
	}
	exec_or_exit(cmd, data->shell);
}

pid_t	pipeline_fork(t_cmd *cmd, t_fork_data *data)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		if (data->out_fd != -1)
			close(data->out_fd);
		return (-1);
	}
	if (pid == 0)
	{
		if (data->close_fd != -1)
			close(data->close_fd);
		exec_child(cmd, data);
	}
	if (data->out_fd != -1)
		close(data->out_fd);
	return (pid);
}
