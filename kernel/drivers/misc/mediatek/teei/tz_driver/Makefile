include $(srctree)/drivers/misc/mediatek/Makefile.custom

ccflags-y += -I$(srctree)/drivers/misc/mediatek/mach/$(MTK_PLATFORM)/$(ARCH_MTK_PROJECT)/touchpanel/GT9XX_hotknot/
ccflags-y += -I$(srctree)/arch/arm/mach-$(MTK_PLATFORM)/$(ARCH_MTK_PROJECT)/touchpanel/GT9XX_hotknot/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GT9XX_hotknot/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/


teei-objs += teei_client_main.o
#teei-objs += teei_admin_main.o
teei-objs += tz_service.o
#teei-objs += socketFun.o
obj-$(CONFIG_NEUSOFT_TZ_DRIVER) += teei.o
#obj-$(CONFIG_NEUSOFT_TZ_DRIVER) += tz_driver.o

EXTRA_CFLAGS += -D DEBUG_DO 

