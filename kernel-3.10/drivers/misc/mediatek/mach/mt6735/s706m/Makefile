
include $(srctree)/drivers/misc/mediatek/Makefile.custom

CONFIG_QN8027 = y
obj-$(CONFIG_QN8027) += qn8027/

ifneq ($(CONFIG_ARCH_MTK_PROJECT),)
ARCH_MTK_PROJECT := $(shell echo $(CONFIG_ARCH_MTK_PROJECT))
obj-y   += core/
obj-y   += dct/dct/
endif

#obj-y   +=  lcm/
obj-y   +=  camera/camera/
obj-$(CONFIG_MTK_VIBRATOR)   +=  vibrator/
obj-$(CONFIG_MTK_LEDS)   +=  leds/
obj-y   +=  keypad/
#obj-y   +=  sound/
obj-y   +=  accdet/

ifeq ($(CONFIG_MTK_SENSOR_SUPPORT),y)
ifeq ($(CONFIG_MTK_CM36652_NEW),y)
obj-y	+=  alsps/cm36652-new/
endif

ifeq ($(CONFIG_MTK_MC3410_NEW),y)
obj-y   +=  accelerometer/mc3410-new/
endif

ifeq ($(CONFIG_MTK_LTR501_LITEON),y)
obj-y	+=  alsps/ltr501_liteon/
endif

ifeq ($(CONFIG_MTK_BMA222E_NEW),y)
obj-y   +=  accelerometer/bma222E-new/
endif

ifeq ($(CONFIG_MTK_MC3XXX),y)
obj-y	+=  accelerometer/mc3xxx/
endif

ifeq ($(CONFIG_MTK_STK831X),y)
obj-y	+=  accelerometer/stk831x/
endif

ifeq ($(CONFIG_MTK_ITG1010_NEW),y)
obj-y   +=  gyroscope/ITG1010-new/
endif

ifeq ($(CONFIG_MTK_AKM09911_NEW),y)
obj-y	+=  magnetometer/akm09911-new/
endif

ifeq ($(CONFIG_MTK_APDS9930_NEW),y)
obj-y	+=  alsps/APDS9930-new/
endif

ifeq ($(CONFIG_CUSTOM_KERNEL_FLASHLIGHT),"constant_flashlight")
obj-y   +=  flashlight/constant_flashlight/
endif

endif
