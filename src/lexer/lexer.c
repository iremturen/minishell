#include "../../minishell.h"

// token stringine bakarak tipini belirliyor
static t_token_type	token_type_of(char *value)
{
	if (ft_strncmp(value, ">>", 3) == 0)
		return (TOK_APPEND);
	if (ft_strncmp(value, "<<", 3) == 0)
		return (TOK_HEREDOC);
	if (ft_strncmp(value, "|", 2) == 0)
		return (TOK_PIPE);
	if (ft_strncmp(value, ">", 2) == 0)
		return (TOK_REDIR_OUT);
	if (ft_strncmp(value, "<", 2) == 0)
		return (TOK_REDIR_IN);
	return (TOK_WORD);
}

// yeni bir token dugumu olusturuyor ve deger ile tipi set ediyor
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
	tok->type = token_type_of(value);
	tok->next = NULL;
	return (tok);
}

// bagli listeye token ekliyor, hata durumunda temizleyip null donuyor
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

// ham satiri once split ediyor, syntax kontrolü yapiyor, token listesi donuyor
t_token	*tokenize(char *line)
{
	char	**words;
	t_token	*head;
	t_token	*tail;
	int		i;

	words = split_inputs(line);
	if (!words)
		return (NULL);
	if (validate_lexer_syntax(words) != 0)
	{
		free_array(words);
		return (NULL);
	}
	head = NULL;
	tail = NULL;
	i = 0;
	while (words[i])
	{
		if (!append_token(&head, &tail, words[i]))
		{
			free_tokens(head);
			free_array(words);
			return (NULL);
		}
		i++;
	}
	free_array(words);
	return (head);
}

// token listesini baslindan sonuna kadar temizliyor
void	free_tokens(t_token *head)
{
	t_token	*next;

	while (head)
	{
		next = head->next;
		free(head->value);
		free(head);
		head = next;
	}
}
