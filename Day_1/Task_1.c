#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DoommeR");

int init_mod(void) {
printk(KERN_INFO "Hello world 1.\n");
/*
*  Модуль должен возвращать  0 в случае успешной загрузки.
*/
return 0;
}
void cleanup_mod(void) {
printk(KERN_INFO "Goodbye world 1.\n");
}

module_init(init_mod);
module_exit(cleanup_mod);
