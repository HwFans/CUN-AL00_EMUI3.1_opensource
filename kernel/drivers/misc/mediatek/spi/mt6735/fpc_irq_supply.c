/* Fingerprint Cards, Hybrid Touch sensor driver
 *
 * Copyright (c) 2014,2015 Fingerprint Cards AB <tech@fingerprints.com>
 *
 *
 * Software license : "Dual BSD/GPL"
 * see <linux/module.h> and ./Documentation
 * for  details.
 *
*/

#define DEBUG

#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h> 
#include <linux/delay.h>
#include <linux/poll.h> 
#include <linux/sched.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>

#include "fpc_irq_common.h"
#include "fpc_irq_supply.h"
#include <mach/mt_gpio.h>
#include <mach/mt_spi.h>
#include <mach/mt_clkmgr.h>

#define GPIO_SPI_CS_PIN         (GPIO65 | 0x80000000)
#define GPIO_SPI_CS_PIN_M_GPIO  GPIO_MODE_00
#define GPIO_SPI_CS_PIN_M_KROW  GPIO_MODE_04
#define GPIO_SPI_CS_PIN_M_SPI_CSA   GPIO_MODE_01

#define GPIO_SPI_SCK_PIN         (GPIO66 | 0x80000000)
#define GPIO_SPI_SCK_PIN_M_GPIO  GPIO_MODE_00
#define GPIO_SPI_SCK_PIN_M_KCOL  GPIO_MODE_04
#define GPIO_SPI_SCK_PIN_M_SPI_CKA   GPIO_MODE_01

#define GPIO_SPI_MOSI_PIN         (GPIO67 | 0x80000000)
#define GPIO_SPI_MOSI_PIN_M_GPIO  GPIO_MODE_00
#define GPIO_SPI_MOSI_PIN_M_SPI_MIA   GPIO_MODE_01

#define GPIO_SPI_MISO_PIN         (GPIO68 | 0x80000000)
#define GPIO_SPI_MISO_PIN_M_GPIO  GPIO_MODE_00
#define GPIO_SPI_MISO_PIN_M_SPI_MOA   GPIO_MODE_01
#define GPIO_SPI_MISO_PIN_M_SPI_MIA   GPIO_MODE_02
#define GPIO_SPI_MISO_PIN_M_I2S3_LRCK   GPIO_MODE_03
#define GPIO_SPI_MISO_PIN_M_PTA_TXD   GPIO_MODE_04
#define GPIO_SPI_MISO_PIN_M_ANT_SEL   GPIO_MODE_05
#define GPIO_SPI_MISO_PIN_M_URTS   GPIO_MODE_06
#define GPIO_SPI_MISO_PIN_M_DBG_MON_A   GPIO_MODE_07

#define GPIO_LENOVO_FPC_RESET_PIN         (GPIO60 | 0x80000000)
#define GPIO_LENOVO_FPC_RESET_PIN_M_GPIO  GPIO_MODE_00
#define GPIO_LENOVO_FPC_RESET_PIN_M_MDEINT  GPIO_MODE_05
#define GPIO_LENOVO_FPC_RESET_PIN_M_ANT_SEL   GPIO_MODE_01
#define GPIO_LENOVO_FPC_RESET_PIN_M_C2K_UIM1_HOT_PLUG_IN   GPIO_MODE_02
#define GPIO_LENOVO_FPC_RESET_PIN_M_IRDA_RX   GPIO_MODE_03
#define GPIO_LENOVO_FPC_RESET_PIN_M_C2K_UART0_RXD   GPIO_MODE_04
#define GPIO_LENOVO_FPC_RESET_PIN_M_LTE_URXD   GPIO_MODE_06
#define GPIO_LENOVO_FPC_RESET_PIN_M_DBG_MON_A   GPIO_MODE_07
#define GPIO_FPS_RESET_PIN       GPIO_LENOVO_FPC_RESET_PIN

static inline void FPS_reset(void)
{
        mt_set_gpio_mode(GPIO_FPS_RESET_PIN, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO_FPS_RESET_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_FPS_RESET_PIN, GPIO_OUT_ZERO);
        msleep(10);
        mt_set_gpio_out(GPIO_FPS_RESET_PIN, GPIO_OUT_ONE);
        //msleep(10);
}


void fpc_spi_enable_clk(void)
{
	//#if (!defined(CONFIG_MT_SPI_FPGA_ENABLE))
		enable_clock(MT_CG_PERI_SPI0, "spi");
		//printk("clock enabled !!\n");
	///#endif
		return;
}
void fpc_spi_disable_clk(void)
{
	//#if (!defined(CONFIG_MT_SPI_FPGA_ENABLE))
		disable_clock(MT_CG_PERI_SPI0, "spi");
		//printk("clock disable !!\n");
	///#endif
		return;
}


static void spi_gpio_set(struct mt_spi_t *ms)
{
	/* lenovo-sw, chenzz3, change for fingerprint, begin */
	mt_set_gpio_mode(GPIO_SPI_CS_PIN, 1);
	mt_set_gpio_mode(GPIO_SPI_SCK_PIN, 1);
	mt_set_gpio_mode(GPIO_SPI_MISO_PIN, 1);
	mt_set_gpio_mode(GPIO_SPI_MOSI_PIN, 1);
	/* lenovo-sw, chenzz3, change for fingerprint, end */
	return;
}

/* -------------------------------------------------------------------------- */
int fpc_irq_supply_init(fpc_irq_data_t *fpc_irq_data)
{
	int ret = 0;
	printk("111fpc module power on is start==================lfw!!!\n");
	dev_dbg(fpc_irq_data->dev, "%s\n", __func__);
	//spi_gpio_set(0);   //modify by huaqin zam 20150917 
	//enable_clk();		//modify by huaqin zam 20150917 
	FPS_reset();
	printk("222fpc module power on is start==================lfw!!!\n");
	// Todo: Acquire required regulators

	return ret;
}


/* -------------------------------------------------------------------------- */
int fpc_irq_supply_destroy(fpc_irq_data_t *fpc_irq_data)
{
	int ret = 0;

	dev_dbg(fpc_irq_data->dev, "%s\n", __func__);

	// Todo: release used regulators

	return ret;
}


/* -------------------------------------------------------------------------- */
extern int fpc_irq_supply_set(fpc_irq_data_t *fpc_irq_data, bool req_state)
{
	int ret = 0;
	bool curr_state = fpc_irq_data->pm.supply_on;

	dev_dbg(fpc_irq_data->dev, "%s %s => %s\n",
						__func__,
						(curr_state) ? "ON" : "OFF",
						(req_state) ? "ON" : "OFF");

	if (curr_state != req_state) {

		fpc_irq_data->pm.supply_on = req_state;

		// Todo: enable/disable used regulators
		// Todo: If state == off, also set I/O as required fo not sourcing the sensor.
	}

	return ret;
}


/* -------------------------------------------------------------------------- */

