// yani pipe olmayan durum. pipe a gerek yok
// ls
// pwd
// echo hello
// cat file.txt

#include "../../minishell.h"
#include <sys/wait.h>

void execute_single(t_cmd *cmd, char **envp)
{
    pid_t pid;
    int status;

    if (!cmd || !cmd->argv)
        return;

    cmd->cmd_path = resolve_path(cmd->argv[0], envp);
    if (!cmd->cmd_path)
    {
        write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
        write(2, ": command not found\n", 21);
        return;
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        free(cmd->cmd_path);
        return;
    }

    if (pid == 0)
    {
        execve(cmd->cmd_path, cmd->argv, envp);
        perror("execve");
        exit(1);
    }

    waitpid(pid, &status, 0);
    free(cmd->cmd_path);
}