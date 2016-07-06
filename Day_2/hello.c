#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");


static int acme_bufsize = 10;
static void *acme_buf=NULL;
static int acme_count = 1;
static dev_t acme_dev = MKDEV(202, 128);


static struct cdev acme_cdev;







static ssize_t fl_read(struct file *file, char __user * buf, size_t count, loff_t * ppos)
{
	
/* The acme_buf address corresponds to a device I/O memory area */
	/* of size acme_bufsize, obtained with ioremap() */
	int remaining_size, transfer_size;

	remaining_size = acme_bufsize - (int)(*ppos);
				/* bytes left to transfer */
	if (remaining_size == 0) {
				/* All read, returning 0 (End Of File) */
		return 0;
	}

	/* Size of this transfer */
	transfer_size = min_t(int, remaining_size, count);

	if (copy_to_user
	    (buf /* to */ , acme_buf + *ppos /* from */ , transfer_size)) {
		return -EFAULT;
	} else {		/* Increase the position in the open file */
		*ppos += transfer_size;
		return transfer_size;
	}
	
}

static ssize_t fl_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int remaining_bytes;
	int tail_bytes=0;
	/* Number of bytes not written yet in the device */
	remaining_bytes = acme_bufsize - (*ppos);
	if (*ppos>acme_bufsize-2) *ppos=0;

	if (count > remaining_bytes)  {
		/* Can't write beyond the end of the device */
		 tail_bytes=count- remaining_bytes+1;

		//return -EIO;
	} else tail_bytes=0;

	if (copy_from_user(acme_buf + *ppos /*to*/ , buf /*from*/ , count- tail_bytes)) {
		return -EFAULT;
	} else {
		/* Increase the position in the open file */
		if (tail_bytes!=0) {printk(KERN_ALERT "TAIL DETECTED!\n"); *ppos=0; return count- tail_bytes;} 
			else { *ppos += count;
			return count;}
	}
}

static const struct file_operations acme_fops = {
	.owner = THIS_MODULE,
	.read = fl_read,
	.write = fl_write,
};

static int __init hello_init(void) {
	
	
	printk(KERN_ALERT "Hello\n");

	int err;
	acme_buf=kmalloc(acme_bufsize, GFP_KERNEL);
	memset(acme_buf, 0, acme_bufsize);
	
	


	if (register_chrdev_region(acme_dev, acme_count, "acme")) {
		err = -ENODEV;
		goto err_free_buf;
	}

	cdev_init(&acme_cdev, &acme_fops);

	if (cdev_add(&acme_cdev, acme_dev, acme_count)) {
		err = -ENODEV;
		goto err_dev_unregister;
	}

	return 0;

 err_dev_unregister:
	unregister_chrdev_region(acme_dev, acme_count);
 err_free_buf:
	printk(KERN_ALERT" reg_ch_region err\n");

	return 0;
}


static void __exit hello_exit(void) {
	
	kfree(acme_buf);
	cdev_del(&acme_cdev);
	unregister_chrdev_region(acme_dev, acme_count);

printk(KERN_ALERT "Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);

