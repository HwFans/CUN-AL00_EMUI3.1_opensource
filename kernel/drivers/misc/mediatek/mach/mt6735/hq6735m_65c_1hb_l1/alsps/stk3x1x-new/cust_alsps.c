#include <linux/types.h>
#include <mach/mt_pm_ldo.h>
#include <cust_alsps.h>
#include <mach/upmu_common.h>

static struct alsps_hw cust_alsps_hw = {
    .i2c_num    = 2,
	.polling_mode_ps =0,
	.polling_mode_als =1,
    .power_id   = MT65XX_POWER_NONE,    /*LDO is not used*/
    .power_vol  = VOL_DEFAULT,          /*LDO is not used*/
    //.i2c_addr   = {0x0C, 0x48, 0x78, 0x00},
 //.als_level  = {20,  90, 350, 1100, 3500, 5600, 6000, 6100, 6200, 6300, 6400, 6500, 6600, 6700, 6982},	/* als_code */
    //.als_value  = {0, 5, 10, 40, 115, 355, 562, 672, 872, 930, 1250, 1700, 2600, 5120, 7680, 10240},    /* lux */

    .als_level  = {5, 10, 400, 2000, 4000, 6000, 6100, 6200, 6300, 6400, 6500, 6700, 6800, 6900, 6982},	/* als_code */
    .als_value  = {0, 90, 350, 1100, 3500, 5600, 6000, 6100, 6200, 6300, 6400, 6500, 6700, 6900, 7680, 10240},    /* lux */

   	.state_val = 0x0,		/* disable all */
	.psctrl_val = 0x31,		/* ps_persistance=1, ps_gain=64X, PS_IT=0.391ms */
	.alsctrl_val = 0x2B,	/* als_persistance=1, als_gain=16X, ALS_IT=200ms */
	.ledctrl_val = 0xFF,	/* 100mA IRDR, 64/64 LED duty */
	.wait_val = 0x7,		/* 50 ms */
    .ps_high_thd_val = 1700,
    .ps_low_thd_val = 1500,
};
struct alsps_hw *get_cust_alsps_hw(void) {
    return &cust_alsps_hw;
}

