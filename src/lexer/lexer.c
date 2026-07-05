/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static t_token_type	get_token_type(char *value)
{
	if (!ft_strncmp(value, ">>", 3))
		return (TOK_APPEND);
	if (!ft_strncmp(value, "<<", 3))
		return (TOK_HEREDOC);
	if (!ft_strncmp(value, "|", 2))
		return (TOK_PIPE);
	if (!ft_strncmp(value, ">", 2))
		return (TOK_REDIR_OUT);
	if (!ft_strncmp(value, "<", 2))
		return (TOK_REDIR_IN);
	return (TOK_WORD);
}

static t_token	*new_token(char *value)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->value = ft_strdup(value);
	if (!tok->value)
	{
		free(tok);
		return (NULL);
	}
	tok->next = NULL;
	if (ft_strchr(value, '\'') != NULL || ft_strchr(value, '"') != NULL)
		tok->is_quoted = 1;
	else
		tok->is_quoted = 0;
	tok->type = get_token_type(value);
	return (tok);
}

static t_token	*append_token(t_token **head, t_token **tail, char *word)
{
	t_token	*tok;

	tok = new_token(word);
	if (!tok)
		return (NULL);
	if (!(*head))
		*head = tok;
	else
		(*tail)->next = tok;
	*tail = tok;
	return (tok);
}

static t_token	*build_token_list(char **words)
{
	t_token	*head;
	t_token	*tail;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (words[i])
	{
		if (!append_token(&head, &tail, words[i]))
		{
			free_tokens(head);
			return (NULL);
		}
		i++;
	}
	return (head);
}

t_token	*tokenize(char *line, t_shell *shell)
{
	char	**words;
	t_token	*tokens;

	words = split_inputs(line);
	if (!words)
		return (NULL);
	if (validate_lexer_syntax(words) != 0)
	{
		shell->last_exit = 2;
		free_array(words);
		return (NULL);
	}
	tokens = build_token_list(words);
	free_array(words);
	return (tokens);
}
