#include "hls.h"

/**
 * create_file_node - Create the node
 *
 * @filename: Name of the node
 * @base_path: Absolute/relative path of the parent
 *
 * Return: Pointer to a file_node_t struct
 */
file_node_t *create_file_node(char *filename, char *base_path)
{
	file_node_t *node;

	node = malloc(sizeof(file_node_t));
	if (node == NULL)
		return (NULL);

	node->filename = filename;
	get_upper_filename(node);

	(void) base_path;
	/* 	get_node_info(node, base_path);*/

	node->next = NULL;
	node->prev = NULL;

	return (node);
}

/**
 * get_node_info - Get the stat information of a node
 *
 * @node: Node where the info will be inserted
 * @base_path: Absolute/relative path of the parent
 */
void get_node_info(file_node_t *node, char *base_path)
{
	struct stat sb;
	struct passwd *user_info;
	struct group *group_info;

	char *path;
	char *buff_user, *buff_group;

	path = base_path == NULL ?
		   node->filename :
		   build_path(base_path, node->filename);

	lstat(path, &sb);

	user_info = getpwuid(sb.st_uid);
	if (user_info == NULL)
	{
		buff_user = malloc(10 * sizeof(char));
		_itoa(sb.st_uid, buff_user, 10);
	}

	group_info = getgrgid(sb.st_gid);
	if (group_info == NULL)
	{
		buff_group = malloc(10 * sizeof(char));
		_itoa(sb.st_gid, buff_group, 10);
	}

	node->node_type = get_node_type(sb.st_mode);
	node->group_name = group_info != NULL ? group_info->gr_name : buff_group;
	node->owner_name = user_info != NULL ? user_info->pw_name : buff_user;
	node->last_modification = ctime(&(sb.st_mtime));
	node->size = sb.st_size;
	node->number_links = (long) sb.st_nlink;

	set_permissions(node, sb.st_mode);

	if (base_path != NULL)
		free(path);
}

/**
 * get_node_type - Get the type of the node
 *
 * @mode: Raw permissions and type of the node
 *
 * Return: Symbol type
 */
char get_node_type(mode_t mode)
{
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
