/*********************************************
HAL API
**********************************************/

void vibr_Enable_HW(void);
void vibr_Disable_HW(void);
void vibr_power_set(void);
struct vibrator_hw *mt_get_cust_vibrator_hw(void);
/**********start zhangqiangqiang add for hq_vibrator********************/
void vibr_short_power_set(void);
struct vibrator_hw* mt_get_cust_vibrator_hw(void);
#if 1 //defined(HQ_VIBRATOR_MODE)
struct vibrator_hw* mt_get_cust_short_vibrator_hw(void);
#endif
/**********end zhangqiangqiang add for hq_vibrator********************/
