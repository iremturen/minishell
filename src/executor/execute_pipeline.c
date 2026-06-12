#include "../../minishell.h"

// builtin i forked child sureci icinde direkt calistiriyor
static void	exec_builtin_child(t_cmd *cmd, t_shell *shell)
{
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
	exit(shell->last_exit);
}

// child sureci: fd leri ayarlar, redir uygular, komutu calistirip cikiyor
static void	exec_child(t_cmd *cmd, int in_fd, int out_fd, t_shell *shell)
{
	if (in_fd != -1)
	{
		dup2(in_fd, STDIN_FILENO);
		close(in_fd);
	}
	if (out_fd != -1)
	{
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);
	}
	if (!apply_redirs(cmd->redirs))
		exit(1);
	if (is_builtin(cmd->argv[0]))
		exec_builtin_child(cmd, shell);
	cmd->cmd_path = resolve_path(cmd->argv[0], shell->envp);
	if (!cmd->cmd_path)
	{
		write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
		write(2, ": command not found\n", 20);
		exit(127);
	}
	execve(cmd->cmd_path, cmd->argv, shell->envp);
	perror("execve");
	exit(1);
}

// fork yapar, parent ta gereksiz write ucunu kapatir
static pid_t	fork_cmd(t_cmd *cmd, int in_fd, int out_fd, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		exec_child(cmd, in_fd, out_fd, shell);
	if (out_fd != -1)
		close(out_fd);
	return (pid);
}

// tum alt surecleri bekler, son exit kodunu gunceller
static void	wait_cmds(pid_t *pids, int n, t_shell *shell)
{
	int	status;
	int	i;

	i = 0;
	while (i < n)
	{
		waitpid(pids[i], &status, 0);
		if (i == n - 1 && WIFEXITED(status))
			shell->last_exit = WEXITSTATUS(status);
		i++;
	}
}

// pipe zinciri kurar, her komutu fork layip stdin stdout baglantisini sagliyor
void	execute_pipeline(t_cmd *cmds, t_shell *shell)
{
	pid_t	pids[256];
	int		pipefd[2];
	int		prev_fd;
	int		n;
	t_cmd	*cur;

	prev_fd = -1;
	n = 0;
	cur = cmds;
	while (cur)
	{
		if (cur->next && pipe(pipefd) == -1)
			break ;
		pids[n] = fork_cmd(cur, prev_fd, cur->next ? pipefd[1] : -1, shell);
		if (prev_fd != -1)
			close(prev_fd);
		prev_fd = cur->next ? pipefd[0] : -1;
		n++;
		cur = cur->next;
	}
	if (prev_fd != -1)
		close(prev_fd);
	wait_cmds(pids, n, shell);
}
