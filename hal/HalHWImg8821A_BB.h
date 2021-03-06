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
******************************************************************************/

#ifndef __INC_MP_BB_HW_IMG_8821A_H
#define __INC_MP_BB_HW_IMG_8821A_H

static bool CheckCondition(const u32 Condition, const u32 Hex);

/******************************************************************************
*                           AGC_TAB.TXT
******************************************************************************/

void
ODM_ReadAndConfig_MP_8821A_AGC_TAB( // TC: Test Chip, MP: MP Chip
	PDM_ODM_T  pDM_Odm
);

/******************************************************************************
*                           PHY_REG.TXT
******************************************************************************/

void
ODM_ReadAndConfig_MP_8821A_PHY_REG( // TC: Test Chip, MP: MP Chip
	PDM_ODM_T  pDM_Odm
);

/******************************************************************************
*                           PHY_REG_PG.TXT
******************************************************************************/

void
ODM_ReadAndConfig_MP_8821A_PHY_REG_PG( // TC: Test Chip, MP: MP Chip
	PDM_ODM_T  pDM_Odm
);

#endif
