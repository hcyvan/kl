#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
MODULE_LICENSE("GPL");
static char *who;
static int  times;
module_param(who,charp,0644);
module_param(times,int,0644);
static int __init hello_init(void)
{
	int i;
	for(i = 1;i <= times;i++)
		printk(KERN_ALERT "%d  %s!\n",i,who);
	return 0;
}
static void __exit hello_exit(void)
{
	printk(KERN_ALERT "Goodbye,%s!\n",who);
}
module_init(hello_init);
module_exit(hello_exit); 
