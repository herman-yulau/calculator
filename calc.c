#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> 
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <asm/uaccess.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/parport.h>
#include <linux/pci.h>

#define PROC 1
#define SYS 0
/* Choose required filesystem by setting FILESYSTEM const to the appropriate value from above */
#define FILESYSTEM SYS
#define NAME_DIR  "calc"
#define NAME_FILE1 "fnum"
#define NAME_FILE2 "snum"
#define NAME_FILE3 "operation"
#define NAME_FILE4 "result"
#define LEN 4                 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Herman");
MODULE_DESCRIPTION("A Simple calculator");

static char oper[LEN], number1[LEN], number2[LEN], an[LEN];
static int result;

/* This two functions are for both filesystems */

int calculate(char* str, int num1, int num2)
{	/* do calculations */
	if (!strcmp(str, "sum"))				
		return num1 + num2;
	else if (!strcmp(str, "sub"))
		return num1 - num2;
	else if (!strcmp(str, "mul"))
		return num1 * num2;
	else if (!strcmp(str, "div"))
		return num1 / num2;
	else return 1;
}

/* ----------------------------------------------------------------------------------------------------------- */
/* Declarations for filesystem /sys */
#if FILESYSTEM == SYS

static struct class *calc;
static ssize_t num1_show(struct class *class, struct class_attribute *attr, char *buf) 
{	/* reading first number */
	strcpy(buf, number1);
   	printk(KERN_INFO "Number 1: %s\n", number1);
   	return strlen(buf);
}

static ssize_t num1_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) 
{	/* write first number */
   	strncpy(number1, buf, count);
   	number1[count] = '\0';
	printk(KERN_INFO "Number 1 = %s is stored\n", number1);
   	return count;
}

static ssize_t num2_show(struct class *class, struct class_attribute *attr, char *buf) 
{	/* reading second number */
   	strcpy(buf, number2);
   	printk(KERN_INFO "Number 2: %s\n", number2);
   	return strlen(buf);
}

static ssize_t num2_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) 
{	/* write first number */
   	strncpy(number2, buf, count);
   	number2[count] = '\0';
	printk(KERN_INFO "Number 2 = %s is stored\n", number2);
   	return count;
}

static ssize_t oper_show(struct class *class, struct class_attribute *attr, char *buf) 
{	/* reading operation */
   	strcpy(buf, oper);
   	printk(KERN_INFO "Operation: %s\n", oper);
   	return strlen(buf);
}

static ssize_t oper_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) 
{	/* write operation */
   	int num1, num2;
   	strncpy(oper, buf, count);
   	if ('\n' == oper[count - 1]) 
		oper[count - 1] = '\0';
	else 
		oper[count] = '\0';
	printk(KERN_INFO "Operation = %s is stored\n", oper);
	kstrtoint(number1, 10, &num1);
	kstrtoint(number2, 10, &num2);
	result = calculate(oper, num1, num2);
   	return count;
}

static ssize_t res_show(struct class *class, struct class_attribute *attr, char *buf) 
{	/* reading result */
	snprintf(an, LEN, "%d", result);
   	strcpy(buf, an);
	printk(KERN_INFO "Result is %s\n", an);
   	return strlen(buf);
}

static ssize_t res_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) 
{	/* writing result is  not supported */
	printk(KERN_INFO "User can't write in result\n");
   	return 0;
}

CLASS_ATTR(fnum, 0770, &num1_show, &num1_store);
CLASS_ATTR(snum, 0770, &num2_show, &num2_store);
CLASS_ATTR(operation, 0770, &oper_show, &oper_store);
CLASS_ATTR(result, 0770, &res_show, &res_store);

#endif

/* ----------------------------------------------------------------------------------------------------------- */
/* Declarations for filesystem /proc */
#if FILESYSTEM == PROC

struct proc_dir_entry *new_dir;
static ssize_t num1_read(struct file *file, char *buf, size_t count, loff_t *ppos) 
{	/* reading first number */
	static int endfile = 0;
	if (!endfile) {
		int read_bytes = copy_to_user((void*)buf, &number1, strlen(number1));
		put_user('\n', buf + strlen(number1));   
		read_bytes = strlen( number1 ) + 1;
		printk(KERN_INFO "Number 1: %s\n", number1);
		endfile = 1;
		return read_bytes;
	}
	endfile = 0;
	return 0;
}

static ssize_t num1_write(struct file *file, const char *buf, size_t count, loff_t *ppos) 
{	/* write first number */
	int res, len = count < LEN ? count : LEN;
	res = copy_from_user(&number1, (void*)buf, count);
	if ('\n' == number1[len - 1]) 
		number1[len - 1] = '\0';
	else 
		number1[len] = '\0';
	printk(KERN_INFO "Number 1 = %s is stored\n", number1);
	return len;
}

static ssize_t num2_read(struct file *file, char *buf, size_t count, loff_t *ppos) 
{	/* reading second number */
	static int endfile = 0;
	if (!endfile) {
		int read_bytes = copy_to_user((void*)buf, &number2, strlen(number2));
		put_user('\n', buf + strlen(number2));  
		read_bytes = strlen(number2) + 1;
		printk(KERN_INFO "Number 2: %s\n", number2);
		endfile = 1;
		return read_bytes;
	}
	endfile = 0;
	return 0;
}

static ssize_t num2_write(struct file *file, const char *buf, size_t count, loff_t *ppos) 
{	/* write second number */
	int res, len = count < LEN ? count : LEN;
	res = copy_from_user(&number2, (void*)buf, count);
	if ('\n' == number2[len - 1]) 
		number2[len - 1] = '\0';
	else 
		number2[len] = '\0';
	printk(KERN_INFO "Number 2 = %s is stored\n", number2);
	return len;
}

static ssize_t oper_read(struct file *file, char *buf, size_t count, loff_t *ppos) 
{	/* reading operation */
	static int endfile = 0;
	if (!endfile) {
		int read_bytes = copy_to_user((void*)buf, &oper, strlen(oper));
		read_bytes = strlen(oper);
		printk(KERN_INFO "Operation: %s\n", oper);
		endfile = 1;
		return read_bytes;
	}
	endfile = 0;
	return 0;
}

static ssize_t oper_write(struct file *file, const char *buf, size_t count, loff_t *ppos) 
{	/* write operation */
	int res, len = count < LEN ? count : LEN;
	int num1, num2;
	kstrtoint(number1, 10, &num1);
	kstrtoint(number2, 10, &num2);
	res = copy_from_user(&oper, (void*)buf, count);
	if ( '\n' == oper[len - 1]) 
		oper[len - 1] = '\0';
	else 
		oper[len] = '\0';
	printk(KERN_INFO "Operation = %s is stored\n", oper);
	result = calculate(oper, num1, num2);
	return len;
}

static ssize_t res_read(struct file *file, char *buf, size_t count, loff_t *ppos) 
{	/* reading result */
	static int endfile = 0; 
	int read_bytes;
	if (!endfile) {
		snprintf(an, LEN, "%d", result);
		read_bytes = copy_to_user((void*)buf, &an, strlen(an));
		put_user('\n', buf + strlen(an));   
		read_bytes = strlen(an) + 1;
		printk(KERN_INFO "Result: %d\n", result);
		endfile = 1;
		return read_bytes;
	}
	endfile = 0;
	return 0;
}

static const struct file_operations for1file = {	/* operations for every file */
	   .owner = THIS_MODULE,
	   .read = num1_read,
	   .write = num1_write
	};

static const struct file_operations for2file = {
	   .owner = THIS_MODULE,
	   .read = num2_read,
	   .write = num2_write
	};

static const struct file_operations for3file = {
	   .owner = THIS_MODULE,
	   .read = oper_read,
	   .write = oper_write
	};

static const struct file_operations for4file = {
	   .owner = THIS_MODULE,
	   .read = res_read
	};
#endif

/* ----------------------------------------------------------------------------------------------------------- */

static int __init proc_init( void ) {
	
/* ----------------------------------------------------------------------------------------------------------- */
	/* Initializations for filesystem /proc */
	#if FILESYSTEM == PROC
	int ret; 
	struct proc_dir_entry *fnum;
	struct proc_dir_entry *snum;
	struct proc_dir_entry *opr;
	struct proc_dir_entry *res;
	
	new_dir = proc_mkdir(NAME_DIR, NULL); 		/* creating dir "calc" */
	if (NULL == new_dir) { 
		ret = -ENOMEM; 
		printk(KERN_ERR "Can't create /proc/%s\n", NAME_DIR); 
		goto err_dir; 
	} 
	fnum = proc_create(NAME_FILE1, 0, new_dir, &for1file); 	/* creating all files */
	if (NULL == fnum) { 
	      	ret = -ENOMEM; 
	      	printk(KERN_ERR "can't create /proc/%s/%s\n", NAME_DIR, NAME_FILE1); 
	      	goto err_file1; 
	}
	
	snum = proc_create(NAME_FILE2, 0, new_dir, &for2file); 
	if (NULL == snum) { 
	      	ret = -ENOMEM; 
	      	printk(KERN_ERR "can't create /proc/%s/%s\n", NAME_DIR, NAME_FILE2); 
	      	goto err_file2; 
	} 
	
	opr = proc_create(NAME_FILE3, 0, new_dir, &for3file); 
	if (NULL == opr) { 
	      	ret = -ENOMEM; 
	      	printk(KERN_ERR "can't create /proc/%s/%s\n", NAME_DIR, NAME_FILE3); 
	      	goto err_file3; 
	}
	
	res = proc_create(NAME_FILE4, 0, new_dir, &for4file); 
	if (NULL == res) { 
	      	ret = -ENOMEM; 
	      	printk(KERN_ERR "can't create /proc/%s/%s\n", NAME_DIR, NAME_FILE4); 
	      	goto err_file4; 
	} 
	printk( KERN_INFO "Module was loaded successfully!\n"); 
	return 0; 
	
	err_file1: 
		remove_proc_entry(NAME_FILE1, new_dir);
		return ret;
	err_file2: 
		remove_proc_entry(NAME_FILE2, new_dir);
		return ret;
	err_file3: 
		remove_proc_entry(NAME_FILE3, new_dir); 
		return ret;
	err_file4: 
		remove_proc_entry(NAME_FILE4, new_dir); 
		return ret;
	err_dir: 
		remove_proc_entry(NAME_DIR, NULL);
		return ret;
	#endif
	
/* ----------------------------------------------------------------------------------------------------------- */
	/* Initializations for filesystem /sys */
	#if FILESYSTEM == SYS
   	calc = class_create(THIS_MODULE, NAME_DIR);
   	if (IS_ERR(calc)) 
		printk("bad class create\n");
   	class_create_file(calc, &class_attr_fnum);
	class_create_file(calc, &class_attr_snum);
	class_create_file(calc, &class_attr_operation);
	class_create_file(calc, &class_attr_result);
   	printk( "Module was loaded succesfully\n" );
   	return 0;
	#endif
/* ----------------------------------------------------------------------------------------------------------- */
}

static void __exit proc_exit(void) {
/* ----------------------------------------------------------------------------------------------------------- */
	#if FILESYSTEM == PROC
	remove_proc_entry(NAME_FILE1, new_dir);
	remove_proc_entry(NAME_FILE2, new_dir);
	remove_proc_entry(NAME_FILE3, new_dir);
	remove_proc_entry(NAME_FILE4, new_dir);
	remove_proc_entry(NAME_DIR, NULL);
	printk(KERN_INFO "/proc/%s/%s removed\n", NAME_DIR, NAME_FILE1);
	printk(KERN_INFO "/proc/%s/%s removed\n", NAME_DIR, NAME_FILE2);
	printk(KERN_INFO "/proc/%s/%s removed\n", NAME_DIR, NAME_FILE3);
	printk(KERN_INFO "/proc/%s/%s removed\n", NAME_DIR, NAME_FILE4);
	printk(KERN_INFO "/proc/%s removed\n", NAME_DIR);
	#endif
/* ----------------------------------------------------------------------------------------------------------- */
	#if FILESYSTEM == SYS
   	class_remove_file(calc, &class_attr_fnum);
   	class_destroy(calc);
	printk(KERN_INFO "/sys/class/%s/%s removed\n", NAME_DIR, NAME_FILE1);
	printk(KERN_INFO "/sys/class/%s/%s removed\n", NAME_DIR, NAME_FILE2);
	printk(KERN_INFO "/sys/class/%s/%s removed\n", NAME_DIR, NAME_FILE3);
	printk(KERN_INFO "/sys/class/%s/%s removed\n", NAME_DIR, NAME_FILE4);
	printk(KERN_INFO "/sys/class/%s removed\n", NAME_DIR);
	#endif
/* ----------------------------------------------------------------------------------------------------------- */
}

module_init(proc_init);
module_exit(proc_exit);