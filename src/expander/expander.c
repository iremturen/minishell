#include "../../minishell.h"

/*
** TODO: expand_tokens — token listesindeki $VAR ve $? ifadelerini genişletir.
**
** Genel akış:
**   - Her TOK_WORD token'ı tara.
**   - '$' karakteri bulunursa:
**       - '$?' → shell->last_exit değerini string'e çevir
**       - '$VAR' → shell->envp içinde VAR=... ara, değerini koy
**   - Tek tırnak içindeki $ genişletilmez ('hello $HOME' → "hello $HOME")
**   - Çift tırnak içindeki $ genişletilir ("hello $HOME" → "hello /Users/...")
*/
void	expand_tokens(t_token *head, t_shell *shell)
{
	(void)head;
	(void)shell;
}
