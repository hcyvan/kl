#include "fdr.h"

extern struct fdr_dev *fdr_devices;

static void *fdr_seq_start(struct seq_file *s, loff_t *pos)
{
	struct fdr_dev *spos = fdr_devices;
	PDEBUG("fdr_seq_start %d\n",*(int*)pos);
	while (*pos) {
		spos = fdr_devices->next;
		(*pos)--;
	}
	return spos;
}

static void *fdr_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct fdr_dev *spos = (struct fdr_dev *)v;
	spos = spos->next;
	(*pos)++;
	PDEBUG("fdr_seq_next: %d\n",*(int*)pos);
	return spos;
}

static void fdr_seq_stop(struct seq_file *s, void *v)
{
	PDEBUG("fdr_seq_stop\n");
}

static int fdr_seq_show(struct seq_file *s, void *v)
{
	int i = 0;
	struct fdr_dev *spos = (struct fdr_dev *)v;
	PDEBUG("fdr_seq_show...");
	if (!spos->data) {
		PDEBUG("fdr_seq_show : no data");
		return 0;
	}
	while (spos->data[i] && i < QSET) {
		seq_printf(s, "%s", (char *)(spos->data[i]));
		i++;
	}
	//seq_printf(s, "i'm first proc\n");
	return 0;
}

/*
 * Tie them all together into a set of seq_operations.
 */
static struct seq_operations fdr_seq_ops = {
	.start = fdr_seq_start,
	.next  = fdr_seq_next,
	.stop  = fdr_seq_stop,
	.show  = fdr_seq_show,
};

/*
 * Time to set up the file operations for our /proc file.  In this case,
 * all we need is an open function which sets up the sequence ops.
 */
static int fdr_proc_open(struct inode *inodp, struct file *filp)
{
	PDEBUG("fdr_proc_open\n");
	return seq_open(filp, &fdr_seq_ops);
};

/*
 * The file operations structure contains our open function along with
 * set of the canned seq_ ops.
 */
struct file_operations fdr_proc_fops = {
	.owner   = THIS_MODULE,
	.open    = fdr_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release,
};
