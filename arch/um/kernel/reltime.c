#include <linux/proc_fs.h>
#include <linux/seq_file.h>

struct uml_reltime_t {
	long long convergence;
	long double frequency;
};

struct uml_reltime_t reltime = {
	.convergence= 0,
	.frequency= 1
};

/* Convert ascii to long double. */
long double atold(char *line) {
	long double integer = 0;
	long double decimal = 0;
	long double sign = 1;
	int i = 0;
	long double dec_count = 1;
	if (line[i] == '-') {
		sign = -1;
		i++;
	}
	/* Scan and convert integer part */
	while ('0' <= line[i] && line[i] <= '9') {
		integer *= 10;
		integer += line[i] - '0';
		i++;
	}
	/* Scan and convert decimal part */
	if (line[i] == '.') {
		i++;
		while ('0' <= line[i] && line[i] <= '9') {
			decimal *= 10;
			decimal += line[i] - '0';
			i++;
			dec_count *= 10;
		}
	}
	/* Now put everything together */
	return sign * (integer + (decimal / dec_count));
}

#ifdef CONFIG_PROC_FS
/**** add /proc entries ****/
static struct proc_dir_entry *uml_reltime = NULL;
static struct proc_dir_entry *frequency = NULL;
static struct proc_dir_entry *convergence = NULL;

static int conv_show(struct seq_file *file, void *v) {
	seq_printf(file, "%lld\n", reltime.convergence);
	return 0;
}

static int conv_open(struct inode *inode, struct file *file) {
	return single_open(file, conv_show, NULL);
}

static const struct file_operations conv_fops =
{
	.open = conv_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release
};

static int freq_show(struct seq_file *file, void *v) {
	long long unsigned int_part, dec_part, dec_part_unrounded;
	long double val = reltime.frequency;
	char *sign;
	if (val < 0) {
		val *= -1;
		sign = "-";
	}
	else
		sign = "";
	int_part = (long long unsigned) val;
	/* We want to print only the first 6 decimal digits, so we take 7 and
	 *      * round up the last one.  */
	dec_part_unrounded = (long long unsigned) ((val - int_part) * 10000000);
	dec_part = dec_part_unrounded / 10;
	dec_part_unrounded -= dec_part * 10;
	if (dec_part_unrounded >= 5)
		dec_part += 1;

	seq_printf(file, "%s%llu.%06llu\n", sign, int_part, dec_part);
	return 0;
}

static int freq_open(struct inode *inode, struct file *file) {
	return single_open(file, freq_show, NULL);
}

static const struct file_operations freq_fops =
{
	.open = freq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release
};

/* /proc entries do not need to be removed, because this is a "built-in module" so it's not removed before shutdown */
static int __init reltime_init (void)
{
	uml_reltime = proc_mkdir("uml_reltime", NULL);
	if (!uml_reltime)
		return -ENOMEM;
	convergence = proc_create("convergence", 0644, uml_reltime, &conv_fops);
	if (!convergence)
		return -ENOMEM;
	frequency = proc_create("frequency", 0644, uml_reltime, &freq_fops);
	  if (!frequency)
	  return -ENOMEM;
	return 0;
}
__initcall(reltime_init);
#endif
