/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   signals_status.c                                   :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static volatile sig_atomic_t	*signal_status(void)
{
	static volatile sig_atomic_t	status = 0;

	return (&status);
}

int	get_signal(void)
{
	return ((int)(*signal_status()));
}

void	set_signal(int sig)
{
	*signal_status() = sig;
}

void	clear_signal(void)
{
	*signal_status() = 0;
}

void	cleanup_readline_history(void)
{
	clear_history();
	rl_clear_history();
}
