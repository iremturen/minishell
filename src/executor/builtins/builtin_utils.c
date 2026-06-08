#include "../../../minishell.h"

void	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	(void)shell;
	if (!cmd->argv[1])
	{
		write(2, "cd: missing argument\n", 21);
		return ;
	}
	if (chdir(cmd->argv[1]) == -1)
		perror("cd");
}

void	builtin_echo(char **argv)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		write(1, argv[i], ft_strlen(argv[i]));
		if (argv[i + 1])
			write(1, " ", 1);
		i++;
	}
	write(1, "\n", 1);
}

void	builtin_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
	{
		write(1, cwd, ft_strlen(cwd));
		write(1, "\n", 1);
	}
	else
		perror("pwd");
}

void	builtin_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		write(1, envp[i], ft_strlen(envp[i]));
		write(1, "\n", 1);
		i++;
	}
}

void	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	(void)cmd;
	(void)shell;
	// TODO: shell->envp içinden ilgili anahtarı sil
}

void	builtin_exit(t_shell *shell)
{
	(void)shell;
	write(1, "exit\n", 5);
	exit(0);
}

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (!ft_strncmp(cmd, "cd", 3))
		return (1);
	if (!ft_strncmp(cmd, "echo", 5))
		return (1);
	if (!ft_strncmp(cmd, "pwd", 4))
		return (1);
	if (!ft_strncmp(cmd, "unset", 6))
		return (1);
	if (!ft_strncmp(cmd, "env", 4))
		return (1);
	if (!ft_strncmp(cmd, "exit", 5))
		return (1);
	return (0);
}