#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  /* for copy_to_user */
#include <linux/miscdevice.h>
#include <asm-generic/errno.h>
#include <linux/init.h>
#include <linux/tty.h> /* For fg_console */
#include <linux/kd.h>  /* For KDSETLED */
#include <linux/vt_kern.h>
#include <linux/pwm.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/vmalloc.h>


MODULE_DESCRIPTION("Kernel Module - FDI-UCM");
MODULE_AUTHOR("Luna Santos y Jorge Bravo");
MODULE_LICENSE("GPL");

#define SUCCESS 0
#define DEVICE_NAME "buzzer"  /* Dev name as it appears in /proc/devices   */
#define CLASS_NAME "cool"
#define BUF_LEN 80      /* Max length of the message from the device */

/* Frequency of selected notes in centihertz */
#define C4 26163
#define D4 29366
#define E4 32963
#define F4 34923
#define G4 39200
#define C5 52325

#define MANUAL_DEBOUNCE

#define PWM_DEVICE_NAME "pwmchip0"

/*
 *  Prototypes
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int flags;

static int Device_Open = 0; /* Is device open?
                 * Used to prevent multiple access to device */
static char msg[BUF_LEN];   /* The msg the device will give when asked */
static char *msg_Ptr;       /* This will be initialized every time the
                   device is opened successfully */
static int counter = 0;       /* Tracks the number of times the character
                 * device has been opened */
static int beat = 120;

struct timer_list my_timer; /* Structure that describes the kernel timer */

static int timer_period_ms = 1000;
DEFINE_SPINLOCK(sp);

struct pwm_device *pwm_device = NULL;
struct pwm_state pwm_state;

/* Work descriptor */
struct work_struct my_work;

/* Structure to represent a note or rest in a melodic line  */
struct music_step
{
	unsigned int freq : 24; /* Frequency in centihertz */
	unsigned int len : 8;	/* Duration of the note */
};

struct music_step* melodic_line;


static spinlock_t lock; /* Cerrojo para proteger actualización/consulta 
						de variables buzzer_state y buzzer_request */

static struct music_step* next_note=NULL; /* Puntero a la siguiente nota de la melodía 
											actual  (solo alterado por tarea diferida) */

typedef enum {
    BUZZER_STOPPED, /* Buzzer no reproduce nada (la melodía terminó o no ha comenzado) */
    BUZZER_PAUSED,	/* Reproducción pausada por el usuario */
    BUZZER_PLAYING	/* Buzzer reproduce actualmente la melodía */
} buzzer_state_t;

static buzzer_state_t buzzer_state=BUZZER_STOPPED; /* Estado actual de la reproducción */

typedef enum {
    REQUEST_START,		/* Usuario pulsó SW1 durante estado BUZZER_STOPPED */
    REQUEST_RESUME,		/* Usuario pulsó SW1 durante estado BUZZER_PAUSED */
    REQUEST_PAUSE,		/* Usuario pulsó SW1 durante estado BUZZER_PLAYING */
    REQUEST_CONFIG,		/* Usuario está configurando actualmente una nueva melodía vía /dev/buzzer  */
    REQUEST_NONE			/* Indicador de petición ya gestionada (a establecer por tarea diferida) */
} buzzer_request_t;

static buzzer_request_t buzzer_request=REQUEST_NONE;

#define GPIO_BUTTON 22
struct gpio_desc* desc_button = NULL;
static int gpio_button_irqn = -1;
static int button_pulsed = 0; // 1 continua con la musica

/* Transform frequency in centiHZ into period in nanoseconds */
static inline unsigned int freq_to_period_ns(unsigned int frequency)
{
	if (frequency == 0)
		return 0;
	else
		return DIV_ROUND_CLOSEST_ULL(100000000000UL, frequency);
}

/* Check if the current step is and end marker */
static inline int is_end_marker(struct music_step *step)
{
	return (step->freq == 0 && step->len == 0);
}

/**
 *  Transform note length into ms,
 * taking the beat of a quarter note as reference
 */
static inline int calculate_delay_ms(unsigned int note_len, unsigned int qnote_ref)
{
	unsigned char duration = (note_len & 0x7f);
	unsigned char triplet = (note_len & 0x80);
	unsigned char i = 0;
	unsigned char current_duration;
	int total = 0;

	/* Calculate the total duration of the note
	 * as the summation of the figures that make
	 * up this note (bits 0-6)
	 */
	while (duration) {
		current_duration = (duration) & (1 << i);

		if (current_duration) {
			/* Scale note accordingly */
			if (triplet)
				current_duration = (current_duration * 3) / 2;
			/*
			 * 24000/qnote_ref denote number of ms associated
			 * with a whole note (redonda)
			 */
			total += (240000) / (qnote_ref * current_duration);
			/* Clear bit */
			duration &= ~(1 << i);
		}
		i++;
	}
	return total;
}



/* Work's handler function */
static void my_wq_function(struct work_struct *work)
{

    spin_lock_irqsave(&lock, flags);

    if(buzzer_state == BUZZER_PLAYING){

        pwm_init_state(pwm_device, &pwm_state);

        /* Play notes sequentially until end marker is found */
        for (next_note = melodic_line; !is_end_marker(next_note); next_note++) {
            /* Obtain period from frequency */
            pwm_state.period = freq_to_period_ns(next_note->freq);

            /**
             * Disable temporarily to allow repeating the same consecutive
             * notes in the melodic lin
             **/
            pwm_disable(pwm_device);

            /* If period==0, its a rest (silent note) */
            if (pwm_state.period > 0) {
                /* Set duty cycle to 70 to maintain the same timbre */
                pwm_set_relative_duty_cycle(&pwm_state, 70, 100);
                pwm_state.enabled = true;
                /* Apply state */
                pwm_apply_state(pwm_device, &pwm_state);
            } else {
                /* Disable for rest */
                pwm_disable(pwm_device);
            }

            /* Wait for duration of the note or reset */
            msleep(calculate_delay_ms(next_note->len, beat));

            if(buzzer_state !=  BUZZER_PLAYING){
                break;
            }
        }

        buzzer_state = BUZZER_STOPPED;


        pwm_disable(pwm_device);
    }
    
    printk(KERN_ERR "FIN melodía\n");

    spin_unlock_irqrestore(&lock, flags);

    return;

}

/* Interrupt handler for button **/
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
#ifdef MANUAL_DEBOUNCE
  static unsigned long last_interrupt = 0;
  unsigned long diff = jiffies - last_interrupt;
  if (diff < 20)
    return IRQ_HANDLED;

  last_interrupt = jiffies;
#endif

    button_pulsed = ~button_pulsed; 
    printk(KERN_ERR "ccambio puls %i\n", button_pulsed);

  return IRQ_HANDLED;
}

/* Function invoked when timer expires (fires) */
static void fire_timer(struct timer_list *timer)
{
    unsigned long flags;

    spin_lock_irqsave(&sp, flags);

      if (button_pulsed == 0){// 

                buzzer_state = BUZZER_PLAYING;
                printk(KERN_ERR "segunda vez pulsando\n");


        /*if(buzzer_state == BUZZER_PLAYING){
            buzzer_state = BUZZER_PAUSED;
        }else if(buzzer_state == BUZZER_PAUSED){
          buzzer_state = BUZZER_PLAYING;
        }*/

      }else{
                printk(KERN_ERR "primera vez pulsando\n");

         buzzer_state = BUZZER_PAUSED;
      }
    
    spin_unlock_irqrestore(&sp, flags);


    /* Re-activate the timer one second from now */
    mod_timer(timer, jiffies + msecs_to_jiffies(timer_period_ms));
}


static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};


static struct miscdevice misc_chardev = {
    .minor = MISC_DYNAMIC_MINOR,    /* kernel dynamically assigns a free minor# */
    .name = DEVICE_NAME, /* when misc_register() is invoked, the kernel
                        * will auto-create device file as /dev/chardev ;
                        * also populated within /sys/class/misc/ and /sys/devices/virtual/misc/ */
    .mode = 0666,     /* ... dev node perms set as specified here */
    .fops = &fops,    /* connect to this driver's 'functionality' */
};

/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
    int major;      /* Major number assigned to our device driver */
    int minor;      /* Minor number assigned to the associated character device */
    int ret;
    struct device *device;
      unsigned char gpio_out_ok = 0;


    melodic_line = vmalloc(PAGE_SIZE);
    if (!melodic_line) {
        printk(KERN_ERR "Error al reservar memoria para la melodía\n");
        return -ENOMEM;
    }

    ret = misc_register(&misc_chardev);

    if (ret) {
        pr_err("Couldn't register misc device\n");
        return ret;
    }

    device = misc_chardev.this_device;

    /* Access devt field in device structure to retrieve (major,minor) */
    major = MAJOR(device->devt);
    minor = MINOR(device->devt);

    ////////////////////////////

     /* Configure Button */
    if (!(desc_button = gpio_to_desc(GPIO_BUTTON))) {
        pr_err("GPIO %d is not valid\n", GPIO_BUTTON);
        return -EINVAL;
    }

    gpio_out_ok = 1;

    //configure the BUTTON GPIO as input
    gpiod_direction_input(desc_button);
    
    //////////

    spin_lock_init(&lock);

    /* Request utilization of PWM0 device */
	pwm_device = pwm_request(0, PWM_DEVICE_NAME);

	if (IS_ERR(pwm_device))
		return PTR_ERR(pwm_device);

	/* Initialize work structure (with function) */
	INIT_WORK(&my_work, my_wq_function);

    /* Create timer */
    timer_setup(&my_timer, fire_timer, 0);
    my_timer.expires = jiffies + HZ; /* Activate it one second from now */
    /* Activate the timer for the first time */
    add_timer(&my_timer);

	/* Enqueue work */
	schedule_work(&my_work);


    dev_info(device, "I was assigned major number %d. To talk to\n", major);
    dev_info(device, "the driver try to cat and echo to /dev/%s.\n", DEVICE_NAME);
    dev_info(device, "Remove the module when done.\n");

#ifndef MANUAL_DEBOUNCE
  //Debounce the button with a delay of 200ms
  if (gpiod_set_debounce(desc_button, 200) < 0) {
    pr_err("ERROR: gpio_set_debounce - %d\n", GPIO_BUTTON);
    goto err_handle;
  }
#endif

  //Get the IRQ number for our GPIO
  gpio_button_irqn = gpiod_to_irq(desc_button);
  pr_info("IRQ Number = %d\n", gpio_button_irqn);

  if (request_irq(gpio_button_irqn,             //IRQ number
                  gpio_irq_handler,   //IRQ handler
                  IRQF_TRIGGER_RISING,        //Handler will be called in raising edge
                  "button_leds",               //used to identify the device name using this IRQ
                  NULL)) {                    //device id for shared IRQ
    return -EBUSY;
  }

  //set_pi_leds(ALL_LEDS_ON);
  return 0;

    return 0;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{

   vfree(melodic_line);



    /* Wait until defferred work has finished */
	flush_work(&my_work);

    del_timer_sync(&my_timer);


	/* Release PWM device */
	pwm_free(pwm_device);

    gpiod_put(desc_button);


    misc_deregister(&misc_chardev);
    pr_info("Chardev misc driver deregistered. Bye\n");
}

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/chardev"
 */
static int device_open(struct inode *inode, struct file *file)
{
    if (Device_Open)
        return -EBUSY;

    Device_Open++;

    /* Initialize msg */
    sprintf(msg, "I already told you %d times Hello world!\n", counter++);

    /* Initially, this points to the beginning of the message */
    msg_Ptr = msg;

    /* Increment the module's reference counter */
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
    Device_Open--;      /* We're now ready for our next caller */

    /*
     * Decrement the usage count, or else once you opened the file, you'll
     * never get get rid of the module.
     */
    module_put(THIS_MODULE);

    return 0;
}

/*
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp,   /* see include/linux/fs.h   */
                           char *buffer,    /* buffer to fill with data */
                           size_t length,   /* length of the buffer     */
                           loff_t * offset)
{
    /*
     * Number of bytes actually written to the buffer
     */
    int bytes_to_read ;
        char kbuf[20];

    if(*offset > 0)
        return 0;
    

   // printk(KERN_INFO "REAAAADDDDDD\n");

    bytes_to_read=sprintf(kbuf,"%i\n",beat); //lo copio en kbuf 
    /*
     * Actually transfer the data onto the userspace buffer.
     * For this task we use copy_to_user() due to security issues
     */
    if (copy_to_user(buffer, kbuf, bytes_to_read))
        return -EFAULT;


    //printk(KERN_INFO "REAAAADDDDDD222222\n");

    *offset = bytes_to_read;
    /*
     * The read operation returns the actual number of bytes
     * we copied  in the user's buffer
     */
    return bytes_to_read;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/chardev
 */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
   
   int b;
   char kbuf[250];



	if(len > 250){
        printk(KERN_INFO "No hay espacio\n");
		return -EINVAL;
	}

	if(copy_from_user(kbuf, buff, len))
		return -EFAULT; //error de copia desde el espacio de usuario

    

	kbuf[len]= '\0';//marcando final

    printk(KERN_INFO "WRITEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");



    if(sscanf(kbuf, "beat %i", &beat) == 1){
        printk(KERN_INFO "Nuevo ritmo de reproducción: %d\n", beat);
    }
    else if(strncmp(kbuf, "music ", 6) == 0){

        char* cadena = kbuf + 6;
        int nota, duracion;
        char* aux;
	    int i= 0;

        while((aux = strsep(&cadena, ",")) != NULL){

            if(sscanf(aux, "%d:0x%x", &nota, &duracion) == 2){

                /////////////////////////////////////////////
                melodic_line[i].freq = nota;
                melodic_line[i].len = duracion;
                i++;
            }
            else{
                printk(KERN_INFO "Error al escanear las notas de la cancion");
                return -EINVAL;

            }
        }

        spin_lock_irqsave(&lock, flags);
        buzzer_state = BUZZER_PLAYING;
        spin_unlock_irqrestore(&lock, flags);

        schedule_work(&my_work);
        
        printk(KERN_INFO "Nueva melodía\n");
                
	}
    else{
        printk(KERN_INFO "Comando no reconocido\n");
        return -EINVAL;
                
    }
    return len;
 }



