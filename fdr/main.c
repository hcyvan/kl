#include "fdr.h"

MODULE_LICENSE("Dual BSD/GPL");

int fdr_major = FDR_MAJOR;
int fdr_minor = FDR_MINOR;

dev_t dev_num;
int nr_devs = 4;    /* number of dev */

struct cdev *my_cdev;
struct fdr_dev *fdr_devices;

extern struct file_operations fdr_proc_fops;
extern struct file_operations fdr_fops;


static void fdr_setup_cdev(struct fdr_dev *dev, int index)
{
	int err, devno = MKDEV(fdr_major, index);
    
	cdev_init(&dev->cdev, &fdr_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &fdr_fops;
	dev->qset = QSET;	/* The length of quantum-pointer array */
	dev->quantum = QUANTUM;	/* The size of successive memory unit */
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

static int __init hello_init(void)
{
	int err, i;
	/* Regist the major, and accept a dynamic number. */
	if (fdr_major) {
		dev_num = MKDEV(fdr_major, fdr_minor);
		err = register_chrdev_region(dev_num, nr_devs, "navyfdr");
	} else {
		err = alloc_chrdev_region(&dev_num, 0, nr_devs, "navyfdr");
		fdr_major = MAJOR(dev_num);
		fdr_minor = MINOR(dev_num);
	}

	if (err < 0)
		return err;

	/* Alloc kernel memory for fdr_devices */
	fdr_devices = kmalloc(nr_devs*sizeof (struct fdr_dev), GFP_KERNEL);
	if (!fdr_devices) {
		err = -ENOMEM;
		goto fail;
	}
	memset(fdr_devices, 0, nr_devs*sizeof (struct fdr_dev));
	/* Init every devices */
	for (i = 0; i < nr_devs; i++)
		fdr_setup_cdev(fdr_devices + i, i);

	/* Create /proc entry */
	if (!proc_create("fdr", 0, NULL, &fdr_proc_fops)) {
		/* err = ??? */
		goto proc_fail;
	}
	printk(KERN_ALERT "my first dirver init\n");
       	return 0;

	
proc_fail:
	for (i = 0; i < nr_devs; i++)
	  	cdev_del(&fdr_devices[i].cdev);
	kfree(fdr_devices);
fail:
	unregister_chrdev_region(dev_num, nr_devs);
	return err;

}
static void __exit hello_exit(void)
{
	int i;
	remove_proc_entry("fdr", NULL);
	for (i = 0; i < nr_devs; i++)
	  	cdev_del(&fdr_devices[i].cdev);
	kfree(fdr_devices);
	unregister_chrdev_region(dev_num, nr_devs);
	printk(KERN_ALERT "my first dirver exit\n");
}
module_init(hello_init);
module_exit(hello_exit);
