#include "minishell.h"

void execute_cmd(t_cmd *cmd, t_shell *shell)
{
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return;

    if (is_builtin(cmd->argv[0]))
    {
        execute_builtin(cmd, shell);
        return;
    }

    execute_single(cmd, shell->envp);
}