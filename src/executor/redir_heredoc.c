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

static struct sigaction	g_hd_old_sa;

static void	heredoc_sigint(int sig)
{
	(void)sig;
	set_signal(SIGINT);
	write(1, "\n", 1);
}

static void	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = heredoc_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, &g_hd_old_sa);
	signal(SIGQUIT, SIG_IGN);
}

static void	restore_heredoc_signals(t_shell *shell)
{
	sigaction(SIGINT, &g_hd_old_sa, NULL);
	setup_signals_interactive(shell);
}

static void	print_eof_warning(char *delim)
{
	write(2, "minishell: warning: here-document delimited by end-of-file (wanted `",
		68);
	write(2, delim, ft_strlen(delim));
	write(2, "')\n", 3);
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

static char	*read_heredoc_line(int *got_eof)
{
	char	buf[4096];
	int		i;
	ssize_t	n;
	char	c;

	i = 0;
	*got_eof = 0;
	if (isatty(STDIN_FILENO))
		write(1, "> ", 2);
	while (i < 4095)
	{
		n = read(0, &c, 1);
		if (n < 0)
		{
			if (errno == EINTR)
			{
				if (get_signal() == SIGINT)
					return (NULL);
				continue ;
			}
			return (NULL);
		}
		if (n == 0)
		{
			*got_eof = 1;
			break ;
		}
		if (c == '\n')
			break ;
		buf[i++] = c;
	}
	buf[i] = '\0';
	return (ft_strdup(buf));
}

static int	cleanup_heredoc_pipe(int *pipefd, t_redir *redir)
{
	close(pipefd[0]);
	close(pipefd[1]);
	redir->fd = -1;
	return (1);
}

int	process_heredoc(t_redir *redir, t_shell *shell)
{
	int		pipefd[2];
	char	*line;
	size_t	dlen;
	int		got_eof;

	redir->fd = -1;
	setup_heredoc_signals();
	if (pipe(pipefd) == -1)
		return (-1);
	dlen = ft_strlen(redir->file);
	while (1)
	{
		got_eof = 0;
		line = read_heredoc_line(&got_eof);
		if (get_signal() == SIGINT)
		{
			free(line);
			shell->last_exit = 130;
			clear_signal();
			restore_heredoc_signals(shell);
			return (cleanup_heredoc_pipe(pipefd, redir));
		}
		if (got_eof)
		{
			if (line && line[0])
				write_heredoc_line(pipefd[1], line, shell, redir->heredoc_expand);
			free(line);
			print_eof_warning(redir->file);
			break ;
		}
		if (!line)
			break ;
		if (ft_strncmp(line, redir->file, dlen + 1) == 0)
		{
			free(line);
			break ;
		}
		write_heredoc_line(pipefd[1], line, shell, redir->heredoc_expand);
		free(line);
	}
	close(pipefd[1]);
	if (get_signal() == SIGINT)
	{
		shell->last_exit = 130;
		clear_signal();
		close(pipefd[0]);
		redir->fd = -1;
		restore_heredoc_signals(shell);
		return (1);
	}
	redir->fd = pipefd[0];
	restore_heredoc_signals(shell);
	return (0);
}

void	close_heredoc_fds(t_cmd *cmd)
{
	t_redir	*r;

	while (cmd)
	{
		r = cmd->redirs;
		while (r)
		{
			if (r->type == TOK_HEREDOC && r->fd >= 0)
			{
				close(r->fd);
				r->fd = -1;
			}
			r = r->next;
		}
		cmd = cmd->next;
	}
}

int	prepare_heredocs(t_cmd *cmd, t_shell *shell)
{
	t_cmd	*start;
	t_redir	*r;

	start = cmd;
	while (cmd)
	{
		r = cmd->redirs;
		while (r)
		{
			if (r->type == TOK_HEREDOC && process_heredoc(r, shell) != 0)
			{
				close_heredoc_fds(start);
				return (0);
			}
			r = r->next;
		}
		cmd = cmd->next;
	}
	return (1);
}
