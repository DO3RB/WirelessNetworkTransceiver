/**
 * \file
 *
 * \brief Instance description for DSU
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

#ifndef _SAMD21_DSU_INSTANCE_
#define _SAMD21_DSU_INSTANCE_

/* ========== Register definition for DSU peripheral ========== */
#define REG_DSU_CTRL               (*(WoReg8 *)0x41002000U) /**< \brief (DSU) Control */
#define REG_DSU_STATUSA            (*(RwReg8 *)0x41002001U) /**< \brief (DSU) Status A */
#define REG_DSU_STATUSB            (*(RoReg8 *)0x41002002U) /**< \brief (DSU) Status B */
#define REG_DSU_ADDR               (*(RwReg  *)0x41002004U) /**< \brief (DSU) Address */
#define REG_DSU_LENGTH             (*(RwReg  *)0x41002008U) /**< \brief (DSU) Length */
#define REG_DSU_DATA               (*(RwReg  *)0x4100200CU) /**< \brief (DSU) Data */
#define REG_DSU_DCC0               (*(RwReg  *)0x41002010U) /**< \brief (DSU) Debug Communication Channel 0 */
#define REG_DSU_DCC1               (*(RwReg  *)0x41002014U) /**< \brief (DSU) Debug Communication Channel 1 */
#define REG_DSU_DID                (*(RoReg  *)0x41002018U) /**< \brief (DSU) Device Identification */
#define REG_DSU_ENTRY0             (*(RoReg  *)0x41003000U) /**< \brief (DSU) Coresight ROM Table Entry 0 */
#define REG_DSU_ENTRY1             (*(RoReg  *)0x41003004U) /**< \brief (DSU) Coresight ROM Table Entry 1 */
#define REG_DSU_END                (*(RoReg  *)0x41003008U) /**< \brief (DSU) Coresight ROM Table End */
#define REG_DSU_MEMTYPE            (*(RoReg  *)0x41003FCCU) /**< \brief (DSU) Coresight ROM Table Memory Type */
#define REG_DSU_PID4               (*(RoReg  *)0x41003FD0U) /**< \brief (DSU) Peripheral Identification 4 */
#define REG_DSU_PID0               (*(RoReg  *)0x41003FE0U) /**< \brief (DSU) Peripheral Identification 0 */
#define REG_DSU_PID1               (*(RoReg  *)0x41003FE4U) /**< \brief (DSU) Peripheral Identification 1 */
#define REG_DSU_PID2               (*(RoReg  *)0x41003FE8U) /**< \brief (DSU) Peripheral Identification 2 */
#define REG_DSU_PID3               (*(RoReg  *)0x41003FECU) /**< \brief (DSU) Peripheral Identification 3 */
#define REG_DSU_CID0               (*(RoReg  *)0x41003FF0U) /**< \brief (DSU) Component Identification 0 */
#define REG_DSU_CID1               (*(RoReg  *)0x41003FF4U) /**< \brief (DSU) Component Identification 1 */
#define REG_DSU_CID2               (*(RoReg  *)0x41003FF8U) /**< \brief (DSU) Component Identification 2 */
#define REG_DSU_CID3               (*(RoReg  *)0x41003FFCU) /**< \brief (DSU) Component Identification 3 */

/* ========== Instance parameters for DSU peripheral ========== */
#define DSU_CLK_HSB_ID              3        // Index of AHB clock in PM.AHBMASK register

#endif /* _SAMD21_DSU_INSTANCE_ */
