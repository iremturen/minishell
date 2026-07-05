/* ************************************************************************** */
/*                                                                            */
/*                                                      :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                  +#+  +:+       +#+        */
/*   By: azkaraka <azkaraka@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                  #+#    #+#             */
/*   Created: 2025/05/31 16:30:24 by azkaraka          #+#    #+#             */
/*   Updated: 2026/07/04 21:30:00 by azkaraka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <string.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "Libft/libft.h"

# define PROMPT "minishell$ "

// token tipleri: kelime, pipe, yonlendirme operatorleri
typedef enum e_token_type
{
	TOK_WORD,
	TOK_PIPE,
	TOK_REDIR_IN,
	TOK_REDIR_OUT,
	TOK_APPEND,
	TOK_HEREDOC
}	t_token_type;

// tirnak durumu: yok, tek, cift
typedef enum e_quote_state
{
	Q_NONE,
	Q_SINGLE,
	Q_DOUBLE
}	t_quote_state;

// lexer token dugumu
typedef struct s_token
{
	char			*value;
	t_token_type	type;
	int				is_quoted;
	struct s_token	*next;
}	t_token;

// yonlendirme bilgisi: dosya adi, heredoc delimiter, fd, tip
typedef struct s_redir
{
	char			*file;
	char			*delimiter;
	int				fd;
	int				heredoc_expand;
	t_token_type	type;
	struct s_redir	*next;
}	t_redir;

// bir komut dugumu: argümanlar, yol, builtın mi, yonlendirmeler
typedef struct s_cmd
{
	char			**argv;
	char			*cmd_path;
	int				is_builtin;
	t_redir			*redirs;
	struct s_cmd	*next;
}	t_cmd;

// genel shell durumu: env kopyasi ve son exit kodu
typedef struct s_shell
{
	char	**envp;
	int		last_exit;
}	t_shell;

typedef struct s_fork_data
{
	int		in_fd;
	int		out_fd;
	int		close_fd;
	t_shell	*shell;
}	t_fork_data;

typedef struct s_pipe_ctx
{
	int		prev_fd;
	int		pipefd[2];
	pid_t	pids[256];
	int		n;
	t_shell	*shell;
}	t_pipe_ctx;

// init
t_shell		*init_shell(char **envp);
void		free_shell(t_shell *shell);
void		print_prompt_if_needed(void);
void		setup_signals_interactive(t_shell *shell);
void		setup_signals_child(void);
void		reset_readline_line(void);
int			get_signal(void);
void		set_signal(int sig);
void		clear_signal(void);

// lexer
t_token		*tokenize(char *line, t_shell *shell);
void		free_tokens(t_token *head);
int			is_operator(char c);
size_t		get_spaced_len(char *line);
char		**split_inputs(char *line);
int			validate_lexer_syntax(char **args);
int			print_syntax_error(const char *token);
int			has_unclosed_quotes(char *line);
int			word_end_q(char *s, int i);
int			count_words_q(char *s);
void		free_array(char **arr);

// parser
t_cmd		*parse(t_token *tokens);
t_cmd		*new_cmd(void);
void		free_cmds(t_cmd *head);
t_redir		*new_redir(t_token_type type, char *file, int heredoc_expand);
int			cmd_add_redir(t_cmd *cmd, t_token_type type, char *file,
			int heredoc_expand);

// expander
void		expand_tokens(t_token *head, t_shell *shell);
char		*build_expanded(char *str, t_shell *shell);
void		filter_empty_tokens(t_token **head);
void		handle_quotes(t_token *head);
char		*get_env_val(char *name, char **envp);
int			str_append(char **dest, char *src);
int			append_char(char **res, char c);

// executor
void		execute_cmd(t_cmd *cmd, t_shell *shell);
void		execute_builtin(t_cmd *cmd, t_shell *shell);
void		execute_single(t_cmd *cmd, t_shell *shell);
void		execute_pipeline(t_cmd *cmds, t_shell *shell);
pid_t		pipeline_fork(t_cmd *cmd, t_fork_data *data);
int			apply_redirs(t_redir *redir, t_shell *shell);
int			process_heredoc(t_redir *redir, t_shell *shell);
char		**find_path(char **envp);
char		*find_command(char **paths, char *cmd);
char		*resolve_path(char *cmd, char **envp);
void		exec_or_exit(t_cmd *cmd, char **envp);

// builtins
int			is_builtin(char *cmd);
void		builtin_echo(char **argv);
void		builtin_pwd(void);
void		builtin_env(char **envp);
void		builtin_exit(t_cmd *cmd, t_shell *shell);
void		builtin_cd(t_cmd *cmd, t_shell *shell);
void		builtin_unset(t_cmd *cmd, t_shell *shell);
void		builtin_export(t_cmd *cmd, t_shell *shell);
int			env_set(t_shell *shell, char *key, char *val);
int			env_export(t_shell *shell, char *key);
int			env_find(char **envp, char *key);
char		*create_env_entry(char *key, char *val);
int			add_new_env(t_shell *shell, char *entry);
char		**build_exec_envp(char **envp);

#endif
