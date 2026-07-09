/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../../minishell.h"

static int	is_numeric(char *s)
{
	int	i;

	i = 0;
	if (s[i] == '-' || s[i] == '+')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}

void	builtin_exit(t_cmd *cmd, t_shell *shell)
{
	write(1, "exit\n", 5);
	if (!cmd->argv[1])
		child_exit(shell->last_exit, shell);
	if (!is_numeric(cmd->argv[1]))
	{
		write(2, "minishell: exit: ", 17);
		write(2, cmd->argv[1], ft_strlen(cmd->argv[1]));
		write(2, ": numeric argument required\n", 28);
		child_exit(2, shell);
	}
	if (cmd->argv[2])
	{
		write(2, "minishell: exit: too many arguments\n", 36);
		shell->last_exit = 1;
		return ;
	}
	child_exit((unsigned char)ft_atoi(cmd->argv[1]), shell);
}
