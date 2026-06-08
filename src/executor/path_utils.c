#include "../../minishell.h"

char **find_path(char **envp)
{
    int i;
    char *path;
    char **paths;

    i = 0;
    while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
        i++;
    if (!envp[i])
        return (NULL);

    path = ft_strdup(envp[i] + 5);
    paths = ft_split(path, ':');
    free(path);
    return (paths);
}

char *find_command(char **paths, char *cmd)
{
    int i;
    char *tmp;
    char *full;

    i = 0;
    while (paths[i])
    {
        tmp = ft_strjoin(paths[i], "/");
        full = ft_strjoin(tmp, cmd);
        free(tmp);

        if (access(full, X_OK) == 0)
            return (full);

        free(full);
        i++;
    }
    return (NULL);
}

char *resolve_path(char *cmd, char **envp)
{
    char **paths;
    char *result;

    if (!cmd)
        return (NULL);

    if (ft_strchr(cmd, '/'))
    {
        if (access(cmd, X_OK) == 0)
            return (ft_strdup(cmd));
        return (NULL);
    }

    paths = find_path(envp);
    if (!paths)
        return (NULL);

    result = find_command(paths, cmd);
    free_array(paths);

    return (result);
}