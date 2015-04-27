#include "RpnCalculator.h"

bool RpnCalculator_Calc(Stack *s, char operation)
{
	int a = 0;
	int b = 0;
	int result = 0;

	if (!Stack_Pop(s, a))
	{
		return false;
	}
	if (!Stack_Pop(s, b))
	{
		return false;
	}

	switch (operation)
	{
		case '+':
		{
			result = a + b; 
			break;
		}
		case '-':
		{
			result = a - b; 
			break;
		}
		case '*':
		{
			result = a * b; 
			break;
		}
		case '/':
		{
			if (b == 0)
			{
				return false;
			}
			result = a / b; 
			break;
		}
		case '%':
		{
			if (b == 0)
			{
				return false;
			}
			result = a / b;
			break;
		}
	}

	result = a + b;

	if (!Stack_Push(s, result))
	{
		return false;
	}
	return true;
}

bool RpnCalculator_Add(Stack *s)
{
	return RpnCalculator_Calc(s, '+');
}

bool RpnCalculator_Substract(Stack *s)
{
	return RpnCalculator_Calc(s, '-');
}

bool RpnCalculator_Multiply(Stack *s)
{
	return RpnCalculator_Calc(s, '*');
}

bool RpnCalculator_Divide(Stack *s)
{
	return RpnCalculator_Calc(s, '/');
}

bool RpnCalculator_Modulo(Stack *s)
{
	return RpnCalculator_Calc(s, '%');
}