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
#define _HAL_MP_C_

#include <drv_types.h>

#ifdef CONFIG_MP_INCLUDED
	#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
		#include <rtl8812a_hal.h>
	#endif

u8 MgntQuery_NssTxRate(u16 Rate)
{
	u8	NssNum = RF_TX_NUM_NONIMPLEMENT;

	if ((Rate >= MGN_MCS8 && Rate <= MGN_MCS15) ||
	    (Rate >= MGN_VHT2SS_MCS0 && Rate <= MGN_VHT2SS_MCS9))
		NssNum = RF_2TX;
	else if ((Rate >= MGN_MCS16 && Rate <= MGN_MCS23) ||
		 (Rate >= MGN_VHT3SS_MCS0 && Rate <= MGN_VHT3SS_MCS9))
		NssNum = RF_3TX;
	else if ((Rate >= MGN_MCS24 && Rate <= MGN_MCS31) ||
		 (Rate >= MGN_VHT4SS_MCS0 && Rate <= MGN_VHT4SS_MCS9))
		NssNum = RF_4TX;
	else
		NssNum = RF_1TX;

	return NssNum;
}

void hal_mpt_SwitchRfSetting(PADAPTER	pAdapter)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.mpt_ctx);
	u8				ChannelToSw = pMptCtx->MptChannelToSw;
	ULONG				ulRateIdx = pMptCtx->mpt_rate_index;
	ULONG				ulbandwidth = pMptCtx->MptBandWidth;

	/* <20120525, Kordan> Dynamic mechanism for APK, asked by Dennis.*/
	if (IS_HARDWARE_TYPE_8188ES(pAdapter) && (1 <= ChannelToSw && ChannelToSw <= 11) &&
	    (ulRateIdx == MPT_RATE_MCS0 || ulRateIdx == MPT_RATE_1M || ulRateIdx == MPT_RATE_6M)) {
		pMptCtx->backup0x52_RF_A = (u1Byte)phy_query_rf_reg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0);
		pMptCtx->backup0x52_RF_B = (u1Byte)phy_query_rf_reg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0);

		if ((PlatformEFIORead4Byte(pAdapter, 0xF4) & BIT29) == BIT29) {
			phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0, 0xB);
			phy_set_rf_reg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0, 0xB);
		} else {
			phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0, 0xD);
			phy_set_rf_reg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0, 0xD);
		}
	} else if (IS_HARDWARE_TYPE_8188EE(pAdapter)) { /* <20140903, VincentL> Asked by RF Eason and Edlu*/
		if (ChannelToSw == 3 && ulbandwidth == MPT_BW_40MHZ) {
			phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0, 0xB); /*RF 0x52 = 0x0007E4BD*/
			phy_set_rf_reg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0, 0xB); /*RF 0x52 = 0x0007E4BD*/
		} else {
			phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0, 0x9); /*RF 0x52 = 0x0007E49D*/
			phy_set_rf_reg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0, 0x9); /*RF 0x52 = 0x0007E49D*/
		}
	} else if (IS_HARDWARE_TYPE_8188E(pAdapter)) {
		phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0, pMptCtx->backup0x52_RF_A);
		phy_set_rf_reg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0, pMptCtx->backup0x52_RF_B);
	}
}

s32 hal_mpt_SetPowerTracking(PADAPTER padapter, u8 enable)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct PHY_DM_STRUCT		*pDM_Odm = &(pHalData->odmpriv);


	if (!netif_running(padapter->pnetdev)) {
		return _FAIL;
	}

	if (check_fwstate(&padapter->mlmepriv, WIFI_MP_STATE) == _FALSE) {
		return _FAIL;
	}
	if (enable)
		pDM_Odm->rf_calibrate_info.txpowertrack_control = _TRUE;
	else
		pDM_Odm->rf_calibrate_info.txpowertrack_control = _FALSE;

	return _SUCCESS;
}

void hal_mpt_GetPowerTracking(PADAPTER padapter, u8 *enable)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct PHY_DM_STRUCT		*pDM_Odm = &(pHalData->odmpriv);


	*enable = pDM_Odm->rf_calibrate_info.txpowertrack_control;
}


void hal_mpt_CCKTxPowerAdjust(PADAPTER Adapter, BOOLEAN bInCH14)
{
	u32		TempVal = 0, TempVal2 = 0, TempVal3 = 0;
	u32		CurrCCKSwingVal = 0, CCKSwingIndex = 12;
	u8		i;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PMPT_CONTEXT		pMptCtx = &(Adapter->mppriv.mpt_ctx);
	u1Byte				u1Channel = pHalData->current_channel;
	ULONG				ulRateIdx = pMptCtx->mpt_rate_index;
	u1Byte				DataRate = 0xFF;

	/* Do not modify CCK TX filter parameters for 8822B*/
	if(IS_HARDWARE_TYPE_8822B(Adapter) || IS_HARDWARE_TYPE_8821C(Adapter) || IS_HARDWARE_TYPE_8723D(Adapter))
		return;

	DataRate = mpt_to_mgnt_rate(ulRateIdx);

	if (u1Channel == 14 && IS_CCK_RATE(DataRate))
		pHalData->bCCKinCH14 = TRUE;
	else
		pHalData->bCCKinCH14 = FALSE;

	if (IS_HARDWARE_TYPE_8703B(Adapter)) {
		if ((u1Channel == 14) && IS_CCK_RATE(DataRate)) {
			/* Channel 14 in CCK, need to set 0xA26~0xA29 to 0 for 8703B */
			phy_set_bb_reg(Adapter, rCCK0_TxFilter2, bMaskHWord, 0);
			phy_set_bb_reg(Adapter, rCCK0_DebugPort, bMaskLWord, 0);

		} else {
			/* Normal setting for 8703B, just recover to the default setting. */
			/* This hardcore values reference from the parameter which BB team gave. */
			for (i = 0 ; i < 2 ; ++i)
				phy_set_bb_reg(Adapter, pHalData->RegForRecover[i].offset, bMaskDWord, pHalData->RegForRecover[i].value);

		}
	} else if (IS_HARDWARE_TYPE_8723D(Adapter)) {
		/* 2.4G CCK TX DFIR */
		/* 2016.01.20 Suggest from RS BB mingzhi*/
		if ((u1Channel == 14)) {
			phy_set_bb_reg(Adapter, rCCK0_TxFilter2, bMaskDWord, 0x0000B81C);
			phy_set_bb_reg(Adapter, rCCK0_DebugPort, bMaskDWord, 0x00000000);
			phy_set_bb_reg(Adapter, 0xAAC, bMaskDWord, 0x00003667);
		} else {
			for (i = 0 ; i < 3 ; ++i) {
				phy_set_bb_reg(Adapter,
					     pHalData->RegForRecover[i].offset,
					     bMaskDWord,
					     pHalData->RegForRecover[i].value);
			}
		}
	} else if (IS_HARDWARE_TYPE_8188F(Adapter)) {
		/* get current cck swing value and check 0xa22 & 0xa23 later to match the table.*/
		CurrCCKSwingVal = read_bbreg(Adapter, rCCK0_TxFilter1, bMaskHWord);
		CCKSwingIndex = 20; /* default index */

		if (!pHalData->bCCKinCH14) {
			/* Readback the current bb cck swing value and compare with the table to */
			/* get the current swing index */
			for (i = 0; i < CCK_TABLE_SIZE_88F; i++) {
				if (((CurrCCKSwingVal & 0xff) == (u32)cck_swing_table_ch1_ch13_88f[i][0]) &&
				    (((CurrCCKSwingVal & 0xff00) >> 8) == (u32)cck_swing_table_ch1_ch13_88f[i][1])) {
					CCKSwingIndex = i;
					break;
				}
			}
			write_bbreg(Adapter, 0xa22, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][0]);
			write_bbreg(Adapter, 0xa23, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][1]);
			write_bbreg(Adapter, 0xa24, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][2]);
			write_bbreg(Adapter, 0xa25, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][3]);
			write_bbreg(Adapter, 0xa26, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][4]);
			write_bbreg(Adapter, 0xa27, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][5]);
			write_bbreg(Adapter, 0xa28, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][6]);
			write_bbreg(Adapter, 0xa29, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][7]);
			write_bbreg(Adapter, 0xa9a, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][8]);
			write_bbreg(Adapter, 0xa9b, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][9]);
			write_bbreg(Adapter, 0xa9c, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][10]);
			write_bbreg(Adapter, 0xa9d, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][11]);
			write_bbreg(Adapter, 0xaa0, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][12]);
			write_bbreg(Adapter, 0xaa1, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][13]);
			write_bbreg(Adapter, 0xaa2, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][14]);
			write_bbreg(Adapter, 0xaa3, bMaskByte0, cck_swing_table_ch1_ch13_88f[CCKSwingIndex][15]);
			RTW_INFO("%s , cck_swing_table_ch1_ch13_88f[%d]\n", __func__, CCKSwingIndex);
		}  else {
			for (i = 0; i < CCK_TABLE_SIZE_88F; i++) {
				if (((CurrCCKSwingVal & 0xff) == (u32)cck_swing_table_ch14_88f[i][0]) &&
				    (((CurrCCKSwingVal & 0xff00) >> 8) == (u32)cck_swing_table_ch14_88f[i][1])) {
					CCKSwingIndex = i;
					break;
				}
			}
			write_bbreg(Adapter, 0xa22, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][0]);
			write_bbreg(Adapter, 0xa23, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][1]);
			write_bbreg(Adapter, 0xa24, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][2]);
			write_bbreg(Adapter, 0xa25, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][3]);
			write_bbreg(Adapter, 0xa26, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][4]);
			write_bbreg(Adapter, 0xa27, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][5]);
			write_bbreg(Adapter, 0xa28, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][6]);
			write_bbreg(Adapter, 0xa29, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][7]);
			write_bbreg(Adapter, 0xa9a, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][8]);
			write_bbreg(Adapter, 0xa9b, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][9]);
			write_bbreg(Adapter, 0xa9c, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][10]);
			write_bbreg(Adapter, 0xa9d, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][11]);
			write_bbreg(Adapter, 0xaa0, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][12]);
			write_bbreg(Adapter, 0xaa1, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][13]);
			write_bbreg(Adapter, 0xaa2, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][14]);
			write_bbreg(Adapter, 0xaa3, bMaskByte0, cck_swing_table_ch14_88f[CCKSwingIndex][15]);
			RTW_INFO("%s , cck_swing_table_ch14_88f[%d]\n", __func__, CCKSwingIndex);
		}
	} else {

		/* get current cck swing value and check 0xa22 & 0xa23 later to match the table.*/
		CurrCCKSwingVal = read_bbreg(Adapter, rCCK0_TxFilter1, bMaskHWord);

		if (!pHalData->bCCKinCH14) {
			/* Readback the current bb cck swing value and compare with the table to */
			/* get the current swing index */
			for (i = 0; i < CCK_TABLE_SIZE; i++) {
				if (((CurrCCKSwingVal & 0xff) == (u32)cck_swing_table_ch1_ch13[i][0]) &&
				    (((CurrCCKSwingVal & 0xff00) >> 8) == (u32)cck_swing_table_ch1_ch13[i][1])) {
					CCKSwingIndex = i;
					break;
				}
			}

			/*Write 0xa22 0xa23*/
			TempVal = cck_swing_table_ch1_ch13[CCKSwingIndex][0] +
				(cck_swing_table_ch1_ch13[CCKSwingIndex][1] << 8);


			/*Write 0xa24 ~ 0xa27*/
			TempVal2 = 0;
			TempVal2 = cck_swing_table_ch1_ch13[CCKSwingIndex][2] +
				(cck_swing_table_ch1_ch13[CCKSwingIndex][3] << 8) +
				(cck_swing_table_ch1_ch13[CCKSwingIndex][4] << 16) +
				(cck_swing_table_ch1_ch13[CCKSwingIndex][5] << 24);

			/*Write 0xa28  0xa29*/
			TempVal3 = 0;
			TempVal3 = cck_swing_table_ch1_ch13[CCKSwingIndex][6] +
				(cck_swing_table_ch1_ch13[CCKSwingIndex][7] << 8);
		}  else {
			for (i = 0; i < CCK_TABLE_SIZE; i++) {
				if (((CurrCCKSwingVal & 0xff) == (u32)cck_swing_table_ch14[i][0]) &&
				    (((CurrCCKSwingVal & 0xff00) >> 8) == (u32)cck_swing_table_ch14[i][1])) {
					CCKSwingIndex = i;
					break;
				}
			}

			/*Write 0xa22 0xa23*/
			TempVal = cck_swing_table_ch14[CCKSwingIndex][0] +
				  (cck_swing_table_ch14[CCKSwingIndex][1] << 8);

			/*Write 0xa24 ~ 0xa27*/
			TempVal2 = 0;
			TempVal2 = cck_swing_table_ch14[CCKSwingIndex][2] +
				   (cck_swing_table_ch14[CCKSwingIndex][3] << 8) +
				(cck_swing_table_ch14[CCKSwingIndex][4] << 16) +
				   (cck_swing_table_ch14[CCKSwingIndex][5] << 24);

			/*Write 0xa28  0xa29*/
			TempVal3 = 0;
			TempVal3 = cck_swing_table_ch14[CCKSwingIndex][6] +
				   (cck_swing_table_ch14[CCKSwingIndex][7] << 8);
		}

		write_bbreg(Adapter, rCCK0_TxFilter1, bMaskHWord, TempVal);
		write_bbreg(Adapter, rCCK0_TxFilter2, bMaskDWord, TempVal2);
		write_bbreg(Adapter, rCCK0_DebugPort, bMaskLWord, TempVal3);
	}

}

void hal_mpt_SetChannel(PADAPTER pAdapter)
{
	u8 eRFPath;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	struct PHY_DM_STRUCT		*pDM_Odm = &(pHalData->odmpriv);
	struct mp_priv	*pmp = &pAdapter->mppriv;
	u8		channel = pmp->channel;
	u8		bandwidth = pmp->bandwidth;

	hal_mpt_SwitchRfSetting(pAdapter);

	pHalData->bSwChnl = _TRUE;
	pHalData->bSetChnlBW = _TRUE;
	rtw_hal_set_chnl_bw(pAdapter, channel, bandwidth, 0, 0);

	hal_mpt_CCKTxPowerAdjust(pAdapter, pHalData->bCCKinCH14);

}

/*
 * Notice
 *	Switch bandwitdth may change center frequency(channel)
 */
void hal_mpt_SetBandwidth(PADAPTER pAdapter)
{
	struct mp_priv *pmp = &pAdapter->mppriv;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	u8		channel = pmp->channel;
	u8		bandwidth = pmp->bandwidth;

	pHalData->bSwChnl = _TRUE;
	pHalData->bSetChnlBW = _TRUE;
	rtw_hal_set_chnl_bw(pAdapter, channel, bandwidth, 0, 0);

	hal_mpt_SwitchRfSetting(pAdapter);
}

void mpt_SetTxPower_Old(PADAPTER pAdapter, MPT_TXPWR_DEF Rate, u8 *pTxPower)
{
	switch (Rate) {
	case MPT_CCK: {
		u4Byte	TxAGC = 0, pwr = 0;
		u1Byte	rf;

		pwr = pTxPower[ODM_RF_PATH_A];
		if (pwr < 0x3f) {
			TxAGC = (pwr << 16) | (pwr << 8) | (pwr);
			phy_set_bb_reg(pAdapter, rTxAGC_A_CCK1_Mcs32, bMaskByte1, pTxPower[ODM_RF_PATH_A]);
			phy_set_bb_reg(pAdapter, rTxAGC_B_CCK11_A_CCK2_11, 0xffffff00, TxAGC);
		}
		pwr = pTxPower[ODM_RF_PATH_B];
		if (pwr < 0x3f) {
			TxAGC = (pwr << 16) | (pwr << 8) | (pwr);
			phy_set_bb_reg(pAdapter, rTxAGC_B_CCK11_A_CCK2_11, bMaskByte0, pTxPower[ODM_RF_PATH_B]);
			phy_set_bb_reg(pAdapter, rTxAGC_B_CCK1_55_Mcs32, 0xffffff00, TxAGC);
		}
	}
	break;

	case MPT_OFDM_AND_HT: {
		u4Byte	TxAGC = 0;
		u1Byte	pwr = 0, rf;

		pwr = pTxPower[0];
		if (pwr < 0x3f) {
			TxAGC |= ((pwr << 24) | (pwr << 16) | (pwr << 8) | pwr);
			RTW_INFO("HT Tx-rf(A) Power = 0x%x\n", TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_A_Rate18_06, bMaskDWord, TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_A_Rate54_24, bMaskDWord, TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_A_Mcs03_Mcs00, bMaskDWord, TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_A_Mcs07_Mcs04, bMaskDWord, TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_A_Mcs11_Mcs08, bMaskDWord, TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_A_Mcs15_Mcs12, bMaskDWord, TxAGC);
		}
		TxAGC = 0;
		pwr = pTxPower[1];
		if (pwr < 0x3f) {
			TxAGC |= ((pwr << 24) | (pwr << 16) | (pwr << 8) | pwr);
			RTW_INFO("HT Tx-rf(B) Power = 0x%x\n", TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_B_Rate18_06, bMaskDWord, TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_B_Rate54_24, bMaskDWord, TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_B_Mcs03_Mcs00, bMaskDWord, TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_B_Mcs07_Mcs04, bMaskDWord, TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_B_Mcs11_Mcs08, bMaskDWord, TxAGC);
			phy_set_bb_reg(pAdapter, rTxAGC_B_Mcs15_Mcs12, bMaskDWord, TxAGC);
		}
	}
	break;

	default:
		break;
	}
	RTW_INFO("<===mpt_SetTxPower_Old()\n");
}

void
mpt_SetTxPower(
	PADAPTER		pAdapter,
	MPT_TXPWR_DEF	Rate,
	pu1Byte	pTxPower
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	u1Byte path = 0 , i = 0, MaxRate = MGN_6M;
	u1Byte StartPath = ODM_RF_PATH_A, EndPath = ODM_RF_PATH_B;

	if (IS_HARDWARE_TYPE_8814A(pAdapter))
		EndPath = ODM_RF_PATH_D;
	else if (IS_HARDWARE_TYPE_8188F(pAdapter) || IS_HARDWARE_TYPE_8723D(pAdapter) || IS_HARDWARE_TYPE_8821C(pAdapter))
		EndPath = ODM_RF_PATH_A;

	switch (Rate) {
	case MPT_CCK: {
		u1Byte rate[] = {MGN_1M, MGN_2M, MGN_5_5M, MGN_11M};

		for (path = StartPath; path <= EndPath; path++)
			for (i = 0; i < sizeof(rate); ++i)
				PHY_SetTxPowerIndex(pAdapter, pTxPower[path], path, rate[i]);
	}
	break;
	case MPT_OFDM: {
		u1Byte rate[] = {
			MGN_6M, MGN_9M, MGN_12M, MGN_18M,
			MGN_24M, MGN_36M, MGN_48M, MGN_54M,
		};

		for (path = StartPath; path <= EndPath; path++)
			for (i = 0; i < sizeof(rate); ++i)
				PHY_SetTxPowerIndex(pAdapter, pTxPower[path], path, rate[i]);
	}
	break;
	case MPT_HT: {
		u1Byte rate[] = {
			MGN_MCS0, MGN_MCS1, MGN_MCS2, MGN_MCS3, MGN_MCS4,
			MGN_MCS5, MGN_MCS6, MGN_MCS7, MGN_MCS8, MGN_MCS9,
			MGN_MCS10, MGN_MCS11, MGN_MCS12, MGN_MCS13, MGN_MCS14,
			MGN_MCS15, MGN_MCS16, MGN_MCS17, MGN_MCS18, MGN_MCS19,
			MGN_MCS20, MGN_MCS21, MGN_MCS22, MGN_MCS23, MGN_MCS24,
			MGN_MCS25, MGN_MCS26, MGN_MCS27, MGN_MCS28, MGN_MCS29,
			MGN_MCS30, MGN_MCS31,
		};
		if (pHalData->rf_type == RF_3T3R)
			MaxRate = MGN_MCS23;
		else if (pHalData->rf_type == RF_2T2R)
			MaxRate = MGN_MCS15;
		else
			MaxRate = MGN_MCS7;
		for (path = StartPath; path <= EndPath; path++) {
			for (i = 0; i < sizeof(rate); ++i) {
				if (rate[i] > MaxRate)
					break;
				PHY_SetTxPowerIndex(pAdapter, pTxPower[path], path, rate[i]);
			}
		}
	}
	break;
	case MPT_VHT: {
		u1Byte rate[] = {
			MGN_VHT1SS_MCS0, MGN_VHT1SS_MCS1, MGN_VHT1SS_MCS2, MGN_VHT1SS_MCS3, MGN_VHT1SS_MCS4,
			MGN_VHT1SS_MCS5, MGN_VHT1SS_MCS6, MGN_VHT1SS_MCS7, MGN_VHT1SS_MCS8, MGN_VHT1SS_MCS9,
			MGN_VHT2SS_MCS0, MGN_VHT2SS_MCS1, MGN_VHT2SS_MCS2, MGN_VHT2SS_MCS3, MGN_VHT2SS_MCS4,
			MGN_VHT2SS_MCS5, MGN_VHT2SS_MCS6, MGN_VHT2SS_MCS7, MGN_VHT2SS_MCS8, MGN_VHT2SS_MCS9,
			MGN_VHT3SS_MCS0, MGN_VHT3SS_MCS1, MGN_VHT3SS_MCS2, MGN_VHT3SS_MCS3, MGN_VHT3SS_MCS4,
			MGN_VHT3SS_MCS5, MGN_VHT3SS_MCS6, MGN_VHT3SS_MCS7, MGN_VHT3SS_MCS8, MGN_VHT3SS_MCS9,
			MGN_VHT4SS_MCS0, MGN_VHT4SS_MCS1, MGN_VHT4SS_MCS2, MGN_VHT4SS_MCS3, MGN_VHT4SS_MCS4,
			MGN_VHT4SS_MCS5, MGN_VHT4SS_MCS6, MGN_VHT4SS_MCS7, MGN_VHT4SS_MCS8, MGN_VHT4SS_MCS9,
		};
		if (pHalData->rf_type == RF_3T3R)
			MaxRate = MGN_VHT3SS_MCS9;
		else if (pHalData->rf_type == RF_2T2R || pHalData->rf_type == RF_2T4R)
			MaxRate = MGN_VHT2SS_MCS9;
		else
			MaxRate = MGN_VHT1SS_MCS9;

		for (path = StartPath; path <= EndPath; path++) {
			for (i = 0; i < sizeof(rate); ++i) {
				if (rate[i] > MaxRate)
					break;
				PHY_SetTxPowerIndex(pAdapter, pTxPower[path], path, rate[i]);
			}
		}
	}
	break;
	default:
		RTW_INFO("<===mpt_SetTxPower: Illegal channel!!\n");
		break;
	}
}

void hal_mpt_SetTxPower(PADAPTER pAdapter)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.mpt_ctx);
	struct PHY_DM_STRUCT		*pDM_Odm = &pHalData->odmpriv;

	if (pHalData->rf_chip < RF_TYPE_MAX) {
		if (IS_HARDWARE_TYPE_8188E(pAdapter) ||
		    IS_HARDWARE_TYPE_8723B(pAdapter) ||
		    IS_HARDWARE_TYPE_8192E(pAdapter) ||
		    IS_HARDWARE_TYPE_8703B(pAdapter) ||
		    IS_HARDWARE_TYPE_8188F(pAdapter)) {
			u8 path = (pHalData->antenna_tx_path == ANTENNA_A) ? (ODM_RF_PATH_A) : (ODM_RF_PATH_B);

			RTW_INFO("===> MPT_ProSetTxPower: Old\n");

			mpt_SetTxPower_Old(pAdapter, MPT_CCK, pMptCtx->TxPwrLevel);
			mpt_SetTxPower_Old(pAdapter, MPT_OFDM_AND_HT, pMptCtx->TxPwrLevel);

		} else {
			RTW_INFO("===> MPT_ProSetTxPower: Jaguar/Jaguar2\n");
			mpt_SetTxPower(pAdapter, MPT_CCK, pMptCtx->TxPwrLevel);
			mpt_SetTxPower(pAdapter, MPT_OFDM, pMptCtx->TxPwrLevel);
			mpt_SetTxPower(pAdapter, MPT_HT, pMptCtx->TxPwrLevel);
			mpt_SetTxPower(pAdapter, MPT_VHT, pMptCtx->TxPwrLevel);

		}
	} else
		RTW_INFO("RFChipID < RF_TYPE_MAX, the RF chip is not supported - %d\n", pHalData->rf_chip);

	odm_clear_txpowertracking_state(pDM_Odm);
}

void hal_mpt_SetDataRate(PADAPTER pAdapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.mpt_ctx);
	u32 DataRate;

	DataRate = mpt_to_mgnt_rate(pMptCtx->mpt_rate_index);

	hal_mpt_SwitchRfSetting(pAdapter);

	hal_mpt_CCKTxPowerAdjust(pAdapter, pHalData->bCCKinCH14);
}

#define RF_PATH_AB	22

#if	defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
void mpt_SetRFPath_8812A(PADAPTER pAdapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT	pMptCtx = &pAdapter->mppriv.mpt_ctx;
	struct mp_priv *pmp = &pAdapter->mppriv;
	u8		channel = pmp->channel;
	u8		bandwidth = pmp->bandwidth;
	u8		eLNA_2g = pHalData->ExternalLNA_2G;
	u32		ulAntennaTx, ulAntennaRx;
	u32 reg0xC50 = 0;

	ulAntennaTx = pHalData->antenna_tx_path;
	ulAntennaRx = pHalData->AntennaRxPath;

	switch (ulAntennaTx) {
	case ANTENNA_A:
		pMptCtx->mpt_rf_path = ODM_RF_PATH_A;
		phy_set_bb_reg(pAdapter, rTxPath_Jaguar, bMaskLWord, 0x1111);
		if (pHalData->rfe_type == 3 && IS_HARDWARE_TYPE_8812(pAdapter))
			phy_set_bb_reg(pAdapter, r_ANTSEL_SW_Jaguar, bMask_AntselPathFollow_Jaguar, 0x0);
		break;
	case ANTENNA_B:
		pMptCtx->mpt_rf_path = ODM_RF_PATH_B;
		phy_set_bb_reg(pAdapter, rTxPath_Jaguar, bMaskLWord, 0x2222);
		if (pHalData->rfe_type == 3 && IS_HARDWARE_TYPE_8812(pAdapter))
			phy_set_bb_reg(pAdapter,	r_ANTSEL_SW_Jaguar, bMask_AntselPathFollow_Jaguar, 0x1);
		break;
	case ANTENNA_AB:
		pMptCtx->mpt_rf_path = ODM_RF_PATH_AB;
		phy_set_bb_reg(pAdapter, rTxPath_Jaguar, bMaskLWord, 0x3333);
		if (pHalData->rfe_type == 3 && IS_HARDWARE_TYPE_8812(pAdapter))
			phy_set_bb_reg(pAdapter, r_ANTSEL_SW_Jaguar, bMask_AntselPathFollow_Jaguar, 0x0);
		break;
	default:
		pMptCtx->mpt_rf_path = ODM_RF_PATH_AB;
		RTW_INFO("Unknown Tx antenna.\n");
		break;
	}

	switch (ulAntennaRx) {
	case ANTENNA_A:
		phy_set_bb_reg(pAdapter, rRxPath_Jaguar, bMaskByte0, 0x11);
		phy_set_rf_reg(pAdapter, ODM_RF_PATH_B, RF_AC_Jaguar, 0xF0000, 0x1); /*/ RF_B_0x0[19:16] = 1, Standby mode*/
		phy_set_bb_reg(pAdapter, rCCK_RX_Jaguar, bCCK_RX_Jaguar, 0x0);
		phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC_Jaguar, BIT19 | BIT18 | BIT17 | BIT16, 0x3);

		/*/ <20121101, Kordan> To prevent gain table from not switched, asked by Ynlin.*/
		reg0xC50 = phy_query_bb_reg(pAdapter, rA_IGI_Jaguar, bMaskByte0);
		phy_set_bb_reg(pAdapter, rA_IGI_Jaguar, bMaskByte0, reg0xC50 + 2);
		phy_set_bb_reg(pAdapter, rA_IGI_Jaguar, bMaskByte0, reg0xC50);

		/* set PWED_TH for BB Yn user guide R29 */
		if (IS_HARDWARE_TYPE_8812(pAdapter)) {
			if (channel <= 14) { /* 2.4G */
				if (bandwidth == CHANNEL_WIDTH_20
				    && eLNA_2g == 0) {
					/* 0x830[3:1]=3'b010 */
					phy_set_bb_reg(pAdapter, rPwed_TH_Jaguar, BIT1 | BIT2 | BIT3, 0x02);
				} else
					/* 0x830[3:1]=3'b100 */
					phy_set_bb_reg(pAdapter, rPwed_TH_Jaguar, BIT1 | BIT2 | BIT3, 0x04);
			} else
				/* 0x830[3:1]=3'b100 for 5G */
				phy_set_bb_reg(pAdapter, rPwed_TH_Jaguar, BIT1 | BIT2 | BIT3, 0x04);
		}
		break;
	case ANTENNA_B:
		phy_set_bb_reg(pAdapter, rRxPath_Jaguar, bMaskByte0, 0x22);
		phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC_Jaguar, 0xF0000, 0x1);/*/ RF_A_0x0[19:16] = 1, Standby mode */
		phy_set_bb_reg(pAdapter, rCCK_RX_Jaguar, bCCK_RX_Jaguar, 0x1);
		phy_set_rf_reg(pAdapter, ODM_RF_PATH_B, RF_AC_Jaguar, BIT19 | BIT18 | BIT17 | BIT16, 0x3);

		/*/ <20121101, Kordan> To prevent gain table from not switched, asked by Ynlin.*/
		reg0xC50 = phy_query_bb_reg(pAdapter, rB_IGI_Jaguar, bMaskByte0);
		phy_set_bb_reg(pAdapter, rB_IGI_Jaguar, bMaskByte0, reg0xC50 + 2);
		phy_set_bb_reg(pAdapter, rB_IGI_Jaguar, bMaskByte0, reg0xC50);

		/* set PWED_TH for BB Yn user guide R29 */
		if (IS_HARDWARE_TYPE_8812(pAdapter)) {
			if (channel <= 14) {
				if (bandwidth == CHANNEL_WIDTH_20
				    && eLNA_2g == 0) {
					/* 0x830[3:1]=3'b010 */
					phy_set_bb_reg(pAdapter, rPwed_TH_Jaguar, BIT1 | BIT2 | BIT3, 0x02);
				} else
					/* 0x830[3:1]=3'b100 */
					phy_set_bb_reg(pAdapter, rPwed_TH_Jaguar, BIT1 | BIT2 | BIT3, 0x04);
			} else
				/* 0x830[3:1]=3'b100 for 5G */
				phy_set_bb_reg(pAdapter, rPwed_TH_Jaguar, BIT1 | BIT2 | BIT3, 0x04);
		}
		break;
	case ANTENNA_AB:
		phy_set_bb_reg(pAdapter, rRxPath_Jaguar, bMaskByte0, 0x33);
		phy_set_rf_reg(pAdapter, ODM_RF_PATH_B, RF_AC_Jaguar, 0xF0000, 0x3); /*/ RF_B_0x0[19:16] = 3, Rx mode*/
		phy_set_bb_reg(pAdapter, rCCK_RX_Jaguar, bCCK_RX_Jaguar, 0x0);
		/* set PWED_TH for BB Yn user guide R29 */
		phy_set_bb_reg(pAdapter, rPwed_TH_Jaguar, BIT1 | BIT2 | BIT3, 0x04);
		break;
	default:
		RTW_INFO("Unknown Rx antenna.\n");
		break;
	}
}
#endif

void mpt_SetRFPath_819X(PADAPTER	pAdapter)
{
	HAL_DATA_TYPE			*pHalData	= GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.mpt_ctx);
	u4Byte			ulAntennaTx, ulAntennaRx;
	R_ANTENNA_SELECT_OFDM	*p_ofdm_tx;	/* OFDM Tx register */
	R_ANTENNA_SELECT_CCK	*p_cck_txrx;
	u1Byte		r_rx_antenna_ofdm = 0, r_ant_select_cck_val = 0;
	u1Byte		chgTx = 0, chgRx = 0;
	u4Byte		r_ant_sel_cck_val = 0, r_ant_select_ofdm_val = 0, r_ofdm_tx_en_val = 0;

	ulAntennaTx = pHalData->antenna_tx_path;
	ulAntennaRx = pHalData->AntennaRxPath;

	p_ofdm_tx = (R_ANTENNA_SELECT_OFDM *)&r_ant_select_ofdm_val;
	p_cck_txrx = (R_ANTENNA_SELECT_CCK *)&r_ant_select_cck_val;

	p_ofdm_tx->r_ant_ht1			= 0x1;
	p_ofdm_tx->r_ant_ht2			= 0x2;/*Second TX RF path is A*/
	p_ofdm_tx->r_ant_non_ht			= 0x3;/*/ 0x1+0x2=0x3 */

	switch (ulAntennaTx) {
	case ANTENNA_A:
		p_ofdm_tx->r_tx_antenna		= 0x1;
		r_ofdm_tx_en_val		= 0x1;
		p_ofdm_tx->r_ant_l		= 0x1;
		p_ofdm_tx->r_ant_ht_s1		= 0x1;
		p_ofdm_tx->r_ant_non_ht_s1	= 0x1;
		p_cck_txrx->r_ccktx_enable	= 0x8;
		chgTx = 1;
		/*/ From SD3 Willis suggestion !!! Set RF A=TX and B as standby*/
		/*/if (IS_HARDWARE_TYPE_8192S(pAdapter))*/
		{
			phy_set_bb_reg(pAdapter, rFPGA0_XA_HSSIParameter2, 0xe, 2);
			phy_set_bb_reg(pAdapter, rFPGA0_XB_HSSIParameter2, 0xe, 1);
			r_ofdm_tx_en_val			= 0x3;
			/*/ Power save*/
			/*/cosa r_ant_select_ofdm_val = 0x11111111;*/
			/*/ We need to close RFB by SW control*/
			if (pHalData->rf_type == RF_2T2R) {
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT10, 0);
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT26, 1);
				phy_set_bb_reg(pAdapter, rFPGA0_XB_RFInterfaceOE, BIT10, 0);
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFParameter, BIT1, 1);
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFParameter, BIT17, 0);
			}
		}
		pMptCtx->mpt_rf_path = ODM_RF_PATH_A;
		break;
	case ANTENNA_B:
		p_ofdm_tx->r_tx_antenna		= 0x2;
		r_ofdm_tx_en_val		= 0x2;
		p_ofdm_tx->r_ant_l		= 0x2;
		p_ofdm_tx->r_ant_ht_s1		= 0x2;
		p_ofdm_tx->r_ant_non_ht_s1	= 0x2;
		p_cck_txrx->r_ccktx_enable	= 0x4;
		chgTx = 1;
		/*/ From SD3 Willis suggestion !!! Set RF A as standby*/
		/*/if (IS_HARDWARE_TYPE_8192S(pAdapter))*/
		{
			phy_set_bb_reg(pAdapter, rFPGA0_XA_HSSIParameter2, 0xe, 1);
			phy_set_bb_reg(pAdapter, rFPGA0_XB_HSSIParameter2, 0xe, 2);

			/*/ 2008/10/31 MH From SD3 Willi's suggestion. We must read RF 1T table.*/
			/*/ 2009/01/08 MH From Sd3 Willis. We need to close RFA by SW control*/
			if (pHalData->rf_type == RF_2T2R || pHalData->rf_type == RF_1T2R) {
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT10, 1);
				phy_set_bb_reg(pAdapter, rFPGA0_XA_RFInterfaceOE, BIT10, 0);
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT26, 0);
				/*/phy_set_bb_reg(pAdapter, rFPGA0_XB_RFInterfaceOE, BIT10, 0);*/
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFParameter, BIT1, 0);
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFParameter, BIT17, 1);
			}
		}
		pMptCtx->mpt_rf_path = ODM_RF_PATH_B;
		break;
	case ANTENNA_AB:/*/ For 8192S*/
		p_ofdm_tx->r_tx_antenna		= 0x3;
		r_ofdm_tx_en_val		= 0x3;
		p_ofdm_tx->r_ant_l		= 0x3;
		p_ofdm_tx->r_ant_ht_s1		= 0x3;
		p_ofdm_tx->r_ant_non_ht_s1	= 0x3;
		p_cck_txrx->r_ccktx_enable	= 0xC;
		chgTx = 1;
		/*/ From SD3Willis suggestion !!! Set RF B as standby*/
		/*/if (IS_HARDWARE_TYPE_8192S(pAdapter))*/
		{
			phy_set_bb_reg(pAdapter, rFPGA0_XA_HSSIParameter2, 0xe, 2);
			phy_set_bb_reg(pAdapter, rFPGA0_XB_HSSIParameter2, 0xe, 2);
			/* Disable Power save*/
			/*cosa r_ant_select_ofdm_val = 0x3321333;*/
			/* 2009/01/08 MH From Sd3 Willis. We need to enable RFA/B by SW control*/
			if (pHalData->rf_type == RF_2T2R) {
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT10, 0);

				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT26, 0);
				/*/phy_set_bb_reg(pAdapter, rFPGA0_XB_RFInterfaceOE, BIT10, 0);*/
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFParameter, BIT1, 1);
				phy_set_bb_reg(pAdapter, rFPGA0_XAB_RFParameter, BIT17, 1);
			}
		}
		pMptCtx->mpt_rf_path = ODM_RF_PATH_AB;
		break;
	default:
		break;
	}
	switch (ulAntennaRx) {
	case ANTENNA_A:
		r_rx_antenna_ofdm		= 0x1;	/* A*/
		p_cck_txrx->r_cckrx_enable	= 0x0;	/* default: A*/
		p_cck_txrx->r_cckrx_enable_2	= 0x0;	/* option: A*/
		chgRx = 1;
		break;
	case ANTENNA_B:
		r_rx_antenna_ofdm			= 0x2;	/*/ B*/
		p_cck_txrx->r_cckrx_enable	= 0x1;	/*/ default: B*/
		p_cck_txrx->r_cckrx_enable_2	= 0x1;	/*/ option: B*/
		chgRx = 1;
		break;
	case ANTENNA_AB:/*/ For 8192S and 8192E/U...*/
		r_rx_antenna_ofdm		= 0x3;/*/ AB*/
		p_cck_txrx->r_cckrx_enable	= 0x0;/*/ default:A*/
		p_cck_txrx->r_cckrx_enable_2	= 0x1;/*/ option:B*/
		chgRx = 1;
		break;
	default:
		break;
	}


	if (chgTx && chgRx) {
		switch (pHalData->rf_chip) {
		case RF_8225:
		case RF_8256:
		case RF_6052:
			/*/r_ant_sel_cck_val = r_ant_select_cck_val;*/
			phy_set_bb_reg(pAdapter, rFPGA1_TxInfo, 0x7fffffff, r_ant_select_ofdm_val);		/*/OFDM Tx*/
			phy_set_bb_reg(pAdapter, rFPGA0_TxInfo, 0x0000000f, r_ofdm_tx_en_val);		/*/OFDM Tx*/
			phy_set_bb_reg(pAdapter, rOFDM0_TRxPathEnable, 0x0000000f, r_rx_antenna_ofdm);	/*/OFDM Rx*/
			phy_set_bb_reg(pAdapter, rOFDM1_TRxPathEnable, 0x0000000f, r_rx_antenna_ofdm);	/*/OFDM Rx*/
			if (IS_HARDWARE_TYPE_8192E(pAdapter)) {
				phy_set_bb_reg(pAdapter, rOFDM0_TRxPathEnable, 0x000000F0, r_rx_antenna_ofdm);	/*/OFDM Rx*/
				phy_set_bb_reg(pAdapter, rOFDM1_TRxPathEnable, 0x000000F0, r_rx_antenna_ofdm);	/*/OFDM Rx*/
			}
			phy_set_bb_reg(pAdapter, rCCK0_AFESetting, bMaskByte3, r_ant_select_cck_val);/*/r_ant_sel_cck_val); /CCK TxRx*/
			break;

		default:
			RTW_INFO("Unsupported RFChipID for switching antenna.\n");
			break;
		}
	}
}	/* MPT_ProSetRFPath */


void hal_mpt_SetAntenna(PADAPTER	pAdapter)

{
	RTW_INFO("Do %s\n", __func__);
#if	defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
	if (IS_HARDWARE_TYPE_JAGUAR(pAdapter)) {
		mpt_SetRFPath_8812A(pAdapter);
		return;
	}
#endif
	mpt_SetRFPath_819X(pAdapter);
	RTW_INFO("mpt_SetRFPath_819X Do %s\n", __func__);
}

s32 hal_mpt_SetThermalMeter(PADAPTER pAdapter, u8 target_ther)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(pAdapter);

	if (!netif_running(pAdapter->pnetdev)) {
		return _FAIL;
	}


	if (check_fwstate(&pAdapter->mlmepriv, WIFI_MP_STATE) == _FALSE) {
		return _FAIL;
	}


	target_ther &= 0xff;
	if (target_ther < 0x07)
		target_ther = 0x07;
	else if (target_ther > 0x1d)
		target_ther = 0x1d;

	pHalData->eeprom_thermal_meter = target_ther;

	return _SUCCESS;
}


void hal_mpt_TriggerRFThermalMeter(PADAPTER pAdapter)
{
	phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x42, BIT17 | BIT16, 0x03);

}


u8 hal_mpt_ReadRFThermalMeter(PADAPTER pAdapter)

{
	HAL_DATA_TYPE	*hal_data = GET_HAL_DATA(pAdapter);
	struct PHY_DM_STRUCT *p_dm_odm = &hal_data->odmpriv;
	u32 ThermalValue = 0;
	s32 thermal_value_temp = 0;
	s8 thermal_offset = 0;

	ThermalValue = (u1Byte)phy_query_rf_reg(pAdapter, ODM_RF_PATH_A, 0x42, 0xfc00);	/*0x42: RF Reg[15:10]*/
	thermal_offset = phydm_get_thermal_offset(p_dm_odm);

	thermal_value_temp = ThermalValue + thermal_offset;

	if (thermal_value_temp > 63)
		ThermalValue = 63;
	else if (thermal_value_temp < 0)
		ThermalValue = 0;
	else
		ThermalValue = thermal_value_temp;

	return (u8)ThermalValue;
}


void hal_mpt_GetThermalMeter(PADAPTER pAdapter, u8 *value)
{
	hal_mpt_TriggerRFThermalMeter(pAdapter);
	rtw_msleep_os(1000);
	*value = hal_mpt_ReadRFThermalMeter(pAdapter);
}


void hal_mpt_SetSingleCarrierTx(PADAPTER pAdapter, u8 bStart)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(pAdapter);

	pAdapter->mppriv.mpt_ctx.bSingleCarrier = bStart;

	if (bStart) {/*/ Start Single Carrier.*/
		/*/ Start Single Carrier.*/
		/*/ 1. if OFDM block on?*/
		if (!phy_query_bb_reg(pAdapter, rFPGA0_RFMOD, bOFDMEn))
			phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bOFDMEn, 1); /*set OFDM block on*/

		/*/ 2. set CCK test mode off, set to CCK normal mode*/
		phy_set_bb_reg(pAdapter, rCCK0_System, bCCKBBMode, 0);

		/*/ 3. turn on scramble setting*/
		phy_set_bb_reg(pAdapter, rCCK0_System, bCCKScramble, 1);

		/*/ 4. Turn On Continue Tx and turn off the other test modes.*/
#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
		if (IS_HARDWARE_TYPE_JAGUAR_AND_JAGUAR2(pAdapter))
			phy_set_bb_reg(pAdapter, rSingleTone_ContTx_Jaguar, BIT18 | BIT17 | BIT16, OFDM_SingleCarrier);
		else
#endif /* CONFIG_RTL8812A || CONFIG_RTL8821A || CONFIG_RTL8814A || CONFIG_RTL8822B || CONFIG_RTL8821C */
			phy_set_bb_reg(pAdapter, rOFDM1_LSTF, BIT30 | BIT29 | BIT28, OFDM_SingleCarrier);

	} else {
		/*/ Stop Single Carrier.*/
		/*/ Stop Single Carrier.*/
		/*/ Turn off all test modes.*/
#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
		if (IS_HARDWARE_TYPE_JAGUAR_AND_JAGUAR2(pAdapter))
			phy_set_bb_reg(pAdapter, rSingleTone_ContTx_Jaguar, BIT18 | BIT17 | BIT16, OFDM_ALL_OFF);
		else
#endif /* CONFIG_RTL8812A || CONFIG_RTL8821A || CONFIG_RTL8814A || CONFIG_RTL8822B || CONFIG_RTL8821C */
			phy_set_bb_reg(pAdapter, rOFDM1_LSTF, BIT30 | BIT29 | BIT28, OFDM_ALL_OFF);

		rtw_msleep_os(10);
		/*/BB Reset*/
		phy_set_bb_reg(pAdapter, rPMAC_Reset, bBBResetB, 0x0);
		phy_set_bb_reg(pAdapter, rPMAC_Reset, bBBResetB, 0x1);
	}
}


void hal_mpt_SetSingleToneTx(PADAPTER pAdapter, u8 bStart)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.mpt_ctx);
	u4Byte			ulAntennaTx = pHalData->antenna_tx_path;
	static u4Byte		regRF = 0, regBB0 = 0, regBB1 = 0, regBB2 = 0, regBB3 = 0;
	u8 rfPath;

	switch (ulAntennaTx) {
	case ANTENNA_B:
		rfPath = ODM_RF_PATH_B;
		break;
	case ANTENNA_C:
		rfPath = ODM_RF_PATH_C;
		break;
	case ANTENNA_D:
		rfPath = ODM_RF_PATH_D;
		break;
	case ANTENNA_A:
	default:
		rfPath = ODM_RF_PATH_A;
		break;
	}

	pAdapter->mppriv.mpt_ctx.is_single_tone = bStart;
	if (bStart) {
		/*/ Start Single Tone.*/
		/*/ <20120326, Kordan> To amplify the power of tone for Xtal calibration. (asked by Edlu)*/
		if (IS_HARDWARE_TYPE_8188E(pAdapter)) {
			regRF = phy_query_rf_reg(pAdapter, rfPath, lna_low_gain_3, bRFRegOffsetMask);
			phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, lna_low_gain_3, BIT1, 0x1); /*/ RF LO enabled*/
			phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bCCKEn, 0x0);
			phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bOFDMEn, 0x0);
		} else if (IS_HARDWARE_TYPE_8192E(pAdapter)) { /*/ USB need to do RF LO disable first, PCIE isn't required to follow this order.*/
			/*/Set MAC REG 88C: Prevent SingleTone Fail*/
			phy_set_mac_reg(pAdapter, 0x88C, 0xF00000, 0xF);
			phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, lna_low_gain_3, BIT1, 0x1); /*/ RF LO disabled*/
			phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, RF_AC, 0xF0000, 0x2); /*/ Tx mode*/
		} else if (IS_HARDWARE_TYPE_8723B(pAdapter)) {
			if (pMptCtx->mpt_rf_path == ODM_RF_PATH_A) {
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC, 0xF0000, 0x2); /*/ Tx mode*/
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x56, 0xF, 0x1); /*/ RF LO enabled*/
			} else {
				/*/ S0/S1 both use PATH A to configure*/
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC, 0xF0000, 0x2); /*/ Tx mode*/
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x76, 0xF, 0x1); /*/ RF LO enabled*/
			}
		} else if (IS_HARDWARE_TYPE_8703B(pAdapter)) {
			if (pMptCtx->mpt_rf_path == ODM_RF_PATH_A) {
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC, 0xF0000, 0x2); /* Tx mode */
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x53, 0xF000, 0x1); /* RF LO enabled */
			}
		} else if (IS_HARDWARE_TYPE_8188F(pAdapter)) {
			/*Set BB REG 88C: Prevent SingleTone Fail*/
			phy_set_bb_reg(pAdapter, rFPGA0_AnalogParameter4, 0xF00000, 0xF);
			phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, lna_low_gain_3, BIT1, 0x1);
			phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, RF_AC, 0xF0000, 0x2);

		} else if (IS_HARDWARE_TYPE_8723D(pAdapter)) {
			if (pMptCtx->mpt_rf_path == ODM_RF_PATH_A) {
				phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bCCKEn|bOFDMEn, 0);
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC, BIT16, 0x0);
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x53, BIT0, 0x1);
			} else {/* S0/S1 both use PATH A to configure */
				phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bCCKEn|bOFDMEn, 0);
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC, BIT16, 0x0);
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x63, BIT0, 0x1);
			}
		} else if (IS_HARDWARE_TYPE_JAGUAR(pAdapter) || IS_HARDWARE_TYPE_8822B(pAdapter)) {
#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
			u1Byte p = ODM_RF_PATH_A;

			regRF = phy_query_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC_Jaguar, bRFRegOffsetMask);
			regBB0 = phy_query_bb_reg(pAdapter, rA_RFE_Pinmux_Jaguar, bMaskDWord);
			regBB1 = phy_query_bb_reg(pAdapter, rB_RFE_Pinmux_Jaguar, bMaskDWord);
			regBB2 = phy_query_bb_reg(pAdapter, rA_RFE_Pinmux_Jaguar + 4, bMaskDWord);
			regBB3 = phy_query_bb_reg(pAdapter, rB_RFE_Pinmux_Jaguar + 4, bMaskDWord);

			phy_set_bb_reg(pAdapter, rOFDMCCKEN_Jaguar, BIT29 | BIT28, 0x0); /*/ Disable CCK and OFDM*/

			if (pMptCtx->mpt_rf_path == ODM_RF_PATH_AB) {
				for (p = ODM_RF_PATH_A; p <= ODM_RF_PATH_B; ++p) {
					phy_set_rf_reg(pAdapter, p, RF_AC_Jaguar, 0xF0000, 0x2); /*/ Tx mode: RF0x00[19:16]=4'b0010 */
					phy_set_rf_reg(pAdapter, p, RF_AC_Jaguar, 0x1F, 0x0); /*/ Lowest RF gain index: RF_0x0[4:0] = 0*/
					phy_set_rf_reg(pAdapter, p, lna_low_gain_3, BIT1, 0x1); /*/ RF LO enabled*/
				}
			} else {
				phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, RF_AC_Jaguar, 0xF0000, 0x2); /*/ Tx mode: RF0x00[19:16]=4'b0010 */
				phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, RF_AC_Jaguar, 0x1F, 0x0); /*/ Lowest RF gain index: RF_0x0[4:0] = 0*/
				phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, lna_low_gain_3, BIT1, 0x1); /*/ RF LO enabled*/
			}

			phy_set_bb_reg(pAdapter, rA_RFE_Pinmux_Jaguar, 0xFF00F0, 0x77007);  /*/ 0xCB0[[23:16, 7:4] = 0x77007*/
			phy_set_bb_reg(pAdapter, rB_RFE_Pinmux_Jaguar, 0xFF00F0, 0x77007);  /*/ 0xCB0[[23:16, 7:4] = 0x77007*/

			if (pHalData->external_pa_5g) {
				phy_set_bb_reg(pAdapter, rA_RFE_Pinmux_Jaguar + 4, 0xFF00000, 0x12); /*/ 0xCB4[23:16] = 0x12*/
				phy_set_bb_reg(pAdapter, rB_RFE_Pinmux_Jaguar + 4, 0xFF00000, 0x12); /*/ 0xEB4[23:16] = 0x12*/
			} else if (pHalData->ExternalPA_2G) {
				phy_set_bb_reg(pAdapter, rA_RFE_Pinmux_Jaguar + 4, 0xFF00000, 0x11); /*/ 0xCB4[23:16] = 0x11*/
				phy_set_bb_reg(pAdapter, rB_RFE_Pinmux_Jaguar + 4, 0xFF00000, 0x11); /*/ 0xEB4[23:16] = 0x11*/
			}
#endif
		}

		else	/*/ Turn On SingleTone and turn off the other test modes.*/
			phy_set_bb_reg(pAdapter, rOFDM1_LSTF, BIT30 | BIT29 | BIT28, OFDM_SingleTone);

		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000500);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000500);

	} else {/*/ Stop Single Ton e.*/

		if (IS_HARDWARE_TYPE_8188E(pAdapter)) {
			phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, lna_low_gain_3, bRFRegOffsetMask, regRF);
			phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bCCKEn, 0x1);
			phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bOFDMEn, 0x1);
		} else if (IS_HARDWARE_TYPE_8192E(pAdapter)) {
			phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, RF_AC, 0xF0000, 0x3);/*/ Tx mode*/
			phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, lna_low_gain_3, BIT1, 0x0);/*/ RF LO disabled */
			/*/ RESTORE MAC REG 88C: Enable RF Functions*/
			phy_set_mac_reg(pAdapter, 0x88C, 0xF00000, 0x0);
		} else if (IS_HARDWARE_TYPE_8723B(pAdapter)) {
			if (pMptCtx->mpt_rf_path == ODM_RF_PATH_A) {
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC, 0xF0000, 0x3); /*/ Rx mode*/
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x56, 0xF, 0x0); /*/ RF LO disabled*/
			} else {
				/*/ S0/S1 both use PATH A to configure*/
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC, 0xF0000, 0x3); /*/ Rx mode*/
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x76, 0xF, 0x0); /*/ RF LO disabled*/
			}
		} else if (IS_HARDWARE_TYPE_8703B(pAdapter)) {
			if (pMptCtx->mpt_rf_path == ODM_RF_PATH_A) {
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC, 0xF0000, 0x3); /* Rx mode */
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x53, 0xF000, 0x0); /* RF LO disabled */
			}
		} else if (IS_HARDWARE_TYPE_8188F(pAdapter)) {
			phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, RF_AC, 0xF0000, 0x3); /*Tx mode*/
			phy_set_rf_reg(pAdapter, pMptCtx->mpt_rf_path, lna_low_gain_3, BIT1, 0x0); /*RF LO disabled*/
			/*Set BB REG 88C: Prevent SingleTone Fail*/
			phy_set_bb_reg(pAdapter, rFPGA0_AnalogParameter4, 0xF00000, 0xc);
		} else if (IS_HARDWARE_TYPE_8723D(pAdapter)) {
			if (pMptCtx->mpt_rf_path == ODM_RF_PATH_A) {
				phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bCCKEn|bOFDMEn, 0x3);
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC, BIT16, 0x1);
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x53, BIT0, 0x0);
			} else {	/* S0/S1 both use PATH A to configure */
				phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bCCKEn|bOFDMEn, 0x3);
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, RF_AC, BIT16, 0x1);
				phy_set_rf_reg(pAdapter, ODM_RF_PATH_A, 0x63, BIT0, 0x0);
			}
		} else if (IS_HARDWARE_TYPE_JAGUAR(pAdapter) || IS_HARDWARE_TYPE_8822B(pAdapter)) {
#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
			u1Byte p = ODM_RF_PATH_A;

			phy_set_bb_reg(pAdapter, rOFDMCCKEN_Jaguar, BIT29 | BIT28, 0x3); /*/ Disable CCK and OFDM*/

			if (pMptCtx->mpt_rf_path == ODM_RF_PATH_AB) {
				for (p = ODM_RF_PATH_A; p <= ODM_RF_PATH_B; ++p) {
					phy_set_rf_reg(pAdapter, p, RF_AC_Jaguar, bRFRegOffsetMask, regRF);
					phy_set_rf_reg(pAdapter, p, lna_low_gain_3, BIT1, 0x0); /*/ RF LO disabled*/
				}
			} else {
				phy_set_rf_reg(pAdapter, p, RF_AC_Jaguar, bRFRegOffsetMask, regRF);
				phy_set_rf_reg(pAdapter, p, lna_low_gain_3, BIT1, 0x0); /*/ RF LO disabled*/
			}

			phy_set_bb_reg(pAdapter, rA_RFE_Pinmux_Jaguar, bMaskDWord, regBB0);
			phy_set_bb_reg(pAdapter, rB_RFE_Pinmux_Jaguar, bMaskDWord, regBB1);
			phy_set_bb_reg(pAdapter, rA_RFE_Pinmux_Jaguar + 4, bMaskDWord, regBB2);
			phy_set_bb_reg(pAdapter, rB_RFE_Pinmux_Jaguar + 4, bMaskDWord, regBB3);
#endif
		}
		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000100);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000100);

	}
}

void hal_mpt_SetCarrierSuppressionTx(PADAPTER pAdapter, u8 bStart)
{
	u8 Rate;

	pAdapter->mppriv.mpt_ctx.is_carrier_suppression = bStart;

	Rate = HwRateToMPTRate(pAdapter->mppriv.rateidx);
	if (bStart) {/* Start Carrier Suppression.*/
		if (Rate <= MPT_RATE_11M) {
			/*/ 1. if CCK block on?*/
			if (!read_bbreg(pAdapter, rFPGA0_RFMOD, bCCKEn))
				write_bbreg(pAdapter, rFPGA0_RFMOD, bCCKEn, bEnable);/*set CCK block on*/

			/*/Turn Off All Test Mode*/
			if (IS_HARDWARE_TYPE_JAGUAR(pAdapter) || IS_HARDWARE_TYPE_8814A(pAdapter) /*|| IS_HARDWARE_TYPE_8822B(pAdapter)*/)
				phy_set_bb_reg(pAdapter, 0x914, BIT18 | BIT17 | BIT16, OFDM_ALL_OFF); /* rSingleTone_ContTx_Jaguar*/
			else
				phy_set_bb_reg(pAdapter, rOFDM1_LSTF, BIT30 | BIT29 | BIT28, OFDM_ALL_OFF);

			write_bbreg(pAdapter, rCCK0_System, bCCKBBMode, 0x2);    /*/transmit mode*/
			write_bbreg(pAdapter, rCCK0_System, bCCKScramble, 0x0);  /*/turn off scramble setting*/

			/*/Set CCK Tx Test Rate*/
			write_bbreg(pAdapter, rCCK0_System, bCCKTxRate, 0x0);    /*/Set FTxRate to 1Mbps*/
		}

		/*Set for dynamic set Power index*/
		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000500);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000500);

	} else {/* Stop Carrier Suppression.*/

		if (Rate <= MPT_RATE_11M) {
			write_bbreg(pAdapter, rCCK0_System, bCCKBBMode, 0x0);    /*normal mode*/
			write_bbreg(pAdapter, rCCK0_System, bCCKScramble, 0x1);  /*turn on scramble setting*/

			/*BB Reset*/
			write_bbreg(pAdapter, rPMAC_Reset, bBBResetB, 0x0);
			write_bbreg(pAdapter, rPMAC_Reset, bBBResetB, 0x1);
		}
		/*Stop for dynamic set Power index*/
		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000100);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000100);
	}
	RTW_INFO("\n MPT_ProSetCarrierSupp() is finished.\n");
}

u32 hal_mpt_query_phytxok(PADAPTER	pAdapter)
{
	PMPT_CONTEXT pMptCtx = &(pAdapter->mppriv.mpt_ctx);
	RT_PMAC_TX_INFO PMacTxInfo = pMptCtx->PMacTxInfo;
	u16 count = 0;

	if (IS_MPT_CCK_RATE(PMacTxInfo.TX_RATE))
		count = phy_query_bb_reg(pAdapter, 0xF50, bMaskLWord); /* [15:0]*/
	else
		count = phy_query_bb_reg(pAdapter, 0xF50, bMaskHWord); /* [31:16]*/

	if (count > 50000) {
		rtw_reset_phy_trx_ok_counters(pAdapter);
		pAdapter->mppriv.tx.sended += count;
		count = 0;
	}

	return pAdapter->mppriv.tx.sended + count;

}

static	void mpt_StopCckContTx(
	PADAPTER	pAdapter
)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT	pMptCtx = &(pAdapter->mppriv.mpt_ctx);
	u1Byte			u1bReg;

	pMptCtx->bCckContTx = FALSE;
	pMptCtx->bOfdmContTx = FALSE;

	phy_set_bb_reg(pAdapter, rCCK0_System, bCCKBBMode, 0x0);	/*normal mode*/
	phy_set_bb_reg(pAdapter, rCCK0_System, bCCKScramble, 0x1);	/*turn on scramble setting*/

	if (!IS_HARDWARE_TYPE_JAGUAR2(pAdapter)) {
		phy_set_bb_reg(pAdapter, 0xa14, 0x300, 0x0);			/* 0xa15[1:0] = 2b00*/
		phy_set_bb_reg(pAdapter, rOFDM0_TRMuxPar, 0x10000, 0x0);		/* 0xc08[16] = 0*/

		phy_set_bb_reg(pAdapter, rFPGA0_XA_HSSIParameter2, BIT14, 0);
		phy_set_bb_reg(pAdapter, rFPGA0_XB_HSSIParameter2, BIT14, 0);
		phy_set_bb_reg(pAdapter, 0x0B34, BIT14, 0);
	}

	/*BB Reset*/
	phy_set_bb_reg(pAdapter, rPMAC_Reset, bBBResetB, 0x0);
	phy_set_bb_reg(pAdapter, rPMAC_Reset, bBBResetB, 0x1);

	phy_set_bb_reg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000100);
	phy_set_bb_reg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000100);

}	/* mpt_StopCckContTx */


static	void mpt_StopOfdmContTx(
	PADAPTER	pAdapter
)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT	pMptCtx = &(pAdapter->mppriv.mpt_ctx);
	u1Byte			u1bReg;
	u4Byte			data;

	pMptCtx->bCckContTx = FALSE;
	pMptCtx->bOfdmContTx = FALSE;

	if (IS_HARDWARE_TYPE_JAGUAR(pAdapter) || IS_HARDWARE_TYPE_JAGUAR2(pAdapter))
		phy_set_bb_reg(pAdapter, 0x914, BIT18 | BIT17 | BIT16, OFDM_ALL_OFF);
	else
		phy_set_bb_reg(pAdapter, rOFDM1_LSTF, BIT30 | BIT29 | BIT28, OFDM_ALL_OFF);

	rtw_mdelay_os(10);

	if (!IS_HARDWARE_TYPE_JAGUAR(pAdapter) && !IS_HARDWARE_TYPE_JAGUAR2(pAdapter)) {
		phy_set_bb_reg(pAdapter, 0xa14, 0x300, 0x0);			/* 0xa15[1:0] = 0*/
		phy_set_bb_reg(pAdapter, rOFDM0_TRMuxPar, 0x10000, 0x0);		/* 0xc08[16] = 0*/
	}

	/*BB Reset*/
	phy_set_bb_reg(pAdapter, rPMAC_Reset, bBBResetB, 0x0);
	phy_set_bb_reg(pAdapter, rPMAC_Reset, bBBResetB, 0x1);

	phy_set_bb_reg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000100);
	phy_set_bb_reg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000100);
}	/* mpt_StopOfdmContTx */


static	void mpt_StartCckContTx(
	PADAPTER		pAdapter
)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT	pMptCtx = &(pAdapter->mppriv.mpt_ctx);
	u4Byte			cckrate;

	/* 1. if CCK block on */
	if (!phy_query_bb_reg(pAdapter, rFPGA0_RFMOD, bCCKEn))
		phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bCCKEn, 1);/*set CCK block on*/

	/*Turn Off All Test Mode*/
	if (IS_HARDWARE_TYPE_JAGUAR_AND_JAGUAR2(pAdapter))
		phy_set_bb_reg(pAdapter, 0x914, BIT18 | BIT17 | BIT16, OFDM_ALL_OFF);
	else
		phy_set_bb_reg(pAdapter, rOFDM1_LSTF, BIT30 | BIT29 | BIT28, OFDM_ALL_OFF);

	cckrate  = pAdapter->mppriv.rateidx;

	phy_set_bb_reg(pAdapter, rCCK0_System, bCCKTxRate, cckrate);

	phy_set_bb_reg(pAdapter, rCCK0_System, bCCKBBMode, 0x2);	/*transmit mode*/
	phy_set_bb_reg(pAdapter, rCCK0_System, bCCKScramble, 0x1);	/*turn on scramble setting*/

	if (!IS_HARDWARE_TYPE_JAGUAR_AND_JAGUAR2(pAdapter)) {
		phy_set_bb_reg(pAdapter, 0xa14, 0x300, 0x3);			/* 0xa15[1:0] = 11 force cck rxiq = 0*/
		phy_set_bb_reg(pAdapter, rOFDM0_TRMuxPar, 0x10000, 0x1);		/* 0xc08[16] = 1 force ofdm rxiq = ofdm txiq*/
		phy_set_bb_reg(pAdapter, rFPGA0_XA_HSSIParameter2, BIT14, 1);
		phy_set_bb_reg(pAdapter, rFPGA0_XB_HSSIParameter2, BIT14, 1);
		phy_set_bb_reg(pAdapter, 0x0B34, BIT14, 1);
	}

	phy_set_bb_reg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000500);
	phy_set_bb_reg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000500);

	pMptCtx->bCckContTx = TRUE;
	pMptCtx->bOfdmContTx = FALSE;

}	/* mpt_StartCckContTx */


static	void mpt_StartOfdmContTx(
	PADAPTER		pAdapter
)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT	pMptCtx = &(pAdapter->mppriv.mpt_ctx);

	/* 1. if OFDM block on?*/
	if (!phy_query_bb_reg(pAdapter, rFPGA0_RFMOD, bOFDMEn))
		phy_set_bb_reg(pAdapter, rFPGA0_RFMOD, bOFDMEn, 1);/*set OFDM block on*/

	/* 2. set CCK test mode off, set to CCK normal mode*/
	phy_set_bb_reg(pAdapter, rCCK0_System, bCCKBBMode, 0);

	/* 3. turn on scramble setting*/
	phy_set_bb_reg(pAdapter, rCCK0_System, bCCKScramble, 1);

	if (!IS_HARDWARE_TYPE_JAGUAR(pAdapter) && !IS_HARDWARE_TYPE_JAGUAR2(pAdapter)) {
		phy_set_bb_reg(pAdapter, 0xa14, 0x300, 0x3);			/* 0xa15[1:0] = 2b'11*/
		phy_set_bb_reg(pAdapter, rOFDM0_TRMuxPar, 0x10000, 0x1);		/* 0xc08[16] = 1*/
	}

	/* 4. Turn On Continue Tx and turn off the other test modes.*/
	if (IS_HARDWARE_TYPE_JAGUAR(pAdapter) || IS_HARDWARE_TYPE_JAGUAR2(pAdapter))
		phy_set_bb_reg(pAdapter, 0x914, BIT18 | BIT17 | BIT16, OFDM_ContinuousTx);
	else
		phy_set_bb_reg(pAdapter, rOFDM1_LSTF, BIT30 | BIT29 | BIT28, OFDM_ContinuousTx);

	phy_set_bb_reg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000500);
	phy_set_bb_reg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000500);

	pMptCtx->bCckContTx = FALSE;
	pMptCtx->bOfdmContTx = TRUE;
}	/* mpt_StartOfdmContTx */


void hal_mpt_SetContinuousTx(PADAPTER pAdapter, u8 bStart)
{
	u8 Rate;

	RT_TRACE(_module_mp_, _drv_info_,
		 ("SetContinuousTx: rate:%d\n", pAdapter->mppriv.rateidx));
	Rate = HwRateToMPTRate(pAdapter->mppriv.rateidx);
	pAdapter->mppriv.mpt_ctx.is_start_cont_tx = bStart;

	if (Rate <= MPT_RATE_11M) {
		if (bStart)
			mpt_StartCckContTx(pAdapter);
		else
			mpt_StopCckContTx(pAdapter);

	} else if (Rate >= MPT_RATE_6M) {
		if (bStart)
			mpt_StartOfdmContTx(pAdapter);
		else
			mpt_StopOfdmContTx(pAdapter);
	}
}

#endif /* CONFIG_MP_INCLUDE*/
