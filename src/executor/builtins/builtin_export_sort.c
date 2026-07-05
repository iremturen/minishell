/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   builtin_export_sort.c                              :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/05 18:00:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../../minishell.h"

static void	print_export_entry(char *entry)
{
	char	*eq;

	write(1, "declare -x ", 11);
	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		write(1, entry, ft_strlen(entry));
		write(1, "\n", 1);
		return ;
	}
	write(1, entry, eq - entry + 1);
	write(1, "\"", 1);
	write(1, eq + 1, ft_strlen(eq + 1));
	write(1, "\"\n", 2);
}

static size_t	export_key_len(char *s, char *eq)
{
	if (eq)
		return ((size_t)(eq - s));
	return (ft_strlen(s));
}

static int	cmp_env_key(char *a, char *b)
{
	char	*eq_a;
	char	*eq_b;
	size_t	len_a;
	size_t	len_b;
	int		cmp;

	eq_a = ft_strchr(a, '=');
	eq_b = ft_strchr(b, '=');
	len_a = export_key_len(a, eq_a);
	len_b = export_key_len(b, eq_b);
	if (len_a <= len_b)
		cmp = ft_strncmp(a, b, len_a);
	else
		cmp = ft_strncmp(a, b, len_b);
	if (cmp != 0)
		return (cmp);
	if (len_a < len_b)
		return (-1);
	if (len_a > len_b)
		return (1);
	return (0);
}

static void	sort_export_order(int *order, int n, char **envp)
{
	int	i;
	int	j;
	int	tmp;

	i = 0;
	while (i < n - 1)
	{
		j = 0;
		while (j < n - i - 1)
		{
			if (cmp_env_key(envp[order[j]], envp[order[j + 1]]) > 0)
			{
				tmp = order[j];
				order[j] = order[j + 1];
				order[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}

void	print_sorted_export(char **envp)
{
	int		n;
	int		*order;
	int		i;

	n = 0;
	while (envp[n])
		n++;
	order = malloc(sizeof(int) * n);
	if (!order)
		return ;
	i = 0;
	while (i < n)
	{
		order[i] = i;
		i++;
	}
	sort_export_order(order, n, envp);
	i = 0;
	while (i < n)
	{
		print_export_entry(envp[order[i]]);
		i++;
	}
	free(order);
}
