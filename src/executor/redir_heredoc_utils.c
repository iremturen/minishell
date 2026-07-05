/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   redir_heredoc_utils.c                              :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/05 18:00:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static struct sigaction	g_hd_old_sa;

static void	heredoc_sigint(int sig)
{
	(void)sig;
	set_signal(SIGINT);
	write(1, "\n", 1);
}

void	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = heredoc_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, &g_hd_old_sa);
	signal(SIGQUIT, SIG_IGN);
}

void	restore_heredoc_signals(t_shell *shell)
{
	sigaction(SIGINT, &g_hd_old_sa, NULL);
	setup_signals_interactive(shell);
}

void	print_eof_warning(char *delim)
{
	write(2, "minishell: warning: here-document ", 34);
	write(2, "delimited by end-of-file (wanted `", 34);
	write(2, delim, ft_strlen(delim));
	write(2, "')\n", 3);
}

void	write_heredoc_line(int fd, char *line, t_shell *shell, int expand)
{
	char	*exp;

	if (!expand)
	{
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		return ;
	}
	exp = build_expanded(line, shell);
	if (exp)
	{
		write(fd, exp, ft_strlen(exp));
		free(exp);
	}
	else
		write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
}
