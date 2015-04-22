#ifndef RPN_CALC_H
#define RPN_CALC_H

#include "Stack.h"

bool RpnCalculator_Add(Stack *s);
bool RpnCalculator_Substract(Stack *s);
bool RpnCalculator_Multiply(Stack *s);
bool RpnCalculator_Divide(Stack *s);
bool RpnCalculator_Modulo(Stack *s);

#endif