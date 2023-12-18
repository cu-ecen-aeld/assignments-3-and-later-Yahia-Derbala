/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include "aesdchar.h"
int aesd_major =   0; // use dynamic major
int aesd_minor =   0;

MODULE_AUTHOR("Yahia-Derbala"); 
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

int aesd_open(struct inode *inode, struct file *filp)
{
    // Get the pointer to the device structure from the inode
    struct aesd_dev *dev = container_of(inode->i_cdev,struct aesd_dev,cdev);
    // Debugging log message to indicate that the device is being opened
    PDEBUG("open");
    // Update the private data in the file structure with a pointer to the device structure
    filp->private_data=dev;
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    PDEBUG("read %zu bytes with offset %lld",count,*f_pos);

    struct aesd_dev *dev = filp->private_data;
    struct aesd_buffer_entry *eptr;
    size_t cbuffer_offset=0;
    int readsize = count;
    ssize_t ret;

    /* If given offset is beyond device size*/
    if (*f_pos>=dev->size)
        goto out;

    /* Locking the data structure */
    ret = mutex_lock_interuptible(&dev->lock);
    /* Check if the mutex could be locked */
    if(ret != 0)
    {
        /* If mutex was not locked, display warning */
        PDEBUG("Failed to lock mutex!\n");
        /* Indicate system call should restart */
        retval = -ERESTARTSYS;
        goto out;
    }

    /* Find entry offset in respect to the file position */
    eptr = aesd_circular_buffer_find_entry_offset_for_fpos(&dev->cbuffer,*f_pos,cbuffer_offset);
    /* Check if entry was found */
    if(eptr==null)
    {
        PDEBUG("Failed to find entry offset in read \n");
        /* Unlock mutex */
        mutex_unlock(&dev->lock);
        goto out;
    }
    /* Calculate read size and making sure it doesn't exceed device size */
    if(cbuffer_offset + count > dev->size)
    {
        /* Calculate readsize to be the remaining bytes available for reading */
        readsize=dev->size - cbuffer_offset;
    }

    /* Copy data to user space buffer */
    ret = copy_to_user(buf,eptr->buffptr + cbuffer_offset,readsize);
    if(ret != 0)
    {
        PDEBUG("Failed to copy to user in read \n");
        mutex_unlock(&dev->lock);
        /* Indicate a memory access error */
        retval = -EFAULT; 
        goto out;
    }

    mutex_unlock(&dev->lock);
    *f_pos+=count;
    PDEBUG("Success: aesd_read has read %d bytes of %ld\n",readsize,count);
    return count;

out:
    return retval;
} 

int aesd_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");

    return 0;
}


ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos)
{
    ssize_t retval = -ENOMEM;
    PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
    struct aesd_dev *dev ;
    struct aesd_buffer_entry new_entry;
    char *userdata;
    ssize_t ret;
    size_t index =0;
    size_t entry_size=0;
    ssize_t newlinefound = 0;

    dev = filp->private_data;

    userdata=kmalloc(count,GFP_KERNEL);
    if(userdata==NULL)
    {
        PDEBUG("Failed to allocated memory for write data \n");
        goto cleanup;
    }
    return retval;
}
struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
    .read =     aesd_read,
    .write =    aesd_write,
    .open =     aesd_open,
    .release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
    int err, devno = MKDEV(aesd_major, aesd_minor);

    cdev_init(&dev->cdev, &aesd_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &aesd_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding aesd cdev", err);
    }
    return err;
}

int aesd_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, aesd_minor, 1,
            "aesdchar");
    aesd_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", aesd_major);
        return result;
    }
    memset(&aesd_device,0,sizeof(struct aesd_dev));



    /* initialize mutex and circular buffer */
    mutex_init(&aesd_device.lock);
    aesd_circular_buffer_init(&aesd_device.cbuffer);

    result = aesd_setup_cdev(&aesd_device);

    if( result ) {
        unregister_chrdev_region(dev, 1);
    }
    return result;

}

void aesd_cleanup_module(void)
{
    dev_t devno = MKDEV(aesd_major, aesd_minor);

    cdev_del(&aesd_device.cdev);


    mutex_destry(^&aesd_device.lock);
    uint8_t index;
    struct aesd_buffer_entry *eptr;
    /* iterate through each entry and free allocated memory */
    AESD_CIRCULAR_BUFFER_FOREACH(eptr, &aesd_device.cbuffer, index) 
    {
        kfree(eptr->buffptr);
    }
    unregister_chrdev_region(devno, 1);
}



module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
