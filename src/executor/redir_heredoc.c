/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   redir_heredoc.c                                    :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static void	heredoc_sigint(int sig)
{
	set_signal(sig);
	write(1, "\n", 1);
}

static void	write_heredoc_line(int fd, char *line, t_shell *shell, int expand)
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

int	process_heredoc(t_redir *redir, t_shell *shell)
{
	int		pipefd[2];
	char	*line;
	size_t	dlen;

	signal(SIGINT, heredoc_sigint);
	signal(SIGQUIT, SIG_IGN);
	if (pipe(pipefd) == -1)
		return (-1);
	dlen = ft_strlen(redir->file);
	while (1)
	{
		line = readline("> ");
		if (!line || get_signal() || ft_strncmp(line, redir->file, dlen + 1) == 0)
		{
			free(line);
			break ;
		}
		write_heredoc_line(pipefd[1], line, shell, redir->heredoc_expand);
		free(line);
	}
	close(pipefd[1]);
	if (!get_signal())
		return (pipefd[0]);
	close(pipefd[0]);
	return (-1);
}
