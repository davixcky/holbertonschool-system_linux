#include "hls.h"

/**
 * sorted_insert2 - Insert the node taking in count the order
 *
 * @head: Pointer to pointer to the head of the linked list of nodes
 * @new_node: The new node that is going to be added to the list
 */
void sorted_insert2(parent_node_t **head, parent_node_t *new_node)
{
	parent_node_t *current;

	if (head == NULL)
		return;

	if (*head == NULL)
	{
		*head = new_node;
		return;
	}

	if (_strcmp((*head)->filename, new_node->filename) > 0)
	{
		new_node->next = *head;
		new_node->next->prev = new_node;
		*head = new_node;
		return;
	}

	current = *head;
	while (current->next != NULL &&
		   _strcmp(current->next->filename, new_node->filename) < 0)
		current = current->next;

	new_node->next = current->next;
	if (current->next != NULL)
		new_node->next->prev = new_node;

	current->next = new_node;
	new_node->prev = current;
}
