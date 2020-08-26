/*************************************
* Lab 1 Exercise 1
* Name:
* Student No:
* Lab Group:
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "node.h"

// add in your implementation below to the respective functions
// feel free to add any headers you deem fit (although you do not need to)

// inserts a new node with data value at index (counting from the front
// starting at 0)
void insert_node_from_head_at(list *lst, int index, int data)
{
    int i;
    struct NODE * newNode, * temp;
    if (lst->head == NULL) {
        newNode = (struct NODE *)malloc(sizeof(struct NODE));
        newNode->data = data;
        lst->head = newNode;
        lst->tail = newNode;
    } else {
        temp = lst->head;
        i = 0;
        while (i < index - 1 && temp->next != NULL) {
            i++;
            temp = temp->next;
        }
        if (index == 0) {
            newNode = (struct NODE *)malloc(sizeof(struct NODE));
            newNode->data = data;
            newNode->prev = NULL;
            newNode->next = temp;
            temp->prev = newNode;
            lst->head = newNode;
        } else if (temp->next == NULL) {
            newNode = (struct NODE *)malloc(sizeof(struct NODE));
            newNode->data = data;
            temp->next = newNode;
            newNode->prev = temp;
            lst->tail = newNode;
            newNode->next = NULL;
        } else {
            newNode = (struct NODE *)malloc(sizeof(struct NODE));
            newNode->data = data;
            newNode->next = temp->next;
            newNode->prev = temp;
            if (temp->next != NULL) {
                temp->next->prev = newNode;
            }
            temp->next = newNode;
        }
    }
}

// inserts a new node with data value at index (counting from the back
// starting at 0)
void insert_node_from_tail_at(list *lst, int index, int data)
{
    int i;
    struct NODE * newNode, * temp;
    if (lst->tail == NULL) {
        newNode = (struct NODE *)malloc(sizeof(struct NODE));
        newNode->data = data;
        lst->tail = newNode;
        lst->head = newNode;
    } else {
        temp = lst->tail;
        i = 0;
        while (i < index - 1 && temp->prev != NULL) {
            i++;
            temp = temp->prev;
        }
        if (index == 0) {
            newNode = (struct NODE *)malloc(sizeof(struct NODE));
            newNode->data = data;
            newNode->next = NULL;
            newNode->prev = temp;
            temp->next = newNode;
            lst->tail = newNode;
        } else if (temp->prev == NULL) {
            newNode = (struct NODE *)malloc(sizeof(struct NODE));
            newNode->data = data;
            temp->prev = newNode;
            newNode->next = temp;
            lst->head = newNode;
            newNode->prev = NULL;
        } else {
            newNode = (struct NODE *)malloc(sizeof(struct NODE));
            newNode->data = data;
            newNode->prev = temp->prev;
            newNode->next = temp;
            if (temp->prev != NULL) {
                temp->prev->next = newNode;
            }
            temp->prev = newNode;
        }
    }
}

// deletes node at index counting from the front (starting from 0)
// note: index is guaranteed to be valid
void delete_node_from_head_at(list *lst, int index)
{
    int i;
    struct NODE * temp;
    if (lst->head == NULL){
        return;
    } else {
        temp = lst->head;
        i = 0;
        while (i < index - 1 && temp->next != NULL) {
            i++;
            temp = temp->next;
        }
        if (index == 0) {
            struct NODE * first = temp;
            if (temp->next != NULL) {
                temp->next->prev = NULL;
            }
            lst->head = temp->next;
            free(first);
        } else if (temp->next == NULL) {
            if (temp->prev != NULL) {
                temp->prev->next = NULL;
            }
            lst->tail = temp->prev;
            free(temp);
        } else {
            struct NODE * delete = temp->next;
            temp->next = delete->next;
            if (delete->next != NULL) {
                delete->next->prev = temp;
            } else {
                lst->tail = temp;
            }
            free(delete);
        }
    }
}

// deletes node at index counting from the back (starting from 0)
// note: index is guaranteed to be valid
void delete_node_from_tail_at(list *lst, int index)
{
    int i;
    struct NODE * temp;
    if (lst->tail == NULL){
        return;
    } else {
        temp = lst->tail;
        i = 0;
        while (i < index - 1 && temp->prev != NULL) {
            i++;
            temp = temp->prev;
        }
        if (index == 0) {
            struct NODE * last = temp;
            if (temp->prev != NULL) {
                temp->prev->next = NULL;
            }
            lst->tail = temp->prev;
            free(last);
        } else if (temp->prev == NULL) {
            if (temp->next != NULL) {
                temp->next->prev = NULL;
            }
            lst->head = temp->next;
            free(temp);
        } else {
            struct NODE * delete = temp->prev;
            temp->prev = delete->prev;
            if (delete->prev != NULL) {
                delete->prev->next = temp;
            } else {
                lst->head = temp;
            }
            free(delete);
        }
    }
}

// resets list to an empty state (no nodes) and frees any allocated memory in
// the process
void reset_list(list *lst)
{
    struct NODE * node = lst->head;
    while (node != NULL) {
        struct NODE * temp = node;
        if (node->next != NULL) {
            node->next->prev = NULL;
        }
        node = node->next;
        free(temp);
    }
    lst->head = NULL;
    lst->tail = NULL;
}
