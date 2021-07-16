#include "hls.h"

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
		new_node->next->prev = new_node;
		*head = new_node;
		return;
	}

	current = *head;
	while (current->next != NULL &&
		   _strcmp(current->next->filename_upper, new_node->filename_upper) <= 0)
		current = current->next;

	new_node->next = current->next;
	if (current->next != NULL)
		new_node->next->prev = new_node;

	current->next = new_node;
	new_node->prev = current;
}
