#include "../../minishell.h"

// envp de NAME=value satirini arayip value yu donduruyor
static char	*get_env_val(char *name, char **envp)
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

// result stringine src yi ekliyor, bellegi yonetiyor
static int	str_append(char **dest, char *src)
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

// $ den sonraki degiskeni genisletiyor: $? exit kodu, $VAR env degeri
static int	expand_dollar(char **res, char *str, int *i, t_shell *shell)
{
	char	*name;
	char	*num;
	char	*val;
	int		start;

	if (str[*i] == '?')
	{
		num = ft_itoa(shell->last_exit);
		(*i)++;
		if (!str_append(res, num))
			return (free(num), 0);
		return (free(num), 1);
	}
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

// token degerini karakter karakter tarayip $ genisletmesi yaparak yeni string uretir
static char	*build_expanded(char *str, t_shell *shell)
{
	char	*res;
	char	buf[2];
	int		i;
	int		in_sq;

	res = ft_strdup("");
	if (!res)
		return (NULL);
	buf[1] = '\0';
	i = 0;
	in_sq = 0;
	while (str[i])
	{
		if (str[i] == '\'')
			in_sq = !in_sq;
		if (str[i] == '$' && !in_sq && str[i + 1])
		{
			i++;
			if (!expand_dollar(&res, str, &i, shell))
				return (free(res), NULL);
			continue ;
		}
		buf[0] = str[i];
		if (!str_append(&res, buf))
			return (free(res), NULL);
		i++;
	}
	return (res);
}

// her tok_word icin build_expanded cagirip degeri gunceller
void	expand_tokens(t_token *head, t_shell *shell)
{
	char	*new_val;

	while (head)
	{
		if (head->type == TOK_WORD)
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
