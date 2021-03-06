#ifndef _FDR_H_
#define _FDR_H_

#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/delay.h>

#undef PDEBUG
#ifdef FDR_DEBUG
# ifdef __KERNEL__
#define PDEBUG(fmt, args...) if (printk_ratelimit())\
		printk(KERN_ALERT "fdr: " fmt, ## args)
# else
#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
# endif
#else
# define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif
#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */ 

/* proc.c */
extern int proc_setup(void);


/* main.c */
#define FDR_MAJOR 0
#define FDR_MINOR 0

#define QSET 1000
#define QUANTUM 4000

struct fdr_dev {
	struct cdev cdev;
	struct semaphore sem;
	int qset;		/* the length of the quantum-pointer array */
	int quantum;		/* The size of quantum */
	size_t size;		/* The size of data */
	struct fdr_dev *next;
	void **data;
	
};

#endif

