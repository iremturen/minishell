#include "../../minishell.h"

// komut bulunamazsa hata yazdirir ve last_exit i 127 yapar
static void	cmd_not_found(t_cmd *cmd, t_shell *shell)
{
	write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
	write(2, ": command not found\n", 20);
	shell->last_exit = 127;
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
			write(2, "Quit: 3\n", 8);
		else if (WTERMSIG(status) == SIGINT)
			write(1, "\n", 1);
	}
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
	{
		setup_signals_child();
		if (!apply_redirs(cmd->redirs))
			exit(1);
		execve(cmd->cmd_path, cmd->argv, shell->envp);
		perror("execve");
		exit(1);
	}
	waitpid(pid, &status, 0);
	free(cmd->cmd_path);
	cmd->cmd_path = NULL;
	update_exit_status(status, shell);
}