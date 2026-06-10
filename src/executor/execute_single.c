#include "../../minishell.h"

// komut bulunamazsa hata yazdirir ve last_exit i 127 yapar
static void	cmd_not_found(t_cmd *cmd, t_shell *shell)
{
	write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
	write(2, ": command not found\n", 20);
	shell->last_exit = 127;
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
		if (!apply_redirs(cmd->redirs))
			exit(1);
		execve(cmd->cmd_path, cmd->argv, shell->envp);
		perror("execve");
		exit(1);
	}
	waitpid(pid, &status, 0);
	free(cmd->cmd_path);
	cmd->cmd_path = NULL;
	if (WIFEXITED(status))
		shell->last_exit = WEXITSTATUS(status);
}