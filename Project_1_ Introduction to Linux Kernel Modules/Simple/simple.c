#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>
/* This function is called when the module is loaded. */
int simple_init(void)
{
printk(KERN_INFO "Loading Kernel Module∖n");

return 0;
}
/* This function is called when the module is removed. */
void simple_exit(void)
{
	printk(KERN_INFO "Removing Kernel Module∖n");
	printk(KERN_INFO "%lu\n", jiffies);
}
/* Macros for registering module entry and exit points. */
module_init(simple_init);
module_exit(simple_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
