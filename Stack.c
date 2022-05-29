#include "Node.c"
#include <strings.h>
#include <stdio.h>

#define MAX_LENGTH 1024
typedef struct Stack {
    Node *head;
    void * NextFreeAddr;
    int len;
}Stack;

//Function to init the head to NULL and the memory pointer to the begining ob the memory block
void initializeStack(Stack **stack) {
    (*stack)->NextFreeAddr = (*stack)+sizeof(Stack);
    (*stack)->head = NULL;
    (*stack)->len = 0;
}

//Function to return the top value in stack, assumed somthing in there
Node *top(Stack **stack) {
    return (*stack)->head;
}

//Function to remove first element in stack, consider empty stack and stack with 1 element
void pop(Stack **stack) {
    if ((*stack)->len  == 0) {
        return;
    }
    if ((*stack)->len  == 1) {
        (*stack)->head  = NULL;
        (*stack)->len = 0;
        return;
    } else {
        Node *temp = (*stack)->head ;
        (*stack)->head = (*stack)->head->next;
        (*stack)->len--;
        return;
    }

}

//Function to push element into the first position in the stack
void push(Stack **stack, char *data) {
    //Creating and copy new element values into temp Node
    Node *newNode = (Node*)(*stack)->NextFreeAddr;
    newNode->data = (char*)(*stack)->NextFreeAddr+sizeof(Node);
    for (int i = 0; i < MAX_LENGTH; ++i) {
        newNode->data[i] = data[i];
    }
    newNode->next = NULL;
    //If the stack is empty just insert the new element
    if ((*stack) == NULL) {
        (*stack)->head = newNode;
        (*stack)->len++;
    } else { // add new Node at begining and update pointers
        newNode->next = (*stack)->head ;
        (*stack)->head = newNode;
        (*stack)->len++;
    }
    //updating pointer to next free memory
    (*stack)->NextFreeAddr +=MAX_LENGTH+sizeof(Node);
}
