#include "../../minishell.h"

// cmd->next varsa pipeline a yonlendir, yoksa tek komut calistir
void	execute_cmd(t_cmd *cmd, t_shell *shell)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
		return ;
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