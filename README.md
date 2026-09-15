# Minishell 

<p align="center">
  <b>A Unix shell implemented from scratch in C</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&logoColor=black" alt="C">
  <img src="https://img.shields.io/badge/42-Common%20Core-black?style=for-the-badge" alt="42">
  <img src="https://img.shields.io/badge/Unix-Systems%20Programming-lightgrey?style=for-the-badge&logo=linux&logoColor=black" alt="Unix">
  <img src="https://img.shields.io/badge/Readline-CLI-blue?style=for-the-badge" alt="Readline">
  <img src="https://img.shields.io/badge/Makefile-Build-orange?style=for-the-badge" alt="Makefile">
</p>

---

## 📖 About

**Minishell** is a Unix shell implemented from scratch in **C**, inspired by the behavior of Bash.

The project is part of the **42 Madrid Common Core** and focuses on understanding how a shell interacts with the operating system: reading user input, parsing commands, expanding variables, creating processes, connecting them through pipes, applying redirections and executing programs.

Rather than relying on an existing shell parser, the project implements its own command-processing pipeline, including:

* Interactive command-line input
* Lexical analysis and tokenization
* Quote handling
* Environment-variable expansion
* Exit-status expansion
* Wildcard expansion
* Command parsing
* Built-in commands
* Process creation and execution
* Pipelines
* Input/output redirections
* Here-documents
* Signal handling
* File-descriptor management
* Dynamic memory management
* Error and resource handling

The implementation is organized into specialized modules for parsing, expansion, command construction and execution, making the project a practical exercise in **Unix systems programming and interpreter design**.

---

# ✨ Features

## Shell interaction

* Interactive prompt using GNU Readline
* Command history
* Current working directory displayed in the prompt
* `Ctrl-C`, `Ctrl-D` and `Ctrl-\` handling
* Exit-status management

Example:

```text
~/projects Minishell> ls -la
~/projects Minishell> echo "Hello World"
~/projects Minishell> pwd
```

---

## Command execution

Minishell can execute external programs using the system environment and executable paths.

Examples:

```bash
ls
ls -la
pwd
cat file.txt
grep "hello" file.txt
/bin/ls
/usr/bin/env
```

The execution layer manages processes and waits for child processes while preserving the shell's state and exit status.

---

# 🔗 Pipelines

Pipelines allow the standard output of one command to become the standard input of another.

```bash
cat file.txt | grep "error" | wc -l
```

The implementation creates the required processes and connects their standard input/output streams through Unix pipes.

Conceptually:

```text
┌─────────┐      pipe      ┌──────────┐      pipe      ┌─────────┐
│  cat    │ ─────────────► │   grep   │ ─────────────► │   wc    │
└─────────┘                └──────────┘                └─────────┘
   stdout                     stdin/stdout                 stdin
```

The pipeline execution layer manages:

* Pipe creation
* File-descriptor duplication
* Process creation
* Standard input/output redirection
* Closing unused file descriptors
* Child-process synchronization
* Exit-status handling

---

# 📂 Redirections

Minishell implements the standard shell redirection operators.

| Operator | Description                                       |
| :------: | ------------------------------------------------- |
|    `<`   | Redirect standard input from a file               |
|    `>`   | Redirect standard output to a file, truncating it |
|   `>>`   | Redirect standard output and append to a file     |
|   `<<`   | Here-document                                     |

Examples:

```bash
cat < input.txt
echo "Hello" > output.txt
echo "Another line" >> output.txt
```

Redirections are represented internally as dedicated redirection nodes and applied using file descriptors and `dup2()` before command execution.

The implementation handles different open modes depending on the redirection type:

```text
<   → O_RDONLY
>   → O_WRONLY | O_CREAT | O_TRUNC
>>  → O_WRONLY | O_CREAT | O_APPEND
```

---

# 📥 Here-Documents

The `<<` operator is supported through a dedicated here-document implementation.

Example:

```bash
cat << EOF
Hello
This text comes from a heredoc
EOF
```

The shell reads input until the specified delimiter is encountered and then provides that content to the command through standard input.

The project includes dedicated heredoc handling and utility functions to manage this process.

---

# 🔤 Quotes

Minishell handles quoted input and distinguishes between:

```text
' single quotes '
" double quotes "
```

The tokenizer keeps track of quote state while processing input.

This is important because shell expansion rules depend on the type of quotes used.

For example:

```bash
echo '$HOME'
```

does not expand `$HOME`, while:

```bash
echo "$HOME"
```

does.

The tokenizer therefore stores quote information together with each token so that later processing can apply the appropriate expansion rules.

---

# 💲 Environment Variables

Environment variables can be expanded inside commands.

Examples:

```bash
echo $HOME
echo $USER
echo $PATH
```

The shell also supports expansion of the previous command's exit status:

```bash
echo $?
```

Internally, the tokenizer distinguishes between:

```text
NO_EXPANSION
VAR_EXPANSION
EXIT_STATUS_EXPANSION
```

This separation allows expansion behavior to be handled according to the context in which a token was created.

---

# 🌟 Wildcard Expansion

The implementation also supports wildcard expansion using `*`.

Example:

```bash
ls *.c
```

The shell searches the appropriate directory and expands the pattern into matching filenames before command execution.

The implementation contains a dedicated globbing subsystem with:

* Directory traversal
* Pattern matching
* Match collection
* Hidden-file handling
* Dynamic match storage
* Pattern expansion

The project uses dedicated structures for managing the globbing context and the discovered matches.

---

# 🧰 Built-in Commands

Minishell implements the required shell built-ins directly rather than launching external programs.

|  Command | Purpose                                  |
| :------: | ---------------------------------------- |
|  `echo`  | Prints text to standard output           |
|   `cd`   | Changes the current working directory    |
|   `pwd`  | Prints the current working directory     |
| `export` | Creates or updates environment variables |
|  `unset` | Removes environment variables            |
|   `env`  | Displays environment variables           |
|  `exit`  | Terminates the shell                     |

Examples:

```bash
echo Hello
echo -n Hello

cd ..
pwd

export NAME="Soraya"
echo $NAME

unset NAME

env

exit
```

Built-ins are handled differently depending on whether they need to modify the shell's own process state.

For example, commands such as `cd`, `export` and `unset` need to affect the parent shell environment and therefore require dedicated parent-process handling.

---

# 🌳 Internal Architecture

One of the main challenges of Minishell is transforming a raw command line into something that can safely be executed.

The project separates this process into several conceptual stages:

```text
                 USER INPUT
                     │
                     ▼
              ┌─────────────┐
              │   Readline  │
              └──────┬──────┘
                     │
                     ▼
              ┌─────────────┐
              │ Tokenization│
              └──────┬──────┘
                     │
                     ▼
              ┌─────────────┐
              │   Quotes &  │
              │  Expansion  │
              └──────┬──────┘
                     │
                     ▼
              ┌─────────────┐
              │   Parsing   │
              └──────┬──────┘
                     │
                     ▼
              ┌─────────────┐
              │  Commands & │
              │ Redirections│
              └──────┬──────┘
                     │
                     ▼
              ┌─────────────┐
              │  Execution  │
              └──────┬──────┘
                     │
          ┌──────────┼──────────┐
          ▼          ▼          ▼
       Built-ins   Pipelines   External
                              commands
```

This separation makes the shell easier to reason about because parsing and execution are treated as different responsibilities.

---

# 🔍 Tokenization

The tokenizer converts the raw command line into an internal linked list of tokens.

The implementation distinguishes between token types such as:

```text
T_WORD
T_PIPE
T_RED_IN
T_RED_OUT
T_RED_APPEND
T_HEREDOC
T_INFILE
T_OUTFILE
T_EOF
```

Tokens also retain information about their quote state and possible expansion type.

This provides the information needed by later parsing and execution stages.

---

# 🧱 Internal Data Structures

The shell uses dedicated structures to represent the different stages of command processing.

### Environment

```c
typedef struct s_env
{
    int             signal;
    int             exported;
    char            *name;
    char            *value;
    struct s_env    *next;
}   t_env;
```

The environment is represented as a linked list, allowing variables to be added, modified and removed dynamically.

---

### Tokens

```c
typedef struct s_token
{
    int                 adjacent;
    char                *value;
    struct s_token      *next;
    t_token_type        type;
    t_token_quote       quote;
    t_expansion_type    expansion_type;
}   t_token;
```

Each token stores both its textual value and metadata required for parsing and expansion.

---

### Redirections

```c
typedef struct s_redir
{
    int                 type;
    char                *filename;
    struct s_redir      *next;
}   t_redir;
```

Redirections are represented independently from command arguments, allowing multiple redirections to be associated with the same command.

---

### Commands

```c
typedef struct s_command
{
    int                 append;
    int                 is_heredoc;
    char                **argv;
    char                *infile;
    char                *outfile;
    char                *heredoc_file;
    t_redir             *redirs;
    t_redir             *last_redir;
    struct s_command    *next;
}   t_command;
```

Commands are stored separately from tokens, providing a clean boundary between parsing and execution.

---

# ⚙️ Process Management

Minishell provides hands-on experience with Unix process management.

The execution layer works with mechanisms such as:

```text
fork()
execve()
waitpid()
pipe()
dup()
dup2()
close()
```

A typical command execution flow is:

```text
                 Shell
                   │
                 fork()
                   │
          ┌────────┴────────┐
          │                 │
       Parent             Child
          │                 │
          │              Redirections
          │                 │
          │              execve()
          │                 │
          │              Program
          │
       waitpid()
```

For pipelines, several child processes are created and their standard input/output streams are connected through pipes.

---

# 📡 File Descriptor Management

A large part of the project involves correctly managing Unix file descriptors.

The implementation uses:

* `dup()`
* `dup2()`
* `close()`
* `open()`
* `pipe()`

The shell also saves and restores its standard input so that commands involving pipes, heredocs or redirections do not permanently alter the interactive shell.

This is particularly important when executing multiple commands during the same shell session.

---

# 🧠 Signals

Minishell handles interactive signals and changes signal behavior depending on whether the shell or a child process is running.

The implementation uses:

```c
volatile sig_atomic_t g_status;
```

and handles signals such as:

* `SIGINT` — `Ctrl-C`
* `SIGQUIT` — `Ctrl-\`

The shell also handles `Ctrl-D` through EOF detection.

Signal behavior is adjusted between the interactive shell and child processes so that command execution behaves appropriately.

---

# 🌐 Environment Management

The shell maintains its own internal representation of the environment.

This allows it to implement:

```text
export
unset
env
$VARIABLE
$?
```

The environment is converted into the format required by `execve()` when external commands are launched.

The implementation also maintains the shell's exit status so it can be accessed through:

```bash
echo $?
```

---

# 🧩 Built-in Execution Model

Built-ins are separated into parent-process and child-process execution paths.

This distinction is essential because some built-ins modify the state of the shell itself.

For example:

```bash
cd ..
export USERNAME=Soraya
unset USERNAME
```

must modify the environment of the interactive shell rather than a temporary child process.

The project therefore contains dedicated execution paths for parent and child built-ins.

---

# 📁 Project Structure

The repository is organized around the different stages of the shell:

```text
minishell/
│
├── Makefile
├── mini.h
│
├── main.c
│
├── Tokenization & Parsing
│   ├── tokenize_input.c
│   ├── extract_next_token.c
│   ├── extract_token_part.c
│   ├── extract_quoted_token.c
│   ├── extract_metachar.c
│   ├── parse_commands.c
│   ├── command_split.c
│   ├── split_cmd_line.c
│   └── split_loop_and_append.c
│
├── Expansion
│   ├── expand_dollar.c
│   ├── expand_matches.c
│   ├── init_glob.c
│   └── match_glob.c
│
├── Command Processing
│   ├── process_command.c
│   ├── process_segment.c
│   ├── process_pipeline.c
│   ├── process_input.c
│   ├── process_directory.c
│   └── process_export_arg.c
│
├── Execution
│   ├── execute_command.c
│   ├── execute_pipeline.c
│   ├── execute_subshell.c
│   ├── handle_command.c
│   ├── exec_builtin_parent.c
│   ├── exec_builtin_child.c
│   └── utils_execute.c
│
├── Redirections & Heredocs
│   ├── process_redir1.c
│   ├── process_redir2.c
│   ├── here_doc.c
│   └── utils_heredoc.c
│
├── Built-ins & Environment
│   ├── ft_cd.c
│   ├── ft_export.c
│   ├── handle_export_cmd.c
│   ├── set_env_var.c
│   └── make_env_array.c
│
├── Memory & Cleanup
│   ├── free_minishell.c
│   └── free_and_exit.c
│
└── libft/
```

The repository separates the main concerns of the shell into multiple source files rather than implementing the entire interpreter in a monolithic source file.

---

# 🛠️ Technologies

### Language

**C**

### System / OS concepts

* Unix
* Linux
* Processes
* Signals
* File descriptors
* Inter-process communication
* Pipes
* Process synchronization

### Libraries

* GNU Readline
* Custom `libft`

### Development tools

* `cc`
* Make
* Git
* Valgrind
* Unix command-line tools

---

# ⚙️ Compilation

## Requirements

The project requires:

* A Unix-like operating system
* C compiler
* `make`
* GNU Readline development library

---

## Clone

```bash
git clone https://github.com/sorayaafdz/minishell.git
cd minishell
```

---

## Compile

```bash
make
```

The Makefile automatically builds the project's `libft` dependency and then compiles the Minishell executable.

The resulting executable is:

```text
./minishell
```

The project is compiled with:

```text
-Wall -Wextra -Werror
```

and links against:

```text
readline
```

---

## Clean

Remove object files:

```bash
make clean
```

Remove object files and the executable:

```bash
make fclean
```

Rebuild everything:

```bash
make re
```

---

# ▶️ Usage

Start the shell:

```bash
./minishell
```

You can then execute commands interactively:

```bash
Minishell> pwd
Minishell> ls -la
Minishell> echo "Hello World"
Minishell> cat file.txt | grep "hello"
Minishell> export NAME=Soraya
Minishell> echo $NAME
```

Exit the shell with:

```bash
exit
```

or:

```text
Ctrl-D
```

---

# 🧪 Example Session

```text
$ ./minishell

~/projects Minishell> echo "Hello from Minishell"
Hello from Minishell

~/projects Minishell> export NAME="Soraya"

~/projects Minishell> echo "Hello $NAME"
Hello Soraya

~/projects Minishell> ls -la | grep ".c"

~/projects Minishell> cat << EOF
> Minishell
> Unix
> C
> EOF
Minishell
Unix
C

~/projects Minishell> echo $?
0

~/projects Minishell> exit
```

---

# 🧪 Testing & Debugging

Because shell behavior depends heavily on edge cases, testing requires comparing the implementation against Bash and checking different combinations of:

* Quotes
* Variables
* Redirections
* Pipelines
* Heredocs
* Built-ins
* Empty commands
* Invalid syntax
* Missing files
* Invalid commands
* Signals
* Exit statuses
* Wildcard patterns
* Multiple commands and redirections

Memory management can also be investigated with tools such as **Valgrind**, particularly around:

* Token creation and destruction
* Command structures
* Environment variables
* Heredoc buffers
* Wildcard match arrays
* Child-process resources
* File descriptors

---

# 🧠 Key Engineering Challenges

## 1. Parsing shell syntax

Shell input cannot be processed simply by splitting on spaces.

For example:

```bash
echo "hello world"
```

must produce one logical argument:

```text
hello world
```

while:

```bash
echo hello world
```

produces two.

The parser therefore needs to understand context, quoting and shell metacharacters.

---

## 2. Expansion order

Variables and wildcards cannot be expanded blindly.

For example:

```bash
echo '$HOME'
echo "$HOME"
echo $HOME
```

have different semantics.

The implementation tracks quote state and expansion type so that tokens can be processed correctly.

---

## 3. Parent vs child execution

Some built-ins must execute in the parent shell because they modify persistent shell state.

Others can safely execute in a child process.

Handling this distinction correctly is an important part of reproducing shell behavior.

---

## 4. Pipelines and file descriptors

A pipeline such as:

```bash
cat file | grep text | wc -l
```

requires multiple processes and correctly connected file descriptors.

Incorrectly closing or duplicating descriptors can cause:

* Deadlocks
* Broken pipes
* Unexpected EOF
* Hanging processes
* Incorrect command output

This project therefore provides practical experience with Unix IPC and descriptor lifecycle management.

---

## 5. Signal behavior

Signals behave differently while the shell is waiting for user input and while child processes are executing commands.

The implementation therefore changes signal handling depending on the execution context.

---

# 📚 What I Learned

Minishell was one of the most technically demanding projects of the 42 Common Core because it combines multiple areas of systems programming into a single application.

### Unix systems programming

* Process creation
* Process synchronization
* Pipes
* File descriptors
* Signals
* `execve`
* `fork`
* `waitpid`
* `dup2`

### Parsing & interpreters

* Lexical analysis
* Tokenization
* Syntax validation
* Quote handling
* Expansion
* Command construction

### Memory management

* Dynamic allocation
* Ownership management
* Linked data structures
* Cleanup paths
* Error handling
* Resource lifetime

### Software architecture

* Separation of concerns
* Modular source organization
* Internal data models
* Parser/executor separation
* Parent/child execution paths

### CLI development

* Interactive input
* Command history
* Signal handling
* Exit status
* Shell state

---

# 🎯 Skills Demonstrated

```text
                    MINISHELL
                        │
        ┌───────────────┼────────────────┐
        │               │                │
      PARSING        EXECUTION        SYSTEMS
        │               │                │
    Tokenization      fork()           Unix
    Quotes            execve()         Processes
    Expansion         waitpid()        Signals
    Redirections      pipe()           File descriptors
        │             dup2()           IPC
        │               │                │
        └───────────────┼────────────────┘
                        │
                  MEMORY MANAGEMENT
                        │
                Allocation / Cleanup
                        │
                        ▼
                  SHELL INTERPRETER
```

---

# 🎓 42 Common Core

**Minishell** is a major systems-programming project from the **42 Madrid Common Core**.

It builds on concepts developed in earlier projects and combines them into a larger software system involving:

* C programming
* Unix
* Processes
* File descriptors
* Signals
* Parsing
* Memory management
* Data structures
* Command execution

The project was completed as part of my **42 Madrid Common Core**.

---

# 👩‍💻 Author

**Soraya Fernández**

Software Developer · 42 Madrid Common Core

GitHub: [@sorayaafdz](https://github.com/sorayaafdz)

---

<p align="center">
  <i>From command parsing to process execution — understanding how a shell works under the hood.</i>
</p>
