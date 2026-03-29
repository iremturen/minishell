#include "minishell.h"

static void	free_args(char **args)
{
	int	i;

	i = 0;
	while (args && args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

int	main(int argc, char **argv, char **env)
{
	char	*line;
	char	**args;

	(void)argc;
	(void)argv;
	(void)env;

	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
			break ;
		if (*line)
		{
			add_history(line);
			args = split_inputs(line);
			if (args)
			{
				validate_lexer_syntax(args);
				free_args(args);
			}
		}
		free(line);
	}
	return (0);
}