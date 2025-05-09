/**
 * \file
 *
 * \brief Instance description for EVSYS
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

#ifndef _SAMD21_EVSYS_INSTANCE_
#define _SAMD21_EVSYS_INSTANCE_

/* ========== Register definition for EVSYS peripheral ========== */
#define REG_EVSYS_CTRL             (*(WoReg8 *)0x42000400U) /**< \brief (EVSYS) Control */
#define REG_EVSYS_CHANNEL          (*(RwReg  *)0x42000404U) /**< \brief (EVSYS) Channel */
#define REG_EVSYS_USER             (*(RwReg16*)0x42000408U) /**< \brief (EVSYS) User Multiplexer */
#define REG_EVSYS_CHSTATUS         (*(RoReg  *)0x4200040CU) /**< \brief (EVSYS) Channel Status */
#define REG_EVSYS_INTENCLR         (*(RwReg  *)0x42000410U) /**< \brief (EVSYS) Interrupt Enable Clear */
#define REG_EVSYS_INTENSET         (*(RwReg  *)0x42000414U) /**< \brief (EVSYS) Interrupt Enable Set */
#define REG_EVSYS_INTFLAG          (*(RwReg  *)0x42000418U) /**< \brief (EVSYS) Interrupt Flag Status and Clear */

/* ========== Instance parameters for EVSYS peripheral ========== */
#define EVSYS_CHANNELS              12       // Number of Channels
#define EVSYS_CHANNELS_BITS         4        // Number of bits to select Channel
#define EVSYS_CHANNELS_MSB          11       // Number of Channels - 1
#define EVSYS_EXTEVT_NUM            0        // Number of External Event Generators
#define EVSYS_GCLK_ID_0             7
#define EVSYS_GCLK_ID_1             8
#define EVSYS_GCLK_ID_2             9
#define EVSYS_GCLK_ID_3             10
#define EVSYS_GCLK_ID_4             11
#define EVSYS_GCLK_ID_5             12
#define EVSYS_GCLK_ID_6             13
#define EVSYS_GCLK_ID_7             14
#define EVSYS_GCLK_ID_8             15
#define EVSYS_GCLK_ID_9             16
#define EVSYS_GCLK_ID_10            17
#define EVSYS_GCLK_ID_11            18
#define EVSYS_GCLK_ID_LSB           7
#define EVSYS_GCLK_ID_MSB           18
#define EVSYS_GCLK_ID_SIZE          12
#define EVSYS_GENERATORS            83       // Total Number of Event Generators
#define EVSYS_GENERATORS_BITS       7        // Number of bits to select Event Generator
#define EVSYS_USERS                 37       // Total Number of Event Users
#define EVSYS_USERS_BITS            6        // Number of bits to select Event User

// GENERATORS
#define EVSYS_ID_GEN_RTC_CMP_0      1
#define EVSYS_ID_GEN_RTC_CMP_1      2
#define EVSYS_ID_GEN_RTC_OVF        3
#define EVSYS_ID_GEN_RTC_PER_0      4
#define EVSYS_ID_GEN_RTC_PER_1      5
#define EVSYS_ID_GEN_RTC_PER_2      6
#define EVSYS_ID_GEN_RTC_PER_3      7
#define EVSYS_ID_GEN_RTC_PER_4      8
#define EVSYS_ID_GEN_RTC_PER_5      9
#define EVSYS_ID_GEN_RTC_PER_6      10
#define EVSYS_ID_GEN_RTC_PER_7      11
#define EVSYS_ID_GEN_EIC_EXTINT_0   12
#define EVSYS_ID_GEN_EIC_EXTINT_1   13
#define EVSYS_ID_GEN_EIC_EXTINT_2   14
#define EVSYS_ID_GEN_EIC_EXTINT_3   15
#define EVSYS_ID_GEN_EIC_EXTINT_4   16
#define EVSYS_ID_GEN_EIC_EXTINT_5   17
#define EVSYS_ID_GEN_EIC_EXTINT_6   18
#define EVSYS_ID_GEN_EIC_EXTINT_7   19
#define EVSYS_ID_GEN_EIC_EXTINT_8   20
#define EVSYS_ID_GEN_EIC_EXTINT_9   21
#define EVSYS_ID_GEN_EIC_EXTINT_10  22
#define EVSYS_ID_GEN_EIC_EXTINT_11  23
#define EVSYS_ID_GEN_EIC_EXTINT_12  24
#define EVSYS_ID_GEN_EIC_EXTINT_13  25
#define EVSYS_ID_GEN_EIC_EXTINT_14  26
#define EVSYS_ID_GEN_EIC_EXTINT_15  27
#define EVSYS_ID_GEN_EIC_EXTINT_16  28
#define EVSYS_ID_GEN_EIC_EXTINT_17  29
#define EVSYS_ID_GEN_DMAC_CH_0      30
#define EVSYS_ID_GEN_DMAC_CH_1      31
#define EVSYS_ID_GEN_DMAC_CH_2      32
#define EVSYS_ID_GEN_DMAC_CH_3      33
#define EVSYS_ID_GEN_TCC0_OVF       34
#define EVSYS_ID_GEN_TCC0_TRG       35
#define EVSYS_ID_GEN_TCC0_CNT       36
#define EVSYS_ID_GEN_TCC0_MCX_0     37
#define EVSYS_ID_GEN_TCC0_MCX_1     38
#define EVSYS_ID_GEN_TCC0_MCX_2     39
#define EVSYS_ID_GEN_TCC0_MCX_3     40
#define EVSYS_ID_GEN_TCC1_OVF       41
#define EVSYS_ID_GEN_TCC1_TRG       42
#define EVSYS_ID_GEN_TCC1_CNT       43
#define EVSYS_ID_GEN_TCC1_MCX_0     44
#define EVSYS_ID_GEN_TCC1_MCX_1     45
#define EVSYS_ID_GEN_TCC2_OVF       46
#define EVSYS_ID_GEN_TCC2_TRG       47
#define EVSYS_ID_GEN_TCC2_CNT       48
#define EVSYS_ID_GEN_TCC2_MCX_0     49
#define EVSYS_ID_GEN_TCC2_MCX_1     50
#define EVSYS_ID_GEN_TC3_OVF        51
#define EVSYS_ID_GEN_TC3_MCX_0      52
#define EVSYS_ID_GEN_TC3_MCX_1      53
#define EVSYS_ID_GEN_TC4_OVF        54
#define EVSYS_ID_GEN_TC4_MCX_0      55
#define EVSYS_ID_GEN_TC4_MCX_1      56
#define EVSYS_ID_GEN_TC5_OVF        57
#define EVSYS_ID_GEN_TC5_MCX_0      58
#define EVSYS_ID_GEN_TC5_MCX_1      59
#define EVSYS_ID_GEN_TC6_OVF        60
#define EVSYS_ID_GEN_TC6_MCX_0      61
#define EVSYS_ID_GEN_TC6_MCX_1      62
#define EVSYS_ID_GEN_TC7_OVF        63
#define EVSYS_ID_GEN_TC7_MCX_0      64
#define EVSYS_ID_GEN_TC7_MCX_1      65
#define EVSYS_ID_GEN_ADC_RESRDY     66
#define EVSYS_ID_GEN_ADC_WINMON     67
#define EVSYS_ID_GEN_AC_COMP_0      68
#define EVSYS_ID_GEN_AC_COMP_1      69
#define EVSYS_ID_GEN_AC_WIN_0       70
#define EVSYS_ID_GEN_DAC_EMPTY      71
#define EVSYS_ID_GEN_AC1_COMP_0     74
#define EVSYS_ID_GEN_AC1_COMP_1     75
#define EVSYS_ID_GEN_AC1_WIN_0      76
#define EVSYS_ID_GEN_TCC3_OVF       77
#define EVSYS_ID_GEN_TCC3_TRG       78
#define EVSYS_ID_GEN_TCC3_CNT       79
#define EVSYS_ID_GEN_TCC3_MCX_0     80
#define EVSYS_ID_GEN_TCC3_MCX_1     81
#define EVSYS_ID_GEN_TCC3_MCX_2     82
#define EVSYS_ID_GEN_TCC3_MCX_3     83

// USERS
#define EVSYS_ID_USER_DMAC_CH_0     0
#define EVSYS_ID_USER_DMAC_CH_1     1
#define EVSYS_ID_USER_DMAC_CH_2     2
#define EVSYS_ID_USER_DMAC_CH_3     3
#define EVSYS_ID_USER_TCC0_EV_0     4
#define EVSYS_ID_USER_TCC0_EV_1     5
#define EVSYS_ID_USER_TCC0_MC_0     6
#define EVSYS_ID_USER_TCC0_MC_1     7
#define EVSYS_ID_USER_TCC0_MC_2     8
#define EVSYS_ID_USER_TCC0_MC_3     9
#define EVSYS_ID_USER_TCC1_EV_0     10
#define EVSYS_ID_USER_TCC1_EV_1     11
#define EVSYS_ID_USER_TCC1_MC_0     12
#define EVSYS_ID_USER_TCC1_MC_1     13
#define EVSYS_ID_USER_TCC2_EV_0     14
#define EVSYS_ID_USER_TCC2_EV_1     15
#define EVSYS_ID_USER_TCC2_MC_0     16
#define EVSYS_ID_USER_TCC2_MC_1     17
#define EVSYS_ID_USER_TC3_EVU       18
#define EVSYS_ID_USER_TC4_EVU       19
#define EVSYS_ID_USER_TC5_EVU       20
#define EVSYS_ID_USER_TC6_EVU       21
#define EVSYS_ID_USER_TC7_EVU       22
#define EVSYS_ID_USER_ADC_START     23
#define EVSYS_ID_USER_ADC_SYNC      24
#define EVSYS_ID_USER_AC_SOC_0      25
#define EVSYS_ID_USER_AC_SOC_1      26
#define EVSYS_ID_USER_DAC_START     27
#define EVSYS_ID_USER_AC1_SOC_0     29
#define EVSYS_ID_USER_AC1_SOC_1     30
#define EVSYS_ID_USER_TCC3_EV_0     31
#define EVSYS_ID_USER_TCC3_EV_1     32
#define EVSYS_ID_USER_TCC3_MC_0     33
#define EVSYS_ID_USER_TCC3_MC_1     34
#define EVSYS_ID_USER_TCC3_MC_2     35
#define EVSYS_ID_USER_TCC3_MC_3     36

#endif /* _SAMD21_EVSYS_INSTANCE_ */
