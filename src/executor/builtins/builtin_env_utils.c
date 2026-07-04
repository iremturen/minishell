/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   builtin_env_utils.c                                :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../../minishell.h"

int	env_find(char **envp, char *key)
{
	size_t	len;
	int		i;

	len = ft_strlen(key);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], key, len) == 0
			&& (envp[i][len] == '=' || envp[i][len] == '\0'))
			return (i);
		i++;
	}
	return (-1);
}

char	*create_env_entry(char *key, char *val)
{
	char	*tmp;
	char	*entry;

	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (NULL);
	entry = ft_strjoin(tmp, val);
	free(tmp);
	return (entry);
}

int	add_new_env(t_shell *shell, char *entry)
{
	char	**new;
	int		n;

	n = 0;
	while (shell->envp[n])
		n++;
	new = ft_calloc(n + 2, sizeof(char *));
	if (!new)
		return (0);
	ft_memcpy(new, shell->envp, sizeof(char *) * n);
	new[n] = entry;
	free(shell->envp);
	shell->envp = new;
	return (1);
}
