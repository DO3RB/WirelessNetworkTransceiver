/**
 * \file
 *
 * \brief Instance description for TCC0
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

#ifndef _SAMD21_TCC0_INSTANCE_
#define _SAMD21_TCC0_INSTANCE_

/* ========== Register definition for TCC0 peripheral ========== */
#define REG_TCC0_CTRLA             (*(RwReg  *)0x42002000U) /**< \brief (TCC0) Control A */
#define REG_TCC0_CTRLBCLR          (*(RwReg8 *)0x42002004U) /**< \brief (TCC0) Control B Clear */
#define REG_TCC0_CTRLBSET          (*(RwReg8 *)0x42002005U) /**< \brief (TCC0) Control B Set */
#define REG_TCC0_SYNCBUSY          (*(RoReg  *)0x42002008U) /**< \brief (TCC0) Synchronization Busy */
#define REG_TCC0_FCTRLA            (*(RwReg  *)0x4200200CU) /**< \brief (TCC0) Recoverable Fault A Configuration */
#define REG_TCC0_FCTRLB            (*(RwReg  *)0x42002010U) /**< \brief (TCC0) Recoverable Fault B Configuration */
#define REG_TCC0_WEXCTRL           (*(RwReg  *)0x42002014U) /**< \brief (TCC0) Waveform Extension Configuration */
#define REG_TCC0_DRVCTRL           (*(RwReg  *)0x42002018U) /**< \brief (TCC0) Driver Control */
#define REG_TCC0_DBGCTRL           (*(RwReg8 *)0x4200201EU) /**< \brief (TCC0) Debug Control */
#define REG_TCC0_EVCTRL            (*(RwReg  *)0x42002020U) /**< \brief (TCC0) Event Control */
#define REG_TCC0_INTENCLR          (*(RwReg  *)0x42002024U) /**< \brief (TCC0) Interrupt Enable Clear */
#define REG_TCC0_INTENSET          (*(RwReg  *)0x42002028U) /**< \brief (TCC0) Interrupt Enable Set */
#define REG_TCC0_INTFLAG           (*(RwReg  *)0x4200202CU) /**< \brief (TCC0) Interrupt Flag Status and Clear */
#define REG_TCC0_STATUS            (*(RwReg  *)0x42002030U) /**< \brief (TCC0) Status */
#define REG_TCC0_COUNT             (*(RwReg  *)0x42002034U) /**< \brief (TCC0) Count */
#define REG_TCC0_PATT              (*(RwReg16*)0x42002038U) /**< \brief (TCC0) Pattern */
#define REG_TCC0_WAVE              (*(RwReg  *)0x4200203CU) /**< \brief (TCC0) Waveform Control */
#define REG_TCC0_PER               (*(RwReg  *)0x42002040U) /**< \brief (TCC0) Period */
#define REG_TCC0_CC0               (*(RwReg  *)0x42002044U) /**< \brief (TCC0) Compare and Capture 0 */
#define REG_TCC0_CC1               (*(RwReg  *)0x42002048U) /**< \brief (TCC0) Compare and Capture 1 */
#define REG_TCC0_CC2               (*(RwReg  *)0x4200204CU) /**< \brief (TCC0) Compare and Capture 2 */
#define REG_TCC0_CC3               (*(RwReg  *)0x42002050U) /**< \brief (TCC0) Compare and Capture 3 */
#define REG_TCC0_PATTB             (*(RwReg16*)0x42002064U) /**< \brief (TCC0) Pattern Buffer */
#define REG_TCC0_WAVEB             (*(RwReg  *)0x42002068U) /**< \brief (TCC0) Waveform Control Buffer */
#define REG_TCC0_PERB              (*(RwReg  *)0x4200206CU) /**< \brief (TCC0) Period Buffer */
#define REG_TCC0_CCB0              (*(RwReg  *)0x42002070U) /**< \brief (TCC0) Compare and Capture Buffer 0 */
#define REG_TCC0_CCB1              (*(RwReg  *)0x42002074U) /**< \brief (TCC0) Compare and Capture Buffer 1 */
#define REG_TCC0_CCB2              (*(RwReg  *)0x42002078U) /**< \brief (TCC0) Compare and Capture Buffer 2 */
#define REG_TCC0_CCB3              (*(RwReg  *)0x4200207CU) /**< \brief (TCC0) Compare and Capture Buffer 3 */

/* ========== Instance parameters for TCC0 peripheral ========== */
#define TCC0_CC_NUM                 4        // Number of Compare/Capture units
#define TCC0_DITHERING              1        // Dithering feature implemented
#define TCC0_DMAC_ID_MC_0           14
#define TCC0_DMAC_ID_MC_1           15
#define TCC0_DMAC_ID_MC_2           16
#define TCC0_DMAC_ID_MC_3           17
#define TCC0_DMAC_ID_MC_LSB         14
#define TCC0_DMAC_ID_MC_MSB         17
#define TCC0_DMAC_ID_MC_SIZE        4
#define TCC0_DMAC_ID_OVF            13       // DMA overflow/underflow/retrigger trigger
#define TCC0_DTI                    1        // Dead-Time-Insertion feature implemented
#define TCC0_EXT                    31       // (@_DITHERING*16+@_PG*8+@_SWAP*4+@_DTI*2+@_OTMX*1)
#define TCC0_GCLK_ID                26       // Index of Generic Clock
#define TCC0_MASTER                 0       
#define TCC0_OTMX                   1        // Output Matrix feature implemented
#define TCC0_OW_NUM                 8        // Number of Output Waveforms
#define TCC0_PG                     1        // Pattern Generation feature implemented
#define TCC0_SIZE                   24      
#define TCC0_SWAP                   1        // DTI outputs swap feature implemented
#define TCC0_TYPE                   1        // TCC type 0 : NA, 1 : Master, 2 : Slave

#endif /* _SAMD21_TCC0_INSTANCE_ */
