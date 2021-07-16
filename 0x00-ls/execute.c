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
		free(aux);
	}

	closedir(parent->dir_stream);
}

/**
 * build_path - Allocate the absolute path for the file
 *
 * @base_path: Path of the parent directory
 * @filename: Name of the file
 *
 * Return: Absolute path of the file
 */
char *build_path(char *base_path, char *filename)
{
	char *buff;

	buff = malloc(_strlen(base_path) + _strlen(filename) + 2);
	if (buff == NULL)
		return (NULL);

	sprintf(buff, "%s%s%s",
			base_path,
			base_path[_strlen(base_path) - 1] == '/' ? "" : "/",
			filename);

	return (buff);
}

/**
 * set_permissions - Detect the permissions group for the node
 *
 * @node: Node to calculate the permissions
 * @mode: Raw permissions values
 */
void set_permissions(file_node_t *node, mode_t mode)
{
	node_permissions_t *user_permissions, *group_permissions, *other_permissions;

	user_permissions = malloc(sizeof(node_permissions_t));
	if (user_permissions == NULL)
		return;

	group_permissions = malloc(sizeof(node_permissions_t));
	if (group_permissions == NULL)
	{
		free(user_permissions);
		return;
	}

	other_permissions = malloc(sizeof(node_permissions_t));
	if (other_permissions == NULL)
	{
		free(user_permissions);
		free(group_permissions);
		return;
	}

	user_permissions->read = (mode & S_IRUSR) ? 'r' : '-';
	user_permissions->write = (mode & S_IWUSR) ? 'w' : '-';
	user_permissions->execute = (mode & S_IXUSR) ? 'x' : '-';

	group_permissions->read = (mode & S_IRGRP) ? 'r' : '-';
	group_permissions->write = (mode & S_IWGRP) ? 'w' : '-';
	group_permissions->execute = (mode & S_IXGRP) ? 'x' : '-';

	other_permissions->read = (mode & S_IROTH) ? 'r' : '-';
	other_permissions->write = (mode & S_IWOTH) ? 'w' : '-';
	other_permissions->execute = (mode & S_IXOTH) ? 'x' : '-';

	node->user_permissions = user_permissions;
	node->group_permissions = group_permissions;
	node->other_permissions = other_permissions;
}



/**
 * get_upper_filename - Deallocate the for the node filename in uppercase
 *
 * @node: Node to set the upper filename
 */
void get_upper_filename(file_node_t *node)
{
	char *s;

	s = _strdup(node->filename);
	string_toupper(s);

	node->filename_upper = s;
}

/**
 * sorted_insert - Insert the node taking in count the order
 *
 * @head: Pointer to pointer to the head of the linked list of nodes
 * @new_node: The new node that is going to be added to the list
 */
void sorted_insert(file_node_t **head, file_node_t *new_node)
{
	file_node_t *current;

	if (head == NULL)
		return;

	if (*head == NULL)
	{
		*head = new_node;
		return;
	}

	if (_strcmp((*head)->filename_upper, new_node->filename_upper) > 0)
	{
		new_node->next = *head;
		new_node->next->previous = new_node;
		*head = new_node;
		return;
	}

	current = *head;
	while (current->next != NULL &&
			_strcmp(current->next->filename_upper, new_node->filename_upper) <= 0)
		current = current->next;

	new_node->next = current->next;
	if (current->next != NULL)
		new_node->next->previous = new_node;

	current->next = new_node;
	new_node->previous = current;
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
