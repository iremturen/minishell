#include "../../../minishell.h"

// envp icinde key i arayip indeksini donduruyor, bulamazsa -1
static int	env_find(char **envp, char *key)
{
	size_t	len;
	int		i;

	len = ft_strlen(key);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], key, len) == 0
			&& (envp[i][len] == '=' || envp[i][len] == '\0'))
			return (i);
		i++;
	}
	return (-1);
}

// envp ye key=val ekliyor ya da guncelliyior
int	env_set(t_shell *shell, char *key, char *val)
{
	char	**new;
	char	*tmp;
	char	*entry;
	int		idx;
	int		n;

	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (0);
	entry = ft_strjoin(tmp, val);
	free(tmp);
	if (!entry)
		return (0);
	idx = env_find(shell->envp, key);
	if (idx != -1)
	{
		free(shell->envp[idx]);
		shell->envp[idx] = entry;
		return (1);
	}
	n = 0;
	while (shell->envp[n])
		n++;
	new = ft_calloc(n + 2, sizeof(char *));
	if (!new)
	{
		free(entry);
		return (0);
	}
	ft_memcpy(new, shell->envp, sizeof(char *) * n);
	new[n] = entry;
	free(shell->envp);
	shell->envp = new;
	return (1);
}

// cd hedef dizini hesapliyor: no-arg HOME, "-" OLDPWD, digeri argv[1]
static char	*get_cd_path(t_cmd *cmd, t_shell *shell)
{
	int	idx;

	if (!cmd->argv[1])
	{
		idx = env_find(shell->envp, "HOME");
		if (idx == -1)
		{
			write(2, "minishell: cd: HOME not set\n", 28);
			return (NULL);
		}
		return (ft_strchr(shell->envp[idx], '=') + 1);
	}
	if (ft_strncmp(cmd->argv[1], "-", 2) == 0)
	{
		idx = env_find(shell->envp, "OLDPWD");
		if (idx == -1)
		{
			write(2, "minishell: cd: OLDPWD not set\n", 30);
			return (NULL);
		}
		return (ft_strchr(shell->envp[idx], '=') + 1);
	}
	return (cmd->argv[1]);
}

// cd: argumansiz HOME a, "-" ile OLDPWD e, verilen yola gider
void	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	cwd[1024];
	char	old[1024];

	path = get_cd_path(cmd, shell);
	if (!path)
	{
		shell->last_exit = 1;
		return ;
	}
	if (!getcwd(old, sizeof(old)))
	{
		perror("cd: getcwd");
		shell->last_exit = 1;
		return ;
	}
	if (chdir(path) == -1)
	{
		perror("cd");
		shell->last_exit = 1;
		return ;
	}
	env_set(shell, "OLDPWD", old);
	if (getcwd(cwd, sizeof(cwd)))
		env_set(shell, "PWD", cwd);
}

// unset: argv listesindeki tum degiskenleri envp den siliyor
void	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int	idx;
	int	n;
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		idx = env_find(shell->envp, cmd->argv[i]);
		if (idx != -1)
		{
			free(shell->envp[idx]);
			n = idx;
			while (shell->envp[n + 1])
			{
				shell->envp[n] = shell->envp[n + 1];
				n++;
			}
			shell->envp[n] = NULL;
		}
		i++;
	}
}



