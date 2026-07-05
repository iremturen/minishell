/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static int	open_redir_file(t_redir *r)
{
	if (r->type == TOK_REDIR_IN)
		return (open(r->file, O_RDONLY));
	if (r->type == TOK_REDIR_OUT)
		return (open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	if (r->type == TOK_APPEND)
		return (open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (-1);
}

static int	apply_single_redir(t_redir *r)
{
	int	fd;
	int	target;

	fd = open_redir_file(r);
	if (fd == -1)
	{
		perror(r->file);
		return (0);
	}
	target = STDOUT_FILENO;
	if (r->type == TOK_REDIR_IN)
		target = STDIN_FILENO;
	if (dup2(fd, target) == -1)
	{
		close(fd);
		return (0);
	}
	close(fd);
	return (1);
}

static int	apply_heredoc(t_redir *redir, t_shell *shell)
{
	int	fd;

	fd = process_heredoc(redir->file, shell);
	setup_signals_interactive();
	if (fd == -1)
		return (0);
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		close(fd);
		return (0);
	}
	close(fd);
	return (1);
}

int	apply_redirs(t_redir *redir, t_shell *shell)
{
	while (redir)
	{
		if (redir->type == TOK_HEREDOC)
		{
			if (!apply_heredoc(redir, shell))
				return (0);
		}
		else if (!apply_single_redir(redir))
			return (0);
		redir = redir->next;
	}
	return (1);
}
