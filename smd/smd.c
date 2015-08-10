#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
MODULE_LICENSE("Dual BSD/GPL");
struct file_system_type navys_fs = {
	.name = "navych",
};
static int __init hello_init(void)
{
	printk(KERN_ALERT "smd init\n");
	register_filesystem(&navys_fs);
	return 0;
}
static void __exit hello_exit(void)
{
}
module_init(hello_init);
module_exit(hello_exit);
