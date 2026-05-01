#include <minishell.h>

void execute_builtin(t_cmd *cmd, t_shell *shell)
{
    if (!ft_strcmp(cmd->argv[0], "cd"))
        return (builtin_cd(cmd, shell));
    if (!ft_strcmp(cmd->argv[0], "pwd"))
        return (builtin_pwd());
    if (!ft_strcmp(cmd->argv[0], "echo"))
        return (builtin_echo(cmd->argv));
}