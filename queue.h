#ifndef QUEUELIB
#define QUEUELIB

typedef struct queue queue;
typedef struct node node;
queue* init_queue();
void enqueue(queue* q, char* url);
void print_queue(queue* q);
int dequeue(queue* q, char* destination);
void free_queue(queue* q);
int is_empty(queue* q);
int get_queue_length(queue* q);
int contains(queue* q, char* item);

#endif