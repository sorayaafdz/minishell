/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_and_add_seg.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sofernan <sofernan@student.42madrid.es>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 14:43:56 by sofernan          #+#    #+#             */
/*   Updated: 2025/10/15 19:41:48 by sofernan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini.h"

int	handle_quote_parenthesis(char *input, t_split_state *st)
{
	if (input[st->pos] == '\'' || input[st->pos] == '"')
	{
		if (!st->quote)
			st->quote = input[st->pos];
		else if
			(st->quote == input[st->pos]) st->quote = 0;
		st->pos++;
		return (1);
	}
	if (!st->quote && input[st->pos] == '(')
	{
		st->paren_depth++;
		st->pos++;
		return (1);
	}
	if (!st->quote && input[st->pos] == ')')
	{
		if (st->paren_depth > 0)
			st->paren_depth--;
		st->pos++;
		return (1);
	}
	return (0);
}

int	add_str_to_array(char ***arr, int new_size, int copy_count, char *value)
{
	char	**newarr;
	int		i;

	newarr = malloc(sizeof(char *) * new_size);
	if (!newarr)
		return (0);
	i = 0;
	if (*arr)
	{
		while (i < copy_count)
		{
			newarr[i] = (*arr)[i];
			i++;
		}
		free(*arr);
	}
	newarr[copy_count] = value;
	*arr = newarr;
	return (1);
}

int	process_double_operator(char *input, t_split_state *st, char *op)
{
	char	*segment;
	char	*trimmed;
	char	*to_add;

	segment = ft_substr(input, st->start, st->pos - st->start);
	trimmed = trim_whitespace(segment);
	free(segment);
	if (trimmed)
		to_add = trimmed;
	else
	{
		to_add = ft_strdup("");
		if (!to_add)
			return (0);
	}
	if (!add_str_to_array(&st->segments, st->seg_count + 1,
			st->seg_count, to_add))
		return (free(to_add), 0);
	st->seg_count++;
	if (!add_str_to_array(&st->ops, st->seg_count,
			st->seg_count - 1, ft_strdup(op)))
		return (free(st->segments[st->seg_count - 1]), st->seg_count--, 0);
	st->pos += 2;
	st->start = st->pos;
	return (1);
}

int	try_process_operator(char *input, t_split_state *st)
{
	if (!st->quote && st->paren_depth == 0 && st->pos + 1 < st->len)
	{
		if (input[st->pos] == '&' && input[st->pos + 1] == '&')
			return (process_double_operator(input, st, "&&"));
		if (input[st->pos] == '|' && input[st->pos + 1] == '|')
			return (process_double_operator(input, st, "||"));
	}
	return (0);
}

void	extract_and_add_seg(char *input, t_split_state *st)
{
	char	*segment;
	char	*trimmed;

	while (st->pos < st->len)
	{
		if (handle_quote_parenthesis(input, st))
			continue ;
		if (try_process_operator(input, st))
			continue ;
		st->pos++;
	}
	segment = ft_substr(input, st->start, st->len - st->start);
	trimmed = trim_whitespace(segment);
	free(segment);
	if (trimmed == NULL)
		trimmed = ft_strdup("");
	if (!add_str_to_array(&st->segments, st->seg_count + 1,
			st->seg_count, trimmed))
	{
		if (trimmed)
			free(trimmed);
		return ;
	}
	st->seg_count++;
}
