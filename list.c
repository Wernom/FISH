#include "list.h"


void list_create(struct list *self){
    self->first = NULL;
}

bool list_is_empty(const struct list *self){
    return self->first == NULL;
}

void list_add(struct list *self, struct status data){
    struct node *new = malloc(sizeof(struct node));
    new->data = malloc(sizeof(struct status));
    new->data->pid = data.pid;
    new->data->status = data.status;
    new->next = self->first;
    self->first = new;
}

struct status *list_pull(struct list *self){
    struct status *res = self->first->data;
    struct node *curr = self->first;
    self->first = self->first->next;
    //free(curr->data);
    free(curr);
    return res;
}
