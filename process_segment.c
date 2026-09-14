/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_segment.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sofernan <sofernan@student.42madrid.es>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 14:44:09 by sofernan          #+#    #+#             */
/*   Updated: 2025/10/16 13:47:41 by sofernan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini.h"

int	has_outer_parentheses(const char *s)
{
	int		depth;
	size_t	i;
	char	quote;

	depth = 1;
	i = 1;
	quote = 0;
	if (!s || s[0] != '(')
		return (0);
	while (i < ft_strlen(s))
	{
		if ((s[i] == '\'' || s[i] == '"') && !quote)
			quote = s[i];
		else if (s[i] == quote)
			quote = 0;
		else if (!quote)
		{
			if (s[i] == '(')
				depth++;
			else if (s[i] == ')' && --depth == 0)
				break ;
		}
		i++;
	}
	return (depth == 0 && i == ft_strlen(s) - 1);
}

char	*trim_spaces(char *s)
{
	char	*start;
	char	*end;

	if (!s)
		return (NULL);
	start = s;
	while (*start && (*start == ' ' || *start == '\t'))
		start++;
	end = start + ft_strlen(start);
	while (end > start && (*(end - 1) == ' ' || *(end - 1) == '\t'))
		end--;
	return (ft_substr(start, 0, end - start));
}

char	*delete_outer_parentheses(char *s, int *deleted)
{
	char	*cur;
	char	*tmp;
	int		did_delete;
	int		len;

	if (deleted)
		*deleted = 0;
	if (!s)
		return (NULL);
	cur = trim_spaces(s);
	if (!cur)
		return (ft_strdup(""));
	did_delete = 0;
	while (has_outer_parentheses(cur))
	{
		len = ft_strlen(cur);
		tmp = ft_substr(cur, 1, len - 2);
		if (!tmp)
			return (NULL);
		(free(cur), cur = trim_spaces(tmp));
		(free(tmp), did_delete = 1);
	}
	if (deleted && did_delete)
		*deleted = 1;
	return (cur);
}

void	process_segment(t_minishell *mini, char *segment)
{
	char	*inner;
	int		is_group;

	is_group = 0;
	if (!segment || *segment == '\0')
		return ;
	inner = delete_outer_parentheses(segment, &is_group);
	if (is_group)
	{
		if (inner)
			execute_subshell(mini, inner);
		else
			execute_subshell(mini, "");
		if (inner)
			free(inner);
	}
	else
		process_command(mini, segment, inner);
}
