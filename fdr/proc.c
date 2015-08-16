#include "fdr.h"

/*
 * Tie them all together into a set of seq_operations.
 */
static struct seq_operations fdr_seq_ops = {
/*	.start = ct_seq_start,
	.next  = ct_seq_next,
	.stop  = ct_seq_stop,
	.show  = ct_seq_show*/
};

/*
 * Time to set up the file operations for our /proc file.  In this case,
 * all we need is an open function which sets up the sequence ops.
 */
static int fdr_proc_open(struct inode *inodp, struct file *filp)
{
	return seq_open(filp, &fdr_seq_ops);
};

/*
 * The file operations structure contains our open function along with
 * set of the canned seq_ ops.
 */
static struct file_operations fdr_proc_fops = {
	.owner   = THIS_MODULE,
	.open    = fdr_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release,
};

int proc_setup()
{
	proc_create("fdr", 0, NULL, &fdr_proc_fops);
	return 0;
}
