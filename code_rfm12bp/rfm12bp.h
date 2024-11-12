#pragma once

#include <stdint.h>

#ifndef RFM12BP_H
#define RFM12BP_H

// configuration setting command
#define RFM_CFG_X(F) ((uint16_t)((F)-17)) // set crystal load capacitance, 0.5 pF in [17,32]
#define RFM_CFG_433 0x0010 // set frequency band
#define RFM_CFG_EF  0x0040 // enable FIFO mode
#define RFM_CFG_EL  0x0080 // enable internal data register
#define RFM_CFG_CMD 0x8000

// power management command
#define RFM_PWR_DC  0x0001 // disable clock output
#define RFM_PWR_EB  0x0004 // enable low battery detector
#define RFM_PWR_EX  0x0008 // enable crystal oscillator
#define RFM_PWR_ES  0x0010 // enable synthesizer
#define RFM_PWR_ET  0x0020 // enable whole transmitter chain
#define RFM_PWR_EBB 0x0040 // enable baseband circuit
#define RFM_PWR_ER  0x0080 // enable whole receiver chain
#define RFM_PWR_CMD 0x8200

// frequency setting command
#define RFM_FRQ_F(H) ((uint16_t)(((H)-430000000UL)/2500UL)) // set frequency in Hz, H in [430240000,439757500]
#define RFM_FRQ_CMD 0xA000

// data rate command
#define RFM_DRT_R(B) ((uint16_t)((10000000.0/29.0/(B))-0.5)) // set datarate in Baud, B in [9600,115200]
#define RMF_DRT_CS  0x0080 // set clock scaler, datarate / 8
#define RFM_DRT_CMD 0xC600

// receiver control command
#define RFM_RCV_SQL(S) ((uint16_t)(S)) // set RSSI threshold, S in [0,5]
#define RFM_RCV_BW67 0x00C0 // set baseband bandwidth in kHz
#define RFM_RCV_BW134 0x00A0
#define RFM_RCV_BW200 0x0080
#define RFM_RCV_BW270 0x0060
#define RFM_RCV_BW340 0x0040
#define RFM_RCV_BW400 0x0020
#define RFM_RCV_CRL 0x0100 // set VDI response time
#define RFM_RCV_VDI 0x0400 // enable pin 16 as VDI output
#define RFM_RCV_CMD 0x9000

// data filter command
#define RFM_DFI_F(B) ((uint16_t)(B)) // set DQD threshold in bit, B in [0,7]
#define RFM_DFI_ML  0x0040 // set clock recovery mode lock
#define RFM_DFI_AL  0x0080 // set clock recovery auto lock
#define RFM_DFI_CMD 0xC228

// FIFO and reset mode command
#define RFM_FRM_DR  0x0001 // disable LBD reset
#define RFM_FRM_FF  0x0002 // reset FIFO sync pattern recognition
#define RFM_FRM_AL  0x0004 // set FIFO fill condition
#define RFM_FRM_SP  0x0008 // set synchron pattern short
#define RFM_FRM_F(B) ((uint16_t)((B)<<4)) // set FIFO threshold in bit, B in [0,16]
#define RFM_FRM_CMD 0xCA00

// receiver FIFO read command
#define RFM_FFR_CMD 0xB000

// automatic frequency correction command
#define RFM_AFC_EN  0x0001 // enable AFC
#define RFM_AFC_OE  0x0002 // enable offset
#define RFM_AFC_FI  0x0004 // enable fine mode
#define RFM_AFC_ST  0x0008 // set strobe edge
#define RFM_AFC_RL0 0x0010 // set range limit
#define RFM_AFC_RL1 0x0020
#define RFM_AFC_A0  0x0040 // set operation mode
#define RFM_AFC_A1  0x0080
#define RFM_AFC_CMD 0xC400

// transceiver configuration command
#define RFM_TXC_P(D) ((uint16_t)((D)/3UL)) // power attenuation in dB, P in [0,21]
#define RFM_TXC_M(H) ((uint16_t)((((H)/15UL)-1)<<4)) // modulation shift in kHz, H in [15,240]
#define RFM_TXC_MP  0x0100
#define RFM_TXC_CMD 0x9800

// transmitter register write command 
#define RFM_FFW_CMD ((uint16_t) 0xB800)

// Low Battery Detector and Microcontroller Clock Divider Command
#define RFM_DIV_ONE 0x00E0
#define RFM_DIV_CMD ((uint16_t) 0xC000)

// software reset command
#define RFM_RST_CMD ((uint16_t) 0xFE00)

// status read command
#define RFM_SRD_AFC  0x0020 // status AFC toggling
#define RFM_SRD_CRL  0x0040 // status clock recovery lock
#define RFM_SRD_DQD  0x0080 // status data quality detector
#define RFM_SRD_RSSI 0x0100 // status received signal strength indicator
#define RFM_SRD_FFEM 0x0200 // status FIFO empty
#define RFM_SRD_LBD  0x0400 // interrupt low battery detector
#define RFM_SRD_VDI  0x0800 // interrupt valid data indicator
#define RFM_SRD_WKUP 0x1000 // interrupt wake-up timer
#define RFM_SRD_FFOR 0x2000 // interrupt FIFO overflow/underrun
#define RFM_SRD_POR  0x4000 // interrupt power-on-reset
#define RFM_SRD_FFIT 0x8000 // interrupt FIFO
#define RFM_SRD_CMD  0x0000

void rfm12bp_init(void);
void rfm12bp_transmit(void);
void rfm12bp_receive(void);

void rfm12bp_transmit_on(void);
void rfm12bp_transmit_off(void);

void rfm12bp_receive_on(void);
void rfm12bp_receive_off(void);

#endif // RFM12BP_H
