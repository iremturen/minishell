#include "../../minishell.h"

// tek global: son sinyali tutuyor (subject: max 1 global)
int	g_signal = 0;

// ctrl+c: yeni satir basip prompt u tazeler
static void	sigint_handler(int sig)
{
	g_signal = sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_redisplay();
}

// interaktif mod: ctrl+c yakala, ctrl+\ yoksay
void	setup_signals_interactive(void)
{
	struct sigaction	sa;

	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

// child sureci: her iki sinyal icin varsayilan davranisi geri yukle
void	setup_signals_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
