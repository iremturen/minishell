#include "../../minishell.h"

// yeni bos bir komut dugumu olusturuyor
static t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->cmd_path = NULL;
	cmd->is_builtin = 0;
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

// t_cmd bagli listesini baslindan sonuna kadar temizliyor
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
		redir = head->redirs;
		while (redir)
		{
			rnext = redir->next;
			free(redir->file);
			free(redir->delimiter);
			free(redir);
			redir = rnext;
		}
		free(head->cmd_path);
		free(head);
		head = next;
	}
}

// argv dizisini bir buyuk yeni dizi olarak kopyalayip donduruyor
static char	**grow_argv(char **old, int len)
{
	char	**tmp;
	int		i;

	tmp = malloc(sizeof(char *) * (len + 2));
	if (!tmp)
		return (NULL);
	i = 0;
	while (i < len)
	{
		tmp[i] = old[i];
		i++;
	}
	tmp[len] = NULL;
	tmp[len + 1] = NULL;
	return (tmp);
}

// komuta yeni arguman ekliyor, eski argv yi serbest birakiyor
static int	cmd_add_arg(t_cmd *cmd, char *word)
{
	int		len;
	char	**tmp;

	len = 0;
	while (cmd->argv && cmd->argv[len])
		len++;
	tmp = grow_argv(cmd->argv, len);
	if (!tmp)
		return (0);
	tmp[len] = ft_strdup(word);
	if (!tmp[len])
	{
		free(tmp);
		return (0);
	}
	free(cmd->argv);
	cmd->argv = tmp;
	return (1);
}

// redir tokenini isliyip sonraki tokeni dosya adi olarak aliyor
static int	handle_redir(t_cmd *cmd, t_token **cur)
{
	t_token_type	type;

	type = (*cur)->type;
	*cur = (*cur)->next;
	if (!(*cur) || (*cur)->type != TOK_WORD)
		return (0);
	if (!cmd_add_redir(cmd, type, (*cur)->value))
		return (0);
	return (1);
}

// tok_word, tok_pipe ve tok_redir tokenlarini isliyor
t_cmd	*parse(t_token *tokens)
{
	t_cmd	*head;
	t_cmd	*cur;
	t_token	*tok;

	if (!tokens)
		return (NULL);
	head = new_cmd();
	if (!head)
		return (NULL);
	cur = head;
	tok = tokens;
	while (tok)
	{
		if (tok->type == TOK_WORD)
		{
			if (!cmd_add_arg(cur, tok->value))
				return (free_cmds(head), NULL);
		}
		else if (tok->type == TOK_PIPE)
		{
			cur->next = new_cmd();
			if (!cur->next)
				return (free_cmds(head), NULL);
			cur = cur->next;
		}
		else if (!handle_redir(cur, &tok))
			return (free_cmds(head), NULL);
		tok = tok->next;
	}
	if (!head->argv)
		return (free_cmds(head), NULL);
	return (head);
}
