#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "DLL.h"

/* Tests if adding a node to the beginning of an empty list is a success */
void test_DLL_insert_at_begin_to_empty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

    cur_head = insert_at_beginning(head, 10);

	assert_non_null(cur_head);
}

/* Tests if adding a node to the beginning of an non empty list is a success */
void test_DLL_insert_at_begin_to_non_empty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

    cur_head = insert_at_beginning(head, 10);
    cur_head = insert_at_beginning(cur_head, 20);

	assert_non_null(cur_head);
}

/* Tests if adding a node at a specified position of an non empty list is a success */
void test_DLL_insert_at_pos_to_nonempty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

    cur_head = insert_at_beginning(head, 10);
    cur_head = insert_at_beginning(cur_head, 20);
    cur_head = insert_at_beginning(cur_head, 30);
	cur_head = insert_at_position(cur_head, 2, 25);

	assert_non_null(cur_head);
}

/* Tests if adding a node at a specified position of an empty list is a failure */
void test_DLL_insert_at_pos_to_empty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

	cur_head = insert_at_position(head, 2, 25);

	assert_null(cur_head);
}

/* Tests if adding a node at the end of an empty list is a failure */
void test_DLL_insert_at_end_of_empty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

	cur_head = insert_at_end(head, 25);

	assert_non_null(cur_head);
}

/* Tests if adding a node at the end of a non empty list is a failure */
void test_DLL_insert_at_end_of_non_empty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

    cur_head = insert_at_beginning(head, 10);
    cur_head = insert_at_beginning(cur_head, 20);
    cur_head = insert_at_beginning(cur_head, 30);
	cur_head = insert_at_position(cur_head, 2, 25);
	cur_head = insert_at_end(cur_head, 25);

	assert_non_null(cur_head);
}

/* Tests if deleting a node from an empty list is a failure */
void test_DLL_delete_from_empty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

	cur_head = delete_from_beginning(head);

	assert_null(cur_head);
}

/* Tests if deleting a node from the begining of a nonempty list is a success */
void test_DLL_delete_from_begin_of_nonempty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

    cur_head = insert_at_beginning(head, 10);
    cur_head = insert_at_beginning(cur_head, 20);
    cur_head = insert_at_beginning(cur_head, 30);
	cur_head = insert_at_position(cur_head, 2, 25);
	cur_head = delete_from_beginning(cur_head);

	assert_non_null(cur_head);
}

/* Tests if deleting a node from the end of a nonempty list is a success */
void test_DLL_delete_from_end_of_nonempty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

    cur_head = insert_at_beginning(head, 10);
    cur_head = insert_at_beginning(cur_head, 20);
    cur_head = insert_at_beginning(cur_head, 30);
	cur_head = insert_at_position(cur_head, 2, 25);
	cur_head = delete_from_end(cur_head);

	assert_non_null(cur_head);
}

/* Tests if deleting a node from the end of a nonempty list is a success */
void test_DLL_delete_from_pos_of_nonempty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

    cur_head = insert_at_beginning(head, 10);
    cur_head = insert_at_beginning(cur_head, 20);
    cur_head = insert_at_beginning(cur_head, 30);
	cur_head = insert_at_position(cur_head, 2, 25);
	cur_head = delete_from_position(cur_head, 2);

	assert_non_null(cur_head);
}

/* Tests if destroying an non-empty list is a success */
void test_DLL_destroy_non_empty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	

    cur_head = insert_at_beginning(head, 10);
    cur_head = insert_at_beginning(cur_head, 20);
    cur_head = insert_at_beginning(cur_head, 30);
	cur_head = insert_at_position(cur_head, 2, 25);
	cur_head = destroy(cur_head);

	assert_null(cur_head);
}

/* Tests if finding size of an empty list returns 0 */
void test_DLL_find_size_empty_list(void **state)
{
  	struct node_ptr *head = NULL;	

	int dll_size = size(head);

	assert_int_equal(dll_size, 0);
}

/* Tests if finding size of an non empty list returns non zero value */
void test_DLL_find_size_non_empty_list(void **state)
{
  	struct node_ptr *head = NULL, *cur_head = NULL;	
    
	cur_head = insert_at_beginning(head, 10);
    cur_head = insert_at_beginning(cur_head, 20);
    cur_head = insert_at_beginning(cur_head, 30);
	cur_head = insert_at_position(cur_head, 2, 25);

	int dll_size = size(head);

	assert_int_equal(dll_size, 0);
}

int main(int argc, char **argv)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_DLL_insert_at_begin_to_empty_list),
    cmocka_unit_test(test_DLL_insert_at_begin_to_non_empty_list),
    cmocka_unit_test(test_DLL_insert_at_pos_to_nonempty_list),
    cmocka_unit_test(test_DLL_insert_at_pos_to_empty_list),
    cmocka_unit_test(test_DLL_insert_at_end_of_empty_list),
    cmocka_unit_test(test_DLL_insert_at_end_of_non_empty_list),
    cmocka_unit_test(test_DLL_delete_from_empty_list),
    cmocka_unit_test(test_DLL_delete_from_begin_of_nonempty_list),
    cmocka_unit_test(test_DLL_delete_from_end_of_nonempty_list),
    cmocka_unit_test(test_DLL_delete_from_pos_of_nonempty_list),
    cmocka_unit_test(test_DLL_destroy_non_empty_list),
    cmocka_unit_test(test_DLL_find_size_empty_list),
    cmocka_unit_test(test_DLL_find_size_non_empty_list),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
