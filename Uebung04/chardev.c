/*
 *  chardev.c - Create an input/output character device
 */

#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/fs.h>
#include <asm/uaccess.h>	/* for get_user and put_user */

#include "chardev.h"
#define SUCCESS 0
#define FAILURE -1
#define DEVICE_NAME "char_dev"
#define BUF_LEN 80


#define SER_MCR(x) ((x)+4)
#define SR_MCR_DTR 0x01
#define SR_MCR_RTS 0x02
#define SER_MSR(x) ((x)+6)
#define SR_MSR_CTS 0x10
#define SR_MSR_DSR 0x20

unsigned COM1_BASEADRESS = 0x3F8;


/* 
 * Is the device open right now? Used to prevent
 * concurent access into the same device 
 */
static int Device_Open = 0;

/* 
 * The message the device will give when asked 
 */
static char Message[BUF_LEN];

/* 
 * How far did the process reading the message get?
 * Useful if the message is larger than the size of the
 * buffer we get to fill in device_read. 
 */
static char *Message_Ptr;

/* 
 * This is called whenever a process attempts to open the device file 
 */
static int device_open(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "device_open(%p)\n", file);
#endif

	/* 
	 * We don't want to talk to two processes at the same time 
	 */
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	/*
	 * Initialize the message 
	 */
	Message_Ptr = Message;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "device_release(%p,%p)\n", inode, file);
#endif

	/* 
	 * We're now ready for our next caller 
	 */
	Device_Open--;

	module_put(THIS_MODULE);
	return SUCCESS;
}

/* 
 * This function is called whenever a process which has already opened the
 * device file attempts to read from it.
 */
static ssize_t device_read(struct file *file,	/* see include/linux/fs.h   */
			   char __user * buffer,	/* buffer to be
							 * filled with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
	/* 
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0;

#ifdef DEBUG
	printk(KERN_INFO "device_read(%p,%p,%d)\n", file, buffer, length);
#endif

	/* 
	 * If we're at the end of the message, return 0
	 * (which signifies end of file) 
	 */
	if (*Message_Ptr == 0)
		return 0;

	/* 
	 * Actually put the data into the buffer 
	 */
	while (length && *Message_Ptr) {

		/* 
		 * Because the buffer is in the user data segment,
		 * not the kernel data segment, assignment wouldn't
		 * work. Instead, we have to use put_user which
		 * copies data from the kernel data segment to the
		 * user data segment. 
		 */
		put_user(*(Message_Ptr++), buffer++);
		length--;
		bytes_read++;
	}

#ifdef DEBUG
	printk(KERN_INFO "Read %d bytes, %d left\n", bytes_read, length);
#endif

	/* 
	 * Read functions are supposed to return the number
	 * of bytes actually inserted into the buffer 
	 */
	return bytes_read;
}

/* 
 * This function is called when somebody tries to
 * write into our device file. 
 */
static ssize_t
device_write(struct file *file,
	     const char __user * buffer, size_t length, loff_t * offset)
{
	int i;

#ifdef DEBUG
	printk(KERN_INFO "device_write(%p,%s,%d)", file, buffer, length);
#endif

	for (i = 0; i < length && i < BUF_LEN; i++)
		get_user(Message[i], buffer + i);

	Message_Ptr = Message;

	/* 
	 * Again, return the number of input characters used 
	 */
	return i;
}

/* 
 * This function is called whenever a process tries to do an ioctl on our
 * device file. We get two extra parameters (additional to the inode and file
 * structures, which all device functions get): the number of the ioctl called
 * and the parameter given to the ioctl function.
 *
 * If the ioctl is write or read/write (meaning output is returned to the
 * calling process), the ioctl call returns the output of this function.
 *
 */
long device_ioctl(	/* removed inode */
		 struct file *file,	/* ditto */
		 unsigned int ioctl_num,	/* number and param for ioctl */
		 unsigned long ioctl_param)
{
	//int i;
	//char *temp;
	//char ch;

    unsigned char dataBit = 0;
    unsigned char clockBit = 0;
    unsigned char mcr_byte = 0;
    char* clockBitToWrite = 0;
    char* dataBitToWrite = 0;

    printk(KERN_INFO "ioctl was called num: %d, param: %lu\n", ioctl_num, ioctl_param);

	/* 
	 * Switch according to the ioctl called 
	 */
	switch (ioctl_num) {
	case IOCTL_WDM1_READ_DATABIT:
        printk(KERN_INFO "read databit was called\n");

        dataBit = inb(SER_MSR(COM1_BASEADRESS));    
		if (dataBit & 0x10)
		{
			dataBit = 1;
		}
		else
		{
			dataBit = 0;
		}
        printk(KERN_INFO "read DataBit: %d\n", dataBit);	
		return dataBit;
		break;

	case IOCTL_WDM1_READ_CLOCKBIT:
        printk(KERN_INFO "read clockbit was called\n");
		
		clockBit = inb(SER_MSR(COM1_BASEADRESS));    	
		if (clockBit & 0x20)
		{
			clockBit = 1;
		}
		else
		{
			clockBit = 0;
		}		
		printk(KERN_INFO "read clockBit: %d\n", clockBit);	
        return clockBit;        
		break;

	case IOCTL_WDM1_WRITE_DATABIT:
	    printk(KERN_INFO "write databit was called\n");
			
		if (ioctl_param != 0){
			dataBitToWrite = (char*)ioctl_param;
			printk(KERN_INFO "dataBit to write %d\n", dataBitToWrite[0]);
			mcr_byte = inb(SER_MCR(COM1_BASEADRESS));
            printk(KERN_INFO "mcr_byte: %x",mcr_byte);			
            if (dataBitToWrite[0])
			{
				mcr_byte = mcr_byte | 0x01;                
			}
			else
			{
				mcr_byte = mcr_byte & 0xFE;
			}		            
			outb(mcr_byte, SER_MCR(COM1_BASEADRESS));
		}
		else
		{
			printk(KERN_INFO "cant cast ioctl_param\n");
			return FAILURE;
		}
		break;

    case IOCTL_WDM1_WRITE_CLOCKBIT:
		printk(KERN_INFO "write clockbit was called\n");
	
		if (ioctl_param != 0){
			clockBitToWrite = (char*)ioctl_param;
			printk(KERN_INFO "clockbit to write %d\n", clockBitToWrite[0]);
            mcr_byte = inb(SER_MCR(COM1_BASEADRESS));
			if (clockBitToWrite[0])
			{
				mcr_byte = mcr_byte | 0x02;
			}
			else
			{
				mcr_byte = mcr_byte & 0xFD;
			}		
			outb(mcr_byte, SER_MCR(COM1_BASEADRESS));
		}
		else
		{
			printk(KERN_INFO "cant cast ioctl_param\n");
			return FAILURE;
		}
        break;
	}    
	return SUCCESS;
}

/* Module Declarations */

/* 
 * This structure will hold the functions to be called
 * when a process does something to the device we
 * created. Since a pointer to this structure is kept in
 * the devices table, it can't be local to
 * init_module. NULL is for unimplemented functions. 
 */
struct file_operations Fops = {
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = device_ioctl,
	.open = device_open,
	.release = device_release,	/* a.k.a. close */
};

/* 
 * Initialize the module - Register the character device 
 */
int init_module()
{
	int ret_val;
	/* 
	 * Register the character device (atleast try) 
	 */
	ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);

	/* 
	 * Negative values signify an error 
	 */
	if (ret_val < 0) {
		printk(KERN_ALERT "%s failed with %d\n",
		       "Sorry, registering the character device ", ret_val);
		return ret_val;
	}

	printk(KERN_INFO "%s The major device number is %d.\n",
	       "Registeration is a success", MAJOR_NUM);
	printk(KERN_INFO "If you want to talk to the device driver,\n");
	printk(KERN_INFO "you'll have to create a device file. \n");
	printk(KERN_INFO "We suggest you use:\n");
	printk(KERN_INFO "mknod %s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM);
	printk(KERN_INFO "The device file name is important, because\n");
	printk(KERN_INFO "the ioctl program assumes that's the\n");
	printk(KERN_INFO "file you'll use.\n");
    printk(KERN_INFO "IOCTL_WDM1_READ_DATABIT %d.\n", IOCTL_WDM1_READ_DATABIT);
    printk(KERN_INFO "IOCTL_WDM1_READ_CLOCKBIT %d.\n", IOCTL_WDM1_READ_CLOCKBIT);
    printk(KERN_INFO "IOCTL_WDM1_WRITE_DATABIT %d.\n", IOCTL_WDM1_WRITE_DATABIT);
    printk(KERN_INFO "IOCTL_WDM1_WRITE_CLOCKBIT %d.\n", IOCTL_WDM1_WRITE_CLOCKBIT);

	return 0;
}

/* 
 * Cleanup - unregister the appropriate file from /proc 
 */
void cleanup_module()
{
  //int ret;

	/* 
	 * Unregister the device 
	 */
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);

	/* 
	 * If there's an error, report it 
	 */
	//if (ret < 0)
	//	printk(KERN_ALERT "Error: unregister_chrdev: %d\n", ret);
}
