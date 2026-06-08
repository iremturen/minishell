#include "../../../minishell.h"

void	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	if (!ft_strncmp(cmd->argv[0], "cd", 3))
		return (builtin_cd(cmd, shell));
	if (!ft_strncmp(cmd->argv[0], "pwd", 4))
		return (builtin_pwd());
	if (!ft_strncmp(cmd->argv[0], "echo", 5))
		return (builtin_echo(cmd->argv));
	if (!ft_strncmp(cmd->argv[0], "env", 4))
		return (builtin_env(shell->envp));
	if (!ft_strncmp(cmd->argv[0], "unset", 6))
		return (builtin_unset(cmd, shell));
	if (!ft_strncmp(cmd->argv[0], "exit", 5))
		return (builtin_exit(shell));
}