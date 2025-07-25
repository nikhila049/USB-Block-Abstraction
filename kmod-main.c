#include <linux/kthread.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/skbuff.h>
#include <linux/freezer.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>

/* File IO-related headers */
#include <linux/fs.h>
#include <linux/bio.h>
#include <linux/buffer_head.h>
#include <linux/blkdev.h>
#include <linux/version.h>
#include <linux/blkpg.h>     
#include <linux/namei.h>     

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikhila Narapareddy");
MODULE_DESCRIPTION("A Block Abstraction Read/Write for a USB device.");
MODULE_VERSION("1.0");

/* USB device name argument */
static char* device = "/dev/sdb";
module_param(device, charp, S_IRUGO);


/* USB storage disk-related data structures */
static struct block_device *bdevice = NULL;
//struct bio *usb_bio = NULL;
struct file *usb_file = NULL;


bool kmod_ioctl_init(void);
void kmod_ioctl_teardown(void);

static bool open_usb(void)
{
    /* Open a file for the path of the usb */
    struct file *f;
    struct block_device *bdeviceopen;
    
    f = bdev_file_open_by_path(device, BLK_OPEN_READ | BLK_OPEN_WRITE, NULL, NULL);
    if (IS_ERR(f)){
      pr_err("failed to open USB");
      return false;
    }
    
    bdeviceopen = file_bdev(f);
    if (IS_ERR(bdeviceopen)){
      pr_err("Failed to open USB");
      fput(f);
      return false;
    }
    
    usb_file = f;
    bdevice = bdeviceopen;
    
    pr_info("usb opened");
    return true;
}

static void close_usb(void)
{
    /* Close the file and device communication interface */
    if (bdevice){
      bdevice = NULL;
    }
    
    if (usb_file){
      fput(usb_file);
      usb_file = NULL;
    }
    
    pr_info("USB Closed\n");
}

static int __init kmod_init(void)
{
    pr_info("Hello World!\n");
    if (!open_usb()) {
        pr_err("Failed to open USB block device\n");
        return -ENODEV;
    }
    kmod_ioctl_init();
    return 0;
}

static void __exit kmod_fini(void)
{
    close_usb();
    kmod_ioctl_teardown();
    printk("Goodbye, World!\n");
}

module_init(kmod_init);
module_exit(kmod_fini);
