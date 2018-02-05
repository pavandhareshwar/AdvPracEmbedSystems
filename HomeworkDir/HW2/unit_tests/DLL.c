#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "DLL.h"

/*
int main(void)
{
	struct node_ptr *head = NULL;
	struct node_ptr *cur_head = NULL;

	int list_size = size(cur_head);
	printf("Current size of the list: %d\n", list_size);
	cur_head = insert_at_beginning(head, 10);
	cur_head = insert_at_end(cur_head, 40);
	cur_head = insert_at_end(cur_head, 50);

	cur_head = insert_at_position(cur_head, 20, 1);	
	cur_head = insert_at_position(cur_head, 30, 2);
	cur_head = insert_at_beginning(cur_head, 5);
	cur_head = insert_at_position(cur_head, 1, 0);

	print_list(cur_head);

	cur_head = delete_from_position(cur_head, 1);	

	printf("After delete\n");	
	print_list(cur_head);
	
	list_size = size(cur_head);
	printf("Current size of the list: %d\n", list_size);

	return 0;	

}

void print_list(struct node_ptr *head_node)
{
	if (head_node == NULL)
	{
		return;
	}

	printf("Linked List Members: ");

	struct node_ptr *temp = head_node;

	while (temp != NULL)
	{
		struct node *node_to_print = GET_LIST_CONTAINER(temp, struct node, ptr_node);
		printf("%d ", node_to_print->data);

		temp = temp->next;
	}

	printf("\n");
}
*/

struct node_ptr* destroy(struct node_ptr *head_node)
{
	if (head_node == NULL)
	{
		printf("%s:%s:%d Linked list is empty. No nodes to destroy\n", __FILE__, __func__, __LINE__);
		return NULL;
	}

	struct node_ptr *temp = head_node;

	while (temp != NULL)
	{
		struct node *node_to_delete = GET_LIST_CONTAINER(temp, struct node, ptr_node);

		if (node_to_delete)
			free(node_to_delete);

		temp = temp->next;
	}
	
	return NULL;
}

struct node_ptr* insert_at_beginning(struct node_ptr *head_node, uint32_t new_data)
{
	struct node* new_node = (struct node*)malloc(sizeof(struct node));

	new_node->data = new_data;

	new_node->ptr_node.prev = NULL;
	new_node->ptr_node.next = head_node;

	struct node_ptr *new_head = &(new_node->ptr_node);

	if (head_node != NULL)
		head_node->prev = new_head;

	return new_head;
}

struct node_ptr* insert_at_end(struct node_ptr *head_node, uint32_t new_data)
{
	struct node* new_node = (struct node*)malloc(sizeof(struct node));

	new_node->data = new_data;

	new_node->ptr_node.next = NULL;
	
	if (head_node == NULL)
	{
		/* Empty linked list. Adding the node to the linked list and 
		   this becomes the head node  
		*/
		printf("%s:%s:%d Linked list is empty. Created node is made the head\n", __FILE__, __func__, __LINE__);
		new_node->ptr_node.prev = NULL;
		
		struct node_ptr *new_head = &(new_node->ptr_node);
		
		return new_head;
	}
	
	struct node_ptr *temp = head_node;

	while (temp->next != NULL)
		temp = temp->next;

	temp->next = &(new_node->ptr_node); /* new_node will be the next last element of the list */

	new_node->ptr_node.prev = temp;

	return head_node;
}

struct node_ptr* insert_at_position(struct node_ptr *head_node, uint32_t new_data, int index)
{
	if (head_node == NULL)
	{	
		printf("%s:%s:%d Linked list is empty. Node to insert is made the head\n", __FILE__, __func__, __LINE__);
		return NULL;
	}

	struct node* new_node = (struct node*)malloc(sizeof(struct node));

	new_node->data = new_data;

	struct node_ptr *temp = head_node;

	for (int i = 0; i < index; i++)
	{
		if (temp == NULL)
		{
			printf("%s:%s:%d Position at which node is to be added couldn't be reached\n", __FILE__, __func__, __LINE__);
			return head_node;
		}
		temp = temp->next;
	}

	if (index == 0)
	{
		printf("%s:%s:%d Position is %d. Adding node at the beginning of the list\n", __FILE__, __func__, __LINE__, index);
		return insert_at_beginning(head_node, new_data);
	}
	
	if (index == size(head_node))
	{
		printf("%s:%s:%d Position is %d. Adding node to the end of the list\n", __FILE__, __func__, __LINE__, index);
		return insert_at_end(head_node, new_data);
	}

	new_node->ptr_node.next = temp;
	new_node->ptr_node.prev = temp->prev;

	temp->prev = &(new_node->ptr_node);
	temp->prev->next = &(new_node->ptr_node);

	return head_node;
}

struct node_ptr* delete_from_beginning(struct node_ptr *head_node)
{
	if (head_node == NULL)
	{
		/* Linked list is empty. No nodes to delete */
		printf("%s:%s:%d Linked list is empty. No nodes to delete\n", __FILE__, __func__, __LINE__);
		return NULL;
	}	
	
	struct node *node_to_delete = GET_LIST_CONTAINER(head_node, struct node, ptr_node);

	struct node_ptr *head_to_return = head_node->next;

	if (head_to_return != NULL)
		head_to_return->prev = NULL;

	if (node_to_delete)
		free(node_to_delete);

	return head_to_return;	
}

struct node_ptr* delete_from_end(struct node_ptr *head_node)
{
	if (head_node == NULL)
	{
		printf("%s:%s:%d Linked list is empty. No nodes to delete\n", __FILE__, __func__, __LINE__);
		/* Linked list is empty. No nodes to delete */
		return NULL;
	}	

	struct node_ptr *temp = head_node;

	while (temp->next != NULL)
		temp = temp->next;

	temp->prev->next = NULL;
	
	struct node *node_to_delete = GET_LIST_CONTAINER(temp, struct node, ptr_node);
	
	if (node_to_delete)
		free(node_to_delete);

	return head_node;	
}

struct node_ptr* delete_from_position(struct node_ptr *head_node, int index)
{
	if (head_node == NULL)
	{
		printf("%s:%s:%d Linked list is empty. No nodes to delete\n", __FILE__, __func__, __LINE__);
		/* Linked list is empty. No nodes to delete */
		return NULL;
	}

	if (index == 0)
	{
		printf("%s:%s:%d Position is %d. Deleting node from the beginning of the list\n", __FILE__, __func__, __LINE__, index);
		return delete_from_beginning(head_node);
	}	

	if (index == size(head_node))
	{
		printf("%s:%s:%d Position is %d. Deleting node from the end of the list\n", __FILE__, __func__, __LINE__, index);
		return delete_from_end(head_node);
	}	

	struct node_ptr *temp = head_node;

	for (int i = 0; i < index; i++)
	{
		if (temp == NULL)
		{
			printf("%s:%s:%d Node to delete doesn't exist\n", __FILE__, __func__, __LINE__);
			return head_node;
		}
		temp = temp->next;
	}

	if (temp->next != NULL)
		temp->next->prev = temp->prev;
	
	temp->prev->next = temp->next;

	struct node *node_to_delete = GET_LIST_CONTAINER(temp, struct node, ptr_node);

	if (node_to_delete)
		free(node_to_delete);

	return head_node;
}

int size(struct node_ptr *head_node)
{
	int num_links_in_dll = 0;

	if (head_node == NULL)
	{
		printf("%s:%s:%d Linked list is empty. Returning size as 0\n", __FILE__, __func__, __LINE__);
		return 0;
	}

	struct node_ptr *temp = head_node;

	while (temp)
	{
		num_links_in_dll++;
		temp = temp->next;
	}

	return num_links_in_dll;
}
