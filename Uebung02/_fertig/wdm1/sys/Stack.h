#ifndef STACK_H
#define STACK_H

size_t const STACK_MAX = 5;

struct Stack {
	int     data[STACK_MAX];
	int     size;
};
typedef struct Stack Stack;

void Stack_Init(Stack *S);
bool Stack_Push(Stack *S, int d);
bool Stack_Pop(Stack *S, int &d);
bool Stack_Dup(Stack *S);
bool Stack_IsEmpty(Stack *S);
bool Stack_IsFull(Stack *S);

#endif