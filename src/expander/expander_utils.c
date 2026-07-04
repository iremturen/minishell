/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

char	*get_env_val(char *name, char **envp)
{
	size_t	len;
	int		i;

	len = ft_strlen(name);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], name, len) == 0 && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return (NULL);
}

int	str_append(char **dest, char *src)
{
	char	*tmp;

	if (!src)
		return (1);
	tmp = ft_strjoin(*dest, src);
	if (!tmp)
		return (0);
	free(*dest);
	*dest = tmp;
	return (1);
}

int	append_char(char **res, char c)
{
	char	buf[2];

	buf[0] = c;
	buf[1] = '\0';
	return (str_append(res, buf));
}

static void	unlink_empty_token(t_token **head, t_token *prev, t_token *cur)
{
	if (prev)
		prev->next = cur->next;
	else
		*head = cur->next;
	free(cur->value);
	free(cur);
}

void	filter_empty_tokens(t_token **head)
{
	t_token	*cur;
	t_token	*prev;
	t_token	*next;

	cur = *head;
	prev = NULL;
	while (cur)
	{
		if (cur->type == TOK_WORD && ft_strlen(cur->value) == 0
			&& !cur->is_quoted)
		{
			next = cur->next;
			unlink_empty_token(head, prev, cur);
			cur = next;
		}
		else
		{
			prev = cur;
			cur = cur->next;
		}
	}
}
