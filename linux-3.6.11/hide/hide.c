#include <linux/kernel.h>
#include <linux/syscalls.h>
asmlinkage long sys_hide(void)
{
	current->hide=1;
	return 0;
}
