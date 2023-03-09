/*=================*\
kernel module imports
\*=================*/
#include <linux/init.h> //macros used to mark up functions   e.g. __init __exit
#include <linux/module.h> // core header for loading module into kernel
#include <linux/kernel.h> //contains types, macros, functions for the kernel   e.g. KERN_INFO

#include <custom/functions.h> //custom functions and variables

/*========*\
declarations
\*========*/
static int __init ModuleInit(void); // init function
static void __exit ModuleExit(void); // exit function

static int __init ModuleInit(void) {
  printk(KERN_INFO "lelantos: installed\n"); //DEBUG logs to dmesg
  get_syscall_table();
  return 0;
}

static void __exit ModuleExit(void) {
  printk(KERN_INFO "Lelantus: removed\n");
}






module_init(ModuleInit);
module_exit(ModuleExit);



/*==================*\
liscencing information
\*==================*/
MODULE_AUTHOR("Artemis's Angel"); //author
MODULE_DESCRIPTION("Lelantos Client"); //description
MODULE_LICENSE("GPL");// GPL license
MODULE_VERSION("0.01"); //version