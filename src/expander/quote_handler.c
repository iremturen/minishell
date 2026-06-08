#include "../../minishell.h"

/*
** TODO: handle_quotes — token değerlerinden tırnak karakterlerini soyar
**                       ve quote state'ini takip eder.
**
** Genel akış:
**   - Her token'ın value'sunu karakter karakter tara.
**   - ' (tek tırnak) → Q_SINGLE moduna geç; kapanana kadar her şey literal.
**   - " (çift tırnak) → Q_DOUBLE moduna geç; $ expand edilir, \ escape edilir.
**   - Tırnak karakterleri sonuç string'den kaldırılır.
**
** Örnekler:
**   'hello world'  → [hello world]  (tek token, boşluk korunur)
**   "hello $HOME"  → [hello /Users/...] ($ expand edilir)
**   hel'lo'        → [hello]  (tırnak kaldırılır)
*/
void	handle_quotes(t_token *head)
{
	(void)head;
}
