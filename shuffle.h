#ifndef _SHUFFLE_H
#define _SHUFFLE_H
#include "list.h"

/**
 * q_shuffle() - Shuffle the queue.
 * @head: header of queue
 *
 * No effect if queue is NULL or empty. If there has only one element, do
 * nothing.
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_shuffle(struct list_head *head);

#endif /* _SHUFFLE_h */
