#include "hls.h"

/**
 * create_file_node - Create the node
 *
 * @dir: Pointer to the opened file/directory
 * @base_path: Absolute/relative path of the parent
 *
 * Return: Pointer to a file_node_t struct
 */
file_node_t *create_file_node(struct dirent *dir, char *base_path)
{
	file_node_t *node;

	node = malloc(sizeof(file_node_t));
	if (node == NULL)
		return (NULL);

	node->filename = dir->d_name;
	get_upper_filename(node);
	get_node_info(node, base_path);

	node->next = NULL;
	node->previous = NULL;

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

	char *path;

	path = base_path == NULL ?
		   node->filename :
		   build_path(base_path, node->filename);

	lstat(path, &sb);

	node->node_type = get_node_type(sb.st_mode);
	node->group_name = getgrgid(sb.st_gid)->gr_name;
	node->owner_name = getpwuid(sb.st_uid)->pw_name;
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

