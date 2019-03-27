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

static unsigned int pwm_counter = 27;
static unsigned int pwm_counter_irqnumber;

static int seg_clock_refresh = 5;
struct timer_list seg_timer;

static unsigned int pulse_rise_time = 0;
static unsigned int pulse_fall_time = 0;
static unsigned int dutycycle=0;

static void lightswitch(int digit, int num);
static void _TimerHandler(unsigned long data);

static int whichdigit = 0;

//prototype the IRQ Handler of the button interrupt
static irq_handler_t  pwm_counter_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

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


//Switch GPIO initialization
      gpio_request(pwm_counter, "sysfs");
      gpio_direction_input(pwm_counter);
      gpio_export(pwm_counter, false);
      pwm_counter_irqnumber = gpio_to_irq(pwm_counter);


   result = request_irq(pwm_counter_irqnumber, (irq_handler_t) pwm_counter_irq_handler, (IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING), "pwm_counter_irq_handler", NULL);

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

  free_irq(pwm_counter_irqnumber, NULL);

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


  gpio_unexport(pwm_counter);

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

  gpio_free(pwm_counter);

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

  if (whichdigit == 3){
    lightswitch(1, dutycycle%10000/1000);
  }
  else if (whichdigit == 2){
    lightswitch(2, dutycycle%1000/100);
  }
  else if (whichdigit == 1){
    lightswitch(3, dutycycle%100/10);
  }
  else if(whichdigit == 0 ){
    lightswitch(4, dutycycle%10);
  }
  else{
    lightswitch(1, 2);
  }
  whichdigit=(whichdigit+1)%4;

  mod_timer(&seg_timer, jiffies + msecs_to_jiffies(seg_clock_refresh));
}

static irq_handler_t  pwm_counter_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
  if (gpio_get_value(pwm_counter)){
    pulse_rise_time=jiffies_to_msecs(jiffies);
    printk("UP UP UP\n");
  }
  else{
    pulse_fall_time=jiffies_to_msecs(jiffies);
    printk("DOWN DOWN\n");
  }
  if(pulse_fall_time>pulse_rise_time){
    dutycycle=pulse_fall_time-pulse_rise_time;
  }
  printk("Dutycycle is %d\n", dutycycle);
  return (irq_handler_t) IRQ_HANDLED;
}


module_init(initialz);
module_exit(exitz);
