#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");

static unsigned int gpioLED = 48; //GPIO Pin connected to Gate of MOSFET
static unsigned int gpioButton = 20; //GPIO Pin connected to the resistor
static unsigned int irqNumber; //The interrupt number; will get later on
static bool ledState=0; //State of the LED
static unsigned int numberPresses = 0; //number of presses of button

//prototype the IRQ Handler of the button interrupt
static irq_handler_t  button_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

//Initial Function
static int __init initial(void){
//holds value of how interrupt was handled
   int result = 0;

//checks to see if valid GPIO pin
   if (!gpio_is_valid(gpioLED)){
      printk(KERN_INFO "Invalid GPIO\n");
      return -ENODEV;
   }

//Initial State of the LED 
   ledState = true;
//enable GPIO 48 (LED) from sysfs
   gpio_request(gpioLED, "sysfs");
//Set LED_GPIO to output and gives initial value of on
   gpio_direction_output(gpioLED, ledState);
//the gpioLED cannot change from output to input
   gpio_export(gpioLED, false);

// Enable GPIO 20
   gpio_request(gpioButton, "sysfs");
//Set GPIO as Input
   gpio_direction_input(gpioButton);
//Set debounce
   gpio_set_debounce(gpioButton, 200);
//the GPIO cannot change from input to output
   gpio_export(gpioButton, false);
 
//Map GPIO Pin to Interrupt Number
   irqNumber = gpio_to_irq(gpioButton);
 
//Requests Interrupt
   result = request_irq(irqNumber, (irq_handler_t) button_irq_handler, IRQF_TRIGGER_RISING, "button_irq_handler", NULL);
 
   return result;
}


static void __exit exit(void){
   gpio_set_value(gpioLED, 0);              // Turn the LED off, makes it clear the device was unloaded
   gpio_unexport(gpioLED);                  // Unexport the LED GPIO
   free_irq(irqNumber, NULL);               // Free the IRQ number, no *dev_id required in this case
   gpio_unexport(gpioButton);               // Unexport the Button GPIO
   gpio_free(gpioLED);                      // Free the LED GPIO
   gpio_free(gpioButton);                   // Free the Button GPIO
   printk(KERN_INFO "Goodbye from LED\n");
}


static irq_handler_t button_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
//toggle LED value
   ledState = !ledState;
//Toggle LED
   gpio_set_value(gpioLED, ledState);
   numberPresses++;
   return (irq_handler_t) IRQ_HANDLED;
}


module_init(initial);

module_exit(exit);

