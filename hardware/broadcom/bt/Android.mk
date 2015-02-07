LOCAL_PATH := $(call my-dir)

#
# copy brcm binaries into android
#
ifeq ($(BOARD_HAVE_BLUETOOTH),true)

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/bin/msm7627_ffa/btld:system/bin/btld

#modify for fm reset by hui.fan @2010-12-20
#modify for fm voice fluctuation by hui.fan @2011-4-6 
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/bin/msm7627_ffa/BCM4329B1_002_1_002_023_0797_0834.hcd:system/bin/BCM4329B1_002_1_002_023_0797_0834.hcd
#modify end @2010-12-20
    include $(all-subdir-makefiles, adaptation/btl-if/client)
endif
