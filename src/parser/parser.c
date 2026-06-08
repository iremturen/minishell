#include "../../minishell.h"

/*
** TODO: parse — t_token listesini t_cmd listesine dönüştürür.
**
** Genel akış:
**   - Her pipe ( | ) yeni bir t_cmd başlatır.
**   - TOK_WORD tokenlar argv[]'ye eklenir.
**   - TOK_REDIR_IN/OUT/APPEND/HEREDOC sonrasındaki TOK_WORD
**     o komutun t_redir listesine eklenir.
**
** Örnek:
**   token listesi: ["ls", "-la", "|", "grep", "foo", ">", "out.txt"]
**   cmd listesi:   cmd1{argv=["ls","-la"]} → cmd2{argv=["grep","foo"], redirs=[>out.txt]}
*/
t_cmd	*parse(t_token *tokens)
{
	(void)tokens;
	return (NULL);
}

/*
** free_cmds — t_cmd bağlı listesini tamamen serbest bırakır.
*/
void	free_cmds(t_cmd *head)
{
	t_cmd	*next;
	t_redir	*redir;
	t_redir	*rnext;
	int		i;

	while (head)
	{
		next = head->next;
		if (head->argv)
		{
			i = 0;
			while (head->argv[i])
				free(head->argv[i++]);
			free(head->argv);
		}
		free(head->cmd_path);
		redir = head->redirs;
		while (redir)
		{
			rnext = redir->next;
			free(redir->file);
			free(redir->delimiter);
			free(redir);
			redir = rnext;
		}
		free(head);
		head = next;
	}
}
