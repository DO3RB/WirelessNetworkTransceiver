/**
 * \file
 *
 * \brief Instance description for SERCOM3
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

#ifndef _SAMD21_SERCOM3_INSTANCE_
#define _SAMD21_SERCOM3_INSTANCE_

/* ========== Register definition for SERCOM3 peripheral ========== */
#define REG_SERCOM3_I2CM_CTRLA     (*(RwReg  *)0x42001400U) /**< \brief (SERCOM3) I2CM Control A */
#define REG_SERCOM3_I2CM_CTRLB     (*(RwReg  *)0x42001404U) /**< \brief (SERCOM3) I2CM Control B */
#define REG_SERCOM3_I2CM_BAUD      (*(RwReg  *)0x4200140CU) /**< \brief (SERCOM3) I2CM Baud Rate */
#define REG_SERCOM3_I2CM_INTENCLR  (*(RwReg8 *)0x42001414U) /**< \brief (SERCOM3) I2CM Interrupt Enable Clear */
#define REG_SERCOM3_I2CM_INTENSET  (*(RwReg8 *)0x42001416U) /**< \brief (SERCOM3) I2CM Interrupt Enable Set */
#define REG_SERCOM3_I2CM_INTFLAG   (*(RwReg8 *)0x42001418U) /**< \brief (SERCOM3) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM3_I2CM_STATUS    (*(RwReg16*)0x4200141AU) /**< \brief (SERCOM3) I2CM Status */
#define REG_SERCOM3_I2CM_SYNCBUSY  (*(RoReg  *)0x4200141CU) /**< \brief (SERCOM3) I2CM Synchronization Busy */
#define REG_SERCOM3_I2CM_ADDR      (*(RwReg  *)0x42001424U) /**< \brief (SERCOM3) I2CM Address */
#define REG_SERCOM3_I2CM_DATA      (*(RwReg8 *)0x42001428U) /**< \brief (SERCOM3) I2CM Data */
#define REG_SERCOM3_I2CM_DBGCTRL   (*(RwReg8 *)0x42001430U) /**< \brief (SERCOM3) I2CM Debug Control */
#define REG_SERCOM3_I2CS_CTRLA     (*(RwReg  *)0x42001400U) /**< \brief (SERCOM3) I2CS Control A */
#define REG_SERCOM3_I2CS_CTRLB     (*(RwReg  *)0x42001404U) /**< \brief (SERCOM3) I2CS Control B */
#define REG_SERCOM3_I2CS_INTENCLR  (*(RwReg8 *)0x42001414U) /**< \brief (SERCOM3) I2CS Interrupt Enable Clear */
#define REG_SERCOM3_I2CS_INTENSET  (*(RwReg8 *)0x42001416U) /**< \brief (SERCOM3) I2CS Interrupt Enable Set */
#define REG_SERCOM3_I2CS_INTFLAG   (*(RwReg8 *)0x42001418U) /**< \brief (SERCOM3) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM3_I2CS_STATUS    (*(RwReg16*)0x4200141AU) /**< \brief (SERCOM3) I2CS Status */
#define REG_SERCOM3_I2CS_SYNCBUSY  (*(RoReg  *)0x4200141CU) /**< \brief (SERCOM3) I2CS Synchronization Busy */
#define REG_SERCOM3_I2CS_ADDR      (*(RwReg  *)0x42001424U) /**< \brief (SERCOM3) I2CS Address */
#define REG_SERCOM3_I2CS_DATA      (*(RwReg8 *)0x42001428U) /**< \brief (SERCOM3) I2CS Data */
#define REG_SERCOM3_SPI_CTRLA      (*(RwReg  *)0x42001400U) /**< \brief (SERCOM3) SPI Control A */
#define REG_SERCOM3_SPI_CTRLB      (*(RwReg  *)0x42001404U) /**< \brief (SERCOM3) SPI Control B */
#define REG_SERCOM3_SPI_BAUD       (*(RwReg8 *)0x4200140CU) /**< \brief (SERCOM3) SPI Baud Rate */
#define REG_SERCOM3_SPI_INTENCLR   (*(RwReg8 *)0x42001414U) /**< \brief (SERCOM3) SPI Interrupt Enable Clear */
#define REG_SERCOM3_SPI_INTENSET   (*(RwReg8 *)0x42001416U) /**< \brief (SERCOM3) SPI Interrupt Enable Set */
#define REG_SERCOM3_SPI_INTFLAG    (*(RwReg8 *)0x42001418U) /**< \brief (SERCOM3) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM3_SPI_STATUS     (*(RwReg16*)0x4200141AU) /**< \brief (SERCOM3) SPI Status */
#define REG_SERCOM3_SPI_SYNCBUSY   (*(RoReg  *)0x4200141CU) /**< \brief (SERCOM3) SPI Synchronization Busy */
#define REG_SERCOM3_SPI_ADDR       (*(RwReg  *)0x42001424U) /**< \brief (SERCOM3) SPI Address */
#define REG_SERCOM3_SPI_DATA       (*(RwReg16*)0x42001428U) /**< \brief (SERCOM3) SPI Data */
#define REG_SERCOM3_SPI_DBGCTRL    (*(RwReg8 *)0x42001430U) /**< \brief (SERCOM3) SPI Debug Control */
#define REG_SERCOM3_USART_CTRLA    (*(RwReg  *)0x42001400U) /**< \brief (SERCOM3) USART Control A */
#define REG_SERCOM3_USART_CTRLB    (*(RwReg  *)0x42001404U) /**< \brief (SERCOM3) USART Control B */
#define REG_SERCOM3_USART_BAUD     (*(RwReg16*)0x4200140CU) /**< \brief (SERCOM3) USART Baud Rate */
#define REG_SERCOM3_USART_RXPL     (*(RwReg8 *)0x4200140EU) /**< \brief (SERCOM3) USART Receive Pulse Length */
#define REG_SERCOM3_USART_INTENCLR (*(RwReg8 *)0x42001414U) /**< \brief (SERCOM3) USART Interrupt Enable Clear */
#define REG_SERCOM3_USART_INTENSET (*(RwReg8 *)0x42001416U) /**< \brief (SERCOM3) USART Interrupt Enable Set */
#define REG_SERCOM3_USART_INTFLAG  (*(RwReg8 *)0x42001418U) /**< \brief (SERCOM3) USART Interrupt Flag Status and Clear */
#define REG_SERCOM3_USART_STATUS   (*(RwReg16*)0x4200141AU) /**< \brief (SERCOM3) USART Status */
#define REG_SERCOM3_USART_SYNCBUSY (*(RoReg  *)0x4200141CU) /**< \brief (SERCOM3) USART Synchronization Busy */
#define REG_SERCOM3_USART_DATA     (*(RwReg16*)0x42001428U) /**< \brief (SERCOM3) USART Data */
#define REG_SERCOM3_USART_DBGCTRL  (*(RwReg8 *)0x42001430U) /**< \brief (SERCOM3) USART Debug Control */

/* ========== Instance parameters for SERCOM3 peripheral ========== */
#define SERCOM3_DMAC_ID_RX          7        // Index of DMA RX trigger
#define SERCOM3_DMAC_ID_TX          8        // Index of DMA TX trigger
#define SERCOM3_GCLK_ID_CORE        23       // Index of Generic Clock for Core
#define SERCOM3_GCLK_ID_SLOW        19       // Index of Generic Clock for SMbus timeout
#define SERCOM3_INT_MSB             6       

#endif /* _SAMD21_SERCOM3_INSTANCE_ */
