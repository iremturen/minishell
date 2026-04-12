#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "Libft/libft.h"

//********************** 
typedef enum e_token_type
{
	TOK_WORD, // < < | >> << ' " dışındaki her şey
	TOK_PIPE, // |
	TOK_REDIR_IN, // <
	TOK_REDIR_OUT, // >
	TOK_APPEND, // >>
	TOK_HEREDOC // <<
}t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}t_token;
//********************** 
//********************** 

typedef struct s_redir
{
	char			*file; //redirs deki file ı tutar
	t_token_type	type;
	struct s_redir	*next;
}t_redir;

typedef struct s_cmd
{
	char			**argv; //komutun ismi ve yolu
	t_redir			*redirs; // cat < file 
	struct s_cmd	*next;
}t_cmd;
//********************** 


typedef enum e_quote_state
{
	Q_NONE,
	Q_SINGLE,
	Q_DOUBLE
}t_quote_state;


typedef struct s_shell
{
	char	**envp;
	int		last_exit;
}t_shell;

int		is_operator(char c);
char	**split_inputs(char *line);
size_t	get_spaced_len(char *line);
int		print_syntax_error(const char *token);
int		validate_lexer_syntax(char **args);

#endif
