/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   execute_pipeline.c                                 :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static void	fill_fork_data(t_cmd *cur, t_pipe_ctx *ctx, t_fork_data *data)
{
	data->in_fd = ctx->prev_fd;
	if (cur->next)
	{
		data->out_fd = ctx->pipefd[1];
		data->close_fd = ctx->pipefd[0];
	}
	else
	{
		data->out_fd = -1;
		data->close_fd = -1;
	}
	data->shell = ctx->shell;
}

static int	run_pipe_step(t_cmd *cur, t_pipe_ctx *ctx)
{
	t_fork_data	data;

	if (cur->next && pipe(ctx->pipefd) == -1)
		return (0);
	fill_fork_data(cur, ctx, &data);
	ctx->pids[ctx->n] = pipeline_fork(cur, &data);
	if (ctx->prev_fd != -1)
		close(ctx->prev_fd);
	if (cur->next)
		ctx->prev_fd = ctx->pipefd[0];
	else
		ctx->prev_fd = -1;
	ctx->n++;
	return (1);
}

// tum alt surecleri bekler, gecersiz pid leri atlar, son exit kodunu gunceller
static void	wait_cmds(t_pipe_ctx *ctx)
{
	int	status;
	int	i;

	i = 0;
	status = 0;
	while (i < ctx->n)
	{
		if (ctx->pids[i] > 0)
			waitpid(ctx->pids[i], &status, 0);
		i++;
	}
	if (WIFEXITED(status))
		ctx->shell->last_exit = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		ctx->shell->last_exit = 128 + WTERMSIG(status);
		if (WTERMSIG(status) == SIGQUIT)
			write(2, "Quit (core dumped)\n", 19);
		else if (WTERMSIG(status) == SIGINT)
			write(1, "\n", 1);
	}
}

// pipe zinciri kurar, her komutu fork layip stdin stdout baglantisini sagliyor
void	execute_pipeline(t_cmd *cmds, t_shell *shell)
{
	t_pipe_ctx	ctx;
	t_cmd		*cur;

	ctx.prev_fd = -1;
	ctx.n = 0;
	ctx.shell = shell;
	cur = cmds;
	while (cur)
	{
		if (!run_pipe_step(cur, &ctx))
			break ;
		cur = cur->next;
	}
	if (ctx.prev_fd != -1)
		close(ctx.prev_fd);
	wait_cmds(&ctx);
}
