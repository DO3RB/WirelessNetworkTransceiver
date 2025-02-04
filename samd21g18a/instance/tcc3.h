/**
 * \file
 *
 * \brief Instance description for TCC3
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef _SAMD21_TCC3_INSTANCE_
#define _SAMD21_TCC3_INSTANCE_

/* ========== Register definition for TCC3 peripheral ========== */
#define REG_TCC3_CTRLA             (*(RwReg  *)0x42006000UL) /**< \brief (TCC3) Control A */
#define REG_TCC3_CTRLBCLR          (*(RwReg8 *)0x42006004UL) /**< \brief (TCC3) Control B Clear */
#define REG_TCC3_CTRLBSET          (*(RwReg8 *)0x42006005UL) /**< \brief (TCC3) Control B Set */
#define REG_TCC3_SYNCBUSY          (*(RoReg  *)0x42006008UL) /**< \brief (TCC3) Synchronization Busy */
#define REG_TCC3_FCTRLA            (*(RwReg  *)0x4200600CUL) /**< \brief (TCC3) Recoverable Fault A Configuration */
#define REG_TCC3_FCTRLB            (*(RwReg  *)0x42006010UL) /**< \brief (TCC3) Recoverable Fault B Configuration */
#define REG_TCC3_WEXCTRL           (*(RwReg  *)0x42006014UL) /**< \brief (TCC3) Waveform Extension Configuration */
#define REG_TCC3_DRVCTRL           (*(RwReg  *)0x42006018UL) /**< \brief (TCC3) Driver Control */
#define REG_TCC3_DBGCTRL           (*(RwReg8 *)0x4200601EUL) /**< \brief (TCC3) Debug Control */
#define REG_TCC3_EVCTRL            (*(RwReg  *)0x42006020UL) /**< \brief (TCC3) Event Control */
#define REG_TCC3_INTENCLR          (*(RwReg  *)0x42006024UL) /**< \brief (TCC3) Interrupt Enable Clear */
#define REG_TCC3_INTENSET          (*(RwReg  *)0x42006028UL) /**< \brief (TCC3) Interrupt Enable Set */
#define REG_TCC3_INTFLAG           (*(RwReg  *)0x4200602CUL) /**< \brief (TCC3) Interrupt Flag Status and Clear */
#define REG_TCC3_STATUS            (*(RwReg  *)0x42006030UL) /**< \brief (TCC3) Status */
#define REG_TCC3_COUNT             (*(RwReg  *)0x42006034UL) /**< \brief (TCC3) Count */
#define REG_TCC3_PATT              (*(RwReg16*)0x42006038UL) /**< \brief (TCC3) Pattern */
#define REG_TCC3_WAVE              (*(RwReg  *)0x4200603CUL) /**< \brief (TCC3) Waveform Control */
#define REG_TCC3_PER               (*(RwReg  *)0x42006040UL) /**< \brief (TCC3) Period */
#define REG_TCC3_CC0               (*(RwReg  *)0x42006044UL) /**< \brief (TCC3) Compare and Capture 0 */
#define REG_TCC3_CC1               (*(RwReg  *)0x42006048UL) /**< \brief (TCC3) Compare and Capture 1 */
#define REG_TCC3_CC2               (*(RwReg  *)0x4200604CUL) /**< \brief (TCC3) Compare and Capture 2 */
#define REG_TCC3_CC3               (*(RwReg  *)0x42006050UL) /**< \brief (TCC3) Compare and Capture 3 */
#define REG_TCC3_PATTB             (*(RwReg16*)0x42006064UL) /**< \brief (TCC3) Pattern Buffer */
#define REG_TCC3_WAVEB             (*(RwReg  *)0x42006068UL) /**< \brief (TCC3) Waveform Control Buffer */
#define REG_TCC3_PERB              (*(RwReg  *)0x4200606CUL) /**< \brief (TCC3) Period Buffer */
#define REG_TCC3_CCB0              (*(RwReg  *)0x42006070UL) /**< \brief (TCC3) Compare and Capture Buffer 0 */
#define REG_TCC3_CCB1              (*(RwReg  *)0x42006074UL) /**< \brief (TCC3) Compare and Capture Buffer 1 */
#define REG_TCC3_CCB2              (*(RwReg  *)0x42006078UL) /**< \brief (TCC3) Compare and Capture Buffer 2 */
#define REG_TCC3_CCB3              (*(RwReg  *)0x4200607CUL) /**< \brief (TCC3) Compare and Capture Buffer 3 */

/* ========== Instance parameters for TCC3 peripheral ========== */
#define TCC3_CC_NUM                 4        // Number of Compare/Capture units
#define TCC3_DITHERING              1        // Dithering feature implemented
#define TCC3_DMAC_ID_MC_0           46
#define TCC3_DMAC_ID_MC_1           47
#define TCC3_DMAC_ID_MC_2           48
#define TCC3_DMAC_ID_MC_3           49
#define TCC3_DMAC_ID_MC_LSB         46
#define TCC3_DMAC_ID_MC_MSB         49
#define TCC3_DMAC_ID_MC_SIZE        4
#define TCC3_DMAC_ID_OVF            45       // DMA overflow/underflow/retrigger trigger
#define TCC3_DTI                    1        // Dead-Time-Insertion feature implemented
#define TCC3_EXT                    31       // Coding of implemented extended features
#define TCC3_GCLK_ID                37       // Index of Generic Clock
#define TCC3_MASTER                 0       
#define TCC3_OTMX                   1        // Output Matrix feature implemented
#define TCC3_OW_NUM                 8        // Number of Output Waveforms
#define TCC3_PG                     1        // Pattern Generation feature implemented
#define TCC3_SIZE                   24      
#define TCC3_SWAP                   1        // DTI outputs swap feature implemented
#define TCC3_TYPE                   0        // TCC type 0 : NA, 1 : Master, 2 : Slave

#endif /* _SAMD21_TCC3_INSTANCE_ */
