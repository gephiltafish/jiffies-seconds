/**
 * seconds.c
 * 
 * Kernel module that communicates with /proc file system.
 * 
 * The makefile must be modified to compile this program.
 * Change the line "simple.o" or "hello.o" to "seconds.o"
 * 
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

static long initial = 0;

ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};

/* This function is called when the module is loaded. */
int proc_init(void) {
        // creates the /proc/seconds entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);
        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
        initial = (long)jiffies;
	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
        // removes the /proc/seconds entry
        remove_proc_entry(PROC_NAME, NULL);
        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/seconds is read,
 * and is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 * params:
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;
        rv = sprintf(buffer, "Seconds: %lu\n", (jiffies - initial) / HZ);

        //copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("SGG");

//================================================================================
// OUTPUT from seconds.ko
//================================================================================

// osc@ubuntu:~/final-src-osc10e/ch2$ sudo insmod seconds.ko
// osc@ubuntu:~/final-src-osc10e/ch2$ cat /proc/seconds
// Seconds: 4
// osc@ubuntu:~/final-src-osc10e/ch2$ cat /proc/seconds
// Seconds: 7
// osc@ubuntu:~/final-src-osc10e/ch2$ cat /proc/seconds
// Seconds: 10
// osc@ubuntu:~/final-src-osc10e/ch2$ cat /proc/seconds
// Seconds: 13
// osc@ubuntu:~/final-src-osc10e/ch2$ cat /proc/seconds
// Seconds: 20
// osc@ubuntu:~/final-src-osc10e/ch2$ sudo rmmod seconds

//================================================================================
// end of OUTPUT from seconds.ko
//================================================================================