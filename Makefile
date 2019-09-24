EXTRA_CFLAGS += $(USER_EXTRA_CFLAGS)
#EXTRA_CFLAGS += -O1
#EXTRA_CFLAGS += -O3
#EXTRA_CFLAGS += -Wall
#EXTRA_CFLAGS += -Wextra
#EXTRA_CFLAGS += -Werror
#EXTRA_CFLAGS += -pedantic
#EXTRA_CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes

#EXTRA_CFLAGS += -Wno-unused-variable
#EXTRA_CFLAGS += -Wno-unused-value
#EXTRA_CFLAGS += -Wno-unused-label
#EXTRA_CFLAGS += -Wno-unused-parameter
#EXTRA_CFLAGS += -Wno-unused-function
#EXTRA_CFLAGS += -Wno-unused
#EXTRA_CFLAGS += -Wno-uninitialized

GCC_VER_49 := $(shell echo `$(CC) -dumpversion | cut -f1-2 -d.` \>= 4.9 | bc )
ifeq ($(GCC_VER_49),1)
EXTRA_CFLAGS += -Wno-date-time	# Fix compile error && warning on gcc 4.9 and later
endif

EXTRA_CFLAGS += -I$(src)/include
EXTRA_CFLAGS += -I$(src)/hal/phydm

EXTRA_LDFLAGS += --strip-debug

CONFIG_AUTOCFG_CP = n

########################## WIFI IC ############################
CONFIG_MULTIDRV = n
CONFIG_RTL8188E = n
CONFIG_RTL8812A = n
CONFIG_RTL8821A = y
CONFIG_RTL8192E = n
CONFIG_RTL8723B = n
CONFIG_RTL8814A = n
CONFIG_RTL8723C = n
CONFIG_RTL8188F = n
CONFIG_RTL8822B = n
CONFIG_RTL8723D = n
CONFIG_RTL8821C = n
######################### Interface ###########################
CONFIG_USB_HCI = y
CONFIG_PCI_HCI = n
CONFIG_SDIO_HCI = n
CONFIG_GSPI_HCI = n
########################## Features ###########################
CONFIG_MP_INCLUDED = y
CONFIG_POWER_SAVING = y
CONFIG_USB_AUTOSUSPEND = n
CONFIG_HW_PWRP_DETECTION = n
CONFIG_WIFI_TEST = n
CONFIG_BT_COEXIST = y
CONFIG_INTEL_WIDI = n
CONFIG_WAPI_SUPPORT = n
CONFIG_EFUSE_CONFIG_FILE = y
CONFIG_EXT_CLK = n
CONFIG_TRAFFIC_PROTECT = y
CONFIG_LOAD_PHY_PARA_FROM_FILE = y
CONFIG_TXPWR_BY_RATE_EN = n
CONFIG_TXPWR_LIMIT_EN = n
CONFIG_RTW_ADAPTIVITY_EN = disable
CONFIG_RTW_ADAPTIVITY_MODE = normal
CONFIG_SIGNAL_SCALE_MAPPING = n
CONFIG_80211W = n
CONFIG_REDUCE_TX_CPU_LOADING = n
CONFIG_BR_EXT = y
CONFIG_TDLS = n
CONFIG_WIFI_MONITOR = n
CONFIG_MCC_MODE = n
CONFIG_APPEND_VENDOR_IE_ENABLE = n
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
######### Notify SDIO Host Keep Power During Syspend ##########
CONFIG_RTW_SDIO_PM_KEEP_POWER = y
###################### MP HW TX MODE FOR VHT #######################
CONFIG_MP_VHT_HW_TX_MODE = n

CONFIG_DRVEXT_MODULE = n

export TopDIR ?= $(shell pwd)

########### COMMON  #################################
ifeq ($(CONFIG_GSPI_HCI), y)
HCI_NAME = gspi
endif

ifeq ($(CONFIG_SDIO_HCI), y)
HCI_NAME = sdio
endif

ifeq ($(CONFIG_USB_HCI), y)
HCI_NAME = usb
endif

ifeq ($(CONFIG_PCI_HCI), y)
HCI_NAME = pci
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

ifeq ($(CONFIG_SDIO_HCI), y)
_OS_INTFS_FILES += os_dep/linux/$(HCI_NAME)_ops_linux.o
endif

ifeq ($(CONFIG_GSPI_HCI), y)
_OS_INTFS_FILES += os_dep/linux/$(HCI_NAME)_ops_linux.o
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
		hal/phydm/phydm_adc_sampling.o\
		hal/phydm/phydm_kfree.o\
		hal/phydm/phydm_ccx.o\
		hal/phydm/phydm_psd.o

EXTRA_CFLAGS += -I$(src)/hal/btc
_OUTSRC_FILES += hal/btc/halbtc8723bwifionly.o \
				hal/btc/halbtc8822bwifionly.o \
				hal/btc/halbtc8821cwifionly.o
ifeq ($(CONFIG_BT_COEXIST), y)
_OUTSRC_FILES += hal/btc/halbtc8192e1ant.o \
				hal/btc/halbtc8192e2ant.o \
				hal/btc/halbtc8723b1ant.o \
				hal/btc/halbtc8723b2ant.o \
				hal/btc/halbtc8812a1ant.o \
				hal/btc/halbtc8812a2ant.o \
				hal/btc/halbtc8821a1ant.o \
				hal/btc/halbtc8821a2ant.o \
				hal/btc/halbtc8703b1ant.o \
				hal/btc/halbtc8723d1ant.o \
				hal/btc/halbtc8723d2ant.o \
				hal/btc/halbtc8822b1ant.o \
				hal/btc/halbtc8822b2ant.o \
				hal/btc/halbtc8821c1ant.o \
				hal/btc/halbtc8821c2ant.o
endif


########### HAL_RTL8188E #################################
ifeq ($(CONFIG_RTL8188E), y)

RTL871X = rtl8188e
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8189es
endif

ifeq ($(CONFIG_GSPI_HCI), y)
MODULE_NAME = 8189es
endif

ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8188eu
endif

ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8188ee
endif
EXTRA_CFLAGS += -DCONFIG_RTL8188E

_HAL_INTFS_FILES +=	hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8188EPwrSeq.o\
 					hal/$(RTL871X)/$(RTL871X)_xmit.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8188e_s_fw.o \
			hal/$(RTL871X)/hal8188e_t_fw.o \
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188E_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188E_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188E_SDIO.o
endif

#hal/OUTSRC/$(RTL871X)/Hal8188EFWImg_CE.o
_OUTSRC_FILES += hal/phydm/$(RTL871X)/halhwimg8188e_mac.o\
		hal/phydm/$(RTL871X)/halhwimg8188e_bb.o\
		hal/phydm/$(RTL871X)/halhwimg8188e_rf.o\
		hal/phydm/$(RTL871X)/halphyrf_8188e_ce.o\
		hal/phydm/$(RTL871X)/phydm_regconfig8188e.o\
		hal/phydm/$(RTL871X)/hal8188erateadaptive.o\
		hal/phydm/$(RTL871X)/phydm_rtl8188e.o

endif

########### HAL_RTL8192E #################################
ifeq ($(CONFIG_RTL8192E), y)

RTL871X = rtl8192e
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8192es
endif

ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8192eu
endif

ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8192ee
endif
EXTRA_CFLAGS += -DCONFIG_RTL8192E
_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8192EPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_xmit.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8192e_fw.o \
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8192E_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8192E_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8192E_SDIO.o
endif

#hal/OUTSRC/$(RTL871X)/HalHWImg8188E_FW.o
_OUTSRC_FILES += hal/phydm/$(RTL871X)/halhwimg8192e_mac.o\
		hal/phydm/$(RTL871X)/halhwimg8192e_bb.o\
		hal/phydm/$(RTL871X)/halhwimg8192e_rf.o\
		hal/phydm/$(RTL871X)/halphyrf_8192e_ce.o\
		hal/phydm/$(RTL871X)/phydm_regconfig8192e.o\
		hal/phydm/$(RTL871X)/phydm_rtl8192e.o

endif

########### HAL_RTL8812A_RTL8821A #################################

ifneq ($(CONFIG_RTL8812A)_$(CONFIG_RTL8821A), n_n)

RTL871X = rtl8812a
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8812au
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8812ae
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8812as
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

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_RTL8812A), y)
ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8812A_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8812A_PCIE.o
endif
endif
ifeq ($(CONFIG_RTL8821A), y)
ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8821A_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8821A_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8821A_SDIO.o
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
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME := 8821ae
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME := 8821as
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

########### HAL_RTL8723B #################################
ifeq ($(CONFIG_RTL8723B), y)

RTL871X = rtl8723b
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8723bu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8723be
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8723bs
endif

EXTRA_CFLAGS += -DCONFIG_RTL8723B

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8723BPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8723b_fw.o

_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723B_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723B_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723B_SDIO.o
endif

_OUTSRC_FILES += hal/phydm/$(RTL871X)/halhwimg8723b_bb.o\
								hal/phydm/$(RTL871X)/halhwimg8723b_mac.o\
								hal/phydm/$(RTL871X)/halhwimg8723b_rf.o\
								hal/phydm/$(RTL871X)/halhwimg8723b_mp.o\
								hal/phydm/$(RTL871X)/phydm_regconfig8723b.o\
								hal/phydm/$(RTL871X)/halphyrf_8723b_ce.o\
								hal/phydm/$(RTL871X)/phydm_rtl8723b.o

endif

########### HAL_RTL8814A #################################
ifeq ($(CONFIG_RTL8814A), y)
## ADD NEW VHT MP HW TX MODE ##
#EXTRA_CFLAGS += -DCONFIG_MP_VHT_HW_TX_MODE
#CONFIG_MP_VHT_HW_TX_MODE = y
##########################################
RTL871X = rtl8814a
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8814au
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8814ae
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8814as
endif

EXTRA_CFLAGS += -DCONFIG_RTL8814A

_HAL_INTFS_FILES +=  hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8814PwrSeq.o \
					hal/$(RTL871X)/$(RTL871X)_xmit.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8814a_fw.o


_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8814A_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8814A_PCIE.o
endif

_OUTSRC_FILES += hal/phydm/$(RTL871X)/halhwimg8814a_bb.o\
								hal/phydm/$(RTL871X)/halhwimg8814a_mac.o\
								hal/phydm/$(RTL871X)/halhwimg8814a_rf.o\
								hal/phydm/$(RTL871X)/phydm_iqk_8814a.o\
								hal/phydm/$(RTL871X)/phydm_regconfig8814a.o\
								hal/phydm/$(RTL871X)/halphyrf_8814a_ce.o\
								hal/phydm/$(RTL871X)/phydm_rtl8814a.o\
								hal/phydm/txbf/haltxbf8814a.o

endif

########### HAL_RTL8723C #################################
ifeq ($(CONFIG_RTL8723C), y)

RTL871X = rtl8703b
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8723cu
MODULE_SUB_NAME = 8703bu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8723ce
MODULE_SUB_NAME = 8703be
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8723cs
MODULE_SUB_NAME = 8703bs
endif

EXTRA_CFLAGS += -DCONFIG_RTL8703B

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8703BPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8703b_fw.o

_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8703B_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8703B_PCIE.o
endif

_OUTSRC_FILES += hal/phydm/$(RTL871X)/halhwimg8703b_bb.o\
								hal/phydm/$(RTL871X)/halhwimg8703b_mac.o\
								hal/phydm/$(RTL871X)/halhwimg8703b_rf.o\
								hal/phydm/$(RTL871X)/phydm_regconfig8703b.o\
								hal/phydm/$(RTL871X)/halphyrf_8703b.o
endif

########### HAL_RTL8723D #################################
ifeq ($(CONFIG_RTL8723D), y)

RTL871X = rtl8723d
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8723du
MODULE_SUB_NAME = 8723du
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8723de
MODULE_SUB_NAME = 8723de
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8723ds
MODULE_SUB_NAME = 8723ds
endif

EXTRA_CFLAGS += -DCONFIG_RTL8723D

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8723DPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8723d_fw.o \
			hal/$(RTL871X)/$(RTL871X)_lps_poff.o


_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723D_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723D_PCIE.o
endif

_OUTSRC_FILES += hal/phydm/$(RTL871X)/halhwimg8723d_bb.o\
								hal/phydm/$(RTL871X)/halhwimg8723d_mac.o\
								hal/phydm/$(RTL871X)/halhwimg8723d_rf.o\
								hal/phydm/$(RTL871X)/phydm_regconfig8723d.o\
								hal/phydm/$(RTL871X)/phydm_rtl8723d.o\
								hal/phydm/$(RTL871X)/halphyrf_8723d.o
endif

########### HAL_RTL8188F #################################
ifeq ($(CONFIG_RTL8188F), y)

RTL871X = rtl8188f
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8188fu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8188fe
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8189fs
endif

EXTRA_CFLAGS += -DCONFIG_RTL8188F

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8188FPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8188f_fw.o

_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188F_USB.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188F_SDIO.o
endif

_OUTSRC_FILES += hal/phydm/$(RTL871X)/halhwimg8188f_bb.o\
								hal/phydm/$(RTL871X)/halhwimg8188f_mac.o\
								hal/phydm/$(RTL871X)/halhwimg8188f_rf.o\
								hal/phydm/$(RTL871X)/phydm_regconfig8188f.o\
								hal/phydm/$(RTL871X)/halphyrf_8188f.o \
								hal/phydm/$(RTL871X)/phydm_rtl8188f.o

endif

########### HAL_RTL8822B #################################
ifeq ($(CONFIG_RTL8822B), y)
include $(TopDIR)/rtl8822b.mk
endif

########### HAL_RTL8821C #################################
ifeq ($(CONFIG_RTL8821C), y)
include $(TopDIR)/rtl8821c.mk
endif

########### AUTO_CFG  #################################

ifeq ($(CONFIG_AUTOCFG_CP), y)

ifeq ($(CONFIG_MULTIDRV), y)
$(shell cp $(TopDIR)/autoconf_multidrv_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else
ifeq ($(CONFIG_RTL8188E)$(CONFIG_SDIO_HCI),yy)
$(shell cp $(TopDIR)/autoconf_rtl8189e_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else ifeq ($(CONFIG_RTL8188F)$(CONFIG_SDIO_HCI),yy)
$(shell cp $(TopDIR)/autoconf_rtl8189f_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else ifeq ($(CONFIG_RTL8723C),y)
$(shell cp $(TopDIR)/autoconf_rtl8723c_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else
$(shell cp $(TopDIR)/autoconf_$(RTL871X)_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
endif
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

ifeq ($(CONFIG_WIFI_TEST), y)
EXTRA_CFLAGS += -DCONFIG_WIFI_TEST
endif

ifeq ($(CONFIG_BT_COEXIST), y)
EXTRA_CFLAGS += -DCONFIG_BT_COEXIST
endif

ifeq ($(CONFIG_INTEL_WIDI), y)
EXTRA_CFLAGS += -DCONFIG_INTEL_WIDI
endif

ifeq ($(CONFIG_WAPI_SUPPORT), y)
EXTRA_CFLAGS += -DCONFIG_WAPI_SUPPORT
endif


ifeq ($(CONFIG_EFUSE_CONFIG_FILE), y)
EXTRA_CFLAGS += -DCONFIG_EFUSE_CONFIG_FILE

#EFUSE_MAP_PATH
USER_EFUSE_MAP_PATH ?=
ifneq ($(USER_EFUSE_MAP_PATH),)
EXTRA_CFLAGS += -DEFUSE_MAP_PATH=\"$(USER_EFUSE_MAP_PATH)\"
else ifeq ($(MODULE_NAME), 8189es)
EXTRA_CFLAGS += -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_8189e.map\"
else ifeq ($(MODULE_NAME), 8723bs)
EXTRA_CFLAGS += -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_8723bs.map\"
else
EXTRA_CFLAGS += -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_$(MODULE_NAME).map\"
endif

#WIFIMAC_PATH
USER_WIFIMAC_PATH ?=
ifneq ($(USER_WIFIMAC_PATH),)
EXTRA_CFLAGS += -DWIFIMAC_PATH=\"$(USER_WIFIMAC_PATH)\"
else
EXTRA_CFLAGS += -DWIFIMAC_PATH=\"/data/wifimac.txt\"
endif

endif

ifeq ($(CONFIG_EXT_CLK), y)
EXTRA_CFLAGS += -DCONFIG_EXT_CLK
endif

ifeq ($(CONFIG_TRAFFIC_PROTECT), y)
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
endif

ifeq ($(CONFIG_LOAD_PHY_PARA_FROM_FILE), y)
EXTRA_CFLAGS += -DCONFIG_LOAD_PHY_PARA_FROM_FILE
#EXTRA_CFLAGS += -DREALTEK_CONFIG_PATH_WITH_IC_NAME_FOLDER
#EXTRA_CFLAGS += -DREALTEK_CONFIG_PATH=\"/lib/firmware/\"
EXTRA_CFLAGS += -DREALTEK_CONFIG_PATH=\"\"
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

ifeq ($(CONFIG_80211W), y)
EXTRA_CFLAGS += -DCONFIG_IEEE80211W
endif

ifeq ($(CONFIG_WOWLAN), y)
EXTRA_CFLAGS += -DCONFIG_WOWLAN
ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_RTW_SDIO_PM_KEEP_POWER
endif
ifeq ($(CONFIG_DEFAULT_PATTERNS_EN), y)
EXTRA_CFLAGS += -DCONFIG_DEFAULT_PATTERNS_EN
endif
endif

ifeq ($(CONFIG_AP_WOWLAN), y)
EXTRA_CFLAGS += -DCONFIG_AP_WOWLAN
ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_RTW_SDIO_PM_KEEP_POWER
endif
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

ifeq ($(CONFIG_RTW_SDIO_PM_KEEP_POWER), y)
ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_RTW_SDIO_PM_KEEP_POWER
endif
endif

ifeq ($(CONFIG_REDUCE_TX_CPU_LOADING), y)
EXTRA_CFLAGS += -DCONFIG_REDUCE_TX_CPU_LOADING
endif

ifeq ($(CONFIG_BR_EXT), y)
BR_NAME = br0
EXTRA_CFLAGS += -DCONFIG_BR_EXT
EXTRA_CFLAGS += '-DCONFIG_BR_EXT_BRNAME="'$(BR_NAME)'"'
endif


ifeq ($(CONFIG_TDLS), y)
EXTRA_CFLAGS += -DCONFIG_TDLS
endif

ifeq ($(CONFIG_WIFI_MONITOR), y)
EXTRA_CFLAGS += -DCONFIG_WIFI_MONITOR
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

ifeq ($(CONFIG_APPEND_VENDOR_IE_ENABLE), y)
EXTRA_CFLAGS += -DCONFIG_APPEND_VENDOR_IE_ENABLE
endif

ifeq ($(CONFIG_RTW_DEBUG), y)
EXTRA_CFLAGS += -DCONFIG_RTW_DEBUG
EXTRA_CFLAGS += -DRTW_LOG_LEVEL=$(CONFIG_RTW_LOG_LEVEL)
endif

EXTRA_CFLAGS += -DDM_ODM_SUPPORT_TYPE=0x04

ifeq ($(CONFIG_MULTIDRV), y)

ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME := rtw_sdio
endif

ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME := rtw_usb
endif

ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME := rtw_pci
endif


endif

USER_MODULE_NAME ?=
ifneq ($(USER_MODULE_NAME),)
MODULE_NAME := $(USER_MODULE_NAME)
endif

ifneq ($(KERNELRELEASE),)

rtk_core :=	core/rtw_cmd.o \
		core/rtw_security.o \
		core/rtw_debug.o \
		core/rtw_io.o \
		core/rtw_ioctl_query.o \
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
		core/rtw_iol.o \
		core/rtw_sreset.o \
		core/rtw_btcoex_wifionly.o \
		core/rtw_btcoex.o \
		core/rtw_beamforming.o \
		core/rtw_odm.o \
		core/efuse/rtw_efuse.o 

ifeq ($(CONFIG_SDIO_HCI), y)
rtk_core += core/rtw_sdio.o
endif

$(MODULE_NAME)-y += $(rtk_core)

$(MODULE_NAME)-$(CONFIG_INTEL_WIDI) += core/rtw_intel_widi.o

$(MODULE_NAME)-$(CONFIG_WAPI_SUPPORT) += core/rtw_wapi.o	\
					core/rtw_wapi_sms4.o

$(MODULE_NAME)-y += $(_OS_INTFS_FILES)
$(MODULE_NAME)-y += $(_HAL_INTFS_FILES)
$(MODULE_NAME)-y += $(_OUTSRC_FILES)

$(MODULE_NAME)-$(CONFIG_MP_INCLUDED) += core/rtw_mp.o

ifeq ($(CONFIG_RTL8723B), y)
$(MODULE_NAME)-$(CONFIG_MP_INCLUDED)+= core/rtw_bt_mp.o
endif

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

