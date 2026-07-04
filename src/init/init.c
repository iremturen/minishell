/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

// shell->envp icin hafiza ayiriyoruz, envp'nin uzunlugunu sayiyoruz
static int	copy_envp(t_shell *shell, char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
		i++;
	shell->envp = malloc(sizeof(char *) * (i + 1));
	if (!shell->envp)
		return (0);
	i = 0;
	while (envp[i])
	{
		shell->envp[i] = ft_strdup(envp[i]);
		if (!shell->envp[i])
			return (0);
		i++;
	}
	shell->envp[i] = NULL;
	return (1);
}

// t_shell yapisini hazir hale getiriyor, envp'yi kopyaliyor
t_shell	*init_shell(char **envp)
{
	t_shell	*shell;

	shell = malloc(sizeof(t_shell));
	if (!shell)
		return (NULL);
	shell->envp = NULL;
	shell->last_exit = 0;
	if (!copy_envp(shell, envp))
	{
		free_shell(shell);
		return (NULL);
	}
	return (shell);
}

// hem envp dizisini hem de shell yapisini tamamen siliyor
void	free_shell(t_shell *shell)
{
	int	i;

	if (!shell)
		return ;
	if (shell->envp)
	{
		i = 0;
		while (shell->envp[i])
			free(shell->envp[i++]);
		free(shell->envp);
	}
	free(shell);
}

void	print_prompt_if_needed(void)
{
	if (!isatty(STDIN_FILENO))
	{
		write(1, "minishell$ ", 11);
		write(1, "\n", 1);
	}
}
