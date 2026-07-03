#include "../../minishell.h"

// envp icinde PATH= satirini bulup dizin listesine ceviriyor
char	**find_path(char **envp)
{
	int		i;
	char	*path;
	char	**paths;

	i = 0;
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	if (!envp[i])
		return (NULL);
	path = ft_strdup(envp[i] + 5);
	if (!path)
		return (NULL);
	paths = ft_split(path, ':');
	free(path);
	return (paths);
}

// path dizinleri icinde calistirilabilir komutu ariyor
char	*find_command(char **paths, char *cmd)
{
	int		i;
	char	*tmp;
	char	*full;

	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		if (!tmp)
			return (NULL);
		full = ft_strjoin(tmp, cmd);
		free(tmp);
		if (!full)
			return (NULL);
		if (access(full, X_OK) == 0)
			return (full);
		free(full);
		i++;
	}
	return (NULL);
}

// / varsa direk dener, yoksa PATH te arar
char	*resolve_path(char *cmd, char **envp)
{
	char	**paths;
	char	*result;

	if (!cmd || ft_strlen(cmd) == 0)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	paths = find_path(envp);
	if (!paths)
		return (NULL);
	result = find_command(paths, cmd);
	free_array(paths);
	return (result);
}

// execve cagirir, basarisizsa hata yazar ve dogru kod ile cikiyor
void	exec_or_exit(t_cmd *cmd, char **envp)
{
	struct stat	path_stat;

	execve(cmd->cmd_path, cmd->argv, envp);
	write(2, "minishell: ", 11);
	write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
	if (cmd->cmd_path && stat(cmd->cmd_path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
	{
		write(2, ": is a directory\n", 17);
		exit(126);
	}
	if (errno == ENOENT)
	{
		write(2, ": No such file or directory\n", 28);
		exit(127);
	}
	if (errno == EACCES)
	{
		write(2, ": Permission denied\n", 20);
		exit(126);
	}
	write(2, ": ", 2);
	write(2, strerror(errno), ft_strlen(strerror(errno)));
	write(2, "\n", 1);
	exit(126);
}