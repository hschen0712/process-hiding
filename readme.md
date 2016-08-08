# linux下实验进程隐藏的三种方法
>作者： hschen  

>邮箱：hschen0712@gmail.com

本项目为研一时候做的操作系统大作业，因为很多人需要，所以将代码开源了，以方便后人。

使用方法：
内核版本为`3.6.11`，使用前需要将`linux-3.6.11`目录下修改过的文件替换掉内核中的相应文件  
使用方法一，需要在`base.c`中取消注释`//DEFINE DEBUG_PROC`并重新编译内核  
使用方法二、三，需要把`base.c`中的`DEFINE DEBUG_PROC`注释掉  
方法二、三内核模块的编译方式为：  
`make -C /usr/src/linux-3.6.11 SUBDIRS=$PWD modules`
然后通过`insmod`命令挂载，`rmmod`命令卸载  
编译测试文件的方法是：  
`gcc -o test test.c`  
最后``./test`执行

三种方法的实现原理：  
[方法一](http://www.cnblogs.com/wacc/p/3674074.html)  
[方法二](http://www.cnblogs.com/wacc/p/3674097.html)  
[方法三](http://www.cnblogs.com/wacc/p/3674102.html)
