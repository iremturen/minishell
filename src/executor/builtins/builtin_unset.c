/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/05 18:00:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../../minishell.h"

static int	is_valid_key(char *key)
{
	int	i;

	if (!key || !key[0])
		return (0);
	if (!ft_isalpha((unsigned char)key[0]) && key[0] != '_')
		return (0);
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum((unsigned char)key[i]) && key[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static void	remove_env_at(t_shell *shell, int idx)
{
	int	n;

	free(shell->envp[idx]);
	n = idx;
	while (shell->envp[n + 1])
	{
		shell->envp[n] = shell->envp[n + 1];
		n++;
	}
	shell->envp[n] = NULL;
}

static int	unset_one(char *key, t_shell *shell)
{
	int	idx;

	if (!is_valid_key(key))
	{
		write(2, "minishell: unset: '", 19);
		write(2, key, ft_strlen(key));
		write(2, "': not a valid identifier\n", 26);
		return (1);
	}
	idx = env_find(shell->envp, key);
	if (idx != -1)
		remove_env_at(shell, idx);
	return (0);
}

void	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	err;

	err = 0;
	i = 1;
	while (cmd->argv[i])
	{
		if (unset_one(cmd->argv[i], shell))
			err = 1;
		i++;
	}
	if (err)
		shell->last_exit = 1;
}
