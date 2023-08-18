#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *l = malloc(sizeof(*l));
    if (l)
        INIT_LIST_HEAD(l);
    return l;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *node, *safe;
    list_for_each_entry_safe (node, safe, l, list)
        q_release_element(node);
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(*ele));
    if (!ele)
        return false;
    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }
    list_add(&ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    // cppcheck-suppress nullPointer
    element_t *ele = list_first_entry(head, element_t, list);
    if (!ele)
        return NULL;
    if (sp && bufsize) {
        size_t from_len = strlen(ele->value);
        size_t to_len = (from_len > bufsize - 1) ? bufsize - 1 : from_len;
        memcpy(sp, ele->value, to_len);
        sp[to_len] = '\0';
    }
    list_del(&ele->list);
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *cur;
    int count = 0;
    list_for_each (cur, head)
        count++;
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head->next, *fast = slow;
    for (; fast != head && fast != head->prev; fast = fast->next->next)
        slow = slow->next;
    // cppcheck-suppress nullPointer
    element_t *ele = list_entry(slow, element_t, list);
    list_del(&ele->list);
    q_release_element(ele);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    bool is_duplicate = false;
    element_t *node = NULL, *safe = NULL;
    // cppcheck-suppress nullPointer
    list_for_each_entry_safe (node, safe, head, list) {
        bool equal =
            &node->list != head->prev && !strcmp(node->value, safe->value);
        if (equal || is_duplicate) {
            list_del(&node->list);
            q_release_element(node);
        }
        is_duplicate = equal;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *node = NULL;
    list_for_each (node, head) {
        if (node == head->prev)
            break;
        list_move(node, node->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head)
        list_move(node, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    struct list_head *node = head->next;
    for (;;) {
        struct list_head *safe = node, *start = node->prev;
        // To see if there are k remaining nodes
        for (int i = 0; i < k; i++, node = node->next) {
            if (node == head)
                return;
        }
        // Reverse k nodes
        node = safe;
        safe = node->next;
        for (int i = 0; i < k; i++, node = safe, safe = safe->next)
            list_move(node, start);
    }
}

static void merge_two_lists(struct list_head *list1,
                            struct list_head *list2,
                            bool descend)
{
    if (!list1 || !list2)
        return;
    LIST_HEAD(temp_head);
    while (!list_empty(list1) && !list_empty(list2)) {
        // cppcheck-suppress nullPointer
        element_t *list1_front = list_first_entry(list1, element_t, list);
        // cppcheck-suppress nullPointer
        element_t *list2_front = list_first_entry(list2, element_t, list);
        int cmp = descend ? strcmp(list1_front->value, list2_front->value)
                          : -strcmp(list1_front->value, list2_front->value);
        list_move_tail((cmp > 0) ? list1->next : list2->next, &temp_head);
    }
    list_splice_tail_init(list1, &temp_head);
    list_splice_tail_init(list2, &temp_head);
    list_splice(&temp_head, list1);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head, *fast = head->next;
    for (; fast != head && fast != head->prev; fast = fast->next->next)
        slow = slow->next;
    LIST_HEAD(left);
    list_cut_position(&left, head, slow);
    q_sort(&left, descend);
    q_sort(head, descend);
    merge_two_lists(head, &left, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
