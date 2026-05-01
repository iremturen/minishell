#include <minishell.h>

void builtin_cd(t_cmd *cmd, t_shell *shell)
{
    (void)shell;

    if (!cmd->argv[1])
    {
        printf("cd: missing argument\n");
        return;
    }

    if (chdir(cmd->argv[1]) == -1)
        perror("cd");
}

void builtin_echo(char **argv)
{
    int i;

    i = 1;
    while (argv[i])
    {
        printf("%s", argv[i]);

        if (argv[i + 1])
            printf(" ");

        i++;
    }

    printf("\n");
}

void builtin_pwd(void)
{
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)))
        printf("%s\n", cwd);
    else
        perror("pwd");
}

void builtin_env(char **envp)
{
    int i;

    i = 0;
    while (envp[i])
    {
        printf("%s\n", envp[i]);
        i++;
    }
}

void builtin_unset(t_cmd *cmd, t_shell *shell)
{
    (void)cmd;
    (void)shell;
}
void builtin_exit(t_shell *shell)
{
    (void)shell;

    printf("exit\n");
    exit(0);
}

//güncellenecek
int is_builtin(char *cmd)
{
    if (!cmd)
        return (0);
    if (!ft_strcmp(cmd, "cd"))
        return (1);
    if (!ft_strcmp(cmd, "echo"))
        return (1);
    if (!ft_strcmp(cmd, "pwd"))
        return (1);
    if (!ft_strcmp(cmd, "unset"))
        return (1);
    if (!ft_strcmp(cmd, "env"))
        return (1);
    if (!ft_strcmp(cmd, "exit"))
        return (1);
    return (0);
}