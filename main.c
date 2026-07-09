/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

static void	run_line(char *line, t_shell *shell)
{
	t_token	*tokens;
	t_cmd	*cmds;

	if (has_unclosed_quotes(line))
	{
		write(2, "minishell: syntax error: unexpected EOF\n", 40);
		shell->last_exit = 2;
		return ;
	}
	tokens = tokenize(line, shell);
	if (!tokens)
		return ;
	expand_tokens(tokens, shell);
	filter_empty_tokens(&tokens);
	handle_quotes(tokens);
	cmds = parse(tokens);
	if (cmds)
	{
		shell->all_cmds = cmds;
		shell->all_tokens = tokens;
		execute_cmd(cmds, shell);
		shell->all_cmds = NULL;
		shell->all_tokens = NULL;
		free_cmds(cmds);
	}
	free_tokens(tokens);
}

static int	handle_null_line(void)
{
	write(1, "exit\n", 5);
	return (0);
}

static void	handle_line(char *line, t_shell *shell)
{
	if (get_signal() == SIGINT)
		shell->last_exit = 130;
	clear_signal();
	if (*line)
	{
		add_history(line);
		run_line(line, shell);
	}
	free(line);
}

static void	shell_loop(t_shell *shell)
{
	char	*line;

	while (1)
	{
		print_prompt_if_needed();
		line = readline(PROMPT);
		if (!line)
		{
			if (handle_null_line())
				continue ;
			break ;
		}
		handle_line(line, shell);
	}
}

int	main(int argc, char **argv, char **env)
{
	t_shell	*shell;
	int		exit_code;

	(void)argc;
	(void)argv;
	shell = init_shell(env);
	if (!shell)
		return (1);
	setup_signals_interactive(shell);
	shell_loop(shell);
	exit_code = shell->last_exit;
	clear_history();
	free_shell(shell);
	return (exit_code);
}
