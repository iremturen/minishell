# minishell

*This project has been created as part of the 42 curriculum by azkaraka, ituren.*

---

## Description

minishell is a project from the 42 School curriculum that challenges students to build a functional Unix shell from scratch in C. The goal is to deeply understand how a shell works — from reading user input to executing processes — by reimplementing a subset of bash behavior.

The shell handles tokenization, parsing, environment variable expansion, quote processing, redirections, pipes, heredocs, built-in commands, and signal management. It uses `readline` for interactive input and history support.

---

## Instructions

### Requirements

- `gcc` with `-Wall -Wextra -Werror`
- `readline` library installed (`brew install readline` on macOS)

### Compilation

```bash
make
```

| Command      | Description                      |
|--------------|----------------------------------|
| `make`       | Build the `minishell` binary     |
| `make re`    | Clean and rebuild from scratch   |
| `make clean` | Remove object files              |
| `make fclean`| Remove objects and binary        |

### Execution

```bash
./minishell
```

The shell displays a prompt and waits for commands. It supports command history (arrow keys).

---

## Features

### Mandatory

- **Prompt** — displays `minishell$` when waiting for input
- **History** — navigate previous commands with arrow keys
- **Command execution** — resolves executables via `PATH`, relative, or absolute path
- **Single quotes** `'...'` — prevents all metacharacter interpretation
- **Double quotes** `"..."` — prevents metacharacter interpretation except `$`
- **Redirections**
  - `<` redirect stdin from a file
  - `>` redirect stdout to a file (truncate)
  - `>>` redirect stdout to a file (append)
  - `<<` heredoc — reads until a delimiter line is found
- **Pipes** `|` — connects stdout of one command to stdin of the next
- **Environment variables** — `$VAR` expands to its value
- **Exit status** — `$?` expands to the exit code of the last foreground pipeline
- **Signals**
  - `ctrl-C` — displays a new prompt on a new line
  - `ctrl-D` — exits the shell
  - `ctrl-\` — does nothing in interactive mode
- **Built-in commands**
  - `echo` with `-n` option
  - `cd` with relative or absolute path
  - `pwd` with no options
  - `export` with no options
  - `unset` with no options
  - `env` with no options or arguments
  - `exit` with no options

### Bonus

- `&&` and `||` with parenthesis for priority grouping
- Wildcard `*` expansion for the current working directory

---

## Project Structure

```
minishell/
├── main.c                  — entry point and main loop
├── minishell.h             — all structs, enums, and prototypes
├── Makefile
├── Libft/                  — custom C library
└── src/
    ├── init/               — shell initialization and environment copy
    ├── lexer/              — input tokenization and syntax validation
    ├── parser/             — token list to command tree conversion
    ├── expander/           — quote stripping and variable expansion
    └── executor/           — process execution, redirections, pipes
        └── builtins/       — built-in command implementations
```

---

## Resources

### Documentation & References

- [Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html) — primary reference for behavior and edge cases
- [GNU Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html) — for interactive input and history
- [POSIX Shell & Utilities](https://pubs.opengroup.org/onlinepubs/9699919799/) — specification for shell behavior
- [Advanced Programming in the UNIX Environment – Stevens](https://www.apuebook.com/) — reference for process, pipe, and signal handling

### AI Usage

AI assistance (Antigravity / Claude) was used during this project for the following purposes:

- **Project scaffolding** — setting up the initial directory structure and Makefile layout
- **Learning and explanation** — understanding how `fork`, `execve`, `pipe`, `dup2`, and `sigaction` interact at a systems level
- **Debugging guidance** — identifying logical errors and memory management issues during development
- **README drafting** — initial template generation, revised and verified by the authors

All code was reviewed, understood, and validated by the project authors. No function or logic was copied without being fully understood and tested.
