/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static t_shell	*g_shell_sig = NULL;

static void	sigint_handler(int sig)
{
	(void)sig;
	if (g_shell_sig)
		g_shell_sig->last_exit = 130;
	write(1, "\n", 1);
	reset_readline_line();
	clear_signal();
}

void	setup_signals_interactive(t_shell *shell)
{
	struct sigaction	sa;

	g_shell_sig = shell;
	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}

void	setup_signals_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
