# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sofernan <sofernan@student.42madrid.es>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/07 14:20:26 by sofernan          #+#    #+#              #
#    Updated: 2025/10/15 17:15:34 by sofernan         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc
CFLAGS = -Wall -Wextra -Werror
MAKEFLAGS += --no-print-directory

LIBFT = libft
LIBFT_DIR = libft/
LIBFT_LIB = $(LIBFT_DIR)libft.a
LDFLAGS = -lreadline
VALGRIND_VALE	:=	./new_suppression.supp
VFLAGS			:=	--leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --suppressions=$(VALGRIND_VALE) --trace-children=yes

OBJ_DIR = obj
SRCS = process_input.c handle_incomplete_cmd.c split_cmd_line.c exec_builtin_parent.c \
		handle_word_token.c init_glob.c split_loop_and_append.c execute_subshell.c \
		process_segment.c ft_cd.c execute_command.c handle_command.c expand_matches.c \
		process_pipeline.c execute_pipeline.c extract_token_part.c tokenize_input.c \
		process_command.c extract_next_token.c main.c parse_commands.c process_redir1.c process_redir2.c \
		utils_heredoc.c utils_execute.c exec_builtin_child.c process_dir.c match_glob.c \
		make_env_array.c free_and_exit.c extract_quoted_token.c ft_export.c set_env_var.c \
		expand_dollar.c extract_metachar.c free_minishell.c here_doc.c process_export_arg.c

OBJS = $(SRCS:.c=.o)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(OBJS))

NAME = minishell
INCLUDES = -I.

TOTAL_FILES := $(words $(SRCS))
COUNT = 0

all: $(LIBFT_LIB) $(NAME)

$(LIBFT_LIB):
	@echo "\033[1;33mCompiling libft...\033[0m"
	@$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJ_FILES) $(LIBFT_LIB)
	@$(CC) $(CFLAGS) $(OBJ_FILES) -o $(NAME) $(LIBFT_LIB) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@if [ "$(COUNT)" -eq "0" ]; then echo "\033[1;33mCompiling Mini...\033[0m"; fi
	$(eval COUNT=$(shell echo $$(( $(COUNT) + 1 ))))
	$(eval PERCENT=$(shell echo $$(( $(COUNT) * 100 / $(TOTAL_FILES) ))))
	@echo "\033[1;32m[ $(PERCENT)%] Building C object $@\033[0m"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "\033[1;33mCleaning all .o files...\033[0m"
	@rm -rf $(OBJ_DIR)
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@rm -rf $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

valgrind: re
	valgrind $(VFLAGS) ./$(NAME)

re: fclean all

.PHONY: all clean fclean re
