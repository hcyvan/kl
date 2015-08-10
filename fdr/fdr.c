#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define FDR_MAJOR 0
#define FDR_MINOR 0

#define QSET 1000
#define QUANTUM 3000

int fdr_major = FDR_MAJOR;
int fdr_minor = FDR_MINOR;

dev_t dev_num;
int nr_devs = 4;    /* number of dev */

struct cdev *my_cdev;
struct fdr_dev {
	struct cdev cdev;	/* embed */
	int qset;		/* the length of the quantum-pointer array */
	int quantum;		/* The size of quantum */
	size_t size;		/* The size of data */
	struct fdr_dev *next;
	void **data;
	
} *fdr_devices;

int fdr_open(struct inode *inodp, struct file *filp)
{

	struct fdr_dev *dev;

	dev = container_of(inodp->i_cdev, struct fdr_dev, cdev);
	filp->private_data = dev;

//	filp->private_data = &fdr_cdev;
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

	if (*fpos > dev->size)
		goto nothing;
	if (count > dev->size - *fpos)
		count = dev->size - *fpos;
	
	/* Translate *fpos to the address of dev's data */
	item = ((long) *(fpos)) / itemsize;
	rest = ((long) *(fpos)) % itemsize;
	qset_item = rest / quantum;
	qset_rest = rest % quantum;
	/* Go along dev list */
	dptr = scullv_follow(dev, item);
	if (!dptr->data)
		goto nothing;
	if (!dptr->data[qset_item])
		goto nothing;
	if (count > quantum - qset_rest)
		count = quantum - qset_rest;
	/* Cope to user-space */
	if (copy_to_user (buf, dptr->data[qset_item]+qset_rest, count)) {
		ret = -EFAULT;
		goto nothing;
	}
//	up (&dev->sem);

	*fpos += count;
	return count;

nothing:
	return ret;
}

struct file_operations fdr_fops = {
	.owner =THIS_MODULE,
	.read =	fdr_read,
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

	
	printk(KERN_ALERT "my first dirver init\n");
       	return 0;
fail:
	unregister_chrdev_region(dev_num, nr_devs);
	return err;

}
static void __exit hello_exit(void)
{
	int i;
	for (i = 0; i < nr_devs; i++)
	  	cdev_del(&fdr_devices[i].cdev);
	kfree(fdr_devices);
	unregister_chrdev_region(dev_num, nr_devs);
	printk(KERN_ALERT "my first dirver exit\n");
}
module_init(hello_init);
module_exit(hello_exit);
