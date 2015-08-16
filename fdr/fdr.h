#ifndef _FDR_H_
#define _FDR_H_

#include <linux/seq_file.h>
#include <linux/proc_fs.h>

#undef PDEBUG
#ifdef FDR_DEBUG
# ifdef __KERNEL__
#define PDEBUG(fmt, args...) if (printk_ratelimit())\
		printk(KERN_ALERT "scull: " fmt, ## args)
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
#endif

