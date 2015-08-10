#include <linux/init.h>
#include <linux/module.h>
//#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/magic.h>
MODULE_LICENSE("Dual BSD/GPL");
static struct kernfs_root *navy_root;
static struct dentry *navy_mount(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data)
{
	struct dentry *root;
	printk(KERN_EMERG "fmd: mount\n");
/*
	void *ns;
	bool new_sb;

	if (!(flags & MS_KERNMOUNT)) {
		if (!capable(CAP_SYS_ADMIN) && !fs_fully_visible(fs_type))
			return ERR_PTR(-EPERM);

		if (!kobj_ns_current_may_mount(KOBJ_NS_TYPE_NET))
			return ERR_PTR(-EPERM);
	}

	ns = kobj_ns_grab_current(KOBJ_NS_TYPE_NET);
	root = kernfs_mount_ns(fs_type, flags, navy_root,
				SYSFS_MAGIC, &new_sb, ns);
	if (IS_ERR(root) || !new_sb)
		kobj_ns_drop(KOBJ_NS_TYPE_NET, ns);
	return root;
*/
	return root;
}

static struct file_system_type navy_fs_type = {
	.name  = "navy",
	.mount = navy_mount, 
};


static int __init hello_init(void)
{
	printk(KERN_EMERG "fmd: init\n");
//	__printk_ratelimit(__func__);

	register_filesystem(&navy_fs_type);
	
	return 0;
}
static void __exit hello_exit(void)
{
	unregister_filesystem(&navy_fs_type);
	
	printk(KERN_EMERG "fmd: exit\n");
}
module_init(hello_init);
module_exit(hello_exit);
