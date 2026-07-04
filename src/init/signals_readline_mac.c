/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   signals_readline_mac.c                             :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

void	reset_readline_line(void)
{
	rl_on_new_line();
	rl_redisplay();
}
