EXTRA_CFLAGS += $(USER_EXTRA_CFLAGS)

EXTRA_CFLAGS += -I$(src)/include
EXTRA_CFLAGS += -I$(src)/hal/phydm

########################## WIFI IC ############################
CONFIG_RTL8812A = n
CONFIG_RTL8821A = y
######################### Interface ###########################
CONFIG_USB_HCI = y
########################## Features ###########################
CONFIG_MP_INCLUDED = y
CONFIG_POWER_SAVING = y
CONFIG_USB_AUTOSUSPEND = n
CONFIG_HW_PWRP_DETECTION = n
CONFIG_BT_COEXIST = y
CONFIG_TRAFFIC_PROTECT = y
CONFIG_TXPWR_BY_RATE_EN = n
CONFIG_TXPWR_LIMIT_EN = n
CONFIG_RTW_ADAPTIVITY_EN = disable
CONFIG_RTW_ADAPTIVITY_MODE = normal
CONFIG_SIGNAL_SCALE_MAPPING = n
CONFIG_BR_EXT = y
CONFIG_TDLS = n
CONFIG_MCC_MODE = n
CONFIG_RTW_NAPI = y
CONFIG_RTW_GRO = y
########################## Debug ###########################
CONFIG_RTW_DEBUG = y
# default log level is _DRV_INFO_ = 4,
# please refer to "How_to_set_driver_debug_log_level.doc" to set the available level.
CONFIG_RTW_LOG_LEVEL = 2
######################## Wake On Lan ##########################
CONFIG_WOWLAN = n
CONFIG_GPIO_WAKEUP = n
CONFIG_DEFAULT_PATTERNS_EN = n
CONFIG_WAKEUP_GPIO_IDX = default
CONFIG_HIGH_ACTIVE = n
CONFIG_PNO_SUPPORT = n
CONFIG_PNO_SET_DEBUG = n
CONFIG_AP_WOWLAN = n
###################### MP HW TX MODE FOR VHT #######################
CONFIG_MP_VHT_HW_TX_MODE = n

export TopDIR ?= $(shell pwd)

########### COMMON  #################################
ifeq ($(CONFIG_USB_HCI), y)
HCI_NAME = usb
endif


_OS_INTFS_FILES :=	os_dep/osdep_service.o \
			os_dep/linux/os_intfs.o \
			os_dep/linux/$(HCI_NAME)_intf.o \
			os_dep/linux/$(HCI_NAME)_ops_linux.o \
			os_dep/linux/ioctl_linux.o \
			os_dep/linux/xmit_linux.o \
			os_dep/linux/mlme_linux.o \
			os_dep/linux/recv_linux.o \
			os_dep/linux/ioctl_cfg80211.o \
			os_dep/linux/rtw_cfgvendor.o \
			os_dep/linux/wifi_regd.o \
			os_dep/linux/rtw_android.o \
			os_dep/linux/rtw_proc.o

ifeq ($(CONFIG_MP_INCLUDED), y)
_OS_INTFS_FILES += os_dep/linux/ioctl_mp.o
endif


_HAL_INTFS_FILES :=	hal/hal_intf.o \
			hal/hal_com.o \
			hal/hal_com_phycfg.o \
			hal/hal_phy.o \
			hal/hal_dm.o \
			hal/hal_btcoex_wifionly.o \
			hal/hal_btcoex.o \
			hal/hal_mp.o \
			hal/hal_mcc.o \
			hal/hal_hci/hal_$(HCI_NAME).o \
			hal/led/hal_$(HCI_NAME)_led.o

			
_OUTSRC_FILES := hal/phydm/phydm_debug.o	\
		hal/phydm/phydm_antdiv.o\
		hal/phydm/phydm_antdect.o\
		hal/phydm/phydm_interface.o\
		hal/phydm/phydm_hwconfig.o\
		hal/phydm/phydm.o\
		hal/phydm/halphyrf_ce.o\
		hal/phydm/phydm_edcaturbocheck.o\
		hal/phydm/phydm_dig.o\
		hal/phydm/phydm_pathdiv.o\
		hal/phydm/phydm_rainfo.o\
		hal/phydm/phydm_dynamicbbpowersaving.o\
		hal/phydm/phydm_powertracking_ce.o\
		hal/phydm/phydm_dynamictxpower.o\
		hal/phydm/phydm_adaptivity.o\
		hal/phydm/phydm_cfotracking.o\
		hal/phydm/phydm_noisemonitor.o\
		hal/phydm/phydm_acs.o\
		hal/phydm/phydm_beamforming.o\
		hal/phydm/phydm_dfs.o\
		hal/phydm/txbf/halcomtxbf.o\
		hal/phydm/txbf/haltxbfinterface.o\
		hal/phydm/txbf/phydm_hal_txbf_api.o\
		hal/phydm/phydm_kfree.o\
		hal/phydm/phydm_ccx.o\
		hal/phydm/phydm_psd.o

EXTRA_CFLAGS += -I$(src)/hal/btc
ifeq ($(CONFIG_BT_COEXIST), y)
_OUTSRC_FILES += hal/btc/halbtc8821a1ant.o \
				hal/btc/halbtc8821a2ant.o 
endif

########### HAL_RTL8812A_RTL8821A #################################

ifneq ($(CONFIG_RTL8812A)_$(CONFIG_RTL8821A), n_n)

RTL871X = rtl8812a
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8812au
endif

_HAL_INTFS_FILES +=  hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8812PwrSeq.o \
					hal/$(RTL871X)/Hal8821APwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_xmit.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o

ifeq ($(CONFIG_RTL8812A), y)
ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8812A_USB.o
endif
endif
ifeq ($(CONFIG_RTL8821A), y)
ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8821A_USB.o
endif
endif

ifeq ($(CONFIG_RTL8812A), y)
EXTRA_CFLAGS += -DCONFIG_RTL8812A
_HAL_INTFS_FILES +=	hal/rtl8812a/hal8812a_fw.o

_OUTSRC_FILES += hal/phydm/$(RTL871X)/halhwimg8812a_mac.o\
		hal/phydm/$(RTL871X)/halhwimg8812a_bb.o\
		hal/phydm/$(RTL871X)/halhwimg8812a_rf.o\
		hal/phydm/$(RTL871X)/halphyrf_8812a_ce.o\
		hal/phydm/$(RTL871X)/phydm_regconfig8812a.o\
		hal/phydm/$(RTL871X)/phydm_rtl8812a.o\
		hal/phydm/txbf/haltxbfjaguar.o
endif

ifeq ($(CONFIG_RTL8821A), y)

ifeq ($(CONFIG_RTL8812A), n)

RTL871X = rtl8821a
ifeq ($(CONFIG_USB_HCI), y)
ifeq ($(CONFIG_BT_COEXIST), y)
MODULE_NAME := 8812au
else
MODULE_NAME := 8811au
endif
endif
endif

EXTRA_CFLAGS += -DCONFIG_RTL8821A

_HAL_INTFS_FILES +=	hal/rtl8812a/hal8821a_fw.o
_OUTSRC_FILES += hal/phydm/rtl8821a/halhwimg8821a_mac.o\
		hal/phydm/rtl8821a/halhwimg8821a_bb.o\
		hal/phydm/rtl8821a/halhwimg8821a_rf.o\
		hal/phydm/rtl8812a/halphyrf_8812a_ce.o\
		hal/phydm/rtl8821a/halphyrf_8821a_ce.o\
		hal/phydm/rtl8821a/phydm_regconfig8821a.o\
		hal/phydm/rtl8821a/phydm_rtl8821a.o\
		hal/phydm/rtl8821a/phydm_iqk_8821a_ce.o\
		hal/phydm/txbf/haltxbfjaguar.o
		
endif

endif

########### END OF PATH  #################################

ifeq ($(CONFIG_USB_HCI), y)
ifeq ($(CONFIG_USB_AUTOSUSPEND), y)
EXTRA_CFLAGS += -DCONFIG_USB_AUTOSUSPEND
endif
endif

ifeq ($(CONFIG_MP_INCLUDED), y)
#MODULE_NAME := $(MODULE_NAME)_mp
EXTRA_CFLAGS += -DCONFIG_MP_INCLUDED
endif

ifeq ($(CONFIG_POWER_SAVING), y)
EXTRA_CFLAGS += -DCONFIG_POWER_SAVING
endif

ifeq ($(CONFIG_HW_PWRP_DETECTION), y)
EXTRA_CFLAGS += -DCONFIG_HW_PWRP_DETECTION
endif

ifeq ($(CONFIG_BT_COEXIST), y)
EXTRA_CFLAGS += -DCONFIG_BT_COEXIST
endif

ifeq ($(CONFIG_TRAFFIC_PROTECT), y)
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
endif

ifeq ($(CONFIG_TXPWR_BY_RATE_EN), n)
EXTRA_CFLAGS += -DCONFIG_TXPWR_BY_RATE_EN=0
else ifeq ($(CONFIG_TXPWR_BY_RATE_EN), y)
EXTRA_CFLAGS += -DCONFIG_TXPWR_BY_RATE_EN=1
else ifeq ($(CONFIG_TXPWR_BY_RATE_EN), auto)
EXTRA_CFLAGS += -DCONFIG_TXPWR_BY_RATE_EN=2
endif

ifeq ($(CONFIG_TXPWR_LIMIT_EN), n)
EXTRA_CFLAGS += -DCONFIG_TXPWR_LIMIT_EN=0
else ifeq ($(CONFIG_TXPWR_LIMIT_EN), y)
EXTRA_CFLAGS += -DCONFIG_TXPWR_LIMIT_EN=1
else ifeq ($(CONFIG_TXPWR_LIMIT_EN), auto)
EXTRA_CFLAGS += -DCONFIG_TXPWR_LIMIT_EN=2
endif

ifeq ($(CONFIG_CALIBRATE_TX_POWER_BY_REGULATORY), y)
EXTRA_CFLAGS += -DCONFIG_CALIBRATE_TX_POWER_BY_REGULATORY
endif

ifeq ($(CONFIG_CALIBRATE_TX_POWER_TO_MAX), y)
EXTRA_CFLAGS += -DCONFIG_CALIBRATE_TX_POWER_TO_MAX
endif

ifeq ($(CONFIG_RTW_ADAPTIVITY_EN), disable)
EXTRA_CFLAGS += -DCONFIG_RTW_ADAPTIVITY_EN=0
else ifeq ($(CONFIG_RTW_ADAPTIVITY_EN), enable)
EXTRA_CFLAGS += -DCONFIG_RTW_ADAPTIVITY_EN=1
endif

ifeq ($(CONFIG_RTW_ADAPTIVITY_MODE), normal)
EXTRA_CFLAGS += -DCONFIG_RTW_ADAPTIVITY_MODE=0
else ifeq ($(CONFIG_RTW_ADAPTIVITY_MODE), carrier_sense)
EXTRA_CFLAGS += -DCONFIG_RTW_ADAPTIVITY_MODE=1
endif

ifeq ($(CONFIG_SIGNAL_SCALE_MAPPING), y)
EXTRA_CFLAGS += -DCONFIG_SIGNAL_SCALE_MAPPING
endif

ifeq ($(CONFIG_WOWLAN), y)
EXTRA_CFLAGS += -DCONFIG_WOWLAN
ifeq ($(CONFIG_DEFAULT_PATTERNS_EN), y)
EXTRA_CFLAGS += -DCONFIG_DEFAULT_PATTERNS_EN
endif
endif

ifeq ($(CONFIG_AP_WOWLAN), y)
EXTRA_CFLAGS += -DCONFIG_AP_WOWLAN
endif

ifeq ($(CONFIG_PNO_SUPPORT), y)
EXTRA_CFLAGS += -DCONFIG_PNO_SUPPORT
ifeq ($(CONFIG_PNO_SET_DEBUG), y)
EXTRA_CFLAGS += -DCONFIG_PNO_SET_DEBUG
endif
endif

ifeq ($(CONFIG_GPIO_WAKEUP), y)
EXTRA_CFLAGS += -DCONFIG_GPIO_WAKEUP
ifeq ($(CONFIG_HIGH_ACTIVE), y)
EXTRA_CFLAGS += -DHIGH_ACTIVE=1
else
EXTRA_CFLAGS += -DHIGH_ACTIVE=0
endif
endif

ifneq ($(CONFIG_WAKEUP_GPIO_IDX), default)
EXTRA_CFLAGS += -DWAKEUP_GPIO_IDX=$(CONFIG_WAKEUP_GPIO_IDX)
endif

ifeq ($(CONFIG_BR_EXT), y)
BR_NAME = br0
EXTRA_CFLAGS += -DCONFIG_BR_EXT
EXTRA_CFLAGS += '-DCONFIG_BR_EXT_BRNAME="'$(BR_NAME)'"'
endif


ifeq ($(CONFIG_TDLS), y)
EXTRA_CFLAGS += -DCONFIG_TDLS
endif

ifeq ($(CONFIG_MCC_MODE), y)
EXTRA_CFLAGS += -DCONFIG_MCC_MODE
endif

ifeq ($(CONFIG_RTW_NAPI), y)
EXTRA_CFLAGS += -DCONFIG_RTW_NAPI
endif

ifeq ($(CONFIG_RTW_GRO), y)
EXTRA_CFLAGS += -DCONFIG_RTW_GRO
endif

ifeq ($(CONFIG_MP_VHT_HW_TX_MODE), y)
EXTRA_CFLAGS += -DCONFIG_MP_VHT_HW_TX_MODE
endif

ifeq ($(CONFIG_RTW_DEBUG), y)
EXTRA_CFLAGS += -DCONFIG_RTW_DEBUG
EXTRA_CFLAGS += -DRTW_LOG_LEVEL=$(CONFIG_RTW_LOG_LEVEL)
endif

EXTRA_CFLAGS += -DDM_ODM_SUPPORT_TYPE=0x04

USER_MODULE_NAME ?=
ifneq ($(USER_MODULE_NAME),)
MODULE_NAME := $(USER_MODULE_NAME)
endif

EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_SUPPORT_USB_INT

ifneq ($(KERNELRELEASE),)

rtk_core :=	core/rtw_cmd.o \
		core/rtw_security.o \
		core/rtw_debug.o \
		core/rtw_io.o \
		core/rtw_ioctl_set.o \
		core/rtw_ieee80211.o \
		core/rtw_mlme.o \
		core/rtw_mlme_ext.o \
		core/rtw_mi.o \
		core/rtw_wlan_util.o \
		core/rtw_vht.o \
		core/rtw_pwrctrl.o \
		core/rtw_rf.o \
		core/rtw_recv.o \
		core/rtw_sta_mgt.o \
		core/rtw_ap.o \
		core/rtw_xmit.o	\
		core/rtw_p2p.o \
		core/rtw_tdls.o \
		core/rtw_br_ext.o \
		core/rtw_sreset.o \
		core/rtw_btcoex_wifionly.o \
		core/rtw_btcoex.o \
		core/rtw_beamforming.o \
		core/rtw_odm.o \
		core/efuse/rtw_efuse.o 

$(MODULE_NAME)-y += $(rtk_core)

$(MODULE_NAME)-y += $(_OS_INTFS_FILES)
$(MODULE_NAME)-y += $(_HAL_INTFS_FILES)
$(MODULE_NAME)-y += $(_OUTSRC_FILES)

$(MODULE_NAME)-$(CONFIG_MP_INCLUDED) += core/rtw_mp.o

obj-$(CONFIG_RTL8812AU) := $(MODULE_NAME).o

else

export CONFIG_RTL8821AU = m

all: modules

modules:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(shell pwd)  modules

strip:
	$(CROSS_COMPILE)strip $(MODULE_NAME).ko --strip-unneeded

install:
	install -p -m 644 *.ko  $(MODDESTDIR)
	/sbin/depmod -a ${KVER}

uninstall:
	rm -f $(MODDESTDIR)/$(MODULE_NAME).ko
	/sbin/depmod -a ${KVER}

config_r:
	@echo "make config"
	/bin/bash script/Configure script/config.in


.PHONY: modules clean

clean:
	#$(MAKE) -C $(KSRC) M=$(shell pwd) clean
	cd hal ; rm -fr */*/*/*.mod.c */*/*/*.mod */*/*/*.o */*/*/.*.cmd */*/*/*.ko
	cd hal ; rm -fr */*/*.mod.c */*/*.mod */*/*.o */*/.*.cmd */*/*.ko
	cd hal ; rm -fr */*.mod.c */*.mod */*.o */.*.cmd */*.ko
	cd hal ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd core/efuse ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd core ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd os_dep/linux ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd os_dep ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd platform ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	rm -fr Module.symvers ; rm -fr Module.markers ; rm -fr modules.order
	rm -fr *.mod.c *.mod *.o .*.cmd *.ko *~
	rm -fr .tmp_versions
endif

