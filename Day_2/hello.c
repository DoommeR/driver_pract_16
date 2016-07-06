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

static int tail;
static int curr;

static struct cdev acme_cdev;

rwlock_t locker;


static ssize_t fl_read(struct file *file, char __user * buf, size_t count, loff_t * ppos)
{
	
	if (curr==tail) {
		write_lock(&locker);
		if (copy_to_user(buf, acme_buf, tail))
			return -EFAULT;
		write_unlock(&locker);
		
	}
	else
		if (copy_to_user(buf, acme_buf, curr))
			return -EFAULT;

}


static ssize_t fl_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	
	tail=acme_bufsize-1;
	count=0;

	read_lock(&locker);
	if (count > tail) {
		 if (copy_from_user(&acme_buf[curr] ,buf ,tail)) 
			return -EFAULT;

		 printk(KERN_ALERT "buff_full!\n");
		 read_unlock(&locker);
		 return tail;
	} 

		else{
			if (copy_from_user(&acme_buf[curr] ,buf ,count))
			return -EFAULT;
		 		else {
		 			printk(KERN_ALERT"copy....\n");
					curr+=count;
					tail-=count;
					return count;

				}
		}	
	
	read_unlock(&locker);

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

