// C code to implement Priority Queue
// using Linked List
#include <stdio.h>
#include <stdlib.h>

// Node
typedef struct node {
    int data;

    // Lower values indicate higher priority
    int priority;

    struct node* next;

} Node;

// Function to Create A New Node
Node* newNode(int d, int p);

// Return the value at head
int circ_peek(Node** head);

// Removes the element with the
// highest priority form the list
void circ_pop(Node** head);

// Function to push according to priority
void circ_push(Node** head, int d, int p);

// Function to check is list is empty
int circ_isEmpty(Node** head);
