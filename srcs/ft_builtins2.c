/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_builtins2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamuller <mamuller@student.42wolfsburg>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 21:26:00 by mamuller          #+#    #+#             */
/*   Updated: 2022/02/25 21:26:00 by mamuller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/minishell.h"

/**
	@brief Handels updating env var list and current dir change.
		In case there is symlink in the path.
	@param abs_path Absolute path string.
	@param current_path Current path string.
	@param pid Proccess id.
	@return None.
	@exception If PWD doesn't exists the OLDPWD is set to empty string.
 */
void	ft_cd_symlink_handler(char *abs_path, char *current_path, int pid)
{
	if (env_value_finder("PWD") == NULL || \
		ft_strlen(env_value_finder("PWD")) == 0)
		ft_update_create_env("OLDPWD", "", pid);
	else
		ft_update_create_env("OLDPWD", current_path, pid);
	chdir(abs_path);
	ft_smart_free((void **)&g_access.dp);
	g_access.dp = ft_strdup(abs_path);
	ft_smart_free((void **)&g_access.pwd);
	g_access.pwd = ft_strdup(abs_path);
	ft_update_env("PWD", abs_path);
}

/**
	@brief Handels updating env var list and current dir change.
		In case there is no symlink in the path.
	@param abs_path Absolute path string.
	@param current_path Current path string.
	@param pid Proccess id.
	@return None.
	@exception If PWD doesn't exists the OLDPWD is set to empty string.
 */
void	ft_cd_not_symlink_handler(char *abs_path, char *current_path, int pid)
{
	if (env_value_finder("PWD") == NULL || \
		ft_strlen(env_value_finder("PWD")) == 0)
		ft_update_create_env("OLDPWD", "", pid);
	else
		ft_update_create_env("OLDPWD", current_path, pid);
	chdir(abs_path);
	ft_smart_free((void **)&g_access.dp);
	ft_smart_free((void **)&g_access.pwd);
	g_access.pwd = ft_strdup(abs_path);
	ft_update_env("PWD", abs_path);
}

/**
	@brief Checks if directory can be opened.
	@param abs_path Absolute path string.
	@param pid Proccess id.
	@param args List of args.
	@return Returns 0 on success else 1.
 */
static int	ft_cd_open_dir_checker(char *abs_path, int pid, char **args)
{
	DIR	*dir;

	dir = opendir(abs_path);
	if (dir == NULL)
	{
		if (pid == 0)
		{
			write(2, "minishell: cd: ", 15);
			write(2, args[1], ft_strlen(args[1]));
			perror(" ");
		}
		ft_smart_free((void **)&abs_path);
		ft_set_lasts(args, pid, 1, FT_LAST_FULL_MODE);
		return (1);
	}
	else
		closedir(dir);
	return (0);
}

/**
	@brief Handels all kinds of links.
	@param abs_path Absolute path string.
	@param current_path Current path string.
	@param pid Proccess id.
	@param args List of args.
	@return None.
 */
static void	ft_cd_link_handler(char *abs_path, char *current_path, \
	int pid, char **args)
{
	int	sym_check;

	sym_check = ft_check_symlink(abs_path, args[1], pid);
	if (sym_check == -1)
	{
		ft_smart_free((void **)&current_path);
		ft_smart_free((void **)&abs_path);
		ft_set_lasts(args, pid, 1, FT_LAST_FULL_MODE);
	}
	else if (sym_check == SYMLINK)
		ft_cd_symlink_handler(abs_path, current_path, pid);
	else if (sym_check == NOT_SYMLINK)
		ft_cd_not_symlink_handler(abs_path, current_path, pid);
	ft_smart_free((void **)&abs_path);
	ft_smart_free((void **)&current_path);
}

/**
	@brief Removes repeating slashes '/'.
	@param cp String to be cleaned of slashes.
	@return None.
 */
static void	ft_path_cleaner(char **cp)
{
	char	*current_path_free;
	int		i;
	int		j;

	i = 0;
	while ((*cp)[i] != '\0')
	{
		j = i;
		while ((*cp)[j] == '/')
			j++;
		if ((j != i && (*cp)[j] == '\0') || (j > i + 1))
		{
			if (!ft_strcmp(*cp, "/"))
				break ;
			current_path_free = *cp;
			if ((*cp)[j] == '\0' && i != 0)
				*cp = ft_strjoin_with_free(ft_substr(*cp, 0, i), &(*cp)[j]);
			else
				*cp = ft_strjoin_with_free(ft_substr(*cp, 0, i + 1), &(*cp)[j]);
			ft_smart_free((void **)&current_path_free);
			i = 0;
			continue ;
		}
		i++;
	}
}

/**
	@brief Builtin command: cd. Changes current working directory and
		updates env vars accordingly.
	@param args List of args.
	@param pid Proccess id.
	@return Always returns 1, to continue execution.
 */
int	minishell_cd(char **args, pid_t pid)
{
	char	*abs_path;
	char	*current_path;

	ft_set_lasts(args, 0, 0, FT_LAST_RETURN_MODE);
	if (args[1] == NULL)
	{
		if (pid == 0)
			write(2, "minishell: cd without an argument not permitted.\n", 49);
		ft_set_lasts(args, pid, 1, FT_LAST_FULL_MODE);
		return (1);
	}
	abs_path = NULL;
	current_path = ft_strdup(args[1]);
	ft_path_cleaner(&current_path);
	ft_rtoa_path(current_path, &abs_path);
	ft_smart_free((void **)&current_path);
	if (ft_cd_open_dir_checker(abs_path, pid, args))
		return (1);
	if (g_access.dp != NULL)
		current_path = ft_strdup(g_access.dp);
	else
		ft_set_global_pwd(&current_path);
	ft_cd_link_handler(abs_path, current_path, pid, args);
	ft_set_lasts(args, 0, 0, FT_LAST_ARG_MODE);
	return (1);
}

/**
	@brief Handels printing of arguments and flag detection.
	@param args List of args.
	@param len Number of arguments.
	@return None.
 */
static void	echo_print_handler(char **args, int len)
{
	int	flag;
	int	i;
	int	temp;

	flag = 0;
	i = 1;
	while (args[i])
	{
		temp = echo_flag(args[i]);
		flag += temp;
		if (flag == 0 || temp == 0)
			break ;
		else
			i++;
	}
	echo_print(args, i, len, flag);
}

/**
	 @brief Builtin command: echo. Handels printing of arguments.
	 	With or without NL in the end. Flag -n is handled.
	 @param args List of args.
	 @param pid Proccess id.
	 @return Always returns 1.
*/
int	minishell_echo(char **args, pid_t pid)
{
	int	len;

	if (pid == 0)
	{
		len = 0;
		while (args[len] != NULL)
			len++;
		if (args[1] == NULL)
			write(1, "\n", 1);
		else
			echo_print_handler(args, len);
	}
	ft_set_lasts(args, pid, 0, FT_LAST_FULL_MODE);
	return (1);
}
