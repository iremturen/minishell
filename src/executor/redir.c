#include "../../minishell.h"

// heredoc sirasindaki sigint: g_signal set eder, satir atlayip cikiyor
static void	heredoc_sigint(int sig)
{
	g_signal = sig;
	write(1, "\n", 1);
}

// redir tipine gore dosyayi acip dup2 ile yonlendiriyor
static int	apply_single_redir(t_redir *r)
{
	int	fd;
	int	target;

	if (r->type == TOK_REDIR_IN)
		fd = open(r->file, O_RDONLY);
	else if (r->type == TOK_REDIR_OUT)
		fd = open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (r->type == TOK_APPEND)
		fd = open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd = -1;
	if (fd == -1)
	{
		perror(r->file);
		return (0);
	}
	target = STDOUT_FILENO;
	if (r->type == TOK_REDIR_IN)
		target = STDIN_FILENO;
	if (dup2(fd, target) == -1)
	{
		close(fd);
		return (0);
	}
	close(fd);
	return (1);
}

// heredoc satiri: $ degiskenlerini genisletip pipe e yazıyor
static void	write_expanded_line(int fd, char *line, t_shell *shell)
{
	char	*exp;

	exp = build_expanded(line, shell);
	if (exp)
	{
		write(fd, exp, ft_strlen(exp));
		free(exp);
	}
	else
		write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
}

// heredoc: delimiter gelene kadar satir satir okuyor, ctrl+c ve expansion destekli
static int	heredoc_read(char *delim, t_shell *shell)
{
	int		pipefd[2];
	char	*line;
	size_t	dlen;

	signal(SIGINT, heredoc_sigint);
	if (pipe(pipefd) == -1)
		return (-1);
	dlen = ft_strlen(delim);
	while (1)
	{
		line = readline("> ");
		if (!line || g_signal || ft_strncmp(line, delim, dlen + 1) == 0)
		{
			free(line);
			break ;
		}
		write_expanded_line(pipefd[1], line, shell);
		free(line);
	}
	close(pipefd[1]);
	if (!g_signal)
		return (pipefd[0]);
	close(pipefd[0]);
	return (-1);
}

// redir listesini siraya gore uygular, basarisizlikta 0 doner
int	apply_redirs(t_redir *redir, t_shell *shell)
{
	int	fd;

	while (redir)
	{
		if (redir->type == TOK_HEREDOC)
		{
			fd = heredoc_read(redir->file, shell);
			setup_signals_interactive();
			if (fd == -1)
				return (0);
			if (dup2(fd, STDIN_FILENO) == -1)
			{
				close(fd);
				return (0);
			}
			close(fd);
		}
		else if (!apply_single_redir(redir))
			return (0);
		redir = redir->next;
	}
	return (1);
}
