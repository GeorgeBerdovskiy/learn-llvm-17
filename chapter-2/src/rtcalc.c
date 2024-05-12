// The runtime library consists of a single file, rtcalc.c
// It has the implementation for the calc_read() and calc_write()
// functions, written in C

#include <stdio.h>
#include <stdlib.h>

void calc_write(int v)
{
	printf("The result is: %d\n", v);
}

int calc_read(char *s)
{
	char buf[64];
	int val;
	printf("Enter a value for %s: ", s);
	fgets(buf, sizeof(buf), stdin);
	if (EOF == sscanf(buf, "%d", &val))
	{
		printf("Value %s is invalid\n", buf);
		exit(1);
	}
	return val;
}

// calc_read reads an integer number from the terminal. Nothing
// prevents the user from entering letters or other characters
// so we must carefully check the input. If the input is not
// a number, we exit the application. A more complex approach
// would be to make the user aware of the problem and ask for
// a number again.