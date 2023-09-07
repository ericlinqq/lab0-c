#include "k_sort.h"
#include <string.h>
#include "queue.h"

static int q_cmp(void *priv,
                 const struct list_head *a,
                 const struct list_head *b)
{
    bool descend = *(bool *) priv;
    // cppcheck-suppress nullPointer
    element_t *a_front = list_entry(a, element_t, list);
    // cppcheck-suppress nullPointer
    element_t *b_front = list_entry(b, element_t, list);
    return descend ? -strcmp(a_front->value, b_front->value)
                   : strcmp(a_front->value, b_front->value);
}

void q_ksort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    list_cmp_func_t cmp = q_cmp;
    list_sort((void *) &descend, head, cmp);
}
