/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   redir_heredoc.c                                    :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/05 18:00:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static int	cleanup_heredoc_pipe(int *pipefd, t_redir *redir)
{
	close(pipefd[0]);
	close(pipefd[1]);
	redir->fd = -1;
	return (1);
}

static int	hd_finish_read(t_hd_ctx *ctx, char *line, int got_eof)
{
	if (got_eof)
	{
		if (line && line[0])
			write_heredoc_line(ctx->pipefd[1], line, ctx->shell,
				ctx->redir->heredoc_expand);
		free(line);
		print_eof_warning(ctx->redir->file);
		return (1);
	}
	if (!line)
		return (1);
	if (ft_strncmp(line, ctx->redir->file, ctx->dlen + 1) == 0)
	{
		free(line);
		return (1);
	}
	return (0);
}

static int	heredoc_read_loop(t_hd_ctx *ctx)
{
	char	*line;
	int		got_eof;

	while (1)
	{
		got_eof = 0;
		line = read_heredoc_line(&got_eof);
		if (get_signal() == SIGINT)
		{
			free(line);
			ctx->shell->last_exit = 130;
			clear_signal();
			restore_heredoc_signals(ctx->shell);
			return (cleanup_heredoc_pipe(ctx->pipefd, ctx->redir));
		}
		if (hd_finish_read(ctx, line, got_eof))
			return (0);
		write_heredoc_line(ctx->pipefd[1], line, ctx->shell,
			ctx->redir->heredoc_expand);
		free(line);
	}
	return (0);
}

static int	hd_finalize(t_redir *redir, t_shell *shell, int *pipefd)
{
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

int	process_heredoc(t_redir *redir, t_shell *shell)
{
	t_hd_ctx	ctx;
	int			pipefd[2];
	int			ret;

	redir->fd = -1;
	setup_heredoc_signals();
	if (pipe(pipefd) == -1)
		return (-1);
	ctx.redir = redir;
	ctx.shell = shell;
	ctx.pipefd = pipefd;
	ctx.dlen = ft_strlen(redir->file);
	ret = heredoc_read_loop(&ctx);
	if (ret == 1)
		return (1);
	return (hd_finalize(redir, shell, pipefd));
}
