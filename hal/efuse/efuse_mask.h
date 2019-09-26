
#if DEV_BUS_TYPE == RT_USB_INTERFACE

	#if defined(CONFIG_RTL8812A)
		#include "rtl8812a/HalEfuseMask8812A_USB.h"
	#endif

	#if defined(CONFIG_RTL8821A)
		#include "rtl8812a/HalEfuseMask8821A_USB.h"
	#endif

#endif
