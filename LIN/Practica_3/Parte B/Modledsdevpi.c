#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <asm-generic/errno.h>
#include <linux/gpio.h>


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Modleds-Dev Kernel Module - FDI-UCM");
MODULE_AUTHOR("Luna Santos y Jorge Bravo");

#define BUFFER_LENGTH 256
#define DEVICE_NAME "leds" /* Dev name as it appears in /proc/devices   */
#define CLASS_NAME "Modleds"

#define ALL_LEDS_ON 0x7
#define ALL_LEDS_OFF 0
#define NR_GPIO_LEDS  3

/*
 * Global variables are declared as static, so are global within the file.
 */
static dev_t start;
static struct cdev* chardev = NULL;
static struct class* class = NULL;
static struct device* device = NULL;
//static char *clipboard;  // Space for the "clipboard"



/* Actual GPIOs used for controlling LEDs */
const int led_gpio[NR_GPIO_LEDS] = {25, 27, 4};

/* Array to hold gpio descriptors */
struct gpio_desc* gpio_descriptors[NR_GPIO_LEDS];

/* Set led state to that specified by mask */
static inline int set_pi_leds(unsigned int mask) {
  int i;
  for (i = 0; i < NR_GPIO_LEDS; i++)
    gpiod_set_value(gpio_descriptors[i], (mask >> i) & 0x1 );
  return 0;
}

static int initLeds(void){

    int i, j;
  int err = 0;
  char gpio_str[10];

  for (i = 0; i < NR_GPIO_LEDS; i++) {
    /* Build string ID */
    sprintf(gpio_str, "led_%d", i);
    /* Request GPIO */
    if ((err = gpio_request(led_gpio[i], gpio_str))) {
      pr_err("Failed GPIO[%d] %d request\n", i, led_gpio[i]);
      goto err_handle;
    }

    /* Transforming into descriptor */
    if (!(gpio_descriptors[i] = gpio_to_desc(led_gpio[i]))) {
      pr_err("GPIO[%d] %d is not valid\n", i, led_gpio[i]);
      err = -EINVAL;
      goto err_handle;
    }

    gpiod_direction_output(gpio_descriptors[i], 0);
  }

  //set_pi_leds(ALL_LEDS_ON);
  return 0;
err_handle:
  for (j = 0; j < i; j++)
    gpiod_put(gpio_descriptors[j]);
  return err;

}


static int exitLeds(void){
    int i = 0;

  set_pi_leds(ALL_LEDS_OFF);

  for (i = 0; i < NR_GPIO_LEDS; i++)
    gpiod_put(gpio_descriptors[i]);

  return 0;
}


static ssize_t modleds_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {

    char kbuf[BUFFER_LENGTH+1];
    unsigned int num;

    printk(KERN_INFO "WRITEEEEEEE 1");



  if ((*off) > 0) /* The application can write in this entry just once !! */
    return 0;

        //Pasa  buf -> kbuf
  if (copy_from_user(kbuf, buf, len)) {
    return -EFAULT; 
  }

  printk(KERN_INFO "WRITEEEEEEE 2");


  if (sscanf(kbuf, "%d", &num) != 1) return -EINVAL;

  printk(KERN_INFO "NUmreooo %d", num);


  if (num > 7 || num < 0){
    printk(KERN_INFO "hay que introducir un num entre 0 y 7");
    return -EINVAL;
  }

  set_pi_leds(num); 

  return len;
}


static int modleds_open(struct inode *i, struct file *filp ){

  /* Increment this module's reference counter */
  try_module_get(THIS_MODULE);

  return 0;
}

static int modleds_release( struct inode *i, struct file *filp  ){
  
  /* Decrement this module's reference counter */
  module_put(THIS_MODULE);

  return 0;  
}

static struct file_operations fops = {
  .write = modleds_write,
  .open = modleds_open,
  .release = modleds_release,
};


static char *custom_devnode(struct device *dev, umode_t *mode)
{
  if (!mode)
    return NULL;
  if (MAJOR(dev->devt) == MAJOR(start))
    *mode = 0666;
  return NULL;
}





int init_modleds_module( void )
{
  int major;    /* Major number assigned to our device driver */
  int minor;    /* Minor number assigned to the associated character device */
  int ret;

 
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

  /* Creating device */
  device = device_create(class, NULL, start, NULL, DEVICE_NAME);

  if (IS_ERR(device)) {
    pr_err("Device_create failed\n");
    ret = PTR_ERR(device);
    goto error_device;
  }
  else{
      printk(KERN_INFO "the driver is ok /dev/%s.\n", DEVICE_NAME);

  }

  major = MAJOR(start);
  minor = MINOR(start);

  printk(KERN_INFO "I was assigned major number %d. To talk to\n", major);
  printk(KERN_INFO "the driver try to cat and echo to /dev/%s.\n", DEVICE_NAME);
  printk(KERN_INFO "Remove the module when done.\n");

  printk(KERN_INFO "Modleds-dev: Module loaded.\n");


  initLeds();

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

  return ret;
}


void exit_modleds_module( void )
{
  if (device)
    device_destroy(class, device->devt);

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

  exitLeds();

  printk(KERN_INFO "Modleds-dev: Module unloaded.\n");
}


module_init( init_modleds_module );
module_exit( exit_modleds_module );
