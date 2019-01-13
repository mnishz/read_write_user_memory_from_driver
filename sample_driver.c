#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include "sample_driver.h"

MODULE_LICENSE("GPL");

#define DRIVER_NAME "sample_driver"

static unsigned int const MINOR_BASE = 0;
static unsigned int const MINOR_NUM  = 1;

static unsigned int sample_driver_major;
static struct cdev sample_driver_cdev;
static struct class* sample_driver_class = NULL;

static int sample_driver_open(struct inode* const inode, struct file* const file)
{
    printk("sample_driver_open\n");
    return 0;
}

static int sample_driver_close(struct inode* const inode, struct file* const file)
{
    printk("sample_driver_close\n");
    return 0;
}

static long sample_driver_ioctl(struct file* const file, unsigned int const cmd, unsigned long const arg)
{
    static int* userDataPtr = NULL;

    switch (cmd) {
        case USER_APP_A:
            printk("ioctl USER_APP_A\n");
            {
                uintptr_t userDataPhysAddr;
                if (copy_from_user(&userDataPhysAddr, (void __user*)arg, sizeof(userDataPhysAddr)) != 0) {
                    return -EFAULT;
                }
                userDataPtr = (int*)ioremap_cache(userDataPhysAddr, sizeof(*userDataPtr));
            }
            break;
        case USER_APP_B:
            printk("ioctl USER_APP_B\n");
            if (userDataPtr != NULL) {
                ++(*userDataPtr);
                iounmap(userDataPtr);
            }
            break;
        default:
            break;
    }

    return 0;
}

struct file_operations sample_driver_fops = {
    .open = sample_driver_open,
    .release = sample_driver_close,
    .unlocked_ioctl = sample_driver_ioctl,
};

static int sample_driver_init(void)
{
    printk("sample_driver_init\n");

    // Reserve an available major number
    dev_t dev;
    int const alloc_ret = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DRIVER_NAME);
    if (alloc_ret != 0) {
        printk(KERN_ERR "alloc_chrdev_region returned %d\n", alloc_ret);
        return -1;
    }
    sample_driver_major = MAJOR(dev);

    // Initialize and register a character device
    cdev_init(&sample_driver_cdev, &sample_driver_fops);
    sample_driver_cdev.owner = THIS_MODULE;
    int const cdev_err = cdev_add(&sample_driver_cdev, dev, MINOR_NUM);
    if (cdev_err != 0) {
        printk(KERN_ERR "cdev_add returned %d\n", cdev_err);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    // Create a device file
    sample_driver_class = class_create(THIS_MODULE, "sample_driver");
    if (IS_ERR(sample_driver_class)) {
        printk(KERN_ERR "class_create\n");
        cdev_del(&sample_driver_cdev);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }
    int minor;
    for (minor = MINOR_BASE; (minor < MINOR_BASE + MINOR_NUM); ++minor) {
        device_create(sample_driver_class, NULL, MKDEV(sample_driver_major, minor), NULL, "sample_driver%d", minor);
    }

    return 0;
}

static void sample_driver_exit(void)
{
    printk("sample_driver_exit\n");

    // Release the device file, the cdev and the major number
    dev_t dev = MKDEV(sample_driver_major, MINOR_BASE);
    int minor;
    for (minor = MINOR_BASE; (minor < MINOR_BASE + MINOR_NUM); ++minor) {
        device_destroy(sample_driver_class, MKDEV(sample_driver_major, minor));
    }
    class_destroy(sample_driver_class);
    cdev_del(&sample_driver_cdev);
    unregister_chrdev_region(dev, MINOR_NUM);
}

module_init(sample_driver_init);
module_exit(sample_driver_exit);
