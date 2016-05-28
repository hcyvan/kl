#include <linux/init.h>
#include <linux/module.h>
#include <linux/kfifo.h>
MODULE_LICENSE("Dual BSD/GPL");

struct kfifo *fifo;
//struct __kfifo *fifo2;

static int __init hello_init(void)
{
	int ret;
	ret = kfifo_alloc(fifo, PAGE_SIZE, GFP_KERNEL);
	printk(KERN_EMERG "fmd: init\n");
	
	return 0;
}
static void __exit hello_exit(void)
{
	printk(KERN_EMERG "fmd: exit\n");
}
module_init(hello_init);
module_exit(hello_exit);
