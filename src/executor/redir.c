#include "../../minishell.h"

// yonlendirme tipine gore dosyayi uygun modda aciyor
static int	open_redir(t_redir *r)
{
	if (r->type == TOK_REDIR_IN)
		return (open(r->file, O_RDONLY));
	if (r->type == TOK_REDIR_OUT)
		return (open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	if (r->type == TOK_APPEND)
		return (open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (-1);
}

// dosyayi acip stdin veya stdout a yonlendiriyor
static int	apply_single_redir(t_redir *r)
{
	int	fd;
	int	target;

	fd = open_redir(r);
	if (fd == -1)
	{
		perror(r->file);
		return (0);
	}
	if (r->type == TOK_REDIR_IN)
		target = STDIN_FILENO;
	else
		target = STDOUT_FILENO;
	if (dup2(fd, target) == -1)
	{
		close(fd);
		return (0);
	}
	close(fd);
	return (1);
}

// heredoc: delimiter gelene kadar satir satir okuyor, pipe e yaziyor
static int	heredoc_read(char *delim)
{
	int		pipefd[2];
	char	*line;
	size_t	dlen;

	if (pipe(pipefd) == -1)
		return (-1);
	dlen = ft_strlen(delim);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strncmp(line, delim, dlen + 1) == 0)
		{
			free(line);
			break ;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]);
	return (pipefd[0]);
}

// redir listesini siraya gore uygular, basarisizlikta 0 doner
int	apply_redirs(t_redir *redir)
{
	int	fd;

	while (redir)
	{
		if (redir->type == TOK_HEREDOC)
		{
			fd = heredoc_read(redir->file);
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
