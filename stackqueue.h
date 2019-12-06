#define MAX_STACK 100
#define MAX_QUEUE 100


typedef struct {
	void* items[MAX_STACK];
	int top;
} Stack;


typedef struct {
	int front, rear;
	void* items[MAX_QUEUE];
} Queue;

Stack* CreateStack() {
	Stack* pstack= (Stack *)malloc(sizeof(Stack));
	pstack->top = -1;
	return pstack;
}

int IsSFull(Stack* pstack) {
	return pstack->top == MAX_STACK - 1;
}

int IsSEmpty(Stack* pstack) {
	return pstack->top == -1;
}

void* SPeek(Stack* pstack) {
	if (IsSEmpty(pstack))
		exit(1); //error: empty stack
	return pstack->items[pstack->top];
}

void Push(Stack* pstack, void* item) {
	if (IsSFull(pstack))
		exit(1); //error: stack full
	pstack->items[++(pstack->top)] = item;
}

void* Pop(Stack* pstack) {
	if (IsSEmpty(pstack))
		exit(1); //error: empty stack
	void* item = pstack->items[pstack->top];
	--(pstack->top);
	return item;
}

Queue* CreateQueue() {
	Queue* pqueue = (Queue*)malloc(sizeof(Queue));
	pqueue->front = pqueue->rear = 0;
	return pqueue;
}

int IsQFull(Queue* pqueue) {
	return pqueue->front == (pqueue->rear + 1) % MAX_QUEUE;
}

int IsQEmpty(Queue* pqueue) {
	return pqueue->front == pqueue->rear;
}

void* QPeek(Queue* pqueue) {
	if (IsQEmpty(pqueue))
		exit(1); //error: empty stack
	return pqueue->items[pqueue->front];
}

void EnQueue(Queue* pqueue, void* item) {
	if (IsQFull(pqueue))
		exit(1); //error: stack full
	pqueue->items[pqueue->rear] = item;
	pqueue->rear = (pqueue->rear + 1) % MAX_QUEUE;
}

void* DeQueue(Queue* pqueue) {
	if (IsQEmpty(pqueue))
		exit(1); //error: empty stack
	void* item = pqueue->items[pqueue->front];
	pqueue->front = (pqueue->front + 1) % MAX_QUEUE;
	return item;
}


//s#pragma once
