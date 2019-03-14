// C code to implement Priority Queue
// using Linked List
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Node
typedef struct node {
    int data;

    // Lower values indicate higher priority
    uint32_t priority;

    struct node* next;

} Node;

// Function to Create A New Node
Node* newNode(int d, uint32_t p);

// Return the value at head
int circ_peek(Node** head);

// Removes the element with the
// highest priority form the list
void circ_pop(Node** head);

// Function to push according to priority
void circ_push(Node** head, int d, uint32_t p);

// Function to check is list is empty
int circ_isEmpty(Node** head);
