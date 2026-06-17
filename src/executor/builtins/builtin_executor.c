#include "../../../minishell.h"

// stdin ve stdout un kopyasini aliyor, redir uyguluyor
static int	save_and_redir(t_cmd *cmd, int *sin, int *sout, t_shell *shell)
{
	*sin = dup(STDIN_FILENO);
	*sout = dup(STDOUT_FILENO);
	if (*sin == -1 || *sout == -1)
		return (0);
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

// kaydedilen stdin ve stdout u geri yukluyor
static void	restore_fds(int sin, int sout)
{
	dup2(sin, STDIN_FILENO);
	dup2(sout, STDOUT_FILENO);
	close(sin);
	close(sout);
}

// builtin i redir uygulayarak calistiriyor, fd leri geri yukluyor
void	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	int	sin;
	int	sout;

	if (!save_and_redir(cmd, &sin, &sout, shell))
		return ;
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
	restore_fds(sin, sout);
}