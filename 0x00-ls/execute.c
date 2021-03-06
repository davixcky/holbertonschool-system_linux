#include "hls.h"

/**
 * execute_directories - Extract the directories for all the nodes
 *
 * @info: General info of the hls command
 */
void execute_directories(general_t *info)
{
	char **aux, *path;
	parent_node_t *current, *head;

	aux = info->argv;
	while (*aux)
	{
		path = *aux;
		aux++;
		if (_strcmp(path, info->argv[0]) == 0)
			continue;

		current = get_path_nodes(info, path);
		if (current == NULL)
			continue;

		sorted_insert2(&info->head_parent, current);
	}

	print_files(info, 1);

	head = info->head_parent;
	while (head)
	{
		current = head->next;
		print_parent_node(head, 1);
		head = current;
	}
}

/**
 * execute_hls - General information and state of the hls command
 *
 * @info: Struct with the global states
 *
 * Return: Errno code
 */
int execute_hls(general_t *info)
{
	parent_node_t *current;

	if (info->argc <= 2)
	{
		current = get_path_nodes(info, info->argc > 1 ? info->argv[1] : ".");
		if (current == NULL)
		{
			switch (info->errno_value)
			{
				case ENOTDIR:
					print_files(info, 0);
					return (0);
				case ENOENT: /* the filename doesn't exists */
				case EACCES:
					return (errno);
			}

			return (errno);
		}

		print_parent_node(current, 0);
		return (0);
	}

	execute_directories(info);

	return (0);
}

/**
 * print_parent_node - Print the parent node and free the memory
 * @node: Node to print and manage
 * @is_multi_file: If it's true it will print some extra
 * line and info otherwise it will show the basics
 */
void print_parent_node(parent_node_t *node, int is_multi_file)
{
	if (is_multi_file)
		printf("%s:\n", node->filename);
	print_directory(node);

	if (is_multi_file)
		printf("\n");
	free_directories(node);
	free(node->filename_upper);
	free(node);
}

/**
 * get_directories_nodes - Get the nodes directories
 *
 * @parent: Parent node where the child nodes where go
 */
void get_directories_nodes(parent_node_t *parent)
{
	struct dirent *read;

	parent->head_file = NULL;
	parent->next = NULL;
	parent->prev = NULL;

	while ((read = readdir(parent->dir_stream)) != NULL)
	{
		if (read->d_name[0] == '.')
			continue;

		sorted_insert(&parent->head_file,
					  create_file_node(read->d_name, parent->filename));
	}
}

/**
 * get_path_nodes - Generate all the directories and files for a parent dir
 *
 * @info: Struct with the global states
 * @path: String of the directory to be open
 *
 * Return: Parent node that contains the list of nodes
 */
parent_node_t *get_path_nodes(general_t *info, char *path)
{
	parent_node_t *parent;

	if (path == NULL)
		return (NULL);

	parent = malloc(sizeof(parent_node_t));
	if (parent == NULL)
		return (NULL);

	parent->dir_stream = opendir(path);
	if (errno)
	{
		info->errno_value = errno;
		errno = 0;
		switch (info->errno_value)
		{
			case ENOTDIR: /* the filename is not a directory, it's a file */
				sorted_insert(&info->head_files, create_file_node(path, ""));
				free(parent);
				return (NULL);
			case ENOENT: /* the filename doesn't exists */
			case EACCES: /* permissions denied */
				free(parent);
				error(*info, path);
				return (NULL);
		}
	}

	parent->filename = path;
	parent->filename_upper = get_upper_filename(path);
	get_directories_nodes(parent);

	return (parent);
}

/**
 * free_directories - Deallocate the spaces allocated for the nodes
 *
 * @parent: Parent node that contains the list of nodes
 */
void free_directories(parent_node_t *parent)
{
	file_node_t *head, *aux;

	head = parent->head_file;
	for (; head != NULL;)
	{
		aux = head;
		head = head->next;

	/**
	 * free(aux->user_permissions);
	 * free(aux->other_permissions);
	 * free(aux->group_permissions);
	 **/
		free(aux->filename_upper);

	/**
	 * if (aux->group_name && _isdigit(aux->group_name[0]))
	 * free(aux->group_name);
	 **/

	/**
	 * if (aux->owner_name && _isdigit(aux->owner_name[0]))
	 * free(aux->owner_name);
	 **/

		free(aux);
	}

	closedir(parent->dir_stream);
}

