#include <stdio.h>
#include <unistd.h>
#define __NR_hide 	313
#define __NR_unhide 	314
int main()
{
	printf("Before hiding:\n");
	system("ps");
	syscall(__NR_hide);
	printf("After hiding:\n");
	system("ps");
	syscall(__NR_unhide);
	printf("After unhiding:\n");
	system("ps");
	return 0;
}
