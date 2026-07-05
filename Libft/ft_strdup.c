/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ituren <ituren@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 00:54:44 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/05 12:26:07 by ituren           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *str)
{
	char	*s;
	size_t	len;

	len = ft_strlen(str);
	s = (char *)malloc(len + 1);
	if ((s) == NULL)
		return (NULL);
	ft_memcpy(s, str, len + 1);
	return (s);
}
