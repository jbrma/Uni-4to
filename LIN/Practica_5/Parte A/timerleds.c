#include <linux/module.h>
#include <asm-generic/errno.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>

#define ALL_LEDS_ON 0x7
#define ALL_LEDS_OFF 0

#define D1_LEDS_OFF 0x1
#define D2_LEDS_OFF 0x2
#define D3_LEDS_OFF 0x4


#define MANUAL_DEBOUNCE

#define NR_GPIO_LEDS  3


const int led_gpio[NR_GPIO_LEDS] = {25, 27, 4};

/* Array to hold gpio descriptors */
struct gpio_desc* gpio_descriptors[NR_GPIO_LEDS];

#define GPIO_BUTTON 22
struct gpio_desc* desc_button = NULL;
static int gpio_button_irqn = -1;

static int sequence[] = {1,2,4};
static int led_state = 1;
static int button_pulsed = 0; // 0 continua con la secuencia, 1 se para


struct timer_list my_timer; /* Structure that describes the kernel timer */

static int timer_period_ms = 1000;

module_param(timer_period_ms, int, 0000);
MODULE_PARM_DESC(timer_period_ms, "A period");

DEFINE_SPINLOCK(sp);

///////////////////////////////////////////////////////////////////////////////////////////
/* Set led state to that specified by mask */
static inline int set_pi_leds(unsigned int mask) {
  int i;
  for (i = 0; i < NR_GPIO_LEDS; i++)
    gpiod_set_value(gpio_descriptors[i], (mask >> i) & 0x1 );
  return 0;
}

/* Function invoked when timer expires (fires) */
static void fire_timer(struct timer_list *timer)
{
    static char flag = 0;
    unsigned long flags;

    spin_lock_irqsave(&sp, flags);

      if (button_pulsed == 0){// continua

        if (flag == 3){
            flag = 0;
        }

        led_state = sequence[flag];

        flag++;
      }
    
    spin_unlock_irqrestore(&sp, flags);

    set_pi_leds(led_state);

    /* Re-activate the timer one second from now */
    mod_timer(timer, jiffies + msecs_to_jiffies(timer_period_ms));
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

    unsigned long b;

    spin_lock_irqsave(&sp, b);

    button_pulsed = ~button_pulsed; 

    spin_unlock_irqrestore(&sp, b);


  return IRQ_HANDLED;
}


static int __init gpioint_init(void)
{
  int i, j;
  int err = 0;
  char gpio_str[10];
  unsigned char gpio_out_ok = 0;

    /////////////////////////////////////7

    /* Create timer */
    timer_setup(&my_timer, fire_timer, 0);
    my_timer.expires = jiffies + HZ; /* Activate it one second from now */
    /* Activate the timer for the first time */
    add_timer(&my_timer);

    //////////////////////////////////////


  for (i = 0; i < NR_GPIO_LEDS; i++) {
    /* Build string ID */
    sprintf(gpio_str, "led_%d", i);
    //Requesting the GPIO
    if ((err = gpio_request(led_gpio[i], gpio_str))) {
      pr_err("Failed GPIO[%d] %d request\n", i, led_gpio[i]);
      goto err_handle;
    }

    /* Transforming into descriptor **/
    if (!(gpio_descriptors[i] = gpio_to_desc(led_gpio[i]))) {
      pr_err("GPIO[%d] %d is not valid\n", i, led_gpio[i]);
      err = -EINVAL;
      goto err_handle;
    }

    gpiod_direction_output(gpio_descriptors[i], 0);
  }


  /* Requesting Button's GPIO */
  if ((err = gpio_request(GPIO_BUTTON, "button"))) {
    pr_err("ERROR: GPIO %d request\n", GPIO_BUTTON);
    goto err_handle;
  }

  /* Configure Button */
  if (!(desc_button = gpio_to_desc(GPIO_BUTTON))) {
    pr_err("GPIO %d is not valid\n", GPIO_BUTTON);
    err = -EINVAL;
    goto err_handle;
  }

  gpio_out_ok = 1;

  //configure the BUTTON GPIO as input
  gpiod_direction_input(desc_button);

  /*
  ** The lines below are commented because gpiod_set_debounce is not supported
  ** in the Raspberry pi. Debounce is handled manually in this driver.
  */
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
    pr_err("my_device: cannot register IRQ ");
    goto err_handle;
  }

  //set_pi_leds(ALL_LEDS_ON);
  return 0;
err_handle:
  for (j = 0; j < i; j++)
    gpiod_put(gpio_descriptors[j]);

  if (gpio_out_ok)
    gpiod_put(desc_button);

  return err;
}

static void __exit gpioint_exit(void) {
  int i = 0;

  /* Wait until completion of the timer function (if it's currently running) and delete timer */
    del_timer_sync(&my_timer);

  free_irq(gpio_button_irqn, NULL);
  set_pi_leds(ALL_LEDS_OFF);
  
  for (i = 0; i < NR_GPIO_LEDS; i++)
    gpiod_put(gpio_descriptors[i]);

  gpiod_put(desc_button);
}

module_init(gpioint_init);
module_exit(gpioint_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Modleds");
