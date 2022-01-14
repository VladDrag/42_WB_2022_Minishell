#include "../incl/minishell.h"
#include "../incl/minishe11.h"

int	main(int argc, char **argv, char**envp)
{

// ************************************Part 1******************************
/* 	char	*args;
	t_list	*list;
	int		temp;
	if (argc > 1)
		printf("Invalid number of arguments for %s with %s\n", argv[0], envp[0]);

	args = NULL;
	// args++;
	temp = 0;
	list = NULL;
	while (1)
	{
		args = readline(">");
		temp = lexor(&list, args);
		if (temp == 0)
		{
			temp = parser();
			if (temp != 0)
				break ;
			//executor(shell);
		}

	} */
// ************************************Part 2******************************
	t_shell *shell;
	char	*line;
	char	**args;
	int		status;

	if (argc > 1)
		printf("Invalid number of arguments for %s\n", argv[0]);
	shell = malloc(sizeof(t_shell) * 1);
	line = NULL;
	status = 1;
	// int i = 0;
	// while (envp[i])
	// {
	// 	printf("%s\n", envp[i]);
	// 	i++;
	// }
	ft_initiator_exc(&(shell), envp);
	// Run command loop.
	while (status)
	{
		line = readline(">");
		args = minishell_split_line(line); // !!!!!!!!!! Lexor/Parser Part !!!!!!!!!!!
		status = minishell_execute(args, shell); // !!!!!!!!!! Execution Part !!!!!!!!!!!
		free(line);
		free(args);
	}
	// free(args);
	return (0);
}
