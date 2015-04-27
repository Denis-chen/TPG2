#include "Stack.h"

void Stack_Init(Stack *S)
{
	S->size = 0;
}

bool Stack_Push(Stack *S, int d)
{
	if (S->size < STACK_MAX)
	{
		S->data[S->size] = d;
		S->size++;
		return true;
	}
	else
	{
		return false;
	}
}

bool Stack_Pop(Stack *S, int &d)
{
	if (S->size == 0)
	{
		return false;
	}
	else
	{
		S->size--;
		d = S->data[S->size];		
		return true;
	}
}

bool Stack_Dup(Stack *S)
{
	if (Stack_IsEmpty(S) || Stack_IsFull(S))
	{
		return false;
	}
	else
	{
		int val = S->data[(S->size)-1];
		S->data[S->size] = val;
		S->size++;
		return true;
	}
}

bool Stack_IsEmpty(Stack *S)
{
	return S->size == 0;
}

bool Stack_IsFull(Stack *S)
{
	return S->size == STACK_MAX;
}