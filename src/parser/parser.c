/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

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

static int	handle_redir(t_cmd *cmd, t_token **cur)
{
	t_token_type	type;

	type = (*cur)->type;
	*cur = (*cur)->next;
	if (!(*cur) || (*cur)->type != TOK_WORD)
		return (0);
	if (!cmd_add_redir(cmd, type, (*cur)->value, !(*cur)->is_quoted))
		return (0);
	return (1);
}

static int	parse_token(t_token **tok, t_cmd **cur)
{
	if ((*tok)->type == TOK_WORD)
	{
		if (!cmd_add_arg(*cur, (*tok)->value))
			return (0);
	}
	else if ((*tok)->type == TOK_PIPE)
	{
		(*cur)->next = new_cmd();
		if (!(*cur)->next)
			return (0);
		*cur = (*cur)->next;
	}
	else
	{
		if (!handle_redir(*cur, tok))
			return (0);
	}
	return (1);
}

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
		if (!parse_token(&tok, &cur))
			return (free_cmds(head), NULL);
		tok = tok->next;
	}
	if (!head->argv && !head->redirs)
		return (free_cmds(head), NULL);
	return (head);
}
