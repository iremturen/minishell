#include "minishell.h"

// her satiri ilgili katmanlara gonderip sonuclari temizliyor
static void	run_line(char *line, t_shell *shell)
{
	t_token	*tokens;
	t_cmd	*cmds;

	tokens = tokenize(line);
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
	free_shell(shell);
	return (shell->last_exit);
}