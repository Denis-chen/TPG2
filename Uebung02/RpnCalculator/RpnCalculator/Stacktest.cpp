/*
*          File: stacktest.c
*        Author: Robert I. Pitts <rip@cs.bu.edu>
* Last Modified: March 7, 2000
*         Topic: Stack - Array Implementation
* ----------------------------------------------------------------
*
* OVERVIEW:
* =========
* This program tests the "stack of characters" module.
*/

#include <iostream>
#include "Stack.h"
#include "RpnCalculator.h"

using namespace std;


int main(void)
{
	Stack s;

	Stack_Init(&s);
	cout << " " << Stack_Push(&s, 10);
	cout << " " << Stack_Push(&s, 100);
	cout << " " << Stack_Push(&s, 1000);
	cout << " " << Stack_Push(&s, 10000);
	cout << endl;

	int val = 0;

	RpnCalculator_Add(&s);
	RpnCalculator_Add(&s);
	RpnCalculator_Add(&s);
	cout << Stack_Pop(&s, val);
	cout << " " << val << endl;
}