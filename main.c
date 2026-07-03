#include "minishell.h"

# define PROMPT "minishell$ "

// pipe ile calisirken readline prompt basmaz; tester ilk ciktiyi yanlis algilar
static void	print_prompt_if_needed(void)
{
	if (!isatty(STDIN_FILENO))
	{
		write(1, PROMPT, ft_strlen(PROMPT));
		write(1, "\n", 1);
	}
}

// her satiri ilgili katmanlara gonderip sonuclari temizliyor
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
		execute_cmd(cmds, shell);
		free_cmds(cmds);
	}
	free_tokens(tokens);
}

// readline NULL dondu: ctrl+c ise devam et (1), ctrl+d ise cik (0)
static int	handle_null_line(void)
{
	if (g_signal == SIGINT)
	{
		g_signal = 0;
		return (1);
	}
	write(1, "exit\n", 5);
	return (0);
}

// satiri isle: ctrl+c sonrasi $? = 130, sonra calistir ve bellek temizle
static void	handle_line(char *line, t_shell *shell)
{
	if (g_signal == SIGINT)
		shell->last_exit = 130;
	g_signal = 0;
	if (*line)
	{
		add_history(line);
		run_line(line, shell);
	}
	free(line);
}

// ana dongu: readline ile satir aliyor, isleme sokuyor
int	main(int argc, char **argv, char **env)
{
	t_shell	*shell;
	char	*line;
	int		exit_code;

	(void)argc;
	(void)argv;
	shell = init_shell(env);
	if (!shell)
		return (1);
	setup_signals_interactive();
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
	exit_code = shell->last_exit;
	clear_history();
	free_shell(shell);
	return (exit_code);
}