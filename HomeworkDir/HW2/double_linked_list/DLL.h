#ifndef _DLL_H_
#define _DLL_H_

/* ------------------------------- MACROS -----------------------------------*/
#define OFFSET_OF(type, member) ((size_t) &((type *)0)->member)

#define GET_LIST_CONTAINER(addr, type, member) ({ 			\
	const typeof( ((type *)0)->member ) *__mptr = (addr); 	\
	(type *) ( (char *)__mptr - OFFSET_OF(type, member) );})


/* ---------------------- STRUCTURES/ENUMERATIONS ---------------------------*/
struct node_ptr 
{
	struct node_ptr *next;
	struct node_ptr *prev;
};

struct node
{
	uint32_t data;
	struct node_ptr ptr_node; 
};

/* ----------------------- FUNCTION PROTOTYPES ------------------------------*/

/**
 *	@brief Destroys all the nodes in the linked list.
 *	
 *	Given the pointer to the head node, this will destroys all the nodes in the
 *  linked list and frees the resources

 *  @param head_node : pointer to linked list data structure 
 *
 *	@return pointer to the head of linked list
*/
struct node_ptr* destroy(struct node_ptr *head_node);

/**
 *	@brief Adds a node to the beginning of the linked list.
 *	
 *	Given the pointer to head node, this will create a new node and add it to 
 *	to the beginning of the linked list. If head node doesn't exist, this new
 *	node is made the head node.
 *
 *  @param head_node : pointer to head node 
 *  @param new_data	 : value that the node newly created will contain 	
 *
 *	@return pointer to the head of linked list
*/
struct node_ptr* insert_at_beginning(struct node_ptr *head_node, uint32_t new_data);

/**
 *	@brief Adds a node to the end of the linked list.
 *	
 *	Given the pointer to head node, this will create a new node and add it to 
 *	to the end of the linked list. If head node doesn't exist, this new node
 *	node is made the head node.
 *
 *  @param head_node : pointer to head node 
 *  @param new_data	 : value that the newly created node will contain 	
 *
 *	@return pointer to the head of linked list
*/
struct node_ptr* insert_at_end(struct node_ptr *head_node, uint32_t new_data);

/**
 *	@brief Adds a node to the given position of the linked list.
 *	
 *	Given the pointer to head node, this will create a new node and add it to 
 *	to the position specified by the index parameter of the linked list. 
 *
 *  @param head_node : pointer to head node 
 *  @param new_data	 : value that the newly created node will contain 	
 *  @param index	 : index of where to add the data
 *
 *	@return pointer to the head of linked list
*/
struct node_ptr* insert_at_position(struct node_ptr *head_node, uint32_t new_data, int index);

/**
 *	@brief Deletes a node from the beginning of the linked list.
 *	
 *	Given the pointer to head node, this will delete a node at the beginning 
 *  of the linked list.
 *
 *  @param head_node : pointer to head node 
 *
 *	@return pointer to the head of linked list
*/
struct node_ptr* delete_from_beginning(struct node_ptr *head_node);

/**
 *	@brief Deletes a node from the end of the linked list.
 *	
 *	Given the pointer to head node, this will delete a node at the end of the
 *  linked list.
 *
 *  @param head_node : pointer to head node 
 *
 *	@return pointer to the head of linked list
*/
struct node_ptr* delete_from_end(struct node_ptr *head_node);

/**
 *	@brief Deletes a node from the given position of the linked list.
 *	
 *	Given the pointer to head node, this will delete a node at the position 
 *  specified by the index parameter.
 *
 *  @param head_node : pointer to head node
 *  @param index	 : index from where to delete the node 
 *
 *	@return pointer to the head of linked list
*/
struct node_ptr* delete_from_position(struct node_ptr *head_node, int index);

/**
 *	@brief Determines the number of links in the linked list.
 *	
 *	Given the pointer to head node, this will determine the number 
 *  of links in the linked list.
 *
 *  @param head_node : pointer to head node
 *
 *	@return size of the linked list
*/
int size(struct node_ptr *head_node);

/**
 *	@brief Prints the data held by nodes of the linked list
 *	
 *	Given the pointer to head node, this will print the data held by the
 *  nodes of the linked list
 *
 *  @param head_node : pointer to head node
*/
void print_list(struct node_ptr *head_node);

#endif
