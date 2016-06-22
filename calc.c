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

static char oper[LEN], number1[LEN], number2[LEN], an[LEN+1];
static long int result;
struct proc_dir_entry *new_dir;

int calculate(char* str, int num1, int num2)
{
	
	if (!strcmp(str, "sum"))				/* and simultaneously do calculations */
	{printk(KERN_INFO "oper: %s\n", str); return num1 + num2;}
	else
		if (!strcmp(str, "sub"))
			return num1 - num2;
		else
			if (!strcmp(str, "mul"))
				return num1 * num2;
			else
				if (!strcmp(str, "div"))
					return num1 / num2;
				else
					return 1;
}

int tostr(int num, char* str)
{
	int sign = 0, i; 
	if (num < 0) {
		sign = 1;
		num *= -1;
		str[0] = '-';
	}
	if (num > 99) {
		i = LEN - 1 + sign;
		str[LEN] = '\0';
	}
	else
		if (num >= 10) {
			i = LEN - 2 + sign;
			str[i] = '\0';
		}
		else {
			i = LEN - 3 + sign;
			str[i] = '\0';
		}
	for (--i; i - sign >= 0; --i) {
		str[i] = '0' + num % 10;
		num /= 10;
	}
	return 0;
}

#if FILESYSTEM == SYS
static struct class *calc;
static ssize_t num1_show(struct class *class, struct class_attribute *attr, char *buf) 
{
	strcpy(buf, number1);
   	printk(KERN_INFO "read %d\n", strlen(buf));
   	return strlen(buf);
}

static ssize_t num1_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) 
{
   	printk(KERN_INFO "write %d\n", count);
   	strncpy(number1, buf, count);
   	number1[count] = '\0';
   	return count;
}

static ssize_t num2_show(struct class *class, struct class_attribute *attr, char *buf) 
{
   	strcpy(buf, number2);
   	printk(KERN_INFO "read %d\n", strlen(buf));
   	return strlen(buf);
}

static ssize_t num2_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) 
{
   	printk(KERN_INFO "write %d\n", count);
   	strncpy(number2, buf, count);
   	number2[count] = '\0';
   	return count;
}

static ssize_t oper_show(struct class *class, struct class_attribute *attr, char *buf) 
{
   	strcpy(buf, oper);
   	printk(KERN_INFO "read %d\n", strlen(buf));
   	return strlen(buf);
}

static ssize_t oper_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) 
{
   	int num1, num2;
	printk(KERN_INFO "write %d\n", count);
   	strncpy(oper, buf, count);
   	if ('\n' == oper[count - 1]) 
		oper[count - 1] = '\0';
	else 
		oper[count] = '\0';
	kstrtoint(number1, 10, &num1);
	kstrtoint(number2, 10, &num2);
	printk(KERN_INFO "Num1: %d, Num2: %d\n", num1, num2);
	result = calculate(oper, num1, num2);
   	return count;
}

static ssize_t res_show(struct class *class, struct class_attribute *attr, char *buf) 
{	
	tostr(result, an);
   	strcpy(buf, an);
   	printk(KERN_INFO "read %d\n", strlen(buf));
   	return strlen(buf);
}

static ssize_t res_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) 
{
	printk(KERN_INFO "User can't write in result\n");
   	return 0;
}

CLASS_ATTR(fnum, 0660, &num1_show, &num1_store);
CLASS_ATTR(snum, 0660, &num2_show, &num2_store);
CLASS_ATTR(operation, 0660, &oper_show, &oper_store);
CLASS_ATTR(result, 0660, &res_show, &res_store);

#endif

#if FILESYSTEM == PROC
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
	return len;
}

static ssize_t oper_read(struct file *file, char *buf, size_t count, loff_t *ppos) 
{	/* reading operation */
	static int endfile = 0;
	if (!endfile) {
		int read_bytes = copy_to_user((void*)buf, &oper, strlen(oper));
		put_user('\n', buf + strlen(oper));   
		read_bytes = strlen(oper) + 1;
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
	result = calculate(oper, num1, num2);
	return len;
}

static ssize_t res_read(struct file *file, char *buf, size_t count, loff_t *ppos) 
{	/* reading result */
	static int endfile = 0; 
	int read_bytes;
	if (!endfile) {
		tostr(result, an);
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

static int __init proc_init( void ) {
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
	
	#if FILESYSTEM == SYS
	int res;
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
}

static void __exit proc_exit(void) {
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
	#if FILESYSTEM == SYS
   	class_remove_file(calc, &class_attr_fnum);
   	class_destroy(calc);
	printk(KERN_INFO "/sys/class/%s/%s removed\n", NAME_DIR, NAME_FILE1);
	printk(KERN_INFO "/sys/class/%s/%s removed\n", NAME_DIR, NAME_FILE2);
	printk(KERN_INFO "/sys/class/%s/%s removed\n", NAME_DIR, NAME_FILE3);
	printk(KERN_INFO "/sys/class/%s/%s removed\n", NAME_DIR, NAME_FILE4);
	printk(KERN_INFO "/sys/class/%s removed\n", NAME_DIR);
	#endif
}

module_init(proc_init);
module_exit(proc_exit);