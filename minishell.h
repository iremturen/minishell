#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "Libft/libft.h"

/* ══════════════════════════════════════════════════════
**  ENUM'LAR
** ══════════════════════════════════════════════════════ */

typedef enum e_token_type
{
	TOK_WORD,		// normal kelime: ls, -la, filename.txt
	TOK_PIPE,		// |
	TOK_REDIR_IN,	// <
	TOK_REDIR_OUT,	// >
	TOK_APPEND,		// >>
	TOK_HEREDOC		// <<
}	t_token_type;

typedef enum e_quote_state
{
	Q_NONE,
	Q_SINGLE,
	Q_DOUBLE
}	t_quote_state;

/* ══════════════════════════════════════════════════════
**  STRUCT'LAR
** ══════════════════════════════════════════════════════ */

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

typedef struct s_redir
{
	char			*file;
	char			*delimiter;
	int				fd;
	t_token_type	type;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	char			*cmd_path;
	int				is_builtin;
	t_redir			*redirs;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_shell
{
	char	**envp;
	int		last_exit;
}	t_shell;

/* ══════════════════════════════════════════════════════
**  INIT
** ══════════════════════════════════════════════════════ */

t_shell		*init_shell(char **envp);
void		free_shell(t_shell *shell);

/* ══════════════════════════════════════════════════════
**  LEXER
** ══════════════════════════════════════════════════════ */

t_token		*tokenize(char *line);
void		free_tokens(t_token *head);

int			is_operator(char c);
size_t		get_spaced_len(char *line);
char		*add_spaces(char *line);
char		**split_inputs(char *line);
int			validate_lexer_syntax(char **args);
int			print_syntax_error(const char *token);

/* ══════════════════════════════════════════════════════
**  PARSER
** ══════════════════════════════════════════════════════ */

t_cmd		*parse(t_token *tokens);
void		free_cmds(t_cmd *head);

/* ══════════════════════════════════════════════════════
**  EXPANDER
** ══════════════════════════════════════════════════════ */

void		expand_tokens(t_token *head, t_shell *shell);
void		handle_quotes(t_token *head);

/* ══════════════════════════════════════════════════════
**  EXECUTOR
** ══════════════════════════════════════════════════════ */

void		execute_cmd(t_cmd *cmd, t_shell *shell);
void		execute_builtin(t_cmd *cmd, t_shell *shell);
void		execute_single(t_cmd *cmd, char **envp);

char		**find_path(char **envp);
char		*find_command(char **paths, char *cmd);
char		*resolve_path(char *cmd, char **envp);
void		free_array(char **arr);

/* ══════════════════════════════════════════════════════
**  BUILTINS
** ══════════════════════════════════════════════════════ */

int			is_builtin(char *cmd);
void		builtin_cd(t_cmd *cmd, t_shell *shell);
void		builtin_echo(char **argv);
void		builtin_pwd(void);
void		builtin_env(char **envp);
void		builtin_unset(t_cmd *cmd, t_shell *shell);
void		builtin_exit(t_shell *shell);

#endif
