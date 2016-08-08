#include <linux/kernel.h>
#include <linux/syscalls.h>
asmlinkage long sys_unhide(void)
{
	current->hide=0;
	return 0;
}
