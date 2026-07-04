/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../../minishell.h"

// echo: ardisik -n bayraklarini atlar, kalan argumanlari yazar
void	builtin_echo(char **argv)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (argv[i] && ft_strncmp(argv[i], "-n", 3) == 0)
	{
		newline = 0;
		i++;
	}
	while (argv[i])
	{
		write(1, argv[i], ft_strlen(argv[i]));
		if (argv[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
}

// pwd: gecerli calisma dizinini yazıyor
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

// env: envp deki tum degiskenleri yazıyor
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

// is_builtin: komut adinin builtin olup olmadigini kontrol ediyor
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (!ft_strncmp(cmd, "echo", 5) || !ft_strncmp(cmd, "pwd", 4))
		return (1);
	if (!ft_strncmp(cmd, "env", 4) || !ft_strncmp(cmd, "exit", 5))
		return (1);
	if (!ft_strncmp(cmd, "cd", 3) || !ft_strncmp(cmd, "unset", 6))
		return (1);
	if (!ft_strncmp(cmd, "export", 7))
		return (1);
	return (0);
}
