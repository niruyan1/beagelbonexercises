#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");

static char *name="world";
module_param(name, charp, S_IRUGO);

static int __init helloworld_initial(void)
{
	struct timespec ts;
	getnstimeofday(&ts);
	printk(KERN_INFO "Hello %s.\n", name);
	printk(KERN_INFO "%.2lu:%.2lu:%.2lu\n", ((ts.tv_sec-3600*4)/3600)%(24),(ts.tv_sec/60)%(60), ts.tv_sec%60);
	
	return 0;
}

static void __exit hellowworld_exit(void)
{
	printk(KERN_INFO "Goodbye %s.\n", name);
}

module_init(helloworld_initial);
module_exit(hellowworld_exit);