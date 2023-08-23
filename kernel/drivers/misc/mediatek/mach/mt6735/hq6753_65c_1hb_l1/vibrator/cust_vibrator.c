#include <cust_vibrator.h>
#include <linux/types.h>

static struct vibrator_hw cust_vibrator_hw = {
	.vib_timer = 25,
  #ifdef CUST_VIBR_LIMIT
	.vib_limit = 9,
  #endif
  #ifdef CUST_VIBR_VOL
	.vib_vol = 0x5,//2.8V for vibr
  #endif
};

struct vibrator_hw *get_cust_vibrator_hw(void)
{
    return &cust_vibrator_hw;
}

//add by zhangqiangqiang,2015-08-06
static struct vibrator_hw cust_short_vibrator_hw = {
	.vib_timer = 40,
  #ifdef CUST_VIBR_LIMIT
	.vib_limit = 9,
  #endif
  #ifdef CUST_VIBR_VOL
	.vib_vol = 0x4,//2.8V for vibr
  #endif
};

struct vibrator_hw *get_cust_short_vibrator_hw(void)
{
    return &cust_short_vibrator_hw;
}
//end by zhangqiangqiang