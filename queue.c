#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node {
    // longest url might be 1000bytes, so need 1000+1 bytes to store it
    char data[1001];
    struct node * next;
} node;

typedef struct queue {
    node* first;
    node* last;
    int size;
} queue;

queue* init_queue(){
    queue* q = malloc(sizeof(queue));
    q->size = 0;
    q->first = NULL;
    q->last = NULL;
}

int get_queue_length(queue* q) {
    return q->size;
}

int contains(queue* q, char* item) {
	
	node* current = q->first;
	while(current != NULL) {
		if(strcmp(current->data, item) == 0) {
			return 1;
		}
		current = current->next;
	}
	return 0;
}


void enqueue(queue* q, char* url) {
    node* new_node = malloc(sizeof(node));
    strcpy(new_node->data, url);
    new_node->next = NULL;
    if(q->size == 0) {
        q->first = new_node;
        q->last = new_node;
    }
    else {
        q->last->next = new_node;
        q->last = new_node;
    }
    q->size++;
}

void print_queue(queue* q) {
    node* current = q->first;
    if(current == NULL) {
        printf("The queue is empty.\n");
        return;
    }
    printf("Items in the queue:\n");
    while(current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }
}

int dequeue(queue* q, char* destination) {
    if(q->size == 0){
        printf("The queue is empty.\n");
        return -1;
    }
    strcpy(destination, q->first->data);
    node* temp = q->first->next;
    free(q->first);
    q->first = temp;
    q->size--;
    return 0;
}

int is_empty(queue* q){
    if(q->size > 0){
        return 0;
    }
    return 1;
}

void free_queue(queue* q) {
    while (q->size > 0){
		node* temp = q->first->next;
		free(q->first);
		q->first = temp;
		q->size--;
	}
	free(q);
}
