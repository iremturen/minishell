/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   builtin_env_ops.c                                  :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../../minishell.h"

int	env_set(t_shell *shell, char *key, char *val)
{
	char	*entry;
	int		idx;

	entry = create_env_entry(key, val);
	if (!entry)
		return (0);
	idx = env_find(shell->envp, key);
	if (idx != -1)
	{
		free(shell->envp[idx]);
		shell->envp[idx] = entry;
		return (1);
	}
	if (!add_new_env(shell, entry))
	{
		free(entry);
		return (0);
	}
	return (1);
}

static char	*get_cd_path(t_cmd *cmd, t_shell *shell)
{
	int	idx;

	if (!cmd->argv[1])
	{
		idx = env_find(shell->envp, "HOME");
		if (idx == -1)
		{
			write(2, "minishell: cd: HOME not set\n", 28);
			return (NULL);
		}
		return (ft_strchr(shell->envp[idx], '=') + 1);
	}
	if (ft_strncmp(cmd->argv[1], "-", 2) == 0)
	{
		idx = env_find(shell->envp, "OLDPWD");
		if (idx == -1)
		{
			write(2, "minishell: cd: OLDPWD not set\n", 30);
			return (NULL);
		}
		return (ft_strchr(shell->envp[idx], '=') + 1);
	}
	return (cmd->argv[1]);
}

void	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	cwd[1024];
	char	old[1024];

	path = get_cd_path(cmd, shell);
	if (!path)
	{
		shell->last_exit = 1;
		return ;
	}
	if (!getcwd(old, sizeof(old)))
	{
		perror("cd: getcwd");
		shell->last_exit = 1;
		return ;
	}
	if (chdir(path) == -1)
	{
		perror("cd");
		shell->last_exit = 1;
		return ;
	}
	env_set(shell, "OLDPWD", old);
	if (getcwd(cwd, sizeof(cwd)))
		env_set(shell, "PWD", cwd);
}

void	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int	idx;
	int	n;
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		idx = env_find(shell->envp, cmd->argv[i]);
		if (idx != -1)
		{
			free(shell->envp[idx]);
			n = idx;
			while (shell->envp[n + 1])
			{
				shell->envp[n] = shell->envp[n + 1];
				n++;
			}
			shell->envp[n] = NULL;
		}
		i++;
	}
}
