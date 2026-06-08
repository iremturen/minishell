/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 00:05:18 by azkaraka          #+#    #+#             */
/*   Updated: 2025/07/01 00:22:19 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_numlen(long num, int sign)
{
	int	len;

	if (num == 0)
		return (1);
	len = 0;
	if (sign == -1)
		len++;
	while (num > 0)
	{
		num /= 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	char	*str;
	long	num;
	int		sign;
	int		num_len;

	num = n;
	sign = 1;
	if (num < 0)
		sign = -1;
	num *= sign;
	num_len = ft_numlen(num, sign);
	str = (char *)malloc(sizeof(char) * (num_len + 1));
	if (!str)
		return (NULL);
	str[num_len] = '\0';
	while (num_len > 0)
	{
		str[--num_len] = (num % 10) + '0';
		num /= 10;
	}
	if (sign == -1)
		str[0] = '-';
	return (str);
}
