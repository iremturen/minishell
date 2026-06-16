#include "minishell.h"

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
	handle_quotes(tokens);
	cmds = parse(tokens);
	if (cmds)
	{
		execute_cmd(cmds, shell);
		free_cmds(cmds);
	}
	free_tokens(tokens);
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
		line = readline("minishell$ ");
		if (!line)
		{
			write(1, "exit\n", 5);
			break ;
		}
		if (*line)
		{
			add_history(line);
			run_line(line, shell);
		}
		g_signal = 0;
		free(line);
	}
	exit_code = shell->last_exit;
	clear_history();
	free_shell(shell);
	return (exit_code);
}