#include "../../minishell.h"

// yeni token dugumu olusturuyor, tipi satir icerigine gore belirleniyor
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
	if (!ft_strncmp(value, ">>", 3))
		tok->type = TOK_APPEND;
	else if (!ft_strncmp(value, "<<", 3))
		tok->type = TOK_HEREDOC;
	else if (!ft_strncmp(value, "|", 2))
		tok->type = TOK_PIPE;
	else if (!ft_strncmp(value, ">", 2))
		tok->type = TOK_REDIR_OUT;
	else if (!ft_strncmp(value, "<", 2))
		tok->type = TOK_REDIR_IN;
	else
		tok->type = TOK_WORD;
	return (tok);
}

// bagli listeye token ekliyor, hata durumunda null doner
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

// words dizisinden token bagli listesi olusturuyor
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

// satiri tokenize eder; syntax hatasi icin last_exit=2 set eder
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
