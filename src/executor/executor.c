#include "../../minishell.h"

// redir-only komut: > file gibi. child fork edip rediri uygulayip cikiyor
static void	execute_redir_only(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
	{
		setup_signals_child();
		if (!apply_redirs(cmd->redirs, shell))
			exit(1);
		exit(0);
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

// cmd->next varsa pipeline a yonlendir, yoksa tek komut calistir
void	execute_cmd(t_cmd *cmd, t_shell *shell)
{
	if (!cmd)
		return ;
	if (!cmd->argv || !cmd->argv[0])
	{
		if (cmd->redirs)
			execute_redir_only(cmd, shell);
		return ;
	}
	if (cmd->next)
	{
		execute_pipeline(cmd, shell);
		return ;
	}
	if (is_builtin(cmd->argv[0]))
	{
		execute_builtin(cmd, shell);
		return ;
	}
	execute_single(cmd, shell);
}