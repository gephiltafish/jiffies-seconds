/**
 * hello.c
 * 
 * Kernel module that communicates with /proc file system.
 * 
 * The makefile must be modified to compile this program.
 * Change the line "simple.o" to "hello.o"
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
#define PROC_NAME "hello"
#define MESSAGE "Hello World\n"

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};

/* This function is called when the module is loaded. */
int proc_init(void) {
        // creates the /proc/hello entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);
        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
        printk(KERN_INFO "GOLDEN_RATIO_PRIME is: %lu\n", GOLDEN_RATIO_PRIME);
        printk(KERN_INFO "In proc_init(), Jiffies is: %lu and HZ is: %d\n", jiffies, HZ);
        return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
        // removes the /proc/hello entry
        remove_proc_entry(PROC_NAME, NULL);
        printk(KERN_INFO "The gcd of 3,300 and 24 is: %lu\n", gcd(3300, 24));
        printk(KERN_INFO "In proc_exit(), jiffies is: %lu\n", jiffies);
        printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/hello is read,
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
        rv = sprintf(buffer, "jiffies in /proc/read is: %lu\n", jiffies);

        printk(KERN_INFO "In proc_read(), jiffies is: %lu\n", jiffies);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}

/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Module");
MODULE_AUTHOR("SGG");







//================================================================================
// shell script for hello.ko
//================================================================================

// #!/bin/bash

// # clear the message queue, then
// # insert simple.ko into the kernel and remove it again, then
// # display the messages as a result of doing so

// sudo -p "osc"
// sudo dmesg -c 
// clear 

// printf "beginning script...\n\n"

// sudo insmod hello.ko

// cat /proc/hello
// sleep 1
// cat /proc/hello
// sleep 2
// cat /proc/hello
// sleep 3
// cat /proc/hello

// sudo rmmod hello
// sudo dmesg 

// printf "\n...end of script\n\n"



//================================================================================
// OUTPUT from hello.ko
//================================================================================

// beginning script...

// jiffies in /proc/read is: 4298226340
// jiffies in /proc/read is: 4298226594
// jiffies in /proc/read is: 4298227099
// jiffies in /proc/read is: 4298227850
// [13336.583205] /proc/hello created
// [13336.583207] GOLDEN_RATIO_PRIME is: 11400862456688148481
// [13336.583208] In proc_init(), Jiffies is: 4298226340 and HZ is: 250
// [13336.584391] In proc_read(), jiffies is: 4298226340
// [13337.597975] In proc_read(), jiffies is: 4298226594
// [13339.621514] In proc_read(), jiffies is: 4298227099
// [13342.625370] In proc_read(), jiffies is: 4298227850
// [13342.628963] The gcd of 3,300 and 24 is: 12
// [13342.628964] In proc_exit(), jiffies is: 4298227851
// [13342.628965] /proc/hello removed

// ...end of script

// osc@ubuntu:~/final-src-osc10e/ch2$ 

//================================================================================
// end of OUTPUT from hello.ko
//================================================================================