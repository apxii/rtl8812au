/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

#ifndef	__ODM_PRECOMP_H__
#define __ODM_PRECOMP_H__

#include "phydm_types.h"

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	#include "Precomp.h"		/* We need to include mp_precomp.h due to batch file setting. */
#else
	#define		TEST_FALG___		1
#endif

/* 2 Config Flags and Structs - defined by each ODM type */

#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	#include "../8192cd_cfg.h"
	#include "../odm_inc.h"

	#include "../8192cd.h"
	#include "../8192cd_util.h"

	#ifdef AP_BUILD_WORKAROUND
		#include "../8192cd_headers.h"
		#include "../8192cd_debug.h"
	#endif

#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	#define __PACK
	#define __WLAN_ATTRIB_PACK__
#endif

/* 2 OutSrc Header Files */

#include "phydm.h"
#include "phydm_hwconfig.h"
#include "phydm_debug.h"
#include "phydm_regdefine11ac.h"
#include "phydm_regdefine11n.h"
#include "phydm_interface.h"
#include "phydm_reg.h"

#if (DM_ODM_SUPPORT_TYPE & ODM_CE)

void
phy_set_tx_power_limit(
	struct PHY_DM_STRUCT	*p_dm_odm,
	u8	*regulation,
	u8	*band,
	u8	*bandwidth,
	u8	*rate_section,
	u8	*rf_path,
	u8	*channel,
	u8	*power_limit
);

#endif

#if (RTL8812A_SUPPORT == 1)
	#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		#include "rtl8812a/halphyrf_8812a_win.h"
	#elif (DM_ODM_SUPPORT_TYPE == ODM_AP)
		#include "rtl8812a/halphyrf_8812a_ap.h"
	#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
		#include "rtl8812a/halphyrf_8812a_ce.h"
	#endif

	#if (DM_ODM_SUPPORT_TYPE != ODM_AP)
		#include "rtl8812a/halhwimg8812a_bb.h"
		#include "rtl8812a/halhwimg8812a_mac.h"
		#include "rtl8812a/halhwimg8812a_rf.h"
		#include "rtl8812a/phydm_regconfig8812a.h"
		#include "rtl8812a/halhwimg8812a_fw.h"
		#include "rtl8812a/phydm_rtl8812a.h"
	#endif

	#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
		#include "rtl8812a_hal.h"
	#endif
	#include "rtl8812a/version_rtl8812a.h"

#endif /* 8812 END */

#if (RTL8821A_SUPPORT == 1)
	#include "rtl8821a/halhwimg8821a_mac.h"
	#include "rtl8821a/halhwimg8821a_rf.h"
	#include "rtl8821a/halhwimg8821a_bb.h"
	#include "rtl8821a/phydm_regconfig8821a.h"
	#include "rtl8821a/phydm_rtl8821a.h"
	#include "rtl8821a/version_rtl8821a.h"
	#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
		#include "rtl8821a/halphyrf_8821a_ce.h"
		#include "rtl8821a/phydm_iqk_8821a_ce.h"/*for IQK*/
		#include "rtl8812a/halphyrf_8812a_ce.h"/*for IQK,LCK,Power-tracking*/
		#include "rtl8812a_hal.h"
	#endif
#endif

#endif /* __ODM_PRECOMP_H__ */
