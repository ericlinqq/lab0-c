#include <stdlib.h>

#include "queue.h"
#include "shuffle.h"

void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    int len = q_size(head);
    struct list_head *new_list = head->prev;

    for (; new_list != head; new_list = new_list->prev, len--) {
        struct list_head *old_list = head->next;
        int count = rand() % len;
        while (count--)
            old_list = old_list->next;

        // swap
        if (new_list == old_list)
            continue;
        struct list_head *temp = old_list->prev;
        list_move(old_list, new_list);
        list_move(new_list, temp);
        new_list = old_list;
    }
}
