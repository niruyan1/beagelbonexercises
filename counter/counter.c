#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

static unsigned int digit1 = 60;
static unsigned int digit2 = 49;
static unsigned int digit3 = 112;
static unsigned int digit4 = 115;

static unsigned int sega = 67;
static unsigned int segb = 68;
static unsigned int segc = 44;
static unsigned int segd = 26;
static unsigned int sege = 46;
static unsigned int segf = 65;
static unsigned int segg = 66;

static unsigned int switch_led = 27;
static unsigned int switch_irqnumber;
static unsigned int reset_counter = 47;
static unsigned int reset_irqnumber;

static unsigned int led = 20;

static unsigned int led_state = 1;

static int seg_clock_refresh = 5;
struct timer_list seg_timer;

static unsigned int msec_elap = 0; //number of presses of button

static void lightswitch(int digit, int num);
static void _TimerHandler(unsigned long data);

static struct timespec ts;
static int whichdigit = 0;

//prototype the IRQ Handler of the button interrupt
static irq_handler_t  switch_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  reset_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

//Initial Function
static int __init initialz(void){
    int result = 0;

  printk(KERN_INFO "Hello from Counter\n");

////////////////////////////////////////////////////////////////////////////////
//INITIALIZE GPIO PINS
//digit1 GPIO initialization
   gpio_request(digit1, "sysfs");
   gpio_direction_output(digit1, 0);
   gpio_export(digit1, false);

//digit2 GPIO initialization
   gpio_request(digit2, "sysfs");
   gpio_direction_output(digit2, 0);
   gpio_export(digit2, false);

//digit3 GPIO initialization
   gpio_request(digit3, "sysfs");
   gpio_direction_output(digit3, 0);
   gpio_export(digit3, false);

//digit4 GPIO initialization
   gpio_request(digit4, "sysfs");
   gpio_direction_output(digit4, 0);
   gpio_export(digit4, false);

//sega GPIO initialization
   gpio_request(sega, "sysfs");
   gpio_direction_output(sega, 1);
   gpio_export(sega, false);

//segb GPIO initialization
   gpio_request(segb, "sysfs");
   gpio_direction_output(segb, 1);
   gpio_export(segb, false);

//segc GPIO initialization
   gpio_request(segc, "sysfs");
   gpio_direction_output(segc, 1);
   gpio_export(segc, false);

//segd GPIO initialization
   gpio_request(segd, "sysfs");
   gpio_direction_output(segd, 1);
   gpio_export(segd, false);

//sege GPIO initialization
   gpio_request(sege, "sysfs");
   gpio_direction_output(sege, 1);
   gpio_export(sege, false);

//segf GPIO initialization
   gpio_request(segf, "sysfs");
   gpio_direction_output(segf, 1);
   gpio_export(segf, false);

//segg GPIO initialization
   gpio_request(segg, "sysfs");
   gpio_direction_output(segg, 1);
   gpio_export(segg, false);

//LED GPIO initialization
      gpio_request(led, "sysfs");
      gpio_direction_output(led, led_state );
      gpio_export(led, false);

//Switch GPIO initialization
      gpio_request(switch_led, "sysfs");
      gpio_direction_input(switch_led);
      gpio_set_debounce(switch_led, 5000);
      gpio_export(switch_led, false);
      switch_irqnumber = gpio_to_irq(switch_led);

//Reset GPIO initialization
      gpio_request(reset_counter, "sysfs");
      gpio_direction_input(reset_counter);
      gpio_set_debounce(reset_counter, 5000);
      gpio_export(reset_counter, false);
      reset_irqnumber = gpio_to_irq(reset_counter);

   result = request_irq(switch_irqnumber, (irq_handler_t) switch_irq_handler, (IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING), "switch_irq_handler", NULL);
   result = request_irq(reset_irqnumber, (irq_handler_t) reset_irq_handler, IRQF_TRIGGER_RISING, "reset_irq_handler", NULL);
   setup_timer(&seg_timer,_TimerHandler,0);
   mod_timer(&seg_timer, jiffies + msecs_to_jiffies(seg_clock_refresh));
////////////////////////////////////////////////////////////////////////////////
return result;
}

static void __exit exitz(void){
  gpio_set_value(digit1, 0);
  gpio_set_value(digit2, 0);
  gpio_set_value(digit3, 0);
  gpio_set_value(digit4, 0);

  gpio_set_value(sega, 1);
  gpio_set_value(segb, 1);
  gpio_set_value(segc, 1);
  gpio_set_value(segd, 1);
  gpio_set_value(sege, 1);
  gpio_set_value(segf, 1);
  gpio_set_value(segg, 1);

  gpio_set_value(led, 0);

  free_irq(switch_irqnumber, NULL);
  free_irq(reset_irqnumber, NULL);

  gpio_unexport(digit1);
  gpio_unexport(digit2);
  gpio_unexport(digit3);
  gpio_unexport(digit4);

  gpio_unexport(sega);
  gpio_unexport(segb);
  gpio_unexport(segc);
  gpio_unexport(segd);
  gpio_unexport(sege);
  gpio_unexport(segf);
  gpio_unexport(segg);

  gpio_unexport(switch_led);
  gpio_unexport(reset_counter);
  gpio_unexport(led);

  gpio_free(digit1);
  gpio_free(digit2);
  gpio_free(digit3);
  gpio_free(digit4);

  gpio_free(sega);
  gpio_free(segb);
  gpio_free(segc);
  gpio_free(segd);
  gpio_free(sege);
  gpio_free(segf);
  gpio_free(segg);

  gpio_free(switch_led);
  gpio_free(reset_counter);
  gpio_free(led);

  del_timer(&seg_timer);

  printk(KERN_INFO "Goodbye from Counter\n");

}

static void lightswitch(int digit, int num){

  gpio_set_value(digit1, 0);
  gpio_set_value(digit2, 0);
  gpio_set_value(digit3, 0);
  gpio_set_value(digit4, 0);

  gpio_set_value(sega, 1);
  gpio_set_value(segb, 1);
  gpio_set_value(segc, 1);
  gpio_set_value(segd, 1);
  gpio_set_value(sege, 1);
  gpio_set_value(segf, 1);
  gpio_set_value(segg, 1);

  if (num == 1){
    gpio_set_value(segb, 0);
    gpio_set_value(segc, 0);
  }
  else if(num == 2){
    gpio_set_value(sega, 0);
    gpio_set_value(segb, 0);
    gpio_set_value(segd, 0);
    gpio_set_value(sege, 0);
    gpio_set_value(segg, 0);
  }
  else if(num == 3){
    gpio_set_value(sega, 0);
    gpio_set_value(segb, 0);
    gpio_set_value(segc, 0);
    gpio_set_value(segd, 0);
    gpio_set_value(segg, 0);
  }
  else if(num == 4){
    gpio_set_value(segb, 0);
    gpio_set_value(segc, 0);
    gpio_set_value(segf, 0);
    gpio_set_value(segg, 0);
  }
  else if(num == 5){
    gpio_set_value(sega, 0);
    gpio_set_value(segc, 0);
    gpio_set_value(segd, 0);
    gpio_set_value(segf, 0);
    gpio_set_value(segg, 0);
  }
  else if(num == 6){
    gpio_set_value(sega, 0);
    gpio_set_value(segc, 0);
    gpio_set_value(segd, 0);
    gpio_set_value(sege, 0);
    gpio_set_value(segf, 0);
    gpio_set_value(segg, 0);
  }
  else if(num == 7){
    gpio_set_value(sega, 0);
    gpio_set_value(segb, 0);
    gpio_set_value(segc, 0);
  }
  else if(num == 8){
    gpio_set_value(sega, 0);
    gpio_set_value(segb, 0);
    gpio_set_value(segc, 0);
    gpio_set_value(segd, 0);
    gpio_set_value(sege, 0);
    gpio_set_value(segf, 0);
    gpio_set_value(segg, 0);
  }
  else if(num == 9){
    gpio_set_value(sega, 0);
    gpio_set_value(segb, 0);
    gpio_set_value(segc, 0);
    gpio_set_value(segf, 0);
    gpio_set_value(segg, 0);
  }
  else if(num == 0){
    gpio_set_value(sega, 0);
    gpio_set_value(segb, 0);
    gpio_set_value(segc, 0);
    gpio_set_value(segd, 0);
    gpio_set_value(sege, 0);
    gpio_set_value(segf, 0);
  }
  else{
    gpio_set_value(sega, 1);
    gpio_set_value(segb, 1);
    gpio_set_value(segc, 1);
    gpio_set_value(segd, 1);
    gpio_set_value(sege, 1);
    gpio_set_value(segf, 1);
    gpio_set_value(segg, 1);
  }

  if (digit == 1){
    gpio_set_value(digit1, 1);
  }
  else if (digit == 2){
    gpio_set_value(digit2, 1);
  }
  else if (digit == 3){
    gpio_set_value(digit3, 1);
  }
  else if (digit == 4){
    gpio_set_value(digit4, 1);
  }
  else{
    gpio_set_value(digit1, 0);
    gpio_set_value(digit2, 0);
    gpio_set_value(digit3, 0);
    gpio_set_value(digit4, 0);
  }
}

static void _TimerHandler(unsigned long data){
  getnstimeofday(&ts);
  if (led_state==1){
    msec_elap+=5;
  }
  if (whichdigit == 3){
    lightswitch(1, msec_elap/1000%10000/1000);
  }
  else if (whichdigit == 2){
    lightswitch(2, msec_elap/1000%1000/100);
  }
  else if (whichdigit == 1){
    lightswitch(3, msec_elap/1000%100/10);
  }
  else if(whichdigit == 0 ){
    lightswitch(4, msec_elap/1000%10);
  }
  else{
    lightswitch(1, 2);
  }
  whichdigit=(whichdigit+1)%4;

  mod_timer(&seg_timer, jiffies + msecs_to_jiffies(seg_clock_refresh));
}

static irq_handler_t  switch_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
  led_state=!led_state;
  gpio_set_value(led,led_state);
  printk("Light is now %d.Time is %d\n",led_state, msec_elap/1000);
  return (irq_handler_t) IRQ_HANDLED;
}

static irq_handler_t  reset_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
  msec_elap = 0;
  printk("Reset now %d\n",msec_elap);
  return (irq_handler_t) IRQ_HANDLED;
}


module_init(initialz);
module_exit(exitz);
