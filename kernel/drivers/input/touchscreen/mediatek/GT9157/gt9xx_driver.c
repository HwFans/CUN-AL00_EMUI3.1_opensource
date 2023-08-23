/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *  
 * MediaTek Inc. (C) 2012. All rights reserved. 
 *  
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 *
 * Version: V2.5
 * Release Date: 2015/01/21
 */

#include "tpd.h"
#include "tpd_custom_gt9xx.h"

#ifndef TPD_NO_GPIO
#include "cust_gpio_usage.h" 
#endif
#ifdef TPD_PROXIMITY
#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>
#endif

#include <linux/proc_fs.h>  /*proc*/
#include <linux/fs.h>


#if GTP_SUPPORT_I2C_DMA
    #include <linux/dma-mapping.h>
#endif
#include <mach/hardwareinfo.h>
extern hardware_info_struct hardware_info;

/******************/
#ifdef I2C_PUSHPULL_FLAG
#undef I2C_PUSHPULL_FLAG
#define I2C_PUSHPULL_FLAG	(0x0100)    /** modify 64bit dma  yudengwu 1218 **/
#endif

#define KEY_SCHANGED 0x21c  
#define KEY_SCCHANGED 0x21d 
#define KEY_SMCHANGED 0x220     
#define KEY_SWCHANGED 0x222
#define KEY_SCCCHANGED 0x228



/*****************/


extern struct tpd_device *tpd;
extern u8 gtp_loading_fw;

static int gt9xx_version;
static int tpd_flag = 0; 
int tpd_halt = 0;
static struct task_struct *thread = NULL;
static DECLARE_WAIT_QUEUE_HEAD(waiter);




#ifdef TPD_HAVE_BUTTON
static int tpd_keys_local[TPD_KEY_COUNT] = TPD_KEYS;
static int tpd_keys_dim_local[TPD_KEY_COUNT][4] = TPD_KEYS_DIM;
#endif

#if GTP_GESTURE_WAKEUP
#ifdef  GTP_HUAWEI_DESIGN
int gesture_echo[20]={0};
char gesture_char_echo[5][12];

#define GT9XX_ECHO_PROC_FILE     "gesture_echo"

static ssize_t gt9xx_echo_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos);
static ssize_t gt9xx_echo_write_proc(struct file *, const char __user *, size_t, loff_t *);
/*
static struct proc_dir_entry *gt9xx_echo_proc = NULL;

static const struct file_operations configecho_proc_ops = {
    .owner = THIS_MODULE,
    .read = gt9xx_echo_read_proc,
    .write = gt9xx_echo_write_proc,
};
*/
#endif
typedef enum
{
    DOZE_DISABLED = 0,
    DOZE_ENABLED = 1,
    DOZE_WAKEUP = 2,
}DOZE_T;
static DOZE_T doze_status = DOZE_DISABLED;
static s8 gtp_enter_doze(struct i2c_client *client);
static int g_wakeup_gesture = 0;
static long huawei_gesture = 0;
static int double_gesture = 0;
static int draw_gesture = 0;
#endif

#if GTP_CHARGER_SWITCH
    #ifdef MT6573
        #define CHR_CON0      (0xF7000000+0x2FA00)
    #else
        extern kal_bool upmu_is_chr_det(void);
    #endif
    static void gtp_charger_switch(s32 dir_update);
#endif 

#if GTP_HAVE_TOUCH_KEY
const u16 touch_key_array[] = GTP_KEY_TAB;
#define GTP_MAX_KEY_NUM ( sizeof( touch_key_array )/sizeof( touch_key_array[0] ) )
#endif

#if (defined(TPD_WARP_START) && defined(TPD_WARP_END))
static int tpd_wb_start_local[TPD_WARP_CNT] = TPD_WARP_START;
static int tpd_wb_end_local[TPD_WARP_CNT]   = TPD_WARP_END;
#endif

#if (defined(TPD_HAVE_CALIBRATION) && !defined(TPD_CUSTOM_CALIBRATION))
//static int tpd_calmat_local[8]     = TPD_CALIBRATION_MATRIX;
static int tpd_def_calmat_local[8] = TPD_CALIBRATION_MATRIX;
#endif

#if GTP_SUPPORT_I2C_DMA
s32 i2c_dma_write(struct i2c_client *client, u16 addr, u8 *txbuf, s32 len);
s32 i2c_dma_read(struct i2c_client *client, u16 addr, u8 *rxbuf, s32 len);

static u8 *gpDMABuf_va = NULL;
static dma_addr_t *gpDMABuf_pa = 0;
#endif

s32 gtp_send_cfg(struct i2c_client *client);
void gtp_reset_guitar(struct i2c_client *client, s32 ms);
static void tpd_eint_interrupt_handler(void);
static int touch_event_handler(void *unused);
static int tpd_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int tpd_i2c_detect(struct i2c_client *client, struct i2c_board_info *info);
static int tpd_i2c_remove(struct i2c_client *client);
s32 gtp_i2c_read_dbl_check(struct i2c_client *client, u16 addr, u8 *rxbuf, int len);
extern void mt_eint_unmask(unsigned int line);
extern void mt_eint_mask(unsigned int line);

#ifndef MT6572
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);
#endif

#if GTP_CREATE_WR_NODE
extern s32 init_wr_node(struct i2c_client *);
extern void uninit_wr_node(void);
#endif

#if (GTP_ESD_PROTECT || GTP_COMPATIBLE_MODE)
static void force_reset_guitar(void);
#endif

#if GTP_ESD_PROTECT
static int clk_tick_cnt = 200;
static struct delayed_work gtp_esd_check_work;
static struct workqueue_struct *gtp_esd_check_workqueue = NULL;
static s32 gtp_init_ext_watchdog(struct i2c_client *client);
static void gtp_esd_check_func(struct work_struct *);
void gtp_esd_switch(struct i2c_client *client, s32 on);
u8 esd_running = 0;
spinlock_t esd_lock;
#endif


#ifdef TPD_PROXIMITY
#define TPD_PROXIMITY_VALID_REG                   0x814E
#define TPD_PROXIMITY_ENABLE_REG                  0x8042
static u8 tpd_proximity_flag = 0;
static u8 tpd_proximity_detect = 1;//0-->close ; 1--> far away
#endif

struct i2c_client *i2c_client_point = NULL;
static const struct i2c_device_id tpd_i2c_id[] = {{"GT915L", 0}, {}};
static unsigned short force[] = {0, 0xBA, I2C_CLIENT_END, I2C_CLIENT_END};
static const unsigned short *const forces[] = { force, NULL };
//static struct i2c_client_address_data addr_data = { .forces = forces,};
static struct i2c_board_info __initdata i2c_tpd = { I2C_BOARD_INFO("GT915L", (0xBA >> 1))};
static struct i2c_driver tpd_i2c_driver =
{
    .probe = tpd_i2c_probe,
    .remove = tpd_i2c_remove,
    .detect = tpd_i2c_detect,
    .driver.name = "gt9xx",
    .id_table = tpd_i2c_id,
    .address_list = (const unsigned short *) forces,
};

static u8 config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
    = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};
#if GTP_CHARGER_SWITCH
static u8 gtp_charger_config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
	= {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};
#endif

#pragma pack(1)
typedef struct
{
    u16 pid;                 //product id   //
    u16 vid;                 //version id   //
} st_tpd_info;
#pragma pack()

st_tpd_info tpd_info;
u8 int_type = 0;
u32 abs_x_max = 0;
u32 abs_y_max = 0;
u8 gtp_rawdiff_mode = 0;
u8 cfg_len = 0;
u8 pnl_init_error = 0;

#if GTP_WITH_PEN
struct input_dev *pen_dev;
#endif

#if GTP_COMPATIBLE_MODE
u8 driver_num = 0;
u8 sensor_num = 0;
u8 gtp_ref_retries = 0;
u8 gtp_clk_retries = 0;
CHIP_TYPE_T gtp_chip_type = CHIP_TYPE_GT9;
u8 rqst_processing = 0;
u8 is_950 = 0;

extern u8 gup_check_fs_mounted(char *path_name);
extern u8 gup_clk_calibration(void);
extern s32 gup_fw_download_proc(void *dir, u8 dwn_mode);
void gtp_get_chip_type(struct i2c_client *client);
u8 gtp_fw_startup(struct i2c_client *client);
static u8 gtp_bak_ref_proc(struct i2c_client *client, u8 mode);
static u8 gtp_main_clk_proc(struct i2c_client *client);
static void gtp_recovery_reset(struct i2c_client *client);
#endif

#ifdef HQ_GTP_GLOVE
static u8 config_hall[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
    = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};
static u8 cfg_len_hall = 0;
static u8 g_glove_switch = 0;
#endif

/* proc file system */
s32 i2c_read_bytes(struct i2c_client *client, u16 addr, u8 *rxbuf, int len);
s32 i2c_write_bytes(struct i2c_client *client, u16 addr, u8 *txbuf, int len);

static ssize_t gt91xx_config_read_proc(struct file *, char __user *, size_t, loff_t *);
static ssize_t gt91xx_config_write_proc(struct file *, const char __user *, size_t, loff_t *);

static struct proc_dir_entry *gt91xx_config_proc = NULL;
static const struct file_operations config_proc_ops = {
    .owner = THIS_MODULE,
    .read = gt91xx_config_read_proc,
    .write = gt91xx_config_write_proc,
};
#define GT9XX_ECHO_PROC_FILE     "gesture_echo"

static ssize_t gt9xx_echo_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos);

static ssize_t gt9xx_echo_write_proc(struct file *filp, const char __user *buffer, size_t count, loff_t *off);

static struct proc_dir_entry *gt9xx_echo_proc = NULL;

static const struct file_operations configecho_proc_ops = {
    .owner = THIS_MODULE,
    .read = gt9xx_echo_read_proc,
    .write = gt9xx_echo_write_proc,
};
#define VELOCITY_CUSTOM
#ifdef VELOCITY_CUSTOM
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

#ifndef TPD_VELOCITY_CUSTOM_X
#define TPD_VELOCITY_CUSTOM_X 10
#endif
#ifndef TPD_VELOCITY_CUSTOM_Y
#define TPD_VELOCITY_CUSTOM_Y 10
#endif

// for magnify velocity********************************************
#define TOUCH_IOC_MAGIC 'A'

#define TPD_GET_VELOCITY_CUSTOM_X _IO(TOUCH_IOC_MAGIC,0)
#define TPD_GET_VELOCITY_CUSTOM_Y _IO(TOUCH_IOC_MAGIC,1)
#define TPD_SET_WAKEUP_FEATURE    _IOW(TOUCH_IOC_MAGIC,2,int)
#define TPD_GET_WAKEUP_FEATURE    _IOR(TOUCH_IOC_MAGIC,3,int)

int g_v_magnify_x = TPD_VELOCITY_CUSTOM_X;
int g_v_magnify_y = TPD_VELOCITY_CUSTOM_Y;
static int tpd_misc_open(struct inode *inode, struct file *file)
{
    return nonseekable_open(inode, file);
}

static int tpd_misc_release(struct inode *inode, struct file *file)
{
    return 0;
}

static long tpd_unlocked_ioctl(struct file *file, unsigned int cmd,
                               unsigned long arg)
{
    //char strbuf[256];
    void __user *data;
	int wakeup = 0;
    long err = 0;

    if (_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }

    if (err)
    {
        printk("tpd: access error: %08X, (%2d, %2d)\n", cmd, _IOC_DIR(cmd), _IOC_SIZE(cmd));
        return -EFAULT;
    }

    switch (cmd)
    {
        case TPD_GET_VELOCITY_CUSTOM_X:
            data = (void __user *) arg;

            if (data == NULL)
            {
                err = -EINVAL;
                break;
            }

            if (copy_to_user(data, &g_v_magnify_x, sizeof(g_v_magnify_x)))
            {
                err = -EFAULT;
                break;
            }

            break;

        case TPD_GET_VELOCITY_CUSTOM_Y:
            data = (void __user *) arg;

            if (data == NULL)
            {
                err = -EINVAL;
                break;
            }

            if (copy_to_user(data, &g_v_magnify_y, sizeof(g_v_magnify_y)))
            {
                err = -EFAULT;
                break;
            }

            break;
	#if GTP_GESTURE_WAKEUP	
	   case TPD_SET_WAKEUP_FEATURE: 
		   data = (void __user *) arg;
		   if(data == NULL)
		   {
	           err = -EINVAL;
	           break;    
		   }                       
		   if(copy_from_user(&wakeup, data, sizeof(wakeup)))
		   {
	           err = -EFAULT;
	           break;
		   }                                
		   printk("tpd: wakeup     %d\n",wakeup);
		   if(wakeup != 1)
		       g_wakeup_gesture = 0;
		   else
		       g_wakeup_gesture = 1;
		   break;
	#endif   
        default:
            printk("tpd: unknown IOCTL: 0x%08x\n", cmd);
            err = -ENOIOCTLCMD;
            break;

    }

    return err;
}
#define GT91XXGC_CONFIG_PROC_FILE     "gesture_enablec"

static ssize_t gt91xxgc_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos);
static ssize_t gt91xxgc_config_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos);

static struct proc_dir_entry *gt91xxgc_config_proc = NULL;

static const struct file_operations configgc_proc_ops = {
    .owner = THIS_MODULE,
    .read = gt91xxgc_config_read_proc,
    .write = gt91xxgc_config_write_proc,
};

#define GT91XXGE_CONFIG_PROC_FILE     "gesture_enablee"

static ssize_t gt91xxge_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos);
static ssize_t gt91xxge_config_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos);

static struct proc_dir_entry *gt91xxge_config_proc = NULL;

static const struct file_operations configge_proc_ops = {
    .owner = THIS_MODULE,
    .read = gt91xxge_config_read_proc,
    .write = gt91xxge_config_write_proc,
};

#define GT91XXGW_CONFIG_PROC_FILE     "gesture_enablew"

static ssize_t gt91xxgw_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos);
static ssize_t gt91xxgw_config_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos);

static struct proc_dir_entry *gt91xxgw_config_proc = NULL;

static const struct file_operations configgw_proc_ops = {
    .owner = THIS_MODULE,
    .read = gt91xxgw_config_read_proc,
    .write = gt91xxgw_config_write_proc,
};

#define GT91XXGM_CONFIG_PROC_FILE     "gesture_enablem"

static ssize_t gt91xxgm_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos);
static ssize_t gt91xxgm_config_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos);

static struct proc_dir_entry *gt91xxgm_config_proc = NULL;

static const struct file_operations configgm_proc_ops = {
    .owner = THIS_MODULE,
    .read = gt91xxgm_config_read_proc,
    .write = gt91xxgm_config_write_proc,
};

#define GT91XXGCC_CONFIG_PROC_FILE     "gesture_enablecc"

static ssize_t gt91xxgcc_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos);
static ssize_t gt91xxgcc_config_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos);

static struct proc_dir_entry *gt91xxgcc_config_proc = NULL;

static const struct file_operations configgcc_proc_ops = {
    .owner = THIS_MODULE,
    .read = gt91xxgcc_config_read_proc,
    .write = gt91xxgcc_config_write_proc,
};


static struct file_operations tpd_fops =
{
//  .owner = THIS_MODULE,
    .open = tpd_misc_open,
    .release = tpd_misc_release,
    .unlocked_ioctl = tpd_unlocked_ioctl,
};
/*----------------------------------------------------------------------------*/
static struct miscdevice tpd_misc_device =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = "touch",
    .fops = &tpd_fops,
};

//**********************************************
#endif

static int tpd_i2c_detect(struct i2c_client *client, struct i2c_board_info *info)
{
    strcpy(info->type, "mtk-tpd");
    return 0;
}

#ifdef TPD_PROXIMITY
static s32 tpd_get_ps_value(void)
{
    return tpd_proximity_detect;
}

static s32 tpd_enable_ps(s32 enable)
{
    u8  state;
    s32 ret = -1;

    if (enable)
    {
        state = 1;
        tpd_proximity_flag = 1;
        GTP_INFO("TPD proximity function to be on.");
    }
    else
    {
        state = 0;
        tpd_proximity_flag = 0;
        GTP_INFO("TPD proximity function to be off.");
    }

    ret = i2c_write_bytes(i2c_client_point, TPD_PROXIMITY_ENABLE_REG, &state, 1);

    if (ret < 0)
    {
        GTP_ERROR("TPD %s proximity cmd failed.", state ? "enable" : "disable");
        return ret;
    }

    GTP_INFO("TPD proximity function %s success.", state ? "enable" : "disable");
    return 0;
}

s32 tpd_ps_operate(void *self, u32 command, void *buff_in, s32 size_in,
                   void *buff_out, s32 size_out, s32 *actualout)
{
    s32 err = 0;
    s32 value;
    hwm_sensor_data *sensor_data;

    switch (command)
    {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int)))
            {
                GTP_ERROR("Set delay parameter error!");
                err = -EINVAL;
            }

            // Do nothing
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int)))
            {
                GTP_ERROR("Enable sensor parameter error!");
                err = -EINVAL;
            }
            else
            {
                value = *(int *)buff_in;
                err = tpd_enable_ps(value);
            }

            break;

        case SENSOR_GET_DATA:
            if ((buff_out == NULL) || (size_out < sizeof(hwm_sensor_data)))
            {
                GTP_ERROR("Get sensor data parameter error!");
                err = -EINVAL;
            }
            else
            {
                sensor_data = (hwm_sensor_data *)buff_out;
                sensor_data->values[0] = tpd_get_ps_value();
                sensor_data->value_divide = 1;
                sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
            }

            break;

        default:
            GTP_ERROR("proxmy sensor operate function no this parameter %d!\n", command);
            err = -1;
            break;
    }

    return err;
}
#endif

#ifdef GTP_HUAWEI_DESIGN
static ssize_t gt9xx_echo_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos)
{
	int len = 0;
	char *p = page;
	
    if (*ppos)  // CMD call again
    {
        return 0;
    }

	int i = 0;
	for(i=0;i<=16;i++)
	{
		p += sprintf(p, "%d ", gesture_echo[i]);
	}
	*ppos += p - page;

        memset(gesture_echo,0,sizeof(gesture_echo));

	return (p-page);
}

static ssize_t gt9xx_echo_write_proc(struct file *filp, const char __user *buffer, size_t count, loff_t *off)
{

}
#endif
static ssize_t gt91xx_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos)
{
    char *ptr = page;
    char temp_data[GTP_CONFIG_MAX_LENGTH + 2] = {0};
    int i;
    
    if (*ppos)  // CMD call again
    {
        return 0;
    }
    
    ptr += sprintf(ptr, "==== GT9XX config init value====\n");

    for (i = 0 ; i < GTP_CONFIG_MAX_LENGTH ; i++)
    {
        ptr += sprintf(ptr, "0x%02X ", config[i + 2]);

        if (i % 8 == 7)
            ptr += sprintf(ptr, "\n");
    }

    ptr += sprintf(ptr, "\n");

    ptr += sprintf(ptr, "==== GT9XX config real value====\n");
    i2c_read_bytes(i2c_client_point, GTP_REG_CONFIG_DATA, temp_data, GTP_CONFIG_MAX_LENGTH);

    for (i = 0 ; i < GTP_CONFIG_MAX_LENGTH ; i++)
    {
        ptr += sprintf(ptr, "0x%02X ", temp_data[i]);

        if (i % 8 == 7)
            ptr += sprintf(ptr, "\n");
    }
    *ppos += ptr - page;
    return (ptr - page);
}

static ssize_t gt91xx_config_write_proc(struct file *filp, const char __user *buffer, size_t count, loff_t *off)
{
    s32 ret = 0;

//    GTP_DEBUG("write count %uld\n", count);

    if (count > GTP_CONFIG_MAX_LENGTH)
    {
//        GTP_ERROR("size not match [%d:%uld]\n", GTP_CONFIG_MAX_LENGTH, count);
        return -EFAULT;
    }

    if (copy_from_user(&config[2], buffer, count))
    {
        GTP_ERROR("copy from user fail\n");
        return -EFAULT;
    }

    ret = gtp_send_cfg(i2c_client_point);
    abs_x_max = (config[RESOLUTION_LOC + 1] << 8) + config[RESOLUTION_LOC];
    abs_y_max = (config[RESOLUTION_LOC + 3] << 8) + config[RESOLUTION_LOC + 2];
    int_type = (config[TRIGGER_LOC]) & 0x03;

    if (ret < 0)
    {
        GTP_ERROR("send config failed.");
    }

    return count;
}

#if GTP_SUPPORT_I2C_DMA
s32 i2c_dma_read(struct i2c_client *client, u16 addr, u8 *rxbuf, s32 len)
{
    int ret;
    s32 retry = 0;
    u8 buffer[2];

    struct i2c_msg msg[2] =
    {
        {
            .addr = (client->addr & I2C_MASK_FLAG),
            .flags = 0,
            .buf = buffer,
            .len = 2,
            .timing = I2C_MASTER_CLOCK
        },
        {
            .addr = (client->addr & I2C_MASK_FLAG),
            .ext_flag = (client->ext_flag | I2C_ENEXT_FLAG | I2C_DMA_FLAG),
            .flags = I2C_M_RD,
            .buf = (u8*)gpDMABuf_pa,     
            .len = len,
            .timing = I2C_MASTER_CLOCK
        },
    };
    
    buffer[0] = (addr >> 8) & 0xFF;
    buffer[1] = addr & 0xFF;

    if (rxbuf == NULL)
        return -1;

    //GTP_DEBUG("dma i2c read: 0x%04X, %d bytes(s)", addr, len);
    for (retry = 0; retry < 5; ++retry)
    {
        ret = i2c_transfer(client->adapter, &msg[0], 2);
        if (ret < 0)
        {
            continue;
        }
        memcpy(rxbuf, gpDMABuf_va, len);
        return 0;
    }
    GTP_ERROR("Dma I2C Read Error: 0x%04X, %d byte(s), err-code: %d", addr, len, ret);
    return ret;
}


s32 i2c_dma_write(struct i2c_client *client, u16 addr, u8 *txbuf, s32 len)
{
    int ret;
    s32 retry = 0;
    u8 *wr_buf = gpDMABuf_va;
    struct i2c_msg msg =
    {
        .addr = (client->addr & I2C_MASK_FLAG),
        .ext_flag = (client->ext_flag | I2C_ENEXT_FLAG | I2C_DMA_FLAG),
        .flags = 0,
        .buf = (u8*)gpDMABuf_pa,
        .len = 2 + len,
        .timing = I2C_MASTER_CLOCK
    };
    
    wr_buf[0] = (u8)((addr >> 8) & 0xFF);
    wr_buf[1] = (u8)(addr & 0xFF);

    if (txbuf == NULL)
        return -1;
    
    //GTP_DEBUG("dma i2c write: 0x%04X, %d bytes(s)", addr, len);
    memcpy(wr_buf+2, txbuf, len);
    for (retry = 0; retry < 5; ++retry)
    {
        ret = i2c_transfer(client->adapter, &msg, 1);
        if (ret < 0)
        {
            continue;
        }
        return 0;
    }
    GTP_ERROR("Dma I2C Write Error: 0x%04X, %d byte(s), err-code: %d", addr, len, ret);
    return ret;
}

s32 i2c_read_bytes_dma(struct i2c_client *client, u16 addr, u8 *rxbuf, s32 len)
{
    s32 left = len;
    s32 read_len = 0;
    u8 *rd_buf = rxbuf;
    s32 ret = 0;    
    
    //GTP_DEBUG("Read bytes dma: 0x%04X, %d byte(s)", addr, len);
    while (left > 0)
    {
        if (left > GTP_DMA_MAX_TRANSACTION_LENGTH)
        {
            read_len = GTP_DMA_MAX_TRANSACTION_LENGTH;
        }
        else
        {
            read_len = left;
        }
        ret = i2c_dma_read(client, addr, rd_buf, read_len);
        if (ret < 0)
        {
            GTP_ERROR("dma read failed");
            return -1;
        }
        
        left -= read_len;
        addr += read_len;
        rd_buf += read_len;
    }
    return 0;
}

s32 i2c_write_bytes_dma(struct i2c_client *client, u16 addr, u8 *txbuf, s32 len)
{

    s32 ret = 0;
    s32 write_len = 0;
    s32 left = len;
    u8 *wr_buf = txbuf;
    
    //GTP_DEBUG("Write bytes dma: 0x%04X, %d byte(s)", addr, len);
    while (left > 0)
    {
        if (left > GTP_DMA_MAX_I2C_TRANSFER_SIZE)
        {
            write_len = GTP_DMA_MAX_I2C_TRANSFER_SIZE;
        }
        else
        {
            write_len = left;
        }
        ret = i2c_dma_write(client, addr, wr_buf, write_len);
        
        if (ret < 0)
        {
            GTP_ERROR("dma i2c write failed!");
            return -1;
        }
        
        left -= write_len;
        addr += write_len;
        wr_buf += write_len;
    }
    return 0;
}
#endif


int i2c_read_bytes_non_dma(struct i2c_client *client, u16 addr, u8 *rxbuf, int len)
{
    u8 buffer[GTP_ADDR_LENGTH];
    u8 retry;
    u16 left = len;
    u16 offset = 0;

    struct i2c_msg msg[2] =
    {
        {
            //.addr = ((client->addr &I2C_MASK_FLAG) | (I2C_ENEXT_FLAG)),
            .addr = ((client->addr &I2C_MASK_FLAG) | (I2C_PUSHPULL_FLAG)),
            .flags = 0,
            .buf = buffer,
            .len = GTP_ADDR_LENGTH,
            .timing = I2C_MASTER_CLOCK
        },
        {
            //.addr = ((client->addr &I2C_MASK_FLAG) | (I2C_ENEXT_FLAG)),
            .addr = ((client->addr &I2C_MASK_FLAG) | (I2C_PUSHPULL_FLAG)),
            .flags = I2C_M_RD,
            .timing = I2C_MASTER_CLOCK
        },
    };

    if (rxbuf == NULL)
        return -1;

    //GTP_DEBUG("i2c_read_bytes to device %02X address %04X len %d\n", client->addr, addr, len);

    while (left > 0)
    {
        buffer[0] = ((addr + offset) >> 8) & 0xFF;
        buffer[1] = (addr + offset) & 0xFF;

        msg[1].buf = &rxbuf[offset];

        if (left > MAX_TRANSACTION_LENGTH)
        {
            msg[1].len = MAX_TRANSACTION_LENGTH;
            left -= MAX_TRANSACTION_LENGTH;
            offset += MAX_TRANSACTION_LENGTH;
        }
        else
        {
            msg[1].len = left;
            left = 0;
        }

        retry = 0;

        while (i2c_transfer(client->adapter, &msg[0], 2) != 2)
        {
            retry++;

            //if (retry == 20)
            if (retry == 5)
            {
                GTP_ERROR("I2C read 0x%X length=%d failed\n", addr + offset, len);
                return -1;
            }
        }
    }

    return 0;
}


int i2c_read_bytes(struct i2c_client *client, u16 addr, u8 *rxbuf, int len)
{
#if GTP_SUPPORT_I2C_DMA
    return i2c_read_bytes_dma(client, addr, rxbuf, len);
#else
    return i2c_read_bytes_non_dma(client, addr, rxbuf, len);
#endif
}

s32 gtp_i2c_read(struct i2c_client *client, u8 *buf, s32 len)
{
    s32 ret = -1;
    u16 addr = (buf[0] << 8) + buf[1];

    ret = i2c_read_bytes_non_dma(client, addr, &buf[2], len - 2);

    if (!ret)
    {
        return 2;
    }
    else
    {
    #if GTP_GESTURE_WAKEUP
	if (g_wakeup_gesture) {
        if (DOZE_ENABLED == doze_status)
        {
            return ret;
        }
	}
    #endif
    #if GTP_COMPATIBLE_MODE
        if (CHIP_TYPE_GT9F == gtp_chip_type)
        {
            gtp_recovery_reset(client);
        }
        else
    #endif
        {
            gtp_reset_guitar(client, 20);
        }
        return ret;
    }
}


s32 gtp_i2c_read_dbl_check(struct i2c_client *client, u16 addr, u8 *rxbuf, int len)
{
    u8 buf[16] = {0};
    u8 confirm_buf[16] = {0};
    u8 retry = 0;
    
    while (retry++ < 3)
    {
        memset(buf, 0xAA, 16);
        buf[0] = (u8)(addr >> 8);
        buf[1] = (u8)(addr & 0xFF);
        gtp_i2c_read(client, buf, len + 2);
        
        memset(confirm_buf, 0xAB, 16);
        confirm_buf[0] = (u8)(addr >> 8);
        confirm_buf[1] = (u8)(addr & 0xFF);
        gtp_i2c_read(client, confirm_buf, len + 2);
        
        if (!memcmp(buf, confirm_buf, len+2))
        {
            memcpy(rxbuf, confirm_buf+2, len);
            return SUCCESS;
        }
    }    
    GTP_ERROR("i2c read 0x%04X, %d bytes, double check failed!", addr, len);
    return FAIL;
}

int i2c_write_bytes_non_dma(struct i2c_client *client, u16 addr, u8 *txbuf, int len)
{
    u8 buffer[MAX_TRANSACTION_LENGTH];
    u16 left = len;
    u16 offset = 0;
    u8 retry = 0;

    struct i2c_msg msg =
    {
        //.addr = ((client->addr &I2C_MASK_FLAG) | (I2C_ENEXT_FLAG)),
        .addr = ((client->addr &I2C_MASK_FLAG) | (I2C_PUSHPULL_FLAG)),
        .flags = 0,
        .buf = buffer,
        .timing = I2C_MASTER_CLOCK,
    };


    if (txbuf == NULL)
        return -1;

    //GTP_DEBUG("i2c_write_bytes to device %02X address %04X len %d\n", client->addr, addr, len);

    while (left > 0)
    {
        retry = 0;

        buffer[0] = ((addr + offset) >> 8) & 0xFF;
        buffer[1] = (addr + offset) & 0xFF;

        if (left > MAX_I2C_TRANSFER_SIZE)
        {
            memcpy(&buffer[GTP_ADDR_LENGTH], &txbuf[offset], MAX_I2C_TRANSFER_SIZE);
            msg.len = MAX_TRANSACTION_LENGTH;
            left -= MAX_I2C_TRANSFER_SIZE;
            offset += MAX_I2C_TRANSFER_SIZE;
        }
        else
        {
            memcpy(&buffer[GTP_ADDR_LENGTH], &txbuf[offset], left);
            msg.len = left + GTP_ADDR_LENGTH;
            left = 0;
        }

        //GTP_DEBUG("byte left %d offset %d\n", left, offset);

        while (i2c_transfer(client->adapter, &msg, 1) != 1)
        {
            retry++;

            //if (retry == 20)
            if (retry == 5)
            {
                GTP_ERROR("I2C write 0x%X%X length=%d failed\n", buffer[0], buffer[1], len);
                return -1;
            }
        }
    }

    return 0;
}

int i2c_write_bytes(struct i2c_client *client, u16 addr, u8 *txbuf, int len)
{
#if GTP_SUPPORT_I2C_DMA
    return i2c_write_bytes_dma(client, addr, txbuf, len);
#else
    return i2c_write_bytes_non_dma(client, addr, txbuf, len);
#endif
}

s32 gtp_i2c_write(struct i2c_client *client, u8 *buf, s32 len)
{
    s32 ret = -1;
    u16 addr = (buf[0] << 8) + buf[1];

    ret = i2c_write_bytes_non_dma(client, addr, &buf[2], len - 2);

    if (!ret)
    {
        return 1;
    }
    else
    {
    #if GTP_GESTURE_WAKEUP
		if (g_wakeup_gesture) {
        	if (DOZE_ENABLED == doze_status)
        	{
            	return ret;
        	}
		}
    #endif
    #if GTP_COMPATIBLE_MODE
        if (CHIP_TYPE_GT9F == gtp_chip_type)
        {
            gtp_recovery_reset(client);
        }
        else
    #endif
        {
            gtp_reset_guitar(client, 20);
        }
        return ret;
    }
}



/*******************************************************
Function:
    Send config Function.

Input:
    client: i2c client.

Output:
    Executive outcomes.0--success,non-0--fail.
*******************************************************/
s32 gtp_send_cfg(struct i2c_client *client)
{
    s32 ret = 1;

#if GTP_DRIVER_SEND_CFG
    s32 retry = 0;

	if (pnl_init_error)
    {
        GTP_INFO("Error occurred in init_panel, no config sent!");
        return 0;
    }
    
    GTP_INFO("Driver Send Config");
    for (retry = 0; retry < 5; retry++)
    {
        ret = gtp_i2c_write(client, config, GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH);

        if (ret > 0)
        {
            break;
        }
    }
#endif
    return ret;
}
#if GTP_CHARGER_SWITCH
static int gtp_send_chr_cfg(struct i2c_client *client)
{
	s32 ret = 1;
#if GTP_DRIVER_SEND_CFG
    s32 retry = 0;

	if (pnl_init_error) {
        GTP_INFO("Error occurred in init_panel, no config sent!");
        return 0;
    }
    
    GTP_INFO("Driver Send Config");
    for (retry = 0; retry < 5; retry++) {
        ret = gtp_i2c_write(client, gtp_charger_config, GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH);
        if (ret > 0) {
            break;
        }
    }
#endif	
	return ret;
}
#endif

#ifdef HQ_GTP_GLOVE
static struct work_struct slideworkTimeOut;
static int g_timeOutTimeMs=0;
static struct hrtimer g_timeOutTimer;

s32 gtp_send_cfg_hall(u8 glove_enable);
void set_gtp_glove_state(void)
{
	//if ( tpd_halt ) return 0;
	printk("zjy_g: glove set %d, cfg_hall: %d\n", g_glove_switch, cfg_len_hall);
	
	if (cfg_len_hall) {
		gtp_send_cfg_hall(g_glove_switch);
	} else {
		GTP_INFO("[error]glove cfg_hall_len is 0\n");
	}

	return 0;
}

static void Glove_timeOutFunc(struct work_struct *data)
{
	set_gtp_glove_state();
   
    //printk(KERN_ALERT "work handler function./n");
    hrtimer_cancel( &g_timeOutTimer );
}

enum hrtimer_restart GloveTimeOutCallback(struct hrtimer *timer)
{
//	printk("jialiang SlideTimeOutCallback\n",);
    schedule_work(&slideworkTimeOut);
    return HRTIMER_NORESTART;
}

void glovetimerInit(void)
{
    INIT_WORK(&slideworkTimeOut, Glove_timeOutFunc);
	g_timeOutTimeMs=100; //1s
	hrtimer_init( &g_timeOutTimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
	g_timeOutTimer.function=GloveTimeOutCallback;
}

void get_gtp_glove_state(int glove_state)
{
	g_glove_switch = glove_state;
	//printk("zjy_g: glove get %d\n", g_glove_switch);
	if(g_timeOutTimeMs!=0)
    {
    	ktime_t ktime;
		ktime = ktime_set( 0, g_timeOutTimeMs*1000000 );
		hrtimer_start( &g_timeOutTimer, ktime, HRTIMER_MODE_REL );
    }
}
EXPORT_SYMBOL(get_gtp_glove_state);

s32 gtp_send_cfg_hall(u8 glove_enable)
{
	printk("zjy\n");
    s32 ret = 1;

#if GTP_DRIVER_SEND_CFG
    s32 retry = 0;
	
	GTP_DEBUG("Driver Send Config");
    for (retry = 0; retry < 5; retry++) {
    	if (glove_enable)
        	ret = gtp_i2c_write(i2c_client_point, config_hall, GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH);
		else
			ret = gtp_i2c_write(i2c_client_point, config, GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH);

        if (ret > 0)
            break;
    }
#endif
    return ret;
}

static ssize_t glove_switch_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos)
{
	char *ptr = page;
	int i;
	if (*ppos)  // CMD call again
		return 0;

	ptr += sprintf(ptr, "%d\n", g_glove_switch);

	*ppos += ptr - page;
	return (ptr - page);
}

static void tpd_resume(struct early_suspend *h);
static ssize_t glove_switch_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
	char wtire_data[32] = {0};

	if (size >= 32)
		return -EFAULT;

	if (copy_from_user( &wtire_data, buff, size ))
		return -EFAULT;

	if (wtire_data[0] == '1')
		g_glove_switch = 1;
	else
		g_glove_switch = 0;
	
	printk("zjh: %s, g_glove_switch = %d\n", __func__, g_glove_switch);

	if (cfg_len_hall) {
		/* FIXME */
		tpd_resume(NULL);
		/* FIXME */
		gtp_send_cfg_hall(g_glove_switch);
	}

	return size;
}
#endif

/*******************************************************
Function:
    Read goodix touchscreen version function.

Input:
    client: i2c client struct.
    version:address to store version info

Output:
    Executive outcomes.0---succeed.
*******************************************************/
s32 gtp_read_version(struct i2c_client *client, u16 *version)
{
    s32 ret = -1;
    s32 i;
    u8 buf[8] = {GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff};

    GTP_DEBUG_FUNC();

    ret = gtp_i2c_read(client, buf, sizeof(buf));

	GTP_INFO("IC VERSION:%d,%d,%d,%d_%02x%02x",
             buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);

    if (ret < 0)
    {
        GTP_ERROR("GTP read version failed");
        return ret;
    }

    if (version)
    {
        *version = (buf[7] << 8) | buf[6];
    }

    tpd_info.vid = *version;
    tpd_info.pid = 0x00;

    for (i = 0; i < 4; i++)
    {
        if (buf[i + 2] < 0x30)break;

        tpd_info.pid |= ((buf[i + 2] - 0x30) << ((3 - i) * 4));
    }

    if (buf[5] == 0x00)
    {        
        GTP_INFO("IC VERSION: %c%c%c_%02x%02x",
             buf[2], buf[3], buf[4], buf[7], buf[6]);  
    }
    else
    {
        GTP_INFO("IC VERSION:%c%c%c%c_%02x%02x",
             buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
	GTP_INFO("IC VERSION:%d,%d,%d,%d_%02x%02x",
             buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
    }

	printk("[GTP darren] tpd_info.pid = %d\n",tpd_info.pid);

	if(buf[2] == '9' && buf[3] == '1' && 
	    buf[4] == '5')
	{
		    tpd_load_status = 1;
			printk("[GTP darren] read GTP ID successs\n");
	}
	else
	{
			  tpd_load_status = 0;
			 printk("[GTP darren] read GTP ID fail\n");
	}
	
    return ret;
}

#if GTP_DRIVER_SEND_CFG
/*******************************************************
Function:
    Get information from ic, such as resolution and 
    int trigger type
Input:
    client: i2c client private struct.

Output:
    FAIL: i2c failed, SUCCESS: i2c ok
*******************************************************/
static s32 gtp_get_info(struct i2c_client *client)
{
    u8 opr_buf[6] = {0};
    s32 ret = 0;
    
    opr_buf[0] = (u8)((GTP_REG_CONFIG_DATA+1) >> 8);
    opr_buf[1] = (u8)((GTP_REG_CONFIG_DATA+1) & 0xFF);
    
    ret = gtp_i2c_read(client, opr_buf, 6);
    if (ret < 0)
    {
        return FAIL;
    }
    
    abs_x_max = (opr_buf[3] << 8) + opr_buf[2];
    abs_y_max = (opr_buf[5] << 8) + opr_buf[4];
    
    opr_buf[0] = (u8)((GTP_REG_CONFIG_DATA+6) >> 8);
    opr_buf[1] = (u8)((GTP_REG_CONFIG_DATA+6) & 0xFF);
    
    ret = gtp_i2c_read(client, opr_buf, 3);
    if (ret < 0)
    {
        return FAIL;
    }
    int_type = opr_buf[2] & 0x03;
    
    GTP_INFO("X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x",
            abs_x_max,abs_y_max, int_type);
            
    return SUCCESS;
}
#endif


/*******************************************************
Function:
    GTP initialize function.

Input:
    client: i2c client private struct.

Output:
    Executive outcomes.0---succeed.
*******************************************************/
static s32 gtp_init_panel(struct i2c_client *client)
{
    s32 ret = 0;

#if GTP_DRIVER_SEND_CFG
    s32 i;
    u8 check_sum = 0;
    u8 opr_buf[16];
    u8 sensor_id = 0;
	 u8 drv_cfg_version;
	 u8 flash_cfg_version;

    u8 cfg_info_group0[] = CTP_CFG_GROUP0;
    u8 cfg_info_group1[] = CTP_CFG_GROUP1;
    u8 cfg_info_group2[] = CTP_CFG_GROUP2;
    u8 cfg_info_group3[] = CTP_CFG_GROUP3;
    u8 cfg_info_group4[] = CTP_CFG_GROUP4;
    u8 cfg_info_group5[] = CTP_CFG_GROUP5;
    u8 *send_cfg_buf[] = {cfg_info_group0, cfg_info_group1, cfg_info_group2,
                        cfg_info_group3, cfg_info_group4, cfg_info_group5};
    u8 cfg_info_len[] = { CFG_GROUP_LEN(cfg_info_group0), 
                          CFG_GROUP_LEN(cfg_info_group1),
                          CFG_GROUP_LEN(cfg_info_group2),
                          CFG_GROUP_LEN(cfg_info_group3), 
                          CFG_GROUP_LEN(cfg_info_group4),
                          CFG_GROUP_LEN(cfg_info_group5)};
#if GTP_CHARGER_SWITCH
	const u8 cfg_grp0_charger[] = GTP_CFG_GROUP0_CHARGER;
	const u8 cfg_grp1_charger[] = GTP_CFG_GROUP1_CHARGER;
	const u8 cfg_grp2_charger[] = GTP_CFG_GROUP2_CHARGER;
	const u8 cfg_grp3_charger[] = GTP_CFG_GROUP3_CHARGER;
	const u8 cfg_grp4_charger[] = GTP_CFG_GROUP4_CHARGER;
	const u8 cfg_grp5_charger[] = GTP_CFG_GROUP5_CHARGER;
	const u8 *cfgs_charger[] = {
		cfg_grp0_charger, cfg_grp1_charger, cfg_grp2_charger,
		cfg_grp3_charger, cfg_grp4_charger, cfg_grp5_charger
	};
	u8 cfg_lens_charger[] = {
						CFG_GROUP_LEN(cfg_grp0_charger),
						CFG_GROUP_LEN(cfg_grp1_charger),
						CFG_GROUP_LEN(cfg_grp2_charger),
						CFG_GROUP_LEN(cfg_grp3_charger),
						CFG_GROUP_LEN(cfg_grp4_charger),
						CFG_GROUP_LEN(cfg_grp5_charger)};
#endif

    GTP_DEBUG("Config Groups\' Lengths: %d, %d, %d, %d, %d, %d", 
        cfg_info_len[0], cfg_info_len[1], cfg_info_len[2], cfg_info_len[3],
        cfg_info_len[4], cfg_info_len[5]);

    if ((!cfg_info_len[1]) && (!cfg_info_len[2]) && 
        (!cfg_info_len[3]) && (!cfg_info_len[4]) && 
        (!cfg_info_len[5]))
    {
        sensor_id = 0; 
    }
    else
    {
    #if GTP_COMPATIBLE_MODE
        if (CHIP_TYPE_GT9F == gtp_chip_type)
        {
            msleep(50);
        }
    #endif
        ret = gtp_i2c_read_dbl_check(client, GTP_REG_SENSOR_ID, &sensor_id, 1);
        if (SUCCESS == ret)
        {
            if (sensor_id >= 0x06)
            {
                GTP_ERROR("Invalid sensor_id(0x%02X), No Config Sent!", sensor_id);
                pnl_init_error = 1;
                return -1;
            }
        }
        else
        {
            GTP_ERROR("Failed to get sensor_id, No config sent!");
            pnl_init_error = 1;
            return -1;
        }
        GTP_INFO("Sensor_ID: %d", sensor_id);
    }
    
    cfg_len = cfg_info_len[sensor_id];
    
    GTP_INFO("CTP_CONFIG_GROUP%d used, config length: %d", sensor_id, cfg_len);
    
    if (cfg_len < GTP_CONFIG_MIN_LENGTH)
    {
        GTP_ERROR("CTP_CONFIG_GROUP%d is INVALID CONFIG GROUP! NO Config Sent! You need to check you header file CFG_GROUP section!", sensor_id);
        pnl_init_error = 1;
        return -1;
    }
    
#if GTP_COMPATIBLE_MODE
    if (CHIP_TYPE_GT9F != gtp_chip_type)
#endif
	{
	    ret = gtp_i2c_read_dbl_check(client, GTP_REG_CONFIG_DATA, &opr_buf[0], 1);    
	    if (ret == SUCCESS)
	    {
	        GTP_DEBUG("CFG_CONFIG_GROUP%d Config Version: %d, 0x%02X; IC Config Version: %d, 0x%02X", sensor_id, 
	                    send_cfg_buf[sensor_id][0], send_cfg_buf[sensor_id][0], opr_buf[0], opr_buf[0]);
	
			flash_cfg_version = opr_buf[0];
			drv_cfg_version = send_cfg_buf[sensor_id][0];       // backup  config version
			
	        if (flash_cfg_version < 90 && flash_cfg_version > drv_cfg_version) {
	            send_cfg_buf[sensor_id][0] = 0x00;
	        }
	    }
	    else
	    {
	        GTP_ERROR("Failed to get ic config version!No config sent!");
	        return -1;
	    }
	}  
    memset(&config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
    memcpy(&config[GTP_ADDR_LENGTH], send_cfg_buf[sensor_id], cfg_len);
#ifdef HQ_GTP_GLOVE
	memset(&config_hall[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
    memcpy(&config_hall[GTP_ADDR_LENGTH], send_cfg_buf_hall[sensor_id], cfg_len_hall);
#endif

#if GTP_CUSTOM_CFG
    config[RESOLUTION_LOC]     = (u8)GTP_MAX_WIDTH;
    config[RESOLUTION_LOC + 1] = (u8)(GTP_MAX_WIDTH>>8);
    config[RESOLUTION_LOC + 2] = (u8)GTP_MAX_HEIGHT;
    config[RESOLUTION_LOC + 3] = (u8)(GTP_MAX_HEIGHT>>8);
    
    if (GTP_INT_TRIGGER == 0)  //RISING
    {
        config[TRIGGER_LOC] &= 0xfe; 
    }
    else if (GTP_INT_TRIGGER == 1)  //FALLING
    {
        config[TRIGGER_LOC] |= 0x01;
    }
#endif  // GTP_CUSTOM_CFG

	check_sum = 0;
	for (i = GTP_ADDR_LENGTH; i < cfg_len; i++)
	{
		check_sum += config[i];
	}
	config[cfg_len] = (~check_sum) + 1;

#if GTP_CHARGER_SWITCH
	GTP_DEBUG("Charger Config Groups Length: %d, %d, %d, %d, %d, %d", cfg_lens_charger[0],
		  cfg_lens_charger[1], cfg_lens_charger[2], cfg_lens_charger[3], cfg_lens_charger[4], cfg_lens_charger[5]);

	memset(&gtp_charger_config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
	if (cfg_lens_charger[sensor_id] == cfg_len) 
		memcpy(&gtp_charger_config[GTP_ADDR_LENGTH], cfgs_charger[sensor_id], cfg_len);

#if GTP_CUSTOM_CFG
	gtp_charger_config[RESOLUTION_LOC] = (u8) GTP_MAX_WIDTH;
	gtp_charger_config[RESOLUTION_LOC + 1] = (u8) (GTP_MAX_WIDTH >> 8);
	gtp_charger_config[RESOLUTION_LOC + 2] = (u8) GTP_MAX_HEIGHT;
	gtp_charger_config[RESOLUTION_LOC + 3] = (u8) (GTP_MAX_HEIGHT >> 8);

	if (GTP_INT_TRIGGER == 0) 	/* RISING  */
		gtp_charger_config[TRIGGER_LOC] &= 0xfe;
	else if (GTP_INT_TRIGGER == 1) /* FALLING */
		gtp_charger_config[TRIGGER_LOC] |= 0x01;
#endif /* END GTP_CUSTOM_CFG */
	if (cfg_lens_charger[sensor_id] != cfg_len) 
		memset(&gtp_charger_config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);	
	
	check_sum = 0;
	for (i = GTP_ADDR_LENGTH; i < cfg_len; i++)
	{
		check_sum += gtp_charger_config[i];
	}
	gtp_charger_config[cfg_len] = (~check_sum) + 1;

#endif /* END GTP_CHARGER_SWITCH */    
  
#else // DRIVER NOT SEND CONFIG
    cfg_len = GTP_CONFIG_MAX_LENGTH;
    ret = gtp_i2c_read(client, config, cfg_len + GTP_ADDR_LENGTH);
    if (ret < 0)
    {
        GTP_ERROR("Read Config Failed, Using DEFAULT Resolution & INT Trigger!");
        abs_x_max = GTP_MAX_WIDTH;
        abs_y_max = GTP_MAX_HEIGHT;
        int_type = GTP_INT_TRIGGER;
    }
#endif // GTP_DRIVER_SEND_CFG

    GTP_DEBUG_FUNC();
    if ((abs_x_max == 0) && (abs_y_max == 0))
    {
        abs_x_max = (config[RESOLUTION_LOC + 1] << 8) + config[RESOLUTION_LOC];
        abs_y_max = (config[RESOLUTION_LOC + 3] << 8) + config[RESOLUTION_LOC + 2];
        int_type = (config[TRIGGER_LOC]) & 0x03; 
    }
    
#if GTP_COMPATIBLE_MODE
    if (CHIP_TYPE_GT9F == gtp_chip_type)
    {
        u8 have_key = 0;
        if (is_950)
        {
            driver_num = config[GTP_REG_MATRIX_DRVNUM - GTP_REG_CONFIG_DATA + 2];
            sensor_num = config[GTP_REG_MATRIX_SENNUM - GTP_REG_CONFIG_DATA + 2];
        }
        else
        {
            driver_num = (config[CFG_LOC_DRVA_NUM]&0x1F) + (config[CFG_LOC_DRVB_NUM]&0x1F);
            sensor_num = (config[CFG_LOC_SENS_NUM]&0x0F) + ((config[CFG_LOC_SENS_NUM]>>4)&0x0F);
        }
        
        have_key = config[GTP_REG_HAVE_KEY - GTP_REG_CONFIG_DATA + 2] & 0x01;  // have key or not
        if (1 == have_key)
        {
            driver_num--;
        }
        
        GTP_INFO("Driver * Sensor: %d * %d(Key: %d), X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x",
            driver_num, sensor_num, have_key, abs_x_max,abs_y_max,int_type);
    }
    else
#endif
    {
#if GTP_DRIVER_SEND_CFG
        ret = gtp_send_cfg(client);
        if (ret < 0)
        {
            GTP_ERROR("Send config error.");
        }
#if GTP_COMPATIBLE_MODE
  	  if (CHIP_TYPE_GT9F != gtp_chip_type)
#endif
	  {
        /* for resume to send config */
		if (flash_cfg_version < 90 && flash_cfg_version > drv_cfg_version) {		
	        config[GTP_ADDR_LENGTH] = drv_cfg_version;
	        check_sum = 0;
	        for (i = GTP_ADDR_LENGTH; i < cfg_len; i++)
	        {
	            check_sum += config[i];
	        }
	        config[cfg_len] = (~check_sum) + 1;
		}
	  }
#endif
        GTP_INFO("X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x",
            abs_x_max,abs_y_max,int_type);
    }
    gt9xx_version=send_cfg_buf[sensor_id][0];
    msleep(10);
    return 0;
}

static s8 gtp_i2c_test(struct i2c_client *client)
{

    u8 retry = 0;
    s8 ret = -1;
    u32 hw_info = 0;

    GTP_DEBUG_FUNC();

    while (retry++ < 5)
    {
        ret = i2c_read_bytes(client, GTP_REG_HW_INFO, (u8 *)&hw_info, sizeof(hw_info));

        if ((!ret) && (hw_info == 0x00900600))              //20121212
        {
            return ret;
        }

        GTP_ERROR("GTP_REG_HW_INFO : %08X", hw_info);
        GTP_ERROR("GTP i2c test failed time %d.", retry);
        msleep(10);
    }

    return -1;
}



/*******************************************************
Function:
    Set INT pin  as input for FW sync.

Note:
  If the INT is high, It means there is pull up resistor attached on the INT pin.
  Pull low the INT pin manaully for FW sync.
*******************************************************/
void gtp_int_sync(s32 ms)
{
    GTP_GPIO_OUTPUT(GTP_INT_PORT, 0);
    msleep(ms);
    GTP_GPIO_AS_INT(GTP_INT_PORT);
}

void gtp_reset_guitar(struct i2c_client *client, s32 ms)
{
    GTP_INFO("GTP RESET!\n");
    GTP_GPIO_OUTPUT(GTP_RST_PORT, 0);
    msleep(ms);
    GTP_GPIO_OUTPUT(GTP_INT_PORT, client->addr == 0x14);

    msleep(2);
    GTP_GPIO_OUTPUT(GTP_RST_PORT, 1);

    msleep(6);                      //must >= 6ms

#if GTP_COMPATIBLE_MODE
    if (CHIP_TYPE_GT9F == gtp_chip_type)
    {
        return;
    }
#endif

    gtp_int_sync(50); 
#if GTP_ESD_PROTECT
    gtp_init_ext_watchdog(i2c_client_point);
#endif
}

static int tpd_power_on(struct i2c_client *client)
{
    int ret = 0;
    int reset_count = 0;

reset_proc:
    GTP_GPIO_OUTPUT(GTP_RST_PORT, 0);   
    GTP_GPIO_OUTPUT(GTP_INT_PORT, 0);
    msleep(10);

#ifdef MT6573
    // power on CTP
    mt_set_gpio_mode(GPIO_CTP_EN_PIN, GPIO_CTP_EN_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EN_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EN_PIN, GPIO_OUT_ONE);

#else   // ( defined(MT6575) || defined(MT6577) || defined(MT6589) )

    #ifdef TPD_POWER_SOURCE_CUSTOM                           
        hwPowerOn(TPD_POWER_SOURCE_CUSTOM, VOL_2800, "TP");    
    #else
        hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
    #endif
    #ifdef TPD_POWER_SOURCE_1800
        hwPowerOn(TPD_POWER_SOURCE_1800, VOL_1800, "TP");
    #endif

#endif

    gtp_reset_guitar(client, 20);

#if GTP_COMPATIBLE_MODE
    gtp_get_chip_type(client);
    
    if (CHIP_TYPE_GT9F == gtp_chip_type)
    {
        ret = gup_fw_download_proc(NULL, GTP_FL_FW_BURN);
    
        if(FAIL == ret)
        {
            GTP_ERROR("[tpd_power_on]Download fw failed.");
            if(reset_count++ < TPD_MAX_RESET_COUNT)
            {
                goto reset_proc;
            }
            else
            {
                return -1;
            }
        }
        
        ret = gtp_fw_startup(client);
        if(FAIL == ret)
        {
            GTP_ERROR("[tpd_power_on]Startup fw failed.");
            if(reset_count++ < TPD_MAX_RESET_COUNT)
            {
                goto reset_proc;
            }
            else
            {
                return -1;
            }
        }
    }
    else  
#endif
    {
    	printk("enter i2c test!\n");
        ret = gtp_i2c_test(client);
    
        if (ret < 0)
        {
            GTP_ERROR("I2C communication ERROR!");
    
            if (reset_count < TPD_MAX_RESET_COUNT)
            {
                reset_count++;
                goto reset_proc;
            }
        }
    }
    return ret;
}

//**************** For GT9XXF Start ********************//
#if GTP_COMPATIBLE_MODE


void gtp_get_chip_type(struct i2c_client *client)
{
    u8 opr_buf[10] = {0x00};
    s32 ret = 0;
    
    msleep(10);
    
    ret = gtp_i2c_read_dbl_check(client, GTP_REG_CHIP_TYPE, opr_buf, 10);
    
    if (FAIL == ret)
    {
        GTP_ERROR("Failed to get chip-type, set chip type default: GOODIX_GT9");
        gtp_chip_type = CHIP_TYPE_GT9;
        return;
    }
    
    if (!memcmp(opr_buf, "GOODIX_GT9", 10))
    {
        gtp_chip_type = CHIP_TYPE_GT9;
    }
    else // GT9XXF
    {
        gtp_chip_type = CHIP_TYPE_GT9F;
    }
    GTP_INFO("Chip Type: %s", (gtp_chip_type == CHIP_TYPE_GT9) ? "GOODIX_GT9" : "GOODIX_GT9F");
}

static u8 gtp_bak_ref_proc(struct i2c_client *client, u8 mode)
{
    s32 i = 0;
    s32 j = 0;
    s32 ret = 0;
    struct file *flp = NULL;
    u8 *refp = NULL;
    u32 ref_len = 0;
    u32 ref_seg_len = 0;
    s32 ref_grps = 0;
    s32 ref_chksum = 0;
    u16 tmp = 0;
    
    GTP_DEBUG("[gtp_bak_ref_proc]Driver:%d,Sensor:%d.", driver_num, sensor_num);

    //check file-system mounted 
    GTP_DEBUG("[gtp_bak_ref_proc]Waiting for FS %d", gtp_ref_retries);        
    if (gup_check_fs_mounted("/data") == FAIL)        
    {
        GTP_DEBUG("[gtp_bak_ref_proc]/data not mounted");
        if(gtp_ref_retries++ < GTP_CHK_FS_MNT_MAX)
        {
            return FAIL;
        }
    }
    else
    {
        GTP_DEBUG("[gtp_bak_ref_proc]/data mounted !!!!");
    }
    
    if (is_950)
    {
        ref_seg_len = (driver_num * (sensor_num - 1) + 2) * 2;
        ref_grps = 6;
        ref_len =  ref_seg_len * 6;  // for GT950, backup-reference for six segments
    }
    else
    {
        ref_len = driver_num*(sensor_num-2)*2 + 4;
        ref_seg_len = ref_len;
        ref_grps = 1;
    }
    
    refp = (u8 *)kzalloc(ref_len, GFP_KERNEL);
    if(refp == NULL)
    {
        GTP_ERROR("Failed to allocate memory for reference buffer!"); 
        return FAIL;
    }
    memset(refp, 0, ref_len);
    
    //get ref file data
    flp = filp_open(GTP_BAK_REF_PATH, O_RDWR | O_CREAT, 0666);
    if (IS_ERR(flp))
    {
        GTP_ERROR("Failed to open/create %s.", GTP_BAK_REF_PATH);
        if (GTP_BAK_REF_SEND == mode)
        {
            goto default_bak_ref;
        }
        else
        {
            goto exit_ref_proc;
        }
    }
    
    switch (mode)
    {
    case GTP_BAK_REF_SEND:
        {
            flp->f_op->llseek(flp, 0, SEEK_SET);
            ret = flp->f_op->read(flp, (char *)refp, ref_len, &flp->f_pos);
            if(ret < 0)
            {
                GTP_ERROR("Read ref file failed, send default bak ref.");
                goto default_bak_ref;
            }
            //checksum ref file
            for (j = 0; j < ref_grps; ++j)
            {
                ref_chksum = 0;
                for(i=0; i<ref_seg_len-2; i+=2)
                {
                    ref_chksum += ((refp[i + j * ref_seg_len]<<8) + refp[i + 1 + j * ref_seg_len]);
                }
            
                GTP_DEBUG("Reference chksum:0x%04X", ref_chksum&0xFF);
                tmp = ref_chksum + (refp[ref_seg_len + j * ref_seg_len -2]<<8) + refp[ref_seg_len + j * ref_seg_len -1];
                if(1 != tmp)
                {
                    GTP_DEBUG("Invalid checksum for reference, reset reference.");
                    memset(&refp[j * ref_seg_len], 0, ref_seg_len);
                    refp[ref_seg_len - 1 + j * ref_seg_len] = 0x01;
                }
                else
                {
                    if (j == (ref_grps - 1))
                    {
                        GTP_INFO("Reference data in %s used.", GTP_BAK_REF_PATH);
                    }
                }
              
            }
            ret = i2c_write_bytes(client, GTP_REG_BAK_REF, refp, ref_len);
            if(-1 == ret)
            {
                GTP_ERROR("Write ref i2c error.");
                ret = FAIL;
                goto exit_ref_proc;
            }
        }
        break;
        
    case GTP_BAK_REF_STORE:
        {
            ret = i2c_read_bytes(client, GTP_REG_BAK_REF, refp, ref_len);
            if(-1 == ret)
            {
                GTP_ERROR("Read ref i2c error.");
                ret = FAIL;
                goto exit_ref_proc;
            }
            flp->f_op->llseek(flp, 0, SEEK_SET);
            flp->f_op->write(flp, (char *)refp, ref_len, &flp->f_pos);
        }
        break;
        
    default:
        GTP_ERROR("Invalid Argument(%d) for backup reference", mode);
        ret = FAIL;
        goto exit_ref_proc;
    }
    
    ret = SUCCESS;
    goto exit_ref_proc;

default_bak_ref:
    for (j = 0; j < ref_grps; ++j)
    {
        memset(&refp[j * ref_seg_len], 0, ref_seg_len);
        refp[j * ref_seg_len + ref_seg_len - 1] = 0x01;  // checksum = 1
    }
    ret = i2c_write_bytes(client, GTP_REG_BAK_REF, refp, ref_len);
    if (flp && !IS_ERR(flp))
    {
        GTP_INFO("Write backup-reference data into %s", GTP_BAK_REF_PATH);
        flp->f_op->llseek(flp, 0, SEEK_SET);
        flp->f_op->write(flp, (char*)refp, ref_len, &flp->f_pos);
    }
    if (ret < 0)
    {
        GTP_ERROR("Failed to load the default backup reference");
        ret = FAIL;
    }
    else
    {
        ret = SUCCESS;
    }
exit_ref_proc:
    if (refp)
    {
        kfree(refp);
    }
    if (flp && !IS_ERR(flp))
    {
        filp_close(flp, NULL);
    }
    return ret;
}

u8 gtp_fw_startup(struct i2c_client *client)
{
    u8 wr_buf[4];
    s32 ret = 0;
    
    //init sw WDT
    wr_buf[0] = 0xAA;
    ret = i2c_write_bytes(client, 0x8041, wr_buf, 1);
    if (ret < 0)
    {
        GTP_ERROR("I2C error to firmware startup.");
        return FAIL;
    }
    //release SS51 & DSP
    wr_buf[0] = 0x00;
    i2c_write_bytes(client, 0x4180, wr_buf, 1);
    
    //int sync
    gtp_int_sync(25);
    
    //check fw run status
    i2c_read_bytes(client, 0x8041, wr_buf, 1);
    if(0xAA == wr_buf[0])
    {
        GTP_ERROR("IC works abnormally,startup failed.");
        return FAIL;
    }
    else
    {
        GTP_DEBUG("IC works normally,Startup success.");
        wr_buf[0] = 0xAA;
        i2c_write_bytes(client, 0x8041, wr_buf, 1);
        return SUCCESS;
    }
}


static void gtp_recovery_reset(struct i2c_client *client)
{
    if(tpd_halt == 0)
    {
#if GTP_ESD_PROTECT
    gtp_esd_switch(client, SWITCH_OFF);
#endif
    force_reset_guitar();
#if GTP_ESD_PROTECT
    gtp_esd_switch(client, SWITCH_ON);
#endif
    }
}

static u8 gtp_check_clk_legality(u8 *p_clk_buf)
{
    u8 i = 0;
    u8 clk_chksum = p_clk_buf[5];
    
    for(i = 0; i < 5; i++)
    {
        if((p_clk_buf[i] < 50) || (p_clk_buf[i] > 120) ||
            (p_clk_buf[i] != p_clk_buf[0]))
        {
            break;
        }
        clk_chksum += p_clk_buf[i];
    }
    
    if((i == 5) && (clk_chksum == 0))
    {
        GTP_DEBUG("Valid main clock data.");
        return SUCCESS;
    }
    GTP_ERROR("Invalid main clock data.");
    return FAIL;
}


static u8 gtp_main_clk_proc(struct i2c_client *client)
{
    s32 ret = 0;
    u8  i = 0;
    u8  clk_cal_result = 0;
    u8  clk_chksum = 0;
    u8  gtp_clk_buf[6] = {0};
    struct file *flp = NULL;
    
    GTP_DEBUG("[gtp_main_clk_proc]Waiting for FS %d", gtp_ref_retries);        
    if (gup_check_fs_mounted("/data") == FAIL)        
    {            
        GTP_DEBUG("[gtp_main_clk_proc]/data not mounted");
        if(gtp_clk_retries++ < GTP_CHK_FS_MNT_MAX)
        {
            return FAIL;
        }
        else
        {
            GTP_ERROR("[gtp_main_clk_proc]Wait for file system timeout,need cal clk");
        }
    }
    else
    {
        GTP_DEBUG("[gtp_main_clk_proc]/data mounted !!!!");
        flp = filp_open(GTP_MAIN_CLK_PATH, O_RDWR | O_CREAT, 0666);
        if (!IS_ERR(flp))
        {
            flp->f_op->llseek(flp, 0, SEEK_SET);
            ret = flp->f_op->read(flp, (char *)gtp_clk_buf, 6, &flp->f_pos);
            if(ret > 0)
            {
                ret = gtp_check_clk_legality(gtp_clk_buf);
                if(SUCCESS == ret)
                {
                        GTP_DEBUG("[gtp_main_clk_proc]Open & read & check clk file success.");
                    goto send_main_clk;
                }
            }
        }
        GTP_ERROR("[gtp_main_clk_proc]Check clk file failed,need cal clk");
    }
    
    //cal clk
#if GTP_ESD_PROTECT
    gtp_esd_switch(client, SWITCH_OFF);
#endif
    clk_cal_result = gup_clk_calibration();
    force_reset_guitar();
    GTP_DEBUG("&&&&&&&&&&clk cal result:%d", clk_cal_result);
    
#if GTP_ESD_PROTECT
    gtp_esd_switch(client, SWITCH_ON);
#endif  

    if(clk_cal_result < 50 || clk_cal_result > 120)
    {
        GTP_ERROR("Invalid main clock: %d", clk_cal_result);
        ret = FAIL;
        goto exit_clk_proc;
    }
    
    for(i = 0;i < 5; i++)
    {
        gtp_clk_buf[i] = clk_cal_result;
        clk_chksum += gtp_clk_buf[i];
    }
    gtp_clk_buf[5] = 0 - clk_chksum;
    
send_main_clk:
    
    ret = i2c_write_bytes(client, 0x8020, gtp_clk_buf, 6);
    
    if (flp && !IS_ERR(flp))
    {
        flp->f_op->llseek(flp, 0, SEEK_SET);
        flp->f_op->write(flp, (char *)gtp_clk_buf, 6, &flp->f_pos);
    }
    
    if(-1 == ret)
    {
        GTP_ERROR("[gtp_main_clk_proc]send main clk i2c error!");
        ret = FAIL;
    }
    else
    {
        ret = SUCCESS;
    }
    
exit_clk_proc:
    if (flp && !IS_ERR(flp))
    {
        filp_close(flp, NULL);
    }
    return ret;
}

#endif

//*******************HQ  proc info*****************//
#define HQ_DBG_CTP_INFO
#ifdef HQ_DBG_CTP_INFO

static ssize_t proc_get_ctp_firmware_id(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
     char *page = NULL;
    char *ptr = NULL;
    char temp_data[GTP_CONFIG_MAX_LENGTH + 2] = {0};
	char temp_sensor_id=0;
    int i, len, err = -1;

	  page = kmalloc(PAGE_SIZE, GFP_KERNEL);	
	  if (!page) 
	  {		
		kfree(page);		
		return -ENOMEM;	
	  }

    ptr = page; 
	if (*ppos)  // CMD call again
	{
		return 0;
	}
    /* Touch PID & VID */
    ptr += sprintf(ptr, "==== GT9XX Version ID ====\n");
    i2c_read_bytes(i2c_client_point, GTP_REG_VERSION, temp_data, 6);
	i2c_read_bytes(i2c_client_point, GTP_REG_SENSOR_ID, &temp_sensor_id, 1);
    ptr += sprintf(ptr, "Chip PID: %c%c%c  VID: 0x%02X%02X  SENSOR_ID: %d\n", temp_data[0], temp_data[1], temp_data[2], temp_data[5], temp_data[4],temp_sensor_id);
    len = ptr - page; 			 	
    if(*ppos >= len)
	  {		
		  kfree(page); 		
		  return 0; 	
	  }	
	  err = copy_to_user(buffer,(char *)page,len); 			
	  *ppos += len; 	
	  if(err) 
	  {		
	    kfree(page); 		
		  return err; 	
	  }	
	  kfree(page); 	
	  return len;	
	
}

// used for gesture function switch
static ssize_t gt91xxgc_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos)
{
	char *ptr = page;
	int i;
	if (*ppos)  // CMD call again
		return 0;

	ptr += sprintf(ptr, "%d\n", g_wakeup_gesture);

	*ppos += ptr - page;
	return (ptr - page);
}

static ssize_t gt91xxgc_config_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
	char wtire_data[32] = {0};

	if (size >= 32)
		return -EFAULT;

	if (copy_from_user( &wtire_data, buff, size ))
		return -EFAULT;

	if (wtire_data[0] == '1'){
		g_wakeup_gesture = 1;
			draw_gesture = 1;
			}
	else{
		g_wakeup_gesture = 0;
        	draw_gesture = 0;
    }
	printk("zjh: %s, wakeup_gesture = %d\n", __func__, g_wakeup_gesture);

	return size;
}

static ssize_t gt91xxge_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos)
{
	char *ptr = page;
	int i;
	if (*ppos)  // CMD call again
		return 0;

	ptr += sprintf(ptr, "%d\n", g_wakeup_gesture);

	*ppos += ptr - page;
	return (ptr - page);
}

static ssize_t gt91xxge_config_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
	char wtire_data[32] = {0};

	if (size >= 32)
		return -EFAULT;

	if (copy_from_user( &wtire_data, buff, size ))
		return -EFAULT;

	if (wtire_data[0] == '1'){
		g_wakeup_gesture = 1;
			draw_gesture = 1;
			}
	else{
		g_wakeup_gesture = 0;
        	draw_gesture = 0;
    }
	printk("zjh: %s, wakeup_gesture = %d\n", __func__, g_wakeup_gesture);

	return size;
}

static ssize_t gt91xxgw_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos)
{
	char *ptr = page;
	int i;
	if (*ppos)  // CMD call again
		return 0;

	ptr += sprintf(ptr, "%d\n", g_wakeup_gesture);

	*ppos += ptr - page;
	return (ptr - page);
}

static ssize_t gt91xxgw_config_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
	char wtire_data[32] = {0};

	if (size >= 32)
		return -EFAULT;

	if (copy_from_user( &wtire_data, buff, size ))
		return -EFAULT;

	if (wtire_data[0] == '1'){
		g_wakeup_gesture = 1;
			draw_gesture = 1;
			}
	else{
		g_wakeup_gesture = 0;
        	draw_gesture = 0;
    }
	printk("zjh: %s, wakeup_gesture = %d\n", __func__, g_wakeup_gesture);

	return size;
}

static ssize_t gt91xxgm_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos)
{
	char *ptr = page;
	int i;
	if (*ppos)  // CMD call again
		return 0;

	ptr += sprintf(ptr, "%d\n", g_wakeup_gesture);

	*ppos += ptr - page;
	return (ptr - page);
}

static ssize_t gt91xxgm_config_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
	char wtire_data[32] = {0};

	if (size >= 32)
		return -EFAULT;

	if (copy_from_user( &wtire_data, buff, size ))
		return -EFAULT;

	if (wtire_data[0] == '1'){
		g_wakeup_gesture = 1;
			draw_gesture = 1;
			}
	else{
		g_wakeup_gesture = 0;
        	draw_gesture = 0;
    }
	printk("zjh: %s, wakeup_gesture = %d\n", __func__, g_wakeup_gesture);

	return size;
}

static ssize_t gt91xxgcc_config_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos)
{
	char *ptr = page;
	int i;
	if (*ppos)  // CMD call again
		return 0;

	ptr += sprintf(ptr, "%d\n", g_wakeup_gesture);

	*ppos += ptr - page;
	return (ptr - page);
}

static ssize_t gt91xxgcc_config_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
	char wtire_data[32] = {0};

	if (size >= 32)
		return -EFAULT;

	if (copy_from_user( &wtire_data, buff, size ))
		return -EFAULT;

	if (wtire_data[0] == '1'){
		  double_gesture = 1;
			}
	else{
		  double_gesture = 0;
    }
	printk("zjh: %s, wakeup_gesture = %d\n", __func__, g_wakeup_gesture);

	return size;
}


#ifdef GTP_HUAWEI_DESIGN
static ssize_t huawei_gesture_switch_read_proc(struct file *file, char __user *page, size_t size, loff_t *ppos)
{
	char *ptr = page;
	int i;
	if (*ppos)  // CMD call again
		return 0;

	ptr += sprintf(ptr, "%ld\n", huawei_gesture);

	*ppos += ptr - page;
	return (ptr - page);
}

static ssize_t huawei_gesture_switch_write_proc(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
	char wtire_data[32] = {0};

	if (size >= 32)
		return -EFAULT;

	if (copy_from_user( &wtire_data, buff, size ))
		return -EFAULT;

	huawei_gesture= simple_strtoul(wtire_data,NULL,10);

	if((huawei_gesture & 1) || (huawei_gesture & 0x2000)){
		g_wakeup_gesture = 1;
	}else{
		g_wakeup_gesture = 0;
	}
	
	if(huawei_gesture & 1)
		double_gesture = 1;
	else
		double_gesture = 0;

	if(huawei_gesture & 0x2000)
		draw_gesture = 1;
	else
		draw_gesture = 0;

	printk("zjh: %s, allen wy g_wakeup_gesture = %d huawei_gesture = %ld double_gesture = %d draw_gesture = %d\n", __func__, g_wakeup_gesture,huawei_gesture,double_gesture,draw_gesture);
	
	return size;
}
#endif
#endif

//************* For GT9XXF End **********************//

#if GTP_WITH_PEN
static void gtp_pen_init(void)
{
    s32 ret = 0;
    
    pen_dev = input_allocate_device();
    if (pen_dev == NULL)
    {
        GTP_ERROR("Failed to allocate input device for pen/stylus.");
        return;
    }
    
    pen_dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) ;
    pen_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
    
    set_bit(BTN_TOOL_PEN, pen_dev->keybit);
    set_bit(INPUT_PROP_DIRECT, pen_dev->propbit);
    //set_bit(INPUT_PROP_POINTER, pen_dev->propbit);
    
#if GTP_PEN_HAVE_BUTTON
    input_set_capability(pen_dev, EV_KEY, BTN_STYLUS);
    input_set_capability(pen_dev, EV_KEY, BTN_STYLUS2);
#endif

    input_set_abs_params(pen_dev, ABS_MT_POSITION_X, 0, TPD_RES_X, 0, 0);
    input_set_abs_params(pen_dev, ABS_MT_POSITION_Y, 0, TPD_RES_Y, 0, 0);
    input_set_abs_params(pen_dev, ABS_MT_PRESSURE, 0, 255, 0, 0);
    input_set_abs_params(pen_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
    input_set_abs_params(pen_dev, ABS_MT_TRACKING_ID, 0, 255, 0, 0);
    
    pen_dev->name = "mtk-pen";
    pen_dev->phys = "input/ts";
    pen_dev->id.bustype = BUS_I2C;
    
    ret = input_register_device(pen_dev);
    if (ret)
    {
        GTP_ERROR("Register %s input device failed", pen_dev->name);
        return;
    }
}

static void gtp_pen_down(s32 x, s32 y, s32 size, s32 id)
{
    input_report_key(pen_dev, BTN_TOOL_PEN, 1);
    input_report_key(pen_dev, BTN_TOUCH, 1);
    input_report_abs(pen_dev, ABS_MT_POSITION_X, x);
    input_report_abs(pen_dev, ABS_MT_POSITION_Y, y);
    if ((!size) && (!id))
    {
        input_report_abs(pen_dev, ABS_MT_PRESSURE, 100);
        input_report_abs(pen_dev, ABS_MT_TOUCH_MAJOR, 100);
    }
    else
    {
        input_report_abs(pen_dev, ABS_MT_PRESSURE, size);
        input_report_abs(pen_dev, ABS_MT_TOUCH_MAJOR, size);
        input_report_abs(pen_dev, ABS_MT_TRACKING_ID, id);
    }
    input_mt_sync(pen_dev);
}

static void gtp_pen_up(void)
{
    input_report_key(pen_dev, BTN_TOOL_PEN, 0);
    input_report_key(pen_dev, BTN_TOUCH, 0);
}
#endif

static s32 tpd_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    s32 err = 0;
    s32 ret = 0;

    u16 version_info;
#if GTP_HAVE_TOUCH_KEY
    s32 idx = 0;
#endif
#ifdef TPD_PROXIMITY
    struct hwmsen_object obj_ps;
#endif

    i2c_client_point = client;
    ret = tpd_power_on(client);

    if (ret < 0)
    {
        GTP_ERROR("I2C communication ERROR!");
		return -1;   //add yudengwu
    }
    
client->timing=100;

#ifdef VELOCITY_CUSTOM

    if ((err = misc_register(&tpd_misc_device)))
    {
        printk("mtk_tpd: tpd_misc_device register failed\n");
    }

#endif
    ret = gtp_read_version(client, &version_info);

    if (ret < 0)
    {
        GTP_ERROR("Read version failed.");
    }    

    ret = gtp_init_panel(client);

    if (ret < 0)
    {
        GTP_ERROR("GTP init panel failed.");
    }
    
    // Create proc file system
/*
#ifdef GTP_HUAWEI_DESIGN
    //add begin
   gt9xx_echo_proc = proc_create(GT9XX_ECHO_PROC_FILE, 0666, NULL, &configecho_proc_ops);
	if (gt9xx_echo_proc == NULL)
	{
		GTP_ERROR("create_proc_entry %s failed\n", GT9XX_ECHO_PROC_FILE);
	}
	else
	{
		GTP_INFO("create proc entry %s success", GT9XX_ECHO_PROC_FILE);
	}
	//add end
#endif
*/
    gt91xx_config_proc = proc_create(GT91XX_CONFIG_PROC_FILE, 0664, NULL, &config_proc_ops);
    if (gt91xx_config_proc == NULL)
    {
        GTP_ERROR("create_proc_entry %s failed\n", GT91XX_CONFIG_PROC_FILE);
    }
    else
    {
        GTP_INFO("create proc entry %s success", GT91XX_CONFIG_PROC_FILE);
    }

#if GTP_CREATE_WR_NODE
    init_wr_node(client);
#endif

    thread = kthread_run(touch_event_handler, 0, TPD_DEVICE);

    if (IS_ERR(thread))
    {
        err = PTR_ERR(thread);
        GTP_INFO(TPD_DEVICE " failed to create kernel thread: %d\n", err);
    }
    
    
#if GTP_HAVE_TOUCH_KEY

    for (idx = 0; idx < GTP_MAX_KEY_NUM; idx++)
    {
        input_set_capability(tpd->dev, EV_KEY, touch_key_array[idx]);
    }

#endif
#if GTP_GESTURE_WAKEUP
    input_set_capability(tpd->dev, EV_KEY, KEY_POWER);
    input_set_capability(tpd->dev, EV_KEY, KEY_F1);
    input_set_capability(tpd->dev, EV_KEY, KEY_F2);
    input_set_capability(tpd->dev, EV_KEY, KEY_F3);
    input_set_capability(tpd->dev, EV_KEY, KEY_F4);
    input_set_capability(tpd->dev, EV_KEY, KEY_F5);
    input_set_capability(tpd->dev, EV_KEY, KEY_F6);
    input_set_capability(tpd->dev, EV_KEY, KEY_F7);
    input_set_capability(tpd->dev, EV_KEY, KEY_F8);
    input_set_capability(tpd->dev, EV_KEY, KEY_SCCHANGED);
    input_set_capability(tpd->dev, EV_KEY, KEY_F9);
    input_set_capability(tpd->dev, EV_KEY, KEY_F10);
    input_set_capability(tpd->dev, EV_KEY, KEY_F11);
    input_set_capability(tpd->dev, EV_KEY, KEY_F12);
	
	__set_bit(KEY_SCHANGED, tpd->dev->keybit);
	__set_bit(KEY_SCCHANGED, tpd->dev->keybit);  //liguangxian 20140913
	__set_bit(KEY_SMCHANGED, tpd->dev->keybit);
	__set_bit(KEY_SWCHANGED, tpd->dev->keybit);  //liguangxian 20140913   
	__set_bit(KEY_SCCCHANGED, tpd->dev->keybit);
	__set_bit(EV_SYN, tpd->dev->evbit);
#endif
    
#if GTP_WITH_PEN
    gtp_pen_init();
#endif
    // set INT mode
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_DISABLE);

    msleep(50);

//#ifdef MT6572

#if 1
    if (!int_type)  //EINTF_TRIGGER
    {
        mt_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, EINTF_TRIGGER_RISING, tpd_eint_interrupt_handler, 1);
    }
    else
    {
        mt_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, EINTF_TRIGGER_FALLING, tpd_eint_interrupt_handler, 1);
    }
    
#else
    mt65xx_eint_set_sens(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_SENSITIVE);
    mt65xx_eint_set_hw_debounce(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_CN);

    if (!int_type)
    {
        mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_POLARITY_HIGH, tpd_eint_interrupt_handler, 1);
    }
    else
    {
        mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_POLARITY_LOW, tpd_eint_interrupt_handler, 1);
    }
#endif

    mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);

#if GTP_ESD_PROTECT
    gtp_esd_switch(client, SWITCH_ON);
#endif

#if GTP_AUTO_UPDATE
    ret = gup_init_update_proc(client);

    if (ret < 0)
    {
        GTP_ERROR("Create update thread error.");
    }
#endif

#ifdef HQ_DBG_CTP_INFO
static struct hq_dbg_entry ctp_proc_entry[]=
   {
   {
			.name="firmware_id",
			{	.owner = THIS_MODULE,
				.read=proc_get_ctp_firmware_id,
				.write=NULL,
			} 
		},

#ifdef GTP_HUAWEI_DESIGN
	{
	   .name="g_huawei_control",	
          {
	    .owner = THIS_MODULE,
	    .read  = huawei_gesture_switch_read_proc,
	    .write = huawei_gesture_switch_write_proc,
	  }
	},
	{
	   .name="gesture_echo",	
          {
	    .owner = THIS_MODULE,
	    .read  = gt9xx_echo_read_proc,
	    .write = gt9xx_echo_write_proc,
	  }
	},
#endif
#ifdef HQ_GTP_GLOVE
		{
			.name = "glove_switch",
			{
				.owner = THIS_MODULE,
				.read  = glove_switch_read_proc,
				.write = glove_switch_write_proc,
			}
		},
#endif
	};
	//hq_gt9xx_client=client;
	proc_hq_dbg_add("ctp", ctp_proc_entry, sizeof(ctp_proc_entry)/sizeof(struct hq_dbg_entry));
		gt91xxgc_config_proc = proc_create(GT91XXGC_CONFIG_PROC_FILE, 0666, NULL, &configgc_proc_ops);
	if (gt91xxgc_config_proc == NULL)
	{
		printk("create_proc_entry %s failed\n", GT91XXGC_CONFIG_PROC_FILE);
	}
	else
	{
		printk("create proc entry %s success", GT91XXGC_CONFIG_PROC_FILE);
	}
		gt91xxge_config_proc = proc_create(GT91XXGE_CONFIG_PROC_FILE, 0666, NULL, &configge_proc_ops);
	if (gt91xxge_config_proc == NULL)
	{
		printk("create_proc_entry %s failed\n", GT91XXGE_CONFIG_PROC_FILE);
	}
	else
	{
		printk("create proc entry %s success", GT91XXGE_CONFIG_PROC_FILE);
	}
		gt91xxgw_config_proc = proc_create(GT91XXGW_CONFIG_PROC_FILE, 0666, NULL, &configgw_proc_ops);
	if (gt91xxgw_config_proc == NULL)
	{
		printk("create_proc_entry %s failed\n", GT91XXGW_CONFIG_PROC_FILE);
	}
	else
	{
		printk("create proc entry %s success", GT91XXGW_CONFIG_PROC_FILE);
	}
		gt91xxgm_config_proc = proc_create(GT91XXGM_CONFIG_PROC_FILE, 0666, NULL, &configgm_proc_ops);
	if (gt91xxgm_config_proc == NULL)
	{
		printk("create_proc_entry %s failed\n", GT91XXGM_CONFIG_PROC_FILE);
	}
	else
	{
		printk("create proc entry %s success", GT91XXGM_CONFIG_PROC_FILE);
	}
		gt91xxgcc_config_proc = proc_create(GT91XXGCC_CONFIG_PROC_FILE, 0666, NULL, &configgcc_proc_ops);
	if (gt91xxgcc_config_proc == NULL)
	{
		printk("create_proc_entry %s failed\n", GT91XXGCC_CONFIG_PROC_FILE);
	}
	else
	{
		printk("create proc entry %s success", GT91XXGCC_CONFIG_PROC_FILE);
	}
		gt9xx_echo_proc = proc_create(GT9XX_ECHO_PROC_FILE, 0666, NULL, &configecho_proc_ops);
	if (gt9xx_echo_proc == NULL)
	{
		GTP_ERROR("create_proc_entry %s failed\n", GT9XX_ECHO_PROC_FILE);
	}
	else
	{
		GTP_INFO("create proc entry %s success", GT9XX_ECHO_PROC_FILE);
	}

#endif	
#ifdef TPD_PROXIMITY
    //obj_ps.self = cm3623_obj;
    obj_ps.polling = 0;         //0--interrupt mode;1--polling mode;
    obj_ps.sensor_operate = tpd_ps_operate;

    if ((err = hwmsen_attach(ID_PROXIMITY, &obj_ps)))
    {
        GTP_ERROR("hwmsen attach fail, return:%d.", err);
    }

#endif

    return 0;
}

static void tpd_eint_interrupt_handler(void)
{
    TPD_DEBUG_PRINT_INT;
    
    tpd_flag = 1;
    
    wake_up_interruptible(&waiter);
}
static int tpd_i2c_remove(struct i2c_client *client)
{
#if GTP_CREATE_WR_NODE
    uninit_wr_node();
#endif

#if GTP_ESD_PROTECT
    destroy_workqueue(gtp_esd_check_workqueue);
#endif

    return 0;
}
#if (GTP_ESD_PROTECT || GTP_COMPATIBLE_MODE)
static void force_reset_guitar(void)
{
    s32 i = 0;
    s32 ret = 0;

    GTP_INFO("force_reset_guitar");
    
    mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);

    GTP_GPIO_OUTPUT(GTP_RST_PORT, 0);   
    GTP_GPIO_OUTPUT(GTP_INT_PORT, 0);
#ifdef MT6573
    //Power off TP
    mt_set_gpio_mode(GPIO_CTP_EN_PIN, GPIO_CTP_EN_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EN_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EN_PIN, GPIO_OUT_ZERO);  
    msleep(30);
    //Power on TP
    mt_set_gpio_out(GPIO_CTP_EN_PIN, GPIO_OUT_ONE);
    msleep(30);
#else           // ( defined(MT6575) || defined(MT6577) || defined(MT6589) )
    // Power off TP
    #ifdef TPD_POWER_SOURCE_CUSTOM
        hwPowerDown(TPD_POWER_SOURCE_CUSTOM, "TP");
    #else
        hwPowerDown(MT65XX_POWER_LDO_VGP2, "TP");
    #endif
        msleep(30); 

    // Power on TP
    #ifdef TPD_POWER_SOURCE_CUSTOM
        hwPowerOn(TPD_POWER_SOURCE_CUSTOM, VOL_2800, "TP");
    #else
        hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
    #endif
        msleep(30);

#endif

    for (i = 0; i < 5; i++)
    {
    #if GTP_COMPATIBLE_MODE
        if (CHIP_TYPE_GT9F == gtp_chip_type)
        {
            ret = gup_fw_download_proc(NULL, GTP_FL_ESD_RECOVERY);
            if(FAIL == ret)
            {
                GTP_ERROR("[force_reset_guitar]Check & repair fw failed.");
                continue;
            }
            //startup fw
            ret = gtp_fw_startup(i2c_client_point);
            if(FAIL == ret)
            {
                GTP_ERROR("[force_reset_guitar]Startup fw failed.");
                continue;
            }
            break;
        }
        else
    #endif
        {
            //Reset Guitar
            gtp_reset_guitar(i2c_client_point, 20);
            msleep(50);
            //Send config
            ret = gtp_send_cfg(i2c_client_point);
    
            if (ret < 0)
            {
                continue;
            }
        }
        break;
    }
    mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
    
    if (i >= 5)
    {
        GTP_ERROR("Failed to reset guitar.");
        return;
    }
    GTP_INFO("Esd recovery successful");
    return;
}
#endif

#if GTP_ESD_PROTECT
static s32 gtp_init_ext_watchdog(struct i2c_client *client)
{
    u8 opr_buffer[2] = {0xAA};
    GTP_DEBUG("Init external watchdog.");
    return i2c_write_bytes(client, 0x8041, opr_buffer, 1);
}

void gtp_esd_switch(struct i2c_client *client, s32 on)
{
    spin_lock(&esd_lock);     
    if (SWITCH_ON == on)     // switch on esd 
    {
        if (!esd_running)
        {
            esd_running = 1;
            spin_unlock(&esd_lock);
            GTP_INFO("Esd started");
            queue_delayed_work(gtp_esd_check_workqueue, &gtp_esd_check_work, clk_tick_cnt);
        }
        else
        {
            spin_unlock(&esd_lock);
        }
    }
    else    // switch off esd
    {
        if (esd_running)
        {
            esd_running = 0;
            spin_unlock(&esd_lock);
            GTP_INFO("Esd cancelled");
            cancel_delayed_work_sync(&gtp_esd_check_work);
        }
        else
        {
            spin_unlock(&esd_lock);
        }
    }
}


static void gtp_esd_check_func(struct work_struct *work)
{
    s32 i = 0;
    s32 ret = -1;
    u8 esd_buf[3] = {0x00};
    if ((tpd_halt) || (gtp_loading_fw))
    {
        GTP_INFO("Esd suspended or IC update firmware!");
        return;
    }
    for (i = 0; i < 3; i++)
    {
        ret = i2c_read_bytes_non_dma(i2c_client_point, 0x8040, esd_buf, 2);
        
        GTP_DEBUG("[Esd]0x8040 = 0x%02X, 0x8041 = 0x%02X", esd_buf[0], esd_buf[1]);
        if (ret < 0)
        {
            // IIC communication problem
            continue;
        }
        else 
        {
            if ((esd_buf[0] == 0xAA) || (esd_buf[1] != 0xAA))
            {
                u8 chk_buf[2] = {0x00};
                i2c_read_bytes_non_dma(i2c_client_point, 0x8040, chk_buf, 2);
                
                GTP_DEBUG("[Check]0x8040 = 0x%02X, 0x8041 = 0x%02X", chk_buf[0], chk_buf[1]);
                
                if ( (chk_buf[0] == 0xAA) || (chk_buf[1] != 0xAA) )
                {
                    i = 3;          // jump to reset guitar
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                // IC works normally, Write 0x8040 0xAA, feed the watchdog
                esd_buf[0] = 0xAA;
                i2c_write_bytes_non_dma(i2c_client_point, 0x8040, esd_buf, 1);
                
                break;
            }
        }
    }

    if (i >= 3)
    {   
    #if GTP_COMPATIBLE_MODE
        if ((CHIP_TYPE_GT9F == gtp_chip_type) && (1 == rqst_processing))
        {
            GTP_INFO("Request Processing, no reset guitar.");
        }
        else
    #endif
        {
            GTP_INFO("IC works abnormally! Process reset guitar.");
            esd_buf[0] = 0x01;
            esd_buf[1] = 0x01;
            esd_buf[2] = 0x01;
            i2c_write_bytes(i2c_client_point, 0x4226, esd_buf, 3);  
            msleep(50);
            force_reset_guitar();
        }
    }

    if (!tpd_halt)
    {
        queue_delayed_work(gtp_esd_check_workqueue, &gtp_esd_check_work, clk_tick_cnt);
    }
    else
    {
        GTP_INFO("Esd suspended!");
    }

    return;
}
#endif

static void tpd_down(s32 x, s32 y, s32 size, s32 id)
{
    if ((!size) && (!id))
    {
        input_report_abs(tpd->dev, ABS_MT_PRESSURE, 100);
        input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, 100);
    }
    else
    {
        input_report_abs(tpd->dev, ABS_MT_PRESSURE, size);
        input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, size);
        /* track id Start 0 */
        input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, id);
    }

    input_report_key(tpd->dev, BTN_TOUCH, 1);
    input_report_abs(tpd->dev, ABS_MT_POSITION_X, x);
    input_report_abs(tpd->dev, ABS_MT_POSITION_Y, y);
    input_mt_sync(tpd->dev);
    TPD_EM_PRINT(x, y, x, y, id, 1);

#if (defined(MT6575)||defined(MT6577))

    if (FACTORY_BOOT == get_boot_mode() || RECOVERY_BOOT == get_boot_mode())
    {
        tpd_button(x, y, 1);
    }

#endif
#ifdef TPD_HAVE_BUTTON

    if (FACTORY_BOOT == get_boot_mode() || RECOVERY_BOOT == get_boot_mode())
    {
        tpd_button(x, y, 1);
    }

#endif
}

static void tpd_up(s32 x, s32 y, s32 id)
{
    input_report_key(tpd->dev, BTN_TOUCH, 0);
    input_mt_sync(tpd->dev);
    TPD_EM_PRINT(x, y, x, y, id, 0);

#if (defined(MT6575) || defined(MT6577))

    if (FACTORY_BOOT == get_boot_mode() || RECOVERY_BOOT == get_boot_mode())
    {
        tpd_button(x, y, 0);
    }

#endif
#ifdef TPD_HAVE_BUTTON

    if (FACTORY_BOOT == get_boot_mode() || RECOVERY_BOOT == get_boot_mode())
    {
        tpd_button(x, y, 0);
    }

#endif
}
#if GTP_CHARGER_SWITCH
static void gtp_charger_switch(s32 dir_update)
{
    u32 chr_status = 0;
    u8 chr_cmd[3] = {0x80, 0x40};
    static u8 chr_pluggedin = 0;
    int ret = 0;
    
#ifdef MT6573
    chr_status = *(volatile u32 *)CHR_CON0;
    chr_status &= (1 << 13);
#else   // ( defined(MT6575) || defined(MT6577) || defined(MT6589) )
    chr_status = upmu_is_chr_det();
#endif
    
    if (chr_status)     // charger plugged in
    {
        if (!chr_pluggedin || dir_update)
        {
            chr_cmd[2] = 6;
            ret = gtp_i2c_write(i2c_client_point, chr_cmd, 3);
            if (ret > 0)
            {
                GTP_INFO("Update status for Charger Plugin");
				if (gtp_send_chr_cfg(i2c_client_point) < 0) {
					GTP_ERROR("Send charger config failed.");
				} else {
					GTP_DEBUG("Send charger config.");
				}
            }
            chr_pluggedin = 1;
        }
    }
    else            // charger plugged out
    {
        if (chr_pluggedin || dir_update)
        {
            chr_cmd[2] = 7;
            ret = gtp_i2c_write(i2c_client_point, chr_cmd, 3);
            if (ret > 0)
            {
                GTP_INFO("Update status for Charger Plugout");
				if (gtp_send_cfg(i2c_client_point) < 0) {
					GTP_ERROR("Send normal config failed.");
				} else {
					GTP_DEBUG("Send normal config.");
				}
            }
            chr_pluggedin = 0;
        }
    }
}
#endif
#define GTP_LARGE_TOUCH 1

static int check_maxdata(int *data)
{
		int index = 0,i;
		int *a;
			a = data;
		if((a[0]>a[1])&&(a[0]>a[2])&&(a[0]>a[3]))
			return 0;
		else if((a[1]>a[2])&&(a[1]>a[3]))
			return 1;
		else if(a[2]>a[3])	
			return 2;
		else	
			return 3;
}

static int check_mindata(int *data)
{
		int index = 0,i;
		int *a;
			a = data;
		
		if((a[0]<a[1])&&(a[0]<a[2])&&(a[0]<a[3]))
			return 0;
		else if((a[1]<a[2])&&(a[1]<a[3]))
			return 1;
		else if(a[2]<a[3])	
			return 2;
		else	
			return 3;
}
static int touch_event_handler(void *unused)
{
    struct sched_param param = { .sched_priority = RTPM_PRIO_TPD };
    u8  end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
    u8  point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
    u8  touch_num = 0;
    u8  finger = 0;
    static u8 pre_touch = 0;
    static u8 pre_key = 0;
#if GTP_LARGE_TOUCH
	static u8 large_touch = 0;
#endif 

#if GTP_WITH_PEN
    u8 pen_active = 0;
    static u8 pre_pen = 0;
#endif
    u8  key_value = 0;
    u8 *coor_data = NULL;
    s32 input_x = 0;
    s32 input_y = 0;
    s32 input_w = 0;
    s32 id = 0;
    s32 i  = 0;
    s32 ret = -1;
    int x_gesture[4]={};
    int y_gesture[4]={};
    s32 x_max,y_max,x_min,y_min,m;
  #ifdef GTP_HUAWEI_DESIGN
    u8 doze_buf[3] = {0x81, 0x4B};
    u8 gesture_start_point[6] = {0x81,0x4D};    //
    u8 gesture_end_point[6] = {0x81,0x51};      //
    u8 gesture_width_height[6] = {0x81, 0x55};  ///
    u8 gesture_mid_point[6] = {0x81, 0x59};     //
    u8 gesture_p1_point[6] = {0x81, 0x5D};     //
    u8 gesture_p2_point[6] = {0x81, 0x61};     //
    u8 gesture_p3_point[6] = {0x81, 0x65};     //
    u8 gesture_p4_point[6] = {0x81, 0x69};     //
    u32 gesture_start_point_x,gesture_start_point_y;
    u32 gesture_end_point_x,gesture_end_point_y;
    u32 gesture_width,gesture_height;
    u32 gesture_mid_point_x,gesture_mid_point_y;
    u32 gesture_p1_point_x,gesture_p1_point_y;
    u32 gesture_p2_point_x,gesture_p2_point_y;
    u32 gesture_p3_point_x,gesture_p3_point_y;
    u32 gesture_p4_point_x,gesture_p4_point_y;
#endif

#if GTP_COMPATIBLE_MODE
    u8  rqst_data[3] = {(u8)(GTP_REG_RQST >> 8), (u8)(GTP_REG_RQST & 0xFF), 0};
#endif

#ifdef TPD_PROXIMITY
    s32 err = 0;
    hwm_sensor_data sensor_data;
    u8 proximity_status;
#endif

#if GTP_GESTURE_WAKEUP
    //u8 doze_buf[3] = {0x81, 0x4B};
    u8 tell_buf[3]={0x81,0x4D};
#endif

    sched_setscheduler(current, SCHED_RR, &param);
    do
    {
        set_current_state(TASK_INTERRUPTIBLE);
        
        while (tpd_halt)
        {
        #if GTP_GESTURE_WAKEUP
            if (DOZE_ENABLED == doze_status)
            {
                break;
            }
        #endif
            tpd_flag = 0;
            msleep(20);
        }

        wait_event_interruptible(waiter, tpd_flag != 0);
        tpd_flag = 0;
        TPD_DEBUG_SET_TIME;
        set_current_state(TASK_RUNNING);

    #if GTP_CHARGER_SWITCH
        gtp_charger_switch(0);
    #endif

    #if GTP_GESTURE_WAKEUP
	if (g_wakeup_gesture) {
        if (DOZE_ENABLED == doze_status)
        {
#ifdef GTP_HUAWEI_DESIGN
//add begin
		ret = gtp_i2c_read(i2c_client_point, gesture_start_point, 6);
		gesture_start_point_x   = gesture_start_point[2] | gesture_start_point[3] << 8;
		gesture_start_point_y  = gesture_start_point[4] | gesture_start_point[5] << 8;
		GTP_DEBUG("gesture_start_point_x =0x%02X, gesture_start_point_y =0x%02X", gesture_start_point_x,gesture_start_point_y);

		ret = gtp_i2c_read(i2c_client_point, gesture_end_point, 6);
		gesture_end_point_x   = gesture_end_point[2] | gesture_end_point[3] << 8;
		gesture_end_point_y  = gesture_end_point[4] | gesture_end_point[5] << 8;
		GTP_DEBUG("gesture_end_point_x = 0x%02X, gesture_end_point_y = 0x%02X", gesture_end_point_x,gesture_end_point_y);

		ret = gtp_i2c_read(i2c_client_point, gesture_width_height, 6);
		gesture_width   = gesture_width_height[2] | gesture_width_height[3] << 8;
		gesture_height  = gesture_width_height[4] | gesture_width_height[5] << 8;
		GTP_DEBUG("gesture_width = 0x%02X, gesture_height = 0x%02X", gesture_width,gesture_height);

		ret = gtp_i2c_read(i2c_client_point, gesture_mid_point, 6);
		gesture_mid_point_x   = gesture_mid_point[2] | gesture_mid_point[3] << 8;
		gesture_mid_point_y   = gesture_mid_point[4] | gesture_mid_point[5] << 8;
		GTP_DEBUG("gesture_mid_point_x = 0x%02X, gesture_mid_point_y = 0x%02X", gesture_mid_point_x,gesture_mid_point_y);

		ret = gtp_i2c_read(i2c_client_point, gesture_p1_point, 6);
		gesture_p1_point_x   = gesture_p1_point[2] | gesture_p1_point[3] << 8;
		gesture_p1_point_y   = gesture_p1_point[4] | gesture_p1_point[5] << 8;
		GTP_DEBUG("gesture_p1_point_x = 0x%02X, gesture_p1_point_y = 0x%02X", gesture_p1_point_x,gesture_p1_point_y);

		ret = gtp_i2c_read(i2c_client_point, gesture_p2_point, 6);
		gesture_p2_point_x   = gesture_p2_point[2] | gesture_p2_point[3] << 8;
		gesture_p2_point_y   = gesture_p2_point[4] | gesture_p2_point[5] << 8;
		GTP_DEBUG("gesture_p2_point_x = 0x%02X, gesture_p2_point_y = 0x%02X", gesture_p2_point_x,gesture_p2_point_y);

		ret = gtp_i2c_read(i2c_client_point, gesture_p3_point, 6);
		gesture_p3_point_x   = gesture_p3_point[2] | gesture_p3_point[3] << 8;
		gesture_p3_point_y   = gesture_p3_point[4] | gesture_p3_point[5] << 8;
		GTP_DEBUG("gesture_p3_point_x = 0x%02X, gesture_p3_point_y = 0x%02X", gesture_p3_point_x,gesture_p3_point_y);

		ret = gtp_i2c_read(i2c_client_point, gesture_p4_point, 6);
		gesture_p4_point_x   = gesture_p4_point[2] | gesture_p4_point[3] << 8;
		gesture_p4_point_y   = gesture_p4_point[4] | gesture_p4_point[5] << 8;
		GTP_DEBUG("gesture_p4_point_x = 0x%02X, gesture_p4_point_y = 0x%02X", gesture_p4_point_x,gesture_p4_point_y);

		ret = gtp_i2c_read(i2c_client_point, doze_buf, 3);
		GTP_DEBUG("0x814B = 0x%02X", doze_buf[2]);


		gesture_echo[0]  = doze_buf[2];	
		gesture_echo[1]  = gesture_start_point_x;
		gesture_echo[2]  = gesture_start_point_y;	
		gesture_echo[3]  = gesture_end_point_x;
		gesture_echo[4]  = gesture_end_point_y;	
		gesture_echo[5]  = gesture_width;
		gesture_echo[6]  = gesture_height;	
		gesture_echo[7]  = gesture_mid_point_x;
		gesture_echo[8]  = gesture_mid_point_y;	
		gesture_echo[9]  = gesture_p1_point_x;
		gesture_echo[10] = gesture_p1_point_y;	
		gesture_echo[11] = gesture_p2_point_x;
		gesture_echo[12] = gesture_p2_point_y;	
		gesture_echo[13] = gesture_p3_point_x;
		gesture_echo[14] = gesture_p3_point_y;	
		gesture_echo[15] = gesture_p4_point_x;
		gesture_echo[16] = gesture_p4_point_y;	
#endif
	
            if (ret > 0)
            {    
			#ifdef HQ_GTP_GLOVE
				if (g_glove_switch) {
					//mutex_unlock(&i2c_access);			 
	                if (0xCC == doze_buf[2])
	                {
	                    GTP_INFO("Double click to light up the screen!");
	                    doze_status = DOZE_WAKEUP;
						input_report_key(tpd->dev, KEY_F7, 1);
	                    input_sync(tpd->dev);
	                    input_report_key(tpd->dev, KEY_F7, 0);
	                    input_sync(tpd->dev);
	                    doze_buf[2] = 0x00;
	                    gtp_i2c_write(i2c_client_point, doze_buf, 3);
	                }
				} else 
			#endif
			{
#if defined(GTP_HUAWEI_DESIGN)
				if(double_gesture){
#endif
				if ((0xCC == doze_buf[2]) && (gesture_start_point_x > 116) && (gesture_end_point_x > 116) && (gesture_start_point_x < 604) && (gesture_end_point_x < 604) && (gesture_start_point_y > 123) && (gesture_end_point_y > 123) && (gesture_start_point_y < 1157) && (gesture_end_point_y < 1157))
	              {
	                GTP_INFO("Double click to light up the screen!");
	                 doze_status = DOZE_WAKEUP;
					input_report_key(tpd->dev, KEY_SCCCHANGED, 1);
	                input_sync(tpd->dev);
	                input_report_key(tpd->dev, KEY_SCCCHANGED, 0);
	                input_sync(tpd->dev);
	                doze_buf[2] = 0x00;
	                gtp_i2c_write(i2c_client_point, doze_buf, 3);
					}
#if defined(GTP_HUAWEI_DESIGN)
				}
				if(draw_gesture)
				{
#else
				else
#endif
				 if((doze_buf[2]=='c') )// length and width
			   	{
			   		GTP_INFO("click c!");
					doze_status = DOZE_WAKEUP;
					input_report_key(tpd->dev, KEY_SCCHANGED, 1);
					input_report_key(tpd->dev, KEY_SCCHANGED, 0);
					input_sync(tpd->dev);		
	                // clear 0x814B
	                doze_buf[2] = 0x00;
	                gtp_i2c_write(i2c_client_point, doze_buf, 3);
			   	}

			    else if((doze_buf[2]=='m') && (gesture_width > 116) && (gesture_height > 123))
			   	{
					GTP_INFO("click m!");
					doze_status = DOZE_WAKEUP;
					input_report_key(tpd->dev, KEY_SMCHANGED, 1);
					// input_sync(tpd->dev);
					input_report_key(tpd->dev, KEY_SMCHANGED, 0);
					input_sync(tpd->dev);
					// clear 0x814B
					doze_buf[2] = 0x00;
					gtp_i2c_write(i2c_client_point, doze_buf, 3);
			   	}
			    else if((doze_buf[2]==0x65) && (gesture_width > 116) && (gesture_height > 123))
			    {
     				GTP_INFO(" click e!");
					doze_status = DOZE_WAKEUP;
					input_report_key(tpd->dev, KEY_SCHANGED, 1);
					//input_sync(tpd->dev);
					input_report_key(tpd->dev, KEY_SCHANGED, 0);
					input_sync(tpd->dev);
					// clear 0x814B
					doze_buf[2] = 0x00;
					gtp_i2c_write(i2c_client_point, doze_buf, 3);
			    }  
				else if ((doze_buf[2]==0x77) && (gesture_width > 116) && (gesture_height > 123)) 
				{
					GTP_INFO(" gesture  Draw w!");

					doze_status = DOZE_WAKEUP;
					input_report_key(tpd->dev, KEY_SWCHANGED, 1);
					input_report_key(tpd->dev, KEY_SWCHANGED, 0);
					input_sync(tpd->dev);
					// clear 0x814B
					doze_buf[2] = 0x00;
					gtp_i2c_write(i2c_client_point, doze_buf, 3);				
				} 
/*			    else if(doze_buf[2]=='o')
				{
					GTP_INFO(" gesture  click o!");
					doze_status = DOZE_WAKEUP;                           
					input_report_key(tpd->dev, KEY_F2, 1);
					//input_sync(tpd->dev);
					input_report_key(tpd->dev, KEY_F2, 0);
					input_sync(tpd->dev);
					// clear 0x814B
					doze_buf[2] = 0x00;
					gtp_i2c_write(i2c_client_point, doze_buf, 3);
			   	}
				else if(doze_buf[2] == 0xBB)
			   	{
					GTP_INFO(" LEFT the screen!");
					doze_status = DOZE_WAKEUP;
					input_report_key(tpd->dev, KEY_F4, 1);
					// input_sync(tpd->dev);
					input_report_key(tpd->dev, KEY_F4, 0);
					input_sync(tpd->dev);
					// clear 0x814B
					doze_buf[2] = 0x00;
					gtp_i2c_write(i2c_client_point, doze_buf, 3);
			   	}
				else if(doze_buf[2] == 0xAA)
			   	{
					GTP_INFO(" RIGHT the screen!");
					doze_status = DOZE_WAKEUP;
					input_report_key(tpd->dev, KEY_F5, 1);
					// input_sync(tpd->dev);
					input_report_key(tpd->dev, KEY_F5, 0);
					input_sync(tpd->dev);
					// clear 0x814B
					doze_buf[2] = 0x00;
					gtp_i2c_write(i2c_client_point, doze_buf, 3);
			   	}  
				else if(doze_buf[2] == 0xBA)
			   	{
					GTP_INFO(" up the screen!");
					doze_status = DOZE_WAKEUP;
					input_report_key(tpd->dev, KEY_F3, 1);
					//input_sync(tpd->dev);
					input_report_key(tpd->dev, KEY_F3, 0);
					input_sync(tpd->dev);
					// clear 0x814B
					doze_buf[2] = 0x00;
					gtp_i2c_write(i2c_client_point, doze_buf, 3);
			   	}  
				else if(doze_buf[2] == 0xAB)
			   	{
					GTP_INFO(" DOWN the screen!");
					doze_status = DOZE_WAKEUP;
					input_report_key(tpd->dev, KEY_F8, 1);
					//input_sync(tpd->dev);
					input_report_key(tpd->dev, KEY_F8, 0);
					input_sync(tpd->dev);
					// clear 0x814B
					doze_buf[2] = 0x00;
					gtp_i2c_write(i2c_client_point, doze_buf, 3);
			   	} 
*/			   	
#if defined(GTP_HUAWEI_DESIGN)	
				}	   	
				if(doze_buf[2])
#else
				else 
#endif
				{
                    // clear 0x814B
			GTP_INFO("***************************");
                    doze_buf[2] = 0x00;
                    gtp_i2c_write(i2c_client_point, doze_buf, 3);
                    gtp_enter_doze(i2c_client_point);
                }
			}
            }
            continue;
        }
	}
    #endif
        ret = gtp_i2c_read(i2c_client_point, point_data, 12);
        if (ret < 0)
        {
            GTP_ERROR("I2C transfer error. errno:%d\n ", ret);
            continue;
        }
        finger = point_data[GTP_ADDR_LENGTH];
        
    #if GTP_COMPATIBLE_MODE
        if ((finger == 0x00) && (CHIP_TYPE_GT9F == gtp_chip_type))
        {
            ret = gtp_i2c_read(i2c_client_point, rqst_data, 3);

            if(ret < 0)
            {
                GTP_ERROR("I2C transfer error. errno:%d\n ", ret);
                continue;
            }
            switch (rqst_data[2])
            {
                case GTP_RQST_BAK_REF:
                    GTP_INFO("Request Ref.");
                    rqst_processing = 1;
                    ret = gtp_bak_ref_proc(i2c_client_point, GTP_BAK_REF_SEND);
                    if(SUCCESS == ret)
                    {
                        GTP_INFO("Send ref success.");
                        rqst_data[2] = GTP_RQST_RESPONDED;
                        gtp_i2c_write(i2c_client_point, rqst_data, 3);
                        rqst_processing = 0;
                    }
                    goto exit_work_func;
                    
                case GTP_RQST_CONFIG:
                    GTP_INFO("Request Config.");
                    ret = gtp_send_cfg(i2c_client_point);
                    if (ret < 0)
                    {
                        GTP_ERROR("Send config error.");
                    }
                    else 
                    {
                        GTP_INFO("Send config success.");
                        rqst_data[2] = GTP_RQST_RESPONDED;
                        gtp_i2c_write(i2c_client_point, rqst_data, 3);
                    }
                    goto exit_work_func;
                    
                case GTP_RQST_MAIN_CLOCK:
                    GTP_INFO("Request main clock.");
                    rqst_processing = 1;
                    ret = gtp_main_clk_proc(i2c_client_point);
                    if(SUCCESS == ret)
                    {
                        GTP_INFO("Send main clk success.");
                        
                        rqst_data[2] = GTP_RQST_RESPONDED;
                        gtp_i2c_write(i2c_client_point, rqst_data, 3);
                        rqst_processing = 0;
                    }
                    goto exit_work_func;
                    
                case GTP_RQST_RESET:
                    GTP_INFO("Request Reset.");
                    gtp_recovery_reset(i2c_client_point);
                    goto exit_work_func;
                    
                default:
                    GTP_INFO("Undefined request code: 0x%02X", rqst_data[2]);
                    rqst_data[2] = GTP_RQST_RESPONDED;
                    gtp_i2c_write(i2c_client_point, rqst_data, 3);
                    break;
            }
        }
    #endif
    
        if (finger == 0x00)
        {
            continue;
        }
        
        if ((finger & 0x80) == 0)
        {
            goto exit_work_func;
        }
        
    #ifdef TPD_PROXIMITY
        if (tpd_proximity_flag == 1)
        {
            proximity_status = point_data[GTP_ADDR_LENGTH];
            GTP_DEBUG("REG INDEX[0x814E]:0x%02X\n", proximity_status);

            if (proximity_status & 0x60)                //proximity or large touch detect,enable hwm_sensor.
            {
                tpd_proximity_detect = 0;
                //sensor_data.values[0] = 0;
            }
            else
            {
                tpd_proximity_detect = 1;
                //sensor_data.values[0] = 1;
            }

            //get raw data
            GTP_DEBUG(" ps change\n");
            GTP_DEBUG("PROXIMITY STATUS:0x%02X\n", tpd_proximity_detect);
            //map and store data to hwm_sensor_data
            sensor_data.values[0] = tpd_get_ps_value();
            sensor_data.value_divide = 1;
            sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
            //report to the up-layer
            ret = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data);

            if (ret)
            {
                GTP_ERROR("Call hwmsen_get_interrupt_data fail = %d\n", err);
            }
        }

    #endif

        touch_num = finger & 0x0f;

        if (touch_num > GTP_MAX_TOUCH)
        {
            goto exit_work_func;
        }
#if GTP_LARGE_TOUCH
		if (finger & 0x40)
		{
			GTP_INFO("large touch!");
			large_touch = 1;
			if (pre_touch) {
				tpd_up(0, 0, 0);	
				if (tpd != NULL && tpd->dev != NULL) {
					input_sync(tpd->dev);
				}
			}
			goto exit_work_func;
		} else if (large_touch) {
			if (touch_num) {
				goto exit_work_func;	
			} else {
				large_touch = 0;	
			}
		}
#endif

        if (touch_num > 1)
        {
            u8 buf[8 * GTP_MAX_TOUCH] = {(GTP_READ_COOR_ADDR + 10) >> 8, (GTP_READ_COOR_ADDR + 10) & 0xff};

            ret = gtp_i2c_read(i2c_client_point, buf, 2 + 8 * (touch_num - 1));
            memcpy(&point_data[12], &buf[2], 8 * (touch_num - 1));
        }

    #if (GTP_HAVE_TOUCH_KEY || GTP_PEN_HAVE_BUTTON || TPD_HAVE_BUTTON)
        key_value = point_data[3 + 8 * touch_num];

        if (key_value || pre_key)
        {
        #if GTP_PEN_HAVE_BUTTON
            if (key_value == 0x40)
            {
                GTP_DEBUG("BTN_STYLUS & BTN_STYLUS2 Down.");
                input_report_key(pen_dev, BTN_STYLUS, 1);
                input_report_key(pen_dev, BTN_STYLUS2, 1);
                pen_active = 1;
            }
            else if (key_value == 0x10)
            {
                GTP_DEBUG("BTN_STYLUS Down, BTN_STYLUS2 Up.");
                input_report_key(pen_dev, BTN_STYLUS, 1);
                input_report_key(pen_dev, BTN_STYLUS2, 0);
                pen_active = 1;
            }
            else if (key_value == 0x20)
            {
                GTP_DEBUG("BTN_STYLUS Up, BTN_STYLUS2 Down.");
                input_report_key(pen_dev, BTN_STYLUS, 0);
                input_report_key(pen_dev, BTN_STYLUS2, 1);
                pen_active = 1;
            }
            else
            {
                GTP_DEBUG("BTN_STYLUS & BTN_STYLUS2 Up.");
                input_report_key(pen_dev, BTN_STYLUS, 0);
                input_report_key(pen_dev, BTN_STYLUS2, 0);
                if ( (pre_key == 0x40) || (pre_key == 0x20) ||
                     (pre_key == 0x10) 
                   )
                {
                    pen_active = 1;
                }
            }
            if (pen_active)
            {
                touch_num = 0;      // shield pen point
                //pre_touch = 0;    // clear last pen status
            }
        #endif
        #if GTP_HAVE_TOUCH_KEY
            if (!pre_touch)
            {
                for (i = 0; i < GTP_MAX_KEY_NUM; i++)
                {
                    input_report_key(tpd->dev, touch_key_array[i], key_value & (0x01 << i));
                }
                touch_num = 0;  // shiled fingers
            }
        #endif
		#if TPD_HAVE_BUTTON
			if (!pre_touch)
            {
                for (i = 0; i < TPD_KEY_COUNT; i++)
                {
                	if (key_value & (0x01 << i)) {
						//GTP_DEBUG("[%d]:[%d %d %d %d]", i, tpd_keys_dim_local[i][0],
						//	tpd_keys_dim_local[i][1], tpd_keys_dim_local[i][2], tpd_keys_dim_local[i][3]);
						tpd_down(tpd_keys_dim_local[i][0], tpd_keys_dim_local[i][1], 0, 0);
                	}
                }
                if((pre_key!=0)&&(key_value ==0))
			    {
					tpd_up( 0, 0, 0);
			    }
				touch_num = 0;
		        pre_touch = 0;
            }
		#endif
        }
    #endif
        pre_key = key_value;

        GTP_DEBUG("pre_touch:%02x, finger:%02x.", pre_touch, finger);
        
        if (touch_num)
        {
            for (i = 0; i < touch_num; i++)
            {
                coor_data = &point_data[i * 8 + 3];

                id = coor_data[0] & 0x0F;      
                input_x  = coor_data[1] | coor_data[2] << 8;
                input_y  = coor_data[3] | coor_data[4] << 8;
                input_w  = coor_data[5] | coor_data[6] << 8;

                input_x = TPD_WARP_X(abs_x_max, input_x);
                input_y = TPD_WARP_Y(abs_y_max, input_y);

            #if GTP_WITH_PEN
                id = coor_data[0];
                if ((id & 0x80))      // pen/stylus is activated
                {
                    GTP_DEBUG("Pen touch DOWN!");
                    pre_pen = 1;
                    //id &= 0x7F;
                    id = 0;
                    GTP_DEBUG("(%d)(%d, %d)[%d]", id, input_x, input_y, input_w);
                    gtp_pen_down(input_x, input_y, input_w, id);
                    pen_active = 1;
                }
                else
            #endif
                {
                    GTP_DEBUG(" (%d)(%d, %d)[%d]", id, input_x, input_y, input_w);
                    tpd_down(input_x, input_y, input_w, id);
                }
            }
        }
        else
        {
            if (pre_touch)
            {
            #if GTP_WITH_PEN
                if (pre_pen)
                {   
                    GTP_DEBUG("Pen touch UP!");
                    gtp_pen_up();
                    pre_pen = 0;
                    pen_active = 1;
                }
                else
            #endif
                {
                    GTP_DEBUG("Touch Release!");
                    tpd_up(0, 0, 0);
                }
            }
        }
        pre_touch = touch_num;
 
    #if GTP_WITH_PEN
        if (pen_active)
        {
            pen_active = 0;
            input_sync(pen_dev);
        }
        else
    #endif
        {
            input_sync(tpd->dev);
        }

exit_work_func:

        if (!gtp_rawdiff_mode)
        {
            ret = gtp_i2c_write(i2c_client_point, end_cmd, 3);

            if (ret < 0)
            {
                GTP_INFO("I2C write end_cmd  error!");
            }
        }

    } while (!kthread_should_stop());

    return 0;
}

static int tpd_local_init(void)
{
#if GTP_ESD_PROTECT
    clk_tick_cnt = 2 * HZ;   // HZ: clock ticks in 1 second generated by system
    GTP_DEBUG("Clock ticks for an esd cycle: %d", clk_tick_cnt);
    INIT_DELAYED_WORK(&gtp_esd_check_work, gtp_esd_check_func);
    gtp_esd_check_workqueue = create_workqueue("gtp_esd_check");
    spin_lock_init(&esd_lock);          // 2.6.39 & later
    // esd_lock = SPIN_LOCK_UNLOCKED;   // 2.6.39 & before
#endif

#if GTP_SUPPORT_I2C_DMA
	//tpd->dev->dev.coherent_dma_mask = DMA_BIT_MASK(32);
//    gpDMABuf_va = (u8 *)dma_alloc_coherent(NULL, GTP_DMA_MAX_TRANSACTION_LENGTH, &gpDMABuf_pa, GFP_KERNEL);
#if defined(CONFIG_64BIT)
    gpDMABuf_va = (u8 *)dma_alloc_coherent(&tpd->dev->dev, GTP_DMA_MAX_TRANSACTION_LENGTH, &gpDMABuf_pa, GFP_KERNEL);
#else
	 gpDMABuf_va = (u8 *)dma_alloc_coherent(NULL, GTP_DMA_MAX_TRANSACTION_LENGTH, &gpDMABuf_pa, GFP_KERNEL);
#endif
    if(!gpDMABuf_va){
        GTP_INFO("[Error] Allocate DMA I2C Buffer failed!\n");
    }
    memset(gpDMABuf_va, 0, GTP_DMA_MAX_TRANSACTION_LENGTH);
#endif
    if (i2c_add_driver(&tpd_i2c_driver) != 0)
    {
        GTP_INFO("unable to add i2c driver.\n");
        return -1;
    }

    if (tpd_load_status == 0) //if(tpd_load_status == 0) // disable auto load touch driver for linux3.0 porting
    {
        GTP_INFO("add error touch panel driver.\n");
        i2c_del_driver(&tpd_i2c_driver);
        return -1;
    }
input_set_abs_params(tpd->dev, ABS_MT_TRACKING_ID, 0, (GTP_MAX_TOUCH - 1), 0, 0); 


#ifdef TPD_HAVE_BUTTON
    tpd_button_setting(TPD_KEY_COUNT, tpd_keys_local, tpd_keys_dim_local);// initialize tpd button data
#endif

#if (defined(TPD_WARP_START) && defined(TPD_WARP_END))
    TPD_DO_WARP = 1;
    memcpy(tpd_wb_start, tpd_wb_start_local, TPD_WARP_CNT * 4);
    memcpy(tpd_wb_end, tpd_wb_start_local, TPD_WARP_CNT * 4);
#endif

#if (defined(TPD_HAVE_CALIBRATION) && !defined(TPD_CUSTOM_CALIBRATION))
    memcpy(tpd_calmat, tpd_def_calmat_local, 8 * 4);
    memcpy(tpd_def_calmat, tpd_def_calmat_local, 8 * 4);
#endif

    // set vendor string
    tpd->dev->id.vendor = 0x00;
    tpd->dev->id.product = tpd_info.pid;
    tpd->dev->id.version = tpd_info.vid;

	hardware_info.tpd_fw_version = tpd_info.vid;
	hardware_info.tpd_cfg_version = gt9xx_version;

    GTP_INFO("end %s, %d\n", __FUNCTION__, __LINE__);
    tpd_type_cap = 1;

    return 0;
}

#if GTP_GESTURE_WAKEUP
static s8 gtp_enter_doze(struct i2c_client *client)
{
    s8 ret = -1;
    s8 retry = 0;
    u8 i2c_control_buf[3] = {(u8)(GTP_REG_SLEEP >> 8), (u8)GTP_REG_SLEEP, 8};

    GTP_DEBUG_FUNC();

    GTP_DEBUG("Entering gesture mode...");
    while(retry++ < 5)
    {
        i2c_control_buf[0] = 0x80;
        i2c_control_buf[1] = 0x46;
        ret = gtp_i2c_write(client, i2c_control_buf, 3);
        if (ret < 0)
        {
            GTP_DEBUG("Failed to set gesture flag into 0x8046, %d", retry);
            continue;
        }
        i2c_control_buf[0] = 0x80;
        i2c_control_buf[1] = 0x40;
        ret = gtp_i2c_write(client, i2c_control_buf, 3);
        if (ret > 0)
        {
            doze_status = DOZE_ENABLED;
            GTP_INFO("Gesture mode enabled.");
            return ret;
        }
        msleep(10);
    }
    GTP_ERROR("GTP send gesture cmd failed.");
    return ret;
}

#endif
/*******************************************************
Function:
    Eter sleep function.

Input:
    client:i2c_client.

Output:
    Executive outcomes.0--success,non-0--fail.
*******************************************************/
static s8 gtp_enter_sleep(struct i2c_client *client)
{
#if GTP_COMPATIBLE_MODE
    if (CHIP_TYPE_GT9F == gtp_chip_type)
    {
        u8 i2c_status_buf[3] = {0x80, 0x44, 0x00};
        s32 ret = 0;
      
        ret = gtp_i2c_read(client, i2c_status_buf, 3);
        if(ret <= 0)
        {
             GTP_ERROR("[gtp_enter_sleep]Read ref status reg error.");
        }
        
        if (i2c_status_buf[2] & 0x80)
        {
            //Store bak ref
            ret = gtp_bak_ref_proc(client, GTP_BAK_REF_STORE);
            if(FAIL == ret)
            {
                GTP_ERROR("[gtp_enter_sleep]Store bak ref failed.");
            }        
        }
    }
#endif
#if GTP_POWER_CTRL_SLEEP

    GTP_GPIO_OUTPUT(GTP_RST_PORT, 0);   
    GTP_GPIO_OUTPUT(GTP_INT_PORT, 0);
    msleep(10);

#ifdef MT6573
    mt_set_gpio_mode(GPIO_CTP_EN_PIN, GPIO_CTP_EN_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EN_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EN_PIN, GPIO_OUT_ZERO);  
    msleep(30);
#else               // ( defined(MT6575) || defined(MT6577) || defined(MT6589) )

    #ifdef TPD_POWER_SOURCE_1800
        hwPowerDown(TPD_POWER_SOURCE_1800, "TP");
    #endif
    
    #ifdef TPD_POWER_SOURCE_CUSTOM
        hwPowerDown(TPD_POWER_SOURCE_CUSTOM, "TP");
    #else
        hwPowerDown(MT65XX_POWER_LDO_VGP2, "TP");
    #endif
#endif
   
    GTP_INFO("GTP enter sleep by poweroff!");
    return 0;
    
#else
    {
        s8 ret = -1;
        s8 retry = 0;
        u8 i2c_control_buf[3] = {(u8)(GTP_REG_SLEEP >> 8), (u8)GTP_REG_SLEEP, 5};
        
        
        GTP_GPIO_OUTPUT(GTP_INT_PORT, 0);
        msleep(5);
    
        while (retry++ < 5)
        {
            ret = gtp_i2c_write(client, i2c_control_buf, 3);
    
            if (ret > 0)
            {
                GTP_INFO("GTP enter sleep!");
                    
                return ret;
            }
     
            msleep(10);
        }
    
        GTP_ERROR("GTP send sleep cmd failed.");
        return ret;
    }
#endif
}


/*******************************************************
Function:
    Wakeup from sleep mode Function.

Input:
    client:i2c_client.

Output:
    Executive outcomes.0--success,non-0--fail.
*******************************************************/
static s8 gtp_wakeup_sleep(struct i2c_client *client)
{
    u8 retry = 0;
    s8 ret = -1;

    GTP_DEBUG("GTP wakeup begin.");

#if (GTP_POWER_CTRL_SLEEP)   

#if GTP_COMPATIBLE_MODE
    if (CHIP_TYPE_GT9F == gtp_chip_type)
    {
        force_reset_guitar();
        GTP_INFO("Esd recovery wakeup.");
        return 0;
    }
#endif

    while (retry++ < 5)
    {
        ret = tpd_power_on(client);

        if (ret < 0)
        {
            GTP_ERROR("I2C Power on ERROR!");
            continue;
        }
        GTP_INFO("Ic wakeup by poweron");
        return 0;
    }
#else

#if GTP_COMPATIBLE_MODE
    if (CHIP_TYPE_GT9F == gtp_chip_type)
    {
        u8 opr_buf[2] = {0};
        
        while (retry++ < 10)
        {
            GTP_GPIO_OUTPUT(GTP_INT_PORT, 1);
            msleep(5);
            
            ret = gtp_i2c_test(client);
    
            if (ret >= 0)
            {  
                // Hold ss51 & dsp
                opr_buf[0] = 0x0C;
                ret = i2c_write_bytes(client, 0x4180, opr_buf, 1);
                if (ret < 0)
                {
                    GTP_DEBUG("Hold ss51 & dsp I2C error,retry:%d", retry);
                    continue;
                }
                
                // Confirm hold
                opr_buf[0] = 0x00;
                ret = i2c_read_bytes(client, 0x4180, opr_buf, 1);
                if (ret < 0)
                {
                    GTP_DEBUG("confirm ss51 & dsp hold, I2C error,retry:%d", retry);
                    continue;
                }
                if (0x0C != opr_buf[0])
                {
                    GTP_DEBUG("ss51 & dsp not hold, val: %d, retry: %d", opr_buf[0], retry);
                    continue;
                }
                GTP_DEBUG("ss51 & dsp has been hold");
                
                ret = gtp_fw_startup(client);
                if (FAIL == ret)
                {
                    GTP_ERROR("[gtp_wakeup_sleep]Startup fw failed.");
                    continue;
                }
                GTP_INFO("flashless wakeup sleep success");
                return ret;
            }
            force_reset_guitar();
            retry = 0;
            break;
        }
        if (retry >= 10)
        {
            GTP_ERROR("wakeup retry timeout, process esd reset");
            force_reset_guitar();
        }
        GTP_ERROR("GTP wakeup sleep failed.");
        return ret;
    }
#endif
    while (retry++ < 10)
    {
    #if GTP_GESTURE_WAKEUP
	
	if (g_wakeup_gesture) {
        if (DOZE_WAKEUP != doze_status)
        {
            GTP_INFO("Powerkey wakeup.");
        }
        else
        {
            GTP_INFO("Gesture wakeup.");
        }
        doze_status = DOZE_DISABLED;
        
        mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
        gtp_reset_guitar(client, 20);
        mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
	} else {
	
        GTP_GPIO_OUTPUT(GTP_INT_PORT, 1);
        msleep(5);
	}
    #else
    
        GTP_GPIO_OUTPUT(GTP_INT_PORT, 1);
        msleep(5);
    #endif
        
        ret = gtp_i2c_test(client);
		//gtp_int_sync(25);

        if (ret >= 0)
        {
            GTP_INFO("GTP wakeup sleep.");
			#if GTP_GESTURE_WAKEUP
        	if (!g_wakeup_gesture) 
			#endif
            {
                gtp_int_sync(25);
            #if GTP_ESD_PROTECT
                gtp_init_ext_watchdog(client);
            #endif
            }
        
            
            return ret;
        }
        gtp_reset_guitar(client, 20);
    }
#endif
    GTP_ERROR("GTP wakeup sleep failed.");
    return ret;
}

/* Function to manage low power suspend */
static void tpd_suspend(struct early_suspend *h)
{
    s32 ret = -1;

    GTP_INFO("System suspend.");

#ifdef TPD_PROXIMITY

    if (tpd_proximity_flag == 1)
    {
        return ;
    }

#endif

    tpd_halt = 1;
#if GTP_ESD_PROTECT
    gtp_esd_switch(i2c_client_point, SWITCH_OFF);
#endif

#ifdef GTP_CHARGER_DETECT
    cancel_delayed_work_sync(&gtp_charger_check_work);
#endif
    //mutex_lock(&i2c_access);

#if GTP_GESTURE_WAKEUP
	if (g_wakeup_gesture)
    	ret = gtp_enter_doze(i2c_client_point);
	else {
		mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
		ret = gtp_enter_sleep(i2c_client_point);
		if (ret < 0)
		{
		    GTP_ERROR("GTP early suspend failed.");
		}
	}
#else
    mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
    ret = gtp_enter_sleep(i2c_client_point);
#endif
    if (ret < 0)
    {
        GTP_ERROR("GTP early suspend failed.");
    }
    // to avoid waking up while not sleeping, delay 48 + 10ms to ensure reliability 
    msleep(58);
}

/* Function to manage power-on resume */
static void tpd_resume(struct early_suspend *h)
{
    s32 ret = -1;

    GTP_INFO("System resume.");
    
#ifdef TPD_PROXIMITY

    if (tpd_proximity_flag == 1)
    {
        return ;
    }

#endif
#if HOTKNOT_BLOCK_RW
    if(hotknot_paired_flag)
    {
        return;
    }  
#endif
    ret = gtp_wakeup_sleep(i2c_client_point);

    if (ret < 0)
    {
        GTP_ERROR("GTP later resume failed.");
    }
    
#if GTP_COMPATIBLE_MODE
    if (CHIP_TYPE_GT9F == gtp_chip_type)
    {
        // do nothing
    }
    else
#endif
    {
        gtp_send_cfg(i2c_client_point);
    }
    
#if GTP_CHARGER_SWITCH
    gtp_charger_switch(1);  // force update
#endif

    tpd_halt = 0;
#ifdef HQ_GTP_GLOVE
	set_gtp_glove_state();
#endif
#if GTP_GESTURE_WAKEUP
	if (g_wakeup_gesture) {
    	doze_status = DOZE_DISABLED;
		tpd_halt = 0;
	} else {
    	//mutex_lock(&i2c_access);
    	tpd_halt = 0;
    	mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);	
    	//mutex_unlock(&i2c_access);
	}
#else
	tpd_halt = 0;
    mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
#endif

#if GTP_ESD_PROTECT
    gtp_esd_switch(i2c_client_point, SWITCH_ON);
#endif

}

static struct tpd_driver_t tpd_device_driver =
{
    .tpd_device_name = "GT915L",
    .tpd_local_init = tpd_local_init,
    .suspend = tpd_suspend,
    .resume = tpd_resume,
#ifdef TPD_HAVE_BUTTON
    .tpd_have_button = 1,
#else
    .tpd_have_button = 0,
#endif
};

/* called when loaded into kernel */
static int __init tpd_driver_init(void)
{
    GTP_INFO("MediaTek gt91xx touch panel driver init\n");
#if 1
    i2c_register_board_info(I2C_BUS_NUMBER, &i2c_tpd, 1);
#else
    i2c_register_board_info(1, &i2c_tpd, 1);
#endif
#ifdef HQ_GTP_GLOVE	 
	glovetimerInit();
#endif

    if (tpd_driver_add(&tpd_device_driver) < 0)
        GTP_INFO("add generic driver failed\n");

    return 0;
}

/* should never be called */
static void __exit tpd_driver_exit(void)
{
    GTP_INFO("MediaTek gt91xx touch panel driver exit\n");
    tpd_driver_remove(&tpd_device_driver);
}

module_init(tpd_driver_init);
module_exit(tpd_driver_exit);

