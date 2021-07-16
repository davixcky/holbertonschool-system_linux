#include "hls.h"

/**
 * print_list - Print list of file_node_t
 * @head: First element of the list
 */
void print_list(file_node_t *head)
{
	file_node_t *tmp;

	tmp = head;
	while (tmp != NULL)
	{
		printf("%s%s", tmp->filename, tmp->next == NULL ? "\n" : "  ");

		tmp = tmp->next;
	}
}

/**
 * print_directory - Print the directory in the short format
 *
 * @parent: Parent node that contains the list of nodes
 */
void print_directory(parent_node_t *parent)
{
	print_list(parent->head_file);
}

/**
 * print_files - Print the files
 * @info: Global pointer state
 */
void print_files(general_t *info)
{
	print_list(info->head_files);
}
