/**
 * \file
 *
 * \brief Instance description for TC3
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_TC3_INSTANCE_
#define _SAMD21_TC3_INSTANCE_

/* ========== Register definition for TC3 peripheral ========== */
#define REG_TC3_CTRLA              (*(RwReg16*)0x42002C00U) /**< \brief (TC3) Control A */
#define REG_TC3_READREQ            (*(RwReg16*)0x42002C02U) /**< \brief (TC3) Read Request */
#define REG_TC3_CTRLBCLR           (*(RwReg8 *)0x42002C04U) /**< \brief (TC3) Control B Clear */
#define REG_TC3_CTRLBSET           (*(RwReg8 *)0x42002C05U) /**< \brief (TC3) Control B Set */
#define REG_TC3_CTRLC              (*(RwReg8 *)0x42002C06U) /**< \brief (TC3) Control C */
#define REG_TC3_DBGCTRL            (*(RwReg8 *)0x42002C08U) /**< \brief (TC3) Debug Control */
#define REG_TC3_EVCTRL             (*(RwReg16*)0x42002C0AU) /**< \brief (TC3) Event Control */
#define REG_TC3_INTENCLR           (*(RwReg8 *)0x42002C0CU) /**< \brief (TC3) Interrupt Enable Clear */
#define REG_TC3_INTENSET           (*(RwReg8 *)0x42002C0DU) /**< \brief (TC3) Interrupt Enable Set */
#define REG_TC3_INTFLAG            (*(RwReg8 *)0x42002C0EU) /**< \brief (TC3) Interrupt Flag Status and Clear */
#define REG_TC3_STATUS             (*(RoReg8 *)0x42002C0FU) /**< \brief (TC3) Status */
#define REG_TC3_COUNT16_COUNT      (*(RwReg16*)0x42002C10U) /**< \brief (TC3) COUNT16 Counter Value */
#define REG_TC3_COUNT16_CC0        (*(RwReg16*)0x42002C18U) /**< \brief (TC3) COUNT16 Compare/Capture 0 */
#define REG_TC3_COUNT16_CC1        (*(RwReg16*)0x42002C1AU) /**< \brief (TC3) COUNT16 Compare/Capture 1 */
#define REG_TC3_COUNT32_COUNT      (*(RwReg  *)0x42002C10U) /**< \brief (TC3) COUNT32 Counter Value */
#define REG_TC3_COUNT32_CC0        (*(RwReg  *)0x42002C18U) /**< \brief (TC3) COUNT32 Compare/Capture 0 */
#define REG_TC3_COUNT32_CC1        (*(RwReg  *)0x42002C1CU) /**< \brief (TC3) COUNT32 Compare/Capture 1 */
#define REG_TC3_COUNT8_COUNT       (*(RwReg8 *)0x42002C10U) /**< \brief (TC3) COUNT8 Counter Value */
#define REG_TC3_COUNT8_PER         (*(RwReg8 *)0x42002C14U) /**< \brief (TC3) COUNT8 Period Value */
#define REG_TC3_COUNT8_CC0         (*(RwReg8 *)0x42002C18U) /**< \brief (TC3) COUNT8 Compare/Capture 0 */
#define REG_TC3_COUNT8_CC1         (*(RwReg8 *)0x42002C19U) /**< \brief (TC3) COUNT8 Compare/Capture 1 */

/* ========== Instance parameters for TC3 peripheral ========== */
#define TC3_CC8_NUM                 2        // Number of 8-bit Counters
#define TC3_CC16_NUM                2        // Number of 16-bit Counters
#define TC3_CC32_NUM                2        // Number of 32-bit Counters
#define TC3_DITHERING_EXT           0        // Dithering feature implemented
#define TC3_DMAC_ID_MC_0            25
#define TC3_DMAC_ID_MC_1            26
#define TC3_DMAC_ID_MC_LSB          25
#define TC3_DMAC_ID_MC_MSB          26
#define TC3_DMAC_ID_MC_SIZE         2
#define TC3_DMAC_ID_OVF             24       // Indexes of DMA Overflow trigger
#define TC3_GCLK_ID                 27       // Index of Generic Clock
#define TC3_MASTER                  0       
#define TC3_OW_NUM                  2        // Number of Output Waveforms
#define TC3_PERIOD_EXT              0        // Period feature implemented
#define TC3_SHADOW_EXT              0        // Shadow feature implemented

#endif /* _SAMD21_TC3_INSTANCE_ */
