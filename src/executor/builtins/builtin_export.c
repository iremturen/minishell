#include "../../../minishell.h"

// declare -x KEY="VALUE" formatinda bir env satirini yazıyor
static void	print_export_entry(char *entry)
{
	char	*eq;

	write(1, "declare -x ", 11);
	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		write(1, entry, ft_strlen(entry));
		write(1, "\n", 1);
		return ;
	}
	write(1, entry, eq - entry + 1);
	write(1, "\"", 1);
	write(1, eq + 1, ft_strlen(eq + 1));
	write(1, "\"\n", 2);
}

// export icin key gecerli degisken ismi mi: harf/_ile baslamali, harf/rakam/_ olmali
static int	is_valid_key(char *key)
{
	int	i;

	if (!key || !key[0])
		return (0);
	if (!ft_isalpha((unsigned char)key[0]) && key[0] != '_')
		return (0);
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum((unsigned char)key[i]) && key[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

// tek bir KEY=VAL argumani isliyor, env e ekliyor
static void	do_export_one(char *arg, t_shell *shell)
{
	char	*eq;
	char	*key;

	eq = ft_strchr(arg, '=');
	if (!eq)
		return ;
	key = ft_substr(arg, 0, eq - arg);
	if (!key)
		return ;
	if (!is_valid_key(key))
	{
		write(2, "minishell: export: '", 20);
		write(2, key, ft_strlen(key));
		write(2, "': not a valid identifier\n", 26);
		free(key);
		return ;
	}
	env_set(shell, key, eq + 1);
	free(key);
}

// export: argumansiz tum env i listeler, varsa her KEY=VAL ciftini ekler
void	builtin_export(t_cmd *cmd, t_shell *shell)
{
	int	i;

	if (!cmd->argv[1])
	{
		i = 0;
		while (shell->envp[i])
		{
			print_export_entry(shell->envp[i]);
			i++;
		}
		return ;
	}
	i = 1;
	while (cmd->argv[i])
	{
		do_export_one(cmd->argv[i], shell);
		i++;
	}
}
