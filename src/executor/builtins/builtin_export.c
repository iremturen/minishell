/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../../minishell.h"

static void	print_export_entry(char *entry)
{
	char	*eq;

	write(1, "declare -x ", 11);
	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		write(1, entry, ft_strlen(entry));
		write(1, "\n", 1);
		return ;
	}
	write(1, entry, eq - entry + 1);
	write(1, "\"", 1);
	write(1, eq + 1, ft_strlen(eq + 1));
	write(1, "\"\n", 2);
}

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

static void	print_export_error(char *arg)
{
	write(2, "minishell: export: '", 20);
	write(2, arg, ft_strlen(arg));
	write(2, "': not a valid identifier\n", 26);
}

static int	do_export_one(char *arg, t_shell *shell)
{
	char	*eq;
	char	*key;

	eq = ft_strchr(arg, '=');
	if (!eq)
	{
		if (!is_valid_key(arg))
			return (print_export_error(arg), 1);
		return (!env_set(shell, arg, ""));
	}
	key = ft_substr(arg, 0, eq - arg);
	if (!key)
		return (1);
	if (!is_valid_key(key))
	{
		print_export_error(arg);
		free(key);
		return (1);
	}
	env_set(shell, key, eq + 1);
	free(key);
	return (0);
}

void	builtin_export(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	err;

	if (!cmd->argv[1])
	{
		i = 0;
		while (shell->envp[i])
		{
			print_export_entry(shell->envp[i]);
			i++;
		}
		return ;
	}
	err = 0;
	i = 1;
	while (cmd->argv[i])
	{
		if (do_export_one(cmd->argv[i], shell))
			err = 1;
		i++;
	}
	if (err)
		shell->last_exit = 1;
}
