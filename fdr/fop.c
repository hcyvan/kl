#include "fdr.h"
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

	if (count <= QUANTUM - qset_rest) {
		if (copy_from_user(dptr->data[qset_item]+qset_rest, buf, count)){
			ret = -EFAULT;
			goto nomem;
		}
		*fpos += count;
		PDEBUG("fdr_write: %d bytes\n", (int)count);
	} else {
		if (copy_from_user(dptr->data[qset_item]+qset_rest, buf,
				   QUANTUM - qset_rest)){
			ret = -EFAULT;
			goto nomem;
		}
		PDEBUG("fdr_write: %d bytes\n", (int)(QUANTUM - qset_rest));
		*fpos += QUANTUM - qset_rest;
		buf = buf + QUANTUM - qset_rest;
		fdr_write(filp, buf, count - QUANTUM + qset_rest, fpos);
	}
		
	if (dev->size < *fpos)
		dev->size = *fpos;
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
