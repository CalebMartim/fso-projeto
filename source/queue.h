#ifndef QUEUE_H
#define QUEUE_H

#include "stdlib.h"
#include "process.h"

typedef struct Node {
    Process *proc;
    struct Node *nxt;
} Node;

typedef struct Queue {
    int size;
    Node *front;
    Node *back;
} Queue;

Queue* new_queue() {
    Queue* q = (Queue*) malloc(sizeof(Queue));

    if (q == NULL) return NULL;

    q->size = 0;
    q->front = q->back = NULL;

    return q;
}

int enqueue(Queue *q, Process* process) {
    if (q == NULL) return 1;

    Node* no = (Node*) malloc(sizeof(Node));
    if (no == NULL) return 1;
    no->proc = process;
    no->nxt = NULL;

    if (q->size == 0) q->front = q->back = no;
    else {
        q->back->nxt = no;
        q->back = no;
    }

    q->size++;

    return 0;
}

Process* dequeue(Queue *q) {
    if (q->size == 0) return NULL;

    Node* no = q->front;
    q->front = q->front->nxt;
    q->size--;

    Process* proc = no->proc;
    free(no);

    return proc;
}

void free_queue(Queue *q) {
    Node *node = q->front;
    while (node != NULL) {
        Node* nxt = node->nxt;
        Process* p = dequeue(q);
        free(p);
        node = nxt;
    }

    free(q);
    return;
}

bool is_empty(Queue* q) {
    return (q->size == 0);
}

// Criado com o chatgpt. Verificado e ok
char* to_string(Queue* q) {
    if(q == NULL) {
        char* s = (char*) malloc(20);
        snprintf(s, 20, "[Fila NULL]");
        return s;
    }

    size_t tam = q->size*12 + 50;   // tamanho da fila formatada para string
    char* ret = (char*) malloc(tam); 
    if(ret == NULL) return NULL;

    size_t offset = 0;

    offset += snprintf(ret+offset, tam - offset, "[");  // snprintf retorna o quanto foi escrito na string formatada (nesse caso offset = 1)

    Node* cur = q->front;
    while(cur != NULL) {
        offset += snprintf(ret + offset, tam - offset, "pid=%d", cur->proc->pid);

        if(cur->nxt != NULL) offset += snprintf(ret + offset, tam - offset, ", ");  // offset += 2

        cur = cur->nxt;
    }

    snprintf(ret+offset, tam - offset, "]");    // offset += 1

    return ret;  // LEMBRAR DE DAR FREE()
}

#endif