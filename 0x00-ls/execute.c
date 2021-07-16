#include "hls.h"

/**
 * execute_hls - General information and state of the hls command
 *
 * @info: Struct with the global states
 *
 * Return: Errno code
 */
int execute_hls(general_t *info)
{
	parent_node_t *parent_node;

	parent_node = get_path_nodes(info, info->argc > 1 ? info->argv[1] : ".");
	if (parent_node == NULL)
		return (errno);

	print_directory(parent_node);
	free_directories(parent_node);
	free(parent_node);
	return (0);
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
	struct dirent *read;
	file_node_t *current;

	if (path == NULL)
		return (NULL);

	parent = malloc(sizeof(parent_node_t));
	if (parent == NULL)
		return (NULL);

	parent->dir_stream = opendir(path);

	switch (errno)
	{
		case ENOTDIR: /* the filename is not a directory, it's a file */
			perror("error 2");
			break;
		case ENOENT: /* the filename doesn't exists */
		case EACCES: /* permissions denied */
			free(parent);
			error(*info, path);
			return (NULL);
	}

	parent->filename = path;
	parent->head_file = NULL;

	current = NULL;

	while ((read = readdir(parent->dir_stream)) != NULL)
	{
		if (read->d_name[0] == '.')
			continue;

		current = create_file_node(read, parent->filename);
		sorted_insert(&parent->head_file, current);
	}

	return (parent);
}

/**
 * print_directory - Print the directory in the short format
 *
 * @parent: Parent node that contains the list of nodes
 */
void print_directory(parent_node_t *parent)
{
	file_node_t *head;

	head = parent->head_file;
	while (head != NULL)
	{
		printf("%s%s", head->filename, head->next == NULL ? "\n" : "  ");

		head = head->next;
	}
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

		free(aux->user_permissions);
		free(aux->other_permissions);
		free(aux->group_permissions);
		free(aux->filename_upper);

		if (aux->group_name && _isdigit(aux->group_name[0]))
			free(aux->group_name);

		if (aux->owner_name && _isdigit(aux->owner_name[0]))
			free(aux->owner_name);

		free(aux);
	}

	closedir(parent->dir_stream);
}

