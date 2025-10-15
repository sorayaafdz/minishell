/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sofernan <sofernan@student.42madrid.es>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 14:44:02 by sofernan          #+#    #+#             */
/*   Updated: 2025/10/15 17:58:23 by sofernan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini.h"

int	handle_pipe_sequence(const char *s, int *i, t_minishell *mini, int cnt)
{
	int		tlen;
	int		j;
	char	*tok;

	while (s[*i] == '|')
	{
		cnt++;
		(*i)++;
	}
	if (cnt >= 3)
	{
		tlen = cnt - 2;
		tok = malloc(tlen + 1);
		if (!tok)
			return (0);
		j = 0;
		while (j < tlen)
		{
			tok[j] = '|';
			j++;
		}
		tok[tlen] = '\0';
		return (report_syntax_error(mini, tok), free(tok), 1);
	}
	return (0);
}

int	check_consecutive_pipes(const char *s, t_minishell *mini, int in_sq,
		int in_dq)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
		if (s[i] == '\'' && !in_dq)
			in_sq = !in_sq;
		else if (s[i] == '"' && !in_sq)
			in_dq = !in_dq;
		else if (!in_sq && !in_dq && s[i] == '|')
		{
			if (handle_pipe_sequence(s, &i, mini, 0))
				return (1);
			continue ;
		}
		i++;
	}
	return (0);
}

void	update_env_status(t_minishell *mini)
{
	char	*status_str;
	int		status;

	if (g_status < 0)
	{
		status = -g_status;
		g_status = SIGINT;
	}
	else
		status = (int)g_status;
	status_str = ft_itoa(status);
	if (status_str)
	{
		set_env_var(mini, "?", status_str, 0);
		free(status_str);
	}
}

void	exec_segments(t_minishell *mini, char **segments,
						char **ops, int seg_count)
{
	if (seg_count <= 0)
		return ;
	process_segment(mini, *segments);
	update_env_status(mini);
	if (seg_count > 1 && ops && *ops)
	{
		if (!ft_strcmp(*ops, "&&") && g_status != 0)
		{
			segments++;
			ops++;
			seg_count--;
		}
		else if (!ft_strcmp(*ops, "||") && g_status == 0)
		{
			segments++;
			ops++;
			seg_count--;
		}
	}
	exec_segments(mini, segments + 1, ops + 1, seg_count - 1);
}

void	process_input(char *input, t_minishell *mini)
{
	char	**segments;
	char	**ops;
	int		seg_count;
	int		history_added;

	segments = NULL;
	ops = NULL;
	seg_count = 0;
	history_added = 0;
	if (input && *input && !is_only_spaces(input) && !ends_with_operator(input))
		(add_history(input), history_added = 1);
	if (check_consecutive_pipes(input, mini, 0, 0))
	{
		if (!history_added && input && *input && !is_only_spaces(input))
			add_history(input);
		update_env_status(mini);
		return ;
	}
	if (!split_cmd_line(input, &segments, &ops, &seg_count))
	{
		update_env_status(mini);
		return ;
	}
	exec_segments(mini, segments, ops, seg_count);
	free_split_result(segments, ops, seg_count);
}
