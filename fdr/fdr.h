#ifndef _FDR_H_
#define _FDR_H_

//#define FDR_DEBUG

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

#endif
