#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> 
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <asm/uaccess.h>	
	
#define NAME_DIR  "calc"
#define NAME_NODE1 "fnum"
#define NAME_NODE2 "snum"
#define NAME_NODE3 "operation"
#define NAME_NODE4 "result"
#define LEN 10                       // длина буфера и сам буфер обмена
static char oper[LEN], number1[LEN], number2[LEN];
static int result;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Herman");
MODULE_DESCRIPTION("A Simple calculator");
struct proc_dir_entry *new_dir;

static ssize_t num1_read(struct file *file, char *buf, size_t count, loff_t *ppos) {
	static int endfile = 0;
	if(!endfile) {
		int res = copy_to_user((void*)buf, &number1, strlen(number1));
	      	endfile = 1;
	      	printk(KERN_INFO "Number 1: %s\n", number1);
	      	return res;
	}
	endfile = 0;
	printk(KERN_INFO "EOF\n" );
	return 0;
}

static ssize_t num1_write(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	int res, len = count < LEN ? count : LEN;
	res = copy_from_user(&number1, (void*)buf, count);
	if ( '\n' == number1[len - 1]) 
		number1[len - 1] = '\0';
	else 
		number1[len] = '\0';
	return len;
}

static ssize_t num2_read(struct file *file, char *buf, size_t count, loff_t *ppos) {
	static int endfile = 0;
	if(!endfile) {
		int res = copy_to_user((void*)buf, &number2, strlen(number2));
	      	endfile = 1;
	      	printk(KERN_INFO "Number 2: %s\n", number2);
	      	return res;
	}
	endfile = 0;
	printk(KERN_INFO "EOF\n" );
	return 0;
}

static ssize_t num2_write(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	int res, len = count < LEN ? count : LEN;
	res = copy_from_user(&number2, (void*)buf, count);
	if ( '\n' == number1[len - 1]) 
		number1[len - 1] = '\0';
	else 
		number1[len] = '\0';
	return len;
}

static ssize_t oper_read(struct file *file, char *buf, size_t count, loff_t *ppos) {
	static int endfile = 0;
	if(!endfile) {
		int res = copy_to_user((void*)buf, &oper, strlen(oper));
	      	endfile = 1;
	      	printk(KERN_INFO "Operation: %s\n", oper);
	      	return res;
	}
	endfile = 0;
	printk(KERN_INFO "EOF\n" );
	return 0;
}

static ssize_t oper_write(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	int res, len = count < LEN ? count : LEN;
	long int num1, num2;
	kstrtol(number1, 10, &num1);
	kstrtol(number2, 10, &num2);
	res = copy_from_user(&oper, (void*)buf, count);
	if ( '\n' == oper[len - 1]) 
		oper[len - 1] = '\0';
	else 
		oper[len] = '\0';
	
	if (!strcmp(oper, "sum")){
		result = num1 + num2; printk(KERN_INFO "asa");}
	else
		if (!strcmp(oper, "sub"))
			result = num1 - num2;
		else
			if (!strcmp(oper, "mul"))
				result = num1 * num2;
			else
				if (!strcmp(oper, "div"))
					result = num1 / num2;
	return len;
}

static ssize_t res_read(struct file *file, char *buf, size_t count, loff_t *ppos) {
	static int endfile = 0;
	if(!endfile) {
		int res = copy_to_user((void*)buf, &result, sizeof(result));
	      	endfile = 1;
	      	printk(KERN_INFO "Result: %d\n", result);
	      	return res;
	}
	endfile = 0;
	printk(KERN_INFO "EOF\n" );
	return 0;
}

static const struct file_operations for1file = {
	   .owner = THIS_MODULE,
	   .read  = num1_read,
	   .write  = num1_write
	};
static const struct file_operations for2file = {
	   .owner = THIS_MODULE,
	   .read  = num2_read,
	   .write  = num2_write
	};
static const struct file_operations for3file = {
	   .owner = THIS_MODULE,
	   .read  = oper_read,
	   .write  = oper_write
	};
static const struct file_operations for4file = {
	   .owner = THIS_MODULE,
	   .read  = res_read
	};

static int __init proc_init( void ) { 
	int ret; 
	struct proc_dir_entry *fnum;
	struct proc_dir_entry *snum;
	struct proc_dir_entry *opr;
	struct proc_dir_entry *res;
	
	new_dir = proc_mkdir(NAME_DIR, NULL); 
	if (NULL == new_dir) { 
		ret = -ENOMEM; 
		printk(KERN_ERR "Can't create /proc/%s\n", NAME_DIR); 
		goto err_dir; 
	} 
	fnum = proc_create(NAME_NODE1, 0, new_dir, &for1file); 
	if (NULL == fnum) { 
	      	ret = -ENOMEM; 
	      	printk(KERN_ERR "can't create /proc/%s/%s\n", NAME_DIR, NAME_NODE1); 
	      	goto err_node; 
	}
	
	snum = proc_create(NAME_NODE2, 0, new_dir, &for2file); 
	if (NULL == snum) { 
	      	ret = -ENOMEM; 
	      	printk(KERN_ERR "can't create /proc/%s/%s\n", NAME_DIR, NAME_NODE2); 
	      	goto err_node; 
	} 
	
	opr = proc_create(NAME_NODE3, 0, new_dir, &for3file); 
	if (NULL == opr) { 
	      	ret = -ENOMEM; 
	      	printk(KERN_ERR "can't create /proc/%s/%s\n", NAME_DIR, NAME_NODE3); 
	      	goto err_node; 
	}
	
	res = proc_create(NAME_NODE4, 0, new_dir, &for4file); 
	if (NULL == res) { 
	      	ret = -ENOMEM; 
	      	printk(KERN_ERR "can't create /proc/%s/%s\n", NAME_DIR, NAME_NODE4); 
	      	goto err_node; 
	} 
	printk( KERN_INFO "Module was loaded successfully!\n"); 
	return 0; 
	err_node: 
		remove_proc_entry(NAME_DIR, NULL); 
	err_dir: 
		return ret; 
}

static void __exit proc_exit(void) {
	remove_proc_entry( NAME_NODE1, new_dir );
	remove_proc_entry( NAME_NODE2, new_dir );
	remove_proc_entry( NAME_NODE3, new_dir );
	remove_proc_entry( NAME_NODE4, new_dir );
	remove_proc_entry( NAME_DIR, NULL );
	printk(KERN_INFO "/proc/%s/%s removed\n", NAME_DIR, NAME_NODE1);
	printk(KERN_INFO "/proc/%s/%s removed\n", NAME_DIR, NAME_NODE2);
	printk(KERN_INFO "/proc/%s/%s removed\n", NAME_DIR, NAME_NODE3);
	printk(KERN_INFO "/proc/%s/%s removed\n", NAME_DIR, NAME_NODE4);
	printk(KERN_INFO "/proc/%s removed\n", NAME_DIR);
}

module_init(proc_init);
module_exit(proc_exit);