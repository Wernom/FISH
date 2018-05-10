#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

struct status{
    pid_t pid;
    int status;
};

struct node{
    struct status *data;
    struct node *next;
};

struct list{
    struct node *first;
};

/**
 * Create an empty list
 * 
 * @param   struct list*    self    pointer on list to create    
 */
void list_create(struct list *self);

/**
 * Test if the list is empty
 * 
 * @param   struct list*    self    pointer on the list to test   
 */
bool list_is_empty(const struct list *self);

/**
 * Add a new element in the list
 * 
 * @param   struct list*    self    pointer on the list
 * @param   struct status   data    data to be added to the list      
 */
void list_add(struct list *self, struct status data);

/**
 * Removes an item from the list and returns the value of data
 * 
 * @param   struct list*    self    pointer on the list   
 */
struct status *list_pull(struct list *self);

#endif