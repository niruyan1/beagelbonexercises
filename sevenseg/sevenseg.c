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

static int seg_clock_refresh = 5;
struct timer_list seg_timer;

static void lightswitch(int digit, int num);
static void _TimerHandler(unsigned long data);

static struct timespec ts;
static int whichdigit = 0;

static unsigned int motorenable = 20;
static unsigned int motorleft = 69;
static unsigned int motorright = 45;


//Initial Function
static int __init initialz(void){

  printk(KERN_INFO "Hello from 7Seg\n");
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

   gpio_request(motorenable, "sysfs");
   gpio_direction_output(motorenable, 1);
   gpio_export(motorenable, false);

   gpio_request(motorleft, "sysfs");
   gpio_direction_output(motorleft, 1);
   gpio_export(motorleft, false);

   gpio_request(motorright, "sysfs");
   gpio_direction_output(motorright, 0);
   gpio_export(motorright, false);

   setup_timer(&seg_timer,_TimerHandler,0);
   mod_timer(&seg_timer, jiffies + msecs_to_jiffies(seg_clock_refresh));

////////////////////////////////////////////////////////////////////////////////


return 0;
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

  gpio_set_value(motorenable, 0);
  gpio_set_value(motorleft, 0);
  gpio_set_value(motorright, 0);

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

  del_timer(&seg_timer);

  printk(KERN_INFO "Goodbye from 7Seg\n");

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
  printk(KERN_INFO "%.2lu:%.2lu:%.2lu %d\n", ((ts.tv_sec-3600*4)/3600)%(24),(ts.tv_sec/60)%(60), ts.tv_sec%60, whichdigit);
  getnstimeofday(&ts);
  if (whichdigit == 0){
    lightswitch(1, ((ts.tv_sec-3600*4)/3600)%(24)/10);
  }
  else if (whichdigit == 1){
    lightswitch(2, ((ts.tv_sec-3600*4)/3600)%(24)%10);
  }
  else if (whichdigit == 2){
    lightswitch(3, (ts.tv_sec/60)%(60)/10);
  }
  else if(whichdigit == 3 ){
    lightswitch(4, (ts.tv_sec/60)%(60)%10);
  }
  else{
    lightswitch(1, 2);
  }
  whichdigit=(whichdigit+1)%4;

  mod_timer(&seg_timer, jiffies + msecs_to_jiffies(seg_clock_refresh));
}

module_init(initialz);
module_exit(exitz);
