/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   redir_heredoc_read.c                               :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/05 18:00:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static ssize_t	read_hd_char(char *c, int *got_eof)
{
	ssize_t	n;

	while (1)
	{
		n = read(0, c, 1);
		if (n >= 0 || errno != EINTR)
			break ;
		if (get_signal() == SIGINT)
			return (-1);
	}
	if (n == 0)
		*got_eof = 1;
	return (n);
}

char	*read_heredoc_line(int *got_eof)
{
	char	buf[4096];
	int		i;
	ssize_t	n;
	char	c;

	i = 0;
	*got_eof = 0;
	if (isatty(STDIN_FILENO))
		write(1, "> ", 2);
	while (i < 4095)
	{
		n = read_hd_char(&c, got_eof);
		if (n < 0)
			return (NULL);
		if (n == 0)
			break ;
		if (c == '\n')
			break ;
		buf[i++] = c;
	}
	buf[i] = '\0';
	return (ft_strdup(buf));
}
