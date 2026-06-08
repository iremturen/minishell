#include "../../minishell.h"

// char** dizisini eleman eleman serbest birakiyor
void	free_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}
