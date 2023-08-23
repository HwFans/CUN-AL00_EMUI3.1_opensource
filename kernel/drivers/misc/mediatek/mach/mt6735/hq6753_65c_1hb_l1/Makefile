
include $(srctree)/drivers/misc/mediatek/Makefile.custom

ifneq ($(CONFIG_ARCH_MTK_PROJECT),)
ARCH_MTK_PROJECT := $(shell echo $(CONFIG_ARCH_MTK_PROJECT))
obj-y   += core/
obj-y   += dct/dct/
endif

ifeq ($(CONFIG_MTK_HDMI_SUPPORT),y)
obj-y	+=  hdmi/
endif

#obj-y   +=  lcm/
obj-y   +=  camera/camera/
obj-$(CONFIG_MTK_VIBRATOR)   +=  vibrator/
obj-$(CONFIG_MTK_LEDS)   +=  leds/
obj-y   +=  keypad/
#obj-y   +=  sound/
obj-y   +=  accdet/

ifeq ($(CONFIG_HUAQIN_FPC1020_SUPPORT),y)
obj-y	+= fpc1020/
endif

ifeq ($(CONFIG_MTK_SENSOR_SUPPORT),y)
ifeq ($(CONFIG_MTK_CM36652_NEW),y)
obj-y	+=  alsps/cm36652-new/
endif

ifeq ($(CONFIG_MTK_KXTJ2_1009),y)
obj-y   +=  accelerometer/KXTJ2_1009/
endif

ifeq ($(CONFIG_MTK_BMA253),y)
obj-y   +=  accelerometer/bma253/
endif

ifeq ($(CONFIG_MTK_ITG1010_NEW),y)
obj-y   +=  gyroscope/ITG1010-new/
endif

ifeq ($(CONFIG_MTK_AKM09911_NEW),y)
obj-y	+=  magnetometer/akm09911-new/
endif

endif
