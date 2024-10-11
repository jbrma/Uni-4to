#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/wait.h>
#include <linux/kfifo.h>

#include <linux/semaphore.h>



MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ModProdCons-update Kernel Module - FDI-UCM");
MODULE_AUTHOR("Luna Santos y Jorge Bravo");

#define MAX_ELEM 25
#define DEVICE_NAME "prodcons" /* Dev name as it appears in /proc/devices   */
#define CLASS_NAME "MprodC"


/*
 * Global variables are declared as static, so are global within the file.
 */

static dev_t start;
static struct cdev* chardev = NULL;
static struct class* class = NULL;
static struct device* device = NULL;

//static char *clipboard;  // Space for the "clipboard"
struct kfifo cbuf;

struct semaphore elementos,huecos, mtx;

/* Timestamp  (tick resolution) to keep track of last time the clipboard was updated */
static unsigned long last_prodcons_update = 0;

static ssize_t prodcons_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {

    char kbuf[MAX_ELEM+1];
    int elem;
    int nr_bytes=0;

  if ((*off) > 0) /* The application can write in this entry just once !! */
    return 0;

        //Pasa  buf -> kbuf
  if (copy_from_user(kbuf, buf, len)) {
    return -EFAULT; 
  }

  if (sscanf(kbuf, "%d", &elem) != 1) return -EINVAL;
  //convierto de char a int 

  if (down_interruptible(&huecos))
      return -EINTR;

  if (down_interruptible(&mtx)){//exclusion mutua
    up(&huecos);
    return -EINTR;
  }


  kfifo_in(&cbuf,&elem,sizeof(int));

  up(&mtx);
  up(&elementos);


  *off += len;          /* Update the file position indicator */
  /* Register timestamp */
  last_prodcons_update = jiffies;

  return len;
}

static ssize_t prodcons_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {

  unsigned long last_update_local;

  char kbuf[MAX_ELEM+1];
  int nr_bytes=0;
  int elem;
  
  if ((*off)>0)
    return 0;


  if (down_interruptible(&elementos))//bloquea si no hay elementos
      return -EINTR;

  if (down_interruptible(&mtx)){//exclusion mutua
    up(&elementos);
    return -EINTR;
  }

/* Extraer el primer entero del buffer */
  kfifo_out(&cbuf,&elem,sizeof(int));
  
  up(&mtx);
  up(&huecos);
    
  nr_bytes=sprintf(kbuf,"%i\n",elem); //lo copio en kbuf 

  //Pasa a las paginas del kernel a las del userspace kbuf -> buf
	if (copy_to_user(buf, kbuf, nr_bytes)) return -EFAULT;

  /* Wait until next clipboard update */
  last_update_local = last_prodcons_update;

  (*off) += len; /* Update the file pointer */

  return nr_bytes;
}

static int prodcons_open(struct inode *i, struct file *filp ){

  /* Increment this module's reference counter */
  try_module_get(THIS_MODULE);

  return 0;
}

static int prodcons_release( struct inode *i, struct file *filp  ){
  
  /* Decrement this module's reference counter */
  module_put(THIS_MODULE);

  return 0;  
}

static struct file_operations fops = {
  .read = prodcons_read,
  .write = prodcons_write,
  .open = prodcons_open,
  .release = prodcons_release,
};


static char *custom_devnode(struct device *dev, umode_t *mode)
{
  if (!mode)
    return NULL;
  if (MAJOR(dev->devt) == MAJOR(start))
    *mode = 0666;
  return NULL;
}

int init_prodcons_module( void )
{
  int major;    /* Major number assigned to our device driver */
  int minor;    /* Minor number assigned to the associated character device */
  int ret;

  if (kfifo_alloc(&cbuf,MAX_ELEM*sizeof(int),GFP_KERNEL))
    return -ENOMEM;

  sema_init(&mtx, 1);
  sema_init(&elementos,0);
  sema_init(&huecos,MAX_ELEM);
  

  /* Get available (major,minor) range */
  if ((ret = alloc_chrdev_region (&start, 0, 1, DEVICE_NAME))) {
    printk(KERN_INFO "Can't allocate chrdev_region()");
    goto error_alloc_region;
  }

  /* Create associated cdev */
  if ((chardev = cdev_alloc()) == NULL) {
    printk(KERN_INFO "cdev_alloc() failed ");
    ret = -ENOMEM;
    goto error_alloc;
  }

  cdev_init(chardev, &fops);

  if ((ret = cdev_add(chardev, start, 1))) {
    printk(KERN_INFO "cdev_add() failed ");
    goto error_add;
  }

  /* Create custom class */
  class = class_create(THIS_MODULE, CLASS_NAME);

  if (IS_ERR(class)) {
    pr_err("class_create() failed \n");
    ret = PTR_ERR(class);
    goto error_class;
  }

  /* Establish function that will take care of setting up permissions for device file */
  class->devnode = custom_devnode;

  /*Creating device*/
  device = device_create(class, NULL, start, NULL, DEVICE_NAME);

  if (IS_ERR(device)) {
    pr_err("Device_create failed\n");
    ret = PTR_ERR(device);
    goto error_device;
  }

  major = MAJOR(start);
  minor = MINOR(start);

  printk(KERN_INFO "I was assigned major number %d. To talk to\n", major);
  printk(KERN_INFO "the driver try to cat and echo to /dev/%s.\n", DEVICE_NAME);
  printk(KERN_INFO "Remove the module when done.\n");

  printk(KERN_INFO "ProdCons-update: Module loaded.\n");

  return 0;

error_device:
  class_destroy(class);
error_class:
  /* Destroy chardev */
  if (chardev) {
    cdev_del(chardev);
    chardev = NULL;
  }
error_add:
  /* Destroy partially initialized chardev */
  if (chardev)
    kobject_put(&chardev->kobj);
error_alloc:
  unregister_chrdev_region(start, 1);
error_alloc_region:
  //vfree(clipboard);
  kfifo_free(&cbuf);

  return ret;
}

void exit_prodcons_module( void )
{
  if (device)
    device_unregister(device);

  if (class)
    class_destroy(class);

  /* Destroy chardev */
  if (chardev)
    cdev_del(chardev);

  /*
   * Release major minor pair
   */
  unregister_chrdev_region(start, 1);

  //vfree(clipboard);
  kfifo_free(&cbuf);


  printk(KERN_INFO "ProdCons-update: Module unloaded.\n");
}


module_init( init_prodcons_module );
module_exit( exit_prodcons_module );
