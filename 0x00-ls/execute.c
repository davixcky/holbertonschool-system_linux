#include "hls.h"

#include <string.h>

parent_node_t *get_path_nodes(general_t *info, char *path);
char get_node_type(mode_t mode);
void get_node_info(file_node_t *node, char *base_path);
char *build_path(char *base_path, char *filename);
void set_permissions(file_node_t *node, mode_t mode);
void free_directories(parent_node_t *parent);
void sort_directories(parent_node_t *parent);

void print_directory(parent_node_t *parent)
{
	file_node_t *head;

	head = parent->head_file;
	while (head != NULL)
	{
		printf("%s%s", head->file_name, head->next == NULL ? "\n" : "  ");

		head = head->next;
	}
}

void sort_directories(parent_node_t *parent) {
	file_node_t *head, *next, *previous;
	char *tmp1, *tmp2;

	head = parent->head_file;
	previous = NULL;
	while (head != NULL && head->next != NULL) {
		next = head->next;

		tmp1 = _strdup(head->file_name);
		tmp2 = _strdup(next->file_name);

		string_toupper(tmp1);
		string_toupper(tmp2);

		if (_strcmp(tmp1, tmp2) > 0) {
			printf("cambiando %s %s\n", tmp1, tmp2);
			head->next = next->next;
			next->next = head;

			if (previous)
				previous->next = next;

			if (head == parent->head_file) {
				parent->head_file = next;
			}
		}

		free(tmp1);
		free(tmp2);

		previous = head;
		head = next;
	}
}

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
		free(aux);
	}

	closedir(parent->dir_stream);
}

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

	return buff;
}

void set_permissions(file_node_t *node, mode_t mode)
{
	node_permissions_t *user_permissions, *group_permissions, *other_permissions;

	user_permissions = malloc(sizeof(node_permissions_t));
	if (user_permissions == NULL)
		return;

	group_permissions = malloc(sizeof(node_permissions_t));
	if (group_permissions == NULL) {
		free(user_permissions);
		return;
	}

	other_permissions = malloc(sizeof(node_permissions_t));
	if (other_permissions == NULL) {
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

file_node_t *create_file_node(struct dirent *dir, char *base_path)
{
	file_node_t *node;

	node = malloc(sizeof(file_node_t));
	if (node == NULL)
		return (NULL);

	node->file_name = dir->d_name;
	get_node_info(node, base_path);

	node->next = NULL;

	return (node);
}

void get_node_info(file_node_t *node, char *base_path)
{
	struct stat sb;
	char *path;

	path = base_path == NULL ?
			node->file_name :
			build_path(base_path, node->file_name);

	lstat(path, &sb);

	node->node_type = get_node_type(sb.st_mode);
	node->owner_name = getpwuid(sb.st_uid)->pw_name;
	node->group_name = getgrgid(sb.st_gid)->gr_name;
	node->last_modification = ctime(&(sb.st_mtime));
	node->size = sb.st_size;
	node->number_links = (long) sb.st_nlink;

	set_permissions(node, sb.st_mode);

	if (base_path != NULL)
		free(path);
}

char get_node_type(mode_t mode) {
	int number_types, j;
	node_type_t types[] = {
			{S_IFLNK, 'l'},
			{S_IFSOCK, 's'},
			{S_IFIFO, 'p'},
			{S_IFREG, '-'},
			{S_IFDIR, 'd'},
			{S_IFCHR, 'c'},
			{S_IFBLK, 'b'}
	};

	number_types = sizeof(types) / sizeof(types[0]);
	for (j = 0; j < number_types; j++)
		if ((mode & S_IFMT) == types[j].key)
			return (types[j].type);

	return ('-');
}

parent_node_t *get_path_nodes(general_t *info, char *path)
{
	parent_node_t *parent;
	struct dirent *read;
	file_node_t *last_node;

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

	if (errno != 0)
	{
		perror("error here");
		exit(errno);
	}

	parent->filename = path;
	parent->head_file = NULL;

	last_node = NULL;

	while ((read = readdir(parent->dir_stream)) != NULL)
	{
		if (read->d_name[0] == '.')
			continue;

        if (parent->head_file == NULL) {
            parent->head_file = create_file_node(read, parent->filename);
			last_node = parent->head_file;
			 continue;
		}

		last_node->next = create_file_node(read, parent->filename);
		last_node = last_node->next;
	}

	return (parent);
}
