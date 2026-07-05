/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static int	expand_exit_status(char **res, int *i, t_shell *shell)
{
	char	*num;
	int		ret;

	num = ft_itoa(shell->last_exit);
	if (!num)
		return (0);
	(*i)++;
	ret = str_append(res, num);
	free(num);
	return (ret);
}

static int	expand_dollar(char **res, char *str, int *i, t_shell *shell)
{
	char	*name;
	char	*val;
	int		start;

	if (str[*i] == '?')
		return (expand_exit_status(res, i, shell));
	if (!ft_isalpha(str[*i]) && str[*i] != '_')
		return (str_append(res, "$"));
	start = *i;
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	name = ft_substr(str, start, *i - start);
	if (!name)
		return (0);
	val = get_env_val(name, shell->envp);
	free(name);
	if (val)
		return (str_append(res, val));
	return (1);
}

char	*build_expanded(char *str, t_shell *shell)
{
	char	*res;
	int		i;
	int		in_sq;

	res = ft_strdup("");
	if (!res)
		return (NULL);
	i = 0;
	in_sq = 0;
	while (str[i])
	{
		if (str[i] == '\'')
			in_sq = !in_sq;
		if (str[i] == '$' && !in_sq && str[i + 1] && ++i)
		{
			if (!expand_dollar(&res, str, &i, shell))
				return (free(res), NULL);
		}
		else if (!append_char(&res, str[i++]))
			return (free(res), NULL);
	}
	return (res);
}

void	expand_tokens(t_token *head, t_shell *shell)
{
	char	*new_val;

	while (head)
	{
		if (head->type == TOK_WORD && head->value)
		{
			new_val = build_expanded(head->value, shell);
			if (new_val)
			{
				free(head->value);
				head->value = new_val;
			}
		}
		head = head->next;
	}
}
