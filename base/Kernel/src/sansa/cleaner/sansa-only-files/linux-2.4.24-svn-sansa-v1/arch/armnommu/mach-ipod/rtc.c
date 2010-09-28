/*
 * rtc.c - RTC driver for Sansa
 *
 * Copyright (c) 2008 Sebastian Duell (Sebastian.Duell@iPodLinux.org)
 *
 * Routines mostly from Rockbox and iPodLinux
 */

#include <linux/module.h>
#include <linux/config.h>
#include <linux/errno.h>
#include <linux/major.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/rtc.h>
#include <linux/init.h>
#include <linux/delay.h>

#include <linux/devfs_fs_kernel.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include <asm/hardware.h>

#include "as3514.h"

#define MINUTE_SECONDS      60
#define HOUR_SECONDS        3600
#define DAY_SECONDS         86400
#define WEEK_SECONDS        604800
#define YEAR_SECONDS        31536000
#define LEAP_YEAR_SECONDS   31622400

static devfs_handle_t rtc_devfs_handle;
static int pcf_rtc_isopen;
/* Days in each month */
static unsigned int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

extern int i2c_readbyte(unsigned int dev_addr, int addr);

static inline bool is_leapyear(int year)
{
   if( ((year%4)==0) && (((year%100)!=0) || ((year%400)==0)) )
      return true;
   else
      return false;
}

static void as_rtc_read(struct rtc_time *tm)
{
    char tmp[4];
    int year;
    int i;
    unsigned int seconds;
    
    ipod_i2c_lock();
    /* RTC_AS3514's slave address is 0x46*/
    for (i=0;i<4;i++){
        tmp[i] = i2c_readbyte(AS3514_I2C_ADDR, RTC_0 + i);
    }
    ipod_i2c_unlock();
    seconds = tmp[0] + (tmp[1]<<8) + (tmp[2]<<16) + (tmp[3]<<24);
    
    /* Convert seconds since Jan-1-1980 */
      
    /* Year */
    year = 80;
    while(seconds>=LEAP_YEAR_SECONDS)
    {
        if(is_leapyear(year)){
            seconds -= LEAP_YEAR_SECONDS;
        } else {
            seconds -= YEAR_SECONDS;
        }

        year++;
    }
    
    if(is_leapyear(year)) {
        days_in_month[1] = 29;
    } else {
        days_in_month[1] = 28;
        if(seconds>YEAR_SECONDS){
            year++;
            seconds -= YEAR_SECONDS;
        }
    }
    tm->tm_year = year;
    
    /* Month */
    for(i=0; i<12; i++)
    {
        if(seconds < days_in_month[i]*DAY_SECONDS){
            tm->tm_mon = i;
            break;
        }
        
        seconds -= days_in_month[i]*DAY_SECONDS;
    }
    
    /* Month Day */
    tm->tm_mday = seconds/DAY_SECONDS;
    seconds -= tm->tm_mday*DAY_SECONDS;

    /* Hour */
    tm->tm_hour = seconds/HOUR_SECONDS;
    seconds -= tm->tm_hour*HOUR_SECONDS;
    
    /* Minute */
    tm->tm_min = seconds/MINUTE_SECONDS;
    seconds -= tm->tm_min*MINUTE_SECONDS;
    
    /* Second */
    tm->tm_sec = seconds;
}

static int as_rtc_open(struct inode *inode, struct file *file)
{
	if (pcf_rtc_isopen) {
		return -EBUSY;
	}

	pcf_rtc_isopen = 1;
	return 0;
}

static int as_rtc_close(struct inode *inode, struct file *file)
{
	pcf_rtc_isopen = 0;

	return 0;
}

static int as_rtc_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct rtc_time now;

	switch (cmd) {
	case RTC_RD_TIME:
		memset(&now, 0x0, sizeof(now));
		as_rtc_read(&now);
		return copy_to_user((void *)arg, &now, sizeof(now)) ? -EFAULT : 0;

	// case RTC_SET_TIME:
	}

	return -EINVAL;
}

static struct file_operations as_rtc_fops = {
	owner: THIS_MODULE,
	open: as_rtc_open,
	release: as_rtc_close,
	ioctl: as_rtc_ioctl,
};


static int __init as3514_init(void)
{
   ipod_i2c_init();

	rtc_devfs_handle = devfs_register(NULL, "rtc", DEVFS_FL_DEFAULT,
		MISC_MAJOR, RTC_MINOR,
		S_IFCHR | S_IWUSR | S_IRUSR,
		&as_rtc_fops, NULL);
        if (rtc_devfs_handle < 0) {
                printk(KERN_WARNING "rtc: failed to register major %d, minor %d\n",
                        MISC_MAJOR, RTC_MINOR);
                return 0;
        }

	return 0;
}

static void __exit as3514_exit(void)
{
	devfs_unregister_chrdev(MISC_MAJOR, "rtc");
	devfs_unregister(rtc_devfs_handle);
}

module_init(as3514_init);
module_exit(as3514_exit);

MODULE_AUTHOR("Sebastian Duell (Sebastian.Duell@iPodLinux.org)");
MODULE_DESCRIPTION("RTC driver for as3514");
MODULE_LICENSE("GPL");

