#include "../../../minishell.h"

// verilen string tamamen sayisal mi kontrol ediyor
static int	is_numeric(char *s)
{
	int	i;

	i = 0;
	if (s[i] == '-' || s[i] == '+')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}

// exit: sayi degil ise 255, cok arguman ise hata, gecerliyse cikiyor
void	builtin_exit(t_cmd *cmd, t_shell *shell)
{
	write(1, "exit\n", 5);
	if (!cmd->argv[1])
		exit(shell->last_exit);
	if (!is_numeric(cmd->argv[1]))
	{
		write(2, "minishell: exit: ", 17);
		write(2, cmd->argv[1], ft_strlen(cmd->argv[1]));
		write(2, ": numeric argument required\n", 28);
		exit(255);
	}
	if (cmd->argv[2])
	{
		write(2, "minishell: exit: too many arguments\n", 36);
		shell->last_exit = 1;
		return ;
	}
	exit((unsigned char)ft_atoi(cmd->argv[1]));
}
