#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>

#include "fdr.h"

MODULE_LICENSE("Dual BSD/GPL");

int fdr_major = FDR_MAJOR;
int fdr_minor = FDR_MINOR;

dev_t dev_num;
int nr_devs = 4;    /* number of dev */

struct cdev *my_cdev;
struct fdr_dev *fdr_devices;
extern struct file_operations fdr_proc_fops;

int fdr_open(struct inode *inodp, struct file *filp)
{
	struct fdr_dev *dev;
	dev = container_of(inodp->i_cdev, struct fdr_dev, cdev);
	filp->private_data = dev;

	/* scull_trim()???? */
	return 0;
}
int fdr_release(struct inode *inodp, struct file *filp)
{
	return 0;
}

struct fdr_dev *scullv_follow(struct fdr_dev *dev, int item)
{	
	while (item--) {
		if (!dev->next) {
			dev->next = kmalloc(sizeof(struct fdr_dev), GFP_KERNEL);
			memset(dev->next, 0, sizeof(struct fdr_dev)); 
		}
		dev = dev->next;
		continue;
	}
	return dev;
}

ssize_t fdr_read(struct file *filp, char __user *buf, size_t count,
		 loff_t *fpos)
{
	struct fdr_dev *dev = filp->private_data;
	struct fdr_dev *dptr;
	int qset = dev->qset;
	int quantum = dev->quantum;
	int itemsize = qset * quantum;
	int item, rest, qset_item, qset_rest;
	int ret = 0;
	PDEBUG("In fdr_read\n");

	if (*fpos > dev->size) {
		PDEBUG("*fops is over dev->size\n");
		goto nothing;
	}
	if (count > dev->size - *fpos)
		count = dev->size - *fpos;
	
	/* Translate *fpos to the address of dev's data */
	item = ((long) *(fpos)) / itemsize;
	rest = ((long) *(fpos)) % itemsize;
	qset_item = rest / quantum;
	qset_rest = rest % quantum;
	/* Go along dev list */
	dptr = scullv_follow(dev, item);
	if (!dptr->data) {
		PDEBUG("data feild of fdr_dev is null\n");
		goto nothing;
	}
	if (!dptr->data[qset_item]) {
		PDEBUG("The last quantum is not exist\n");
		goto nothing;
	}
	if (count > quantum - qset_rest)
		count = quantum - qset_rest;
	/* Cope to user-space */
	if (copy_to_user (buf, dptr->data[qset_item]+qset_rest, count)) {
		ret = -EFAULT;
		PDEBUG("copy_to_user fail\n");
		goto nothing;
	}
//	up (&dev->sem);

	*fpos += count;
	PDEBUG("fdr_read ok\n");
	return count;

nothing:
	PDEBUG("fdr_read fail\n");
	return ret;
}

ssize_t fdr_write(struct file *filp, const char __user *buf, size_t count,
		  loff_t *fpos)
{
	struct fdr_dev *dev = filp->private_data;
	struct fdr_dev *dptr;
	int qset = dev->qset;
	int quantum = dev->quantum;
	int itemsize = qset * quantum;
	int item, rest, qset_item, qset_rest;
	int ret = -ENOMEM;
	PDEBUG("i'm fdr_write\n");

	/* Translate *fpos to the address of dev's data */
	item = ((long) *(fpos)) / itemsize;
	rest = ((long) *(fpos)) % itemsize;
	qset_item = rest / quantum;
	qset_rest = rest % quantum;
	/* Go along dev list */
	dptr = scullv_follow(dev, item);
	/* Ensure the set of quantum pointers */
	if (!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(void *), GFP_KERNEL);
		if (!dptr->data)
			goto nomem;
		memset(dptr->data, 0, qset * sizeof(void *));
	}
	/* Malloc the last quantum */
	if (!dptr->data[qset_item]) {
		dptr->data[qset_item] = (void *)kmalloc(QUANTUM, GFP_KERNEL);
		if (!dptr->data[qset_item])
			goto nomem;
		memset(dptr->data[qset_item], 0, QUANTUM);
	}
	if (copy_from_user(dptr->data[qset_item]+qset_rest, buf, count)){
		ret = -EFAULT;
		goto nomem;
	}
	*fpos += count;

	if (dev->size < *fpos)
		dev->size = *fpos;
	PDEBUG("fdr_write ok\n");
	return count;
nomem:
	PDEBUG("fdr_write fail\n");
	return ret;
}
	

struct file_operations fdr_fops = {
	.owner =THIS_MODULE,
	.read =	fdr_read,
	.write = fdr_write,
	.open =	fdr_open,
	.release = fdr_release,
};

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
