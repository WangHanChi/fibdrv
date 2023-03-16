#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include "bignum.h"

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("National Cheng Kung University, Taiwan");
MODULE_DESCRIPTION("Fibonacci engine driver");
MODULE_VERSION("0.1");

#define DEV_FIBONACCI_NAME "fibonacci"

/* MAX_LENGTH is set to 92 because
 * ssize_t can't fit the number > 92
 */
#define MAX_LENGTH 500

static dev_t fib_dev = 0;
static struct cdev *fib_cdev;
static struct class *fib_class;
static DEFINE_MUTEX(fib_mutex);
static ktime_t kt;

// static inline uint64_t fast_doubling_iter(uint64_t target)
// {
//     if (target <= 2)
//         return !!target;

//     // find first 1
//     uint8_t count = 63 - __builtin_clzll(target);
//     long fib_n0 = 1, fib_n1 = 1;

//     for (uint64_t i = count, fib_2n0, fib_2n1, mask; i-- > 0;) {
//         fib_2n0 = fib_n0 * ((fib_n1 << 1) - fib_n0);
//         fib_2n1 = fib_n0 * fib_n0 + fib_n1 * fib_n1;

//         mask = -!!(target & (1UL << i));
//         fib_n0 = (fib_2n0 & ~mask) + (fib_2n1 & mask);
//         fib_n1 = (fib_2n0 & mask) + fib_2n1;
//     }
//     return fib_n0;
// }

static void fib_sequence(long long k, bn *output)
{
    /* allocate memory */

    bn f0, f1, f;

    /* initialize */
    f0.lower = 0;
    f1.lower = 1;
    f0.upper = f1.upper = 0;
    switch (k) {
    case 0:
        output->lower = 0;
        output->upper = 0;
        break;
    case 1:
        output->lower = 1;
        output->upper = 0;
        break;
    default:
        for (int i = 2; i <= k; i++) {
            f.upper = f0.upper + f1.upper;
            if (f1.lower > ~f0.lower)
                f.upper++;
            f.lower = f0.lower + f1.lower;

            /* reset data */
            f0 = f1;
            f1 = f;
        }
        output->lower = f.lower;
        output->upper = f.upper;
        break;
    }
    // /* FIXME: C99 variable-length array (VLA) is not allowed in Linux kernel.
    // */ long long f[k + 2];

    // f[0] = 0;
    // f[1] = 1;

    // for (int i = 2; i <= k; i++) {
    //     f[i] = f[i - 1] + f[i - 2];
    // }

    // return f[k];
}

static int fib_open(struct inode *inode, struct file *file)
{
    if (!mutex_trylock(&fib_mutex)) {
        printk(KERN_ALERT "fibdrv is in use");
        return -EBUSY;
    }
    return 0;
}

static int fib_release(struct inode *inode, struct file *file)
{
    mutex_unlock(&fib_mutex);
    return 0;
}

/* calculate the fibonacci number at given offset */
static ssize_t fib_read(struct file *file,
                        char *buf,
                        size_t size,
                        loff_t *offset)
{
    bn *output = kmalloc(sizeof(bn), GFP_KERNEL);

    /* get time */
    kt = ktime_get();
    fib_sequence(*offset, output);
    kt = ktime_sub(ktime_get(), kt);

    /* copy_to_user */
    int i = copy_to_user(buf, output, sizeof(bn));
    /* free */
    kfree(output);
    /* check free  */
    if (!i) {
        printk(KERN_ALERT "copy_to_use is not used");
    }
    return (ssize_t) 1;
}

/* write operation is skipped */
static ssize_t fib_write(struct file *file,
                         const char *buf,
                         size_t size,
                         loff_t *offset)
{
    return ktime_to_ns(kt);
}

static loff_t fib_device_lseek(struct file *file, loff_t offset, int orig)
{
    loff_t new_pos = 0;
    switch (orig) {
    case 0: /* SEEK_SET: */
        new_pos = offset;
        break;
    case 1: /* SEEK_CUR: */
        new_pos = file->f_pos + offset;
        break;
    case 2: /* SEEK_END: */
        new_pos = MAX_LENGTH - offset;
        break;
    }

    if (new_pos > MAX_LENGTH)
        new_pos = MAX_LENGTH;  // max case
    if (new_pos < 0)
        new_pos = 0;        // min case
    file->f_pos = new_pos;  // This is what we'll use now
    return new_pos;
}

const struct file_operations fib_fops = {
    .owner = THIS_MODULE,
    .read = fib_read,
    .write = fib_write,
    .open = fib_open,
    .release = fib_release,
    .llseek = fib_device_lseek,
};

static int __init init_fib_dev(void)
{
    int rc = 0;

    mutex_init(&fib_mutex);

    // Let's register the device
    // This will dynamically allocate the major number
    rc = alloc_chrdev_region(&fib_dev, 0, 1, DEV_FIBONACCI_NAME);

    if (rc < 0) {
        printk(KERN_ALERT
               "Failed to register the fibonacci char device. rc = %i",
               rc);
        return rc;
    }

    fib_cdev = cdev_alloc();
    if (fib_cdev == NULL) {
        printk(KERN_ALERT "Failed to alloc cdev");
        rc = -1;
        goto failed_cdev;
    }
    fib_cdev->ops = &fib_fops;
    rc = cdev_add(fib_cdev, fib_dev, 1);

    if (rc < 0) {
        printk(KERN_ALERT "Failed to add cdev");
        rc = -2;
        goto failed_cdev;
    }

    fib_class = class_create(THIS_MODULE, DEV_FIBONACCI_NAME);

    if (!fib_class) {
        printk(KERN_ALERT "Failed to create device class");
        rc = -3;
        goto failed_class_create;
    }

    if (!device_create(fib_class, NULL, fib_dev, NULL, DEV_FIBONACCI_NAME)) {
        printk(KERN_ALERT "Failed to create device");
        rc = -4;
        goto failed_device_create;
    }
    return rc;
failed_device_create:
    class_destroy(fib_class);
failed_class_create:
    cdev_del(fib_cdev);
failed_cdev:
    unregister_chrdev_region(fib_dev, 1);
    return rc;
}

static void __exit exit_fib_dev(void)
{
    mutex_destroy(&fib_mutex);
    device_destroy(fib_class, fib_dev);
    class_destroy(fib_class);
    cdev_del(fib_cdev);
    unregister_chrdev_region(fib_dev, 1);
}

module_init(init_fib_dev);
module_exit(exit_fib_dev);
