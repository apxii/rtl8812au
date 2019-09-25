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

#ifndef	__PHYDMEDCATURBOCHECK_H__
#define    __PHYDMEDCATURBOCHECK_H__

#if PHYDM_SUPPORT_EDCA
/*#define EDCATURBO_VERSION	"2.1"*/
#define EDCATURBO_VERSION	"2.3"	/*2015.07.29 by YuChen*/

struct _EDCA_TURBO_ {
	boolean is_current_turbo_edca;
	boolean is_cur_rdl_state;

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	u32	prv_traffic_idx; /* edca turbo */
#endif

};

void
odm_edca_turbo_check(
	void		*p_dm_void
);
void
odm_edca_turbo_init(
	void		*p_dm_void
);

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
void
odm_edca_turbo_check_mp(
	void		*p_dm_void
);

/* check if edca turbo is disabled */
boolean
odm_is_edca_turbo_disable(
	void		*p_dm_void
);
/* choose edca paramter for special IOT case */
void
odm_edca_para_sel_by_iot(
	void					*p_dm_void,
	u32		*EDCA_BE_UL,
	u32		*EDCA_BE_DL
);
/* check if it is UL or DL */
void
odm_edca_choose_traffic_idx(
	void		*p_dm_void,
	u64			cur_tx_bytes,
	u64			cur_rx_bytes,
	boolean		is_bias_on_rx,
	boolean		*p_is_cur_rdl_state
);

#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
void
odm_edca_turbo_check_ce(
	void		*p_dm_void
);
#endif

#endif /*PHYDM_SUPPORT_EDCA*/

#endif
