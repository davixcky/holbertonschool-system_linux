#include "hls.h"

/**
 * _isdigit - Verify if is a number
 * @c: Character to verify
 * Return: 1 if is a number and 0 if not
 */
int _isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);

	return (0);
}

/**
 * reverse - Change the order of an array of char
 *
 * @str: String to reverse
 * @length: Number of characters
 */
void reverse(char str[], int length)
{
	int start = 0;
	int end = length - 1;
	char aux;

	while (start < end)
	{
		aux = str[start];
		str[start] = str[end];
		str[end] = aux;
		start++;
		end--;
	}
}

/**
 * _itoa - Convert int number to char
 *
 * @num: Number to convert
 * @str: Buffer where the number will go
 * @base: Number base
 *
 * Return: Pointer to char where the value is the number in ascii
 */
char *_itoa(int num, char *str, int base)
{
	int i = 0;
	int isNegative = 0;
	int rem;

	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return (str);
	}

	if (num < 0 && base == 10)
	{
		isNegative = 1;
		num = -num;
	}

	while (num != 0)
	{
		rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	if (isNegative)
		str[i++] = '-';

	str[i] = '\0';

	reverse(str, i);

	return (str);
}
