/*=================*\
kernel module imports
\*=================*/
#include <linux/kernel.h> //contains types, macros, functions for the kernel   e.g. KERN_INFO
#include <linux/unistd.h> //contains syscall names to numbers
#include <linux/version.h> //contains versions  e.g. LINUX_VERSION_CODE KERNEL_VERSION


/*===============================================================*\
kallsyms_lookup_name is no longer exported, so this is a workaround
https://xcellerator.github.io/posts/linux_rootkits_11/
https://gist.github.com/linuxthor/a2b2ef9e39470fd6dd188b25c427322b
\*===============================================================*/
#include <linux/kprobes.h>

static struct kprobe kp = {
        .symbol_name = "kallsyms_lookup_name"
};

/* typedef for kallsyms_lookup_name() so we can easily cast kp.addr */
typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
//function for kallysms_lookup_name_workaround definition
kallsyms_lookup_name_t kallsyms_lookup_name_workaround=(kallsyms_lookup_name_t) 0;


static void generate_kallsyms_lookup_name_workaround(void){
        /* register the kallsyms_lookup_name kprobe*/
        register_kprobe(&kp);
        /* assign kallsyms_lookup_name symbol to kp.addr */
        kallsyms_lookup_name_workaround = (kallsyms_lookup_name_t) kp.addr;
        /* done with the kallsyms_lookup_name kprobe, so unregister it */
        unregister_kprobe(&kp);
}



/*======================================*\
functions to get syscall table and index it
\*======================================*/

//syscall table
unsigned long * __sys_call_table = NULL;  //NULL = (void*)0

static void get_syscall_table(void){
  //uses kallsyms_lookup_name to find location
  if (kallsyms_lookup_name_workaround==(kallsyms_lookup_name_t) NULL){
    generate_kallsyms_lookup_name_workaround();
  }
  __sys_call_table = (unsigned long*) kallsyms_lookup_name_workaround("sys_call_table");
}
