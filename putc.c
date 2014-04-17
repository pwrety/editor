#include<stdio.h>

int main(int argc, char *argv[])
{
	fputs("\033[10;6H", stdout);
	fputs("\033[10;6HHello\n", stdout);

	return 0;
}


