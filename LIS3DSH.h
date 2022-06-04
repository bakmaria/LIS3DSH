/*
 * LIS3DSH.h
 *
 *  Created on: 15.07.2021
 *      Author: Maria B¹k
 */

#ifndef INC_LIS3DSH_H_
#define INC_LIS3DSH_H_

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"
#include "main.h"

#define OUT_T						0x0C //Temperature Output
#define INFO1						0x0D //Information register 1 //wartosc: 0b00100001
#define INFO2						0x0E //Information register 2 //wartosc: 0b00000000
#define WHO_AM_I					0x0F //Who am I ID //wartosc: 0x3F
#define OFF_X						0x10 //X-axis offset correction
#define OFF_Y						0x11 //Y-axis offset correction
#define OFF_Z						0x12 //Z-axis offset correction
#define CS_X						0x13 //Constant shift X-axis
#define CS_Y						0x14 //Constant shift Y-axis
#define CS_Z						0x15 //Constant shift Z-axis
#define LC_L						0x16 //Long counter register low // for interrupt state machine program timing
#define LC_H						0x17 //Long counter register high
#define STAT						0x18 //Interrupt synchronization //INTERRUPT STATUS
#define PEAK1						0x19 //Peak detection value for State Machine 1
#define PEAK2						0x1A //Peak detection value for State Machine 2
#define VFC_1						0x1B //Vector filter coefficient 1
#define VFC_2						0x1C //Vector filter coefficient 2
#define VFC_3						0x1D //Vector filter coefficient 3
#define VFC_4						0x1E //Vector filter coefficient 4
#define THRS3						0x1F //Threshold value 3

#define CTRL_REG4					0x20
#define CTRL_REG1					0x21
#define CTRL_REG2					0x22
#define CTRL_REG3					0x23
#define CTRL_REG5					0x24
#define CTRL_REG6					0x25
#define STATUS						0x27
#define OUT_X_L						0x28
#define OUT_X_H						0x29
#define OUT_Y_L						0x2A
#define OUT_Y_H						0x2B
#define OUT_Z_L						0x2C
#define OUT_Z_H						0x2D
#define FIFO_CTRL					0x2E
#define FIFO_SRC		 			0x2F

#define ST1_X						0x40	 // a¿ do 0x4F //State Machine 1 code register
#define TIM4_1						0x50 //State Machine 1 general timer
#define TIM3_1						0x51 //State Machine 1 general timer
#define TIM2_1						0x52 //-0x53 //State Machine 1 general timer
#define TIM1_1						0x54 //-0x55 //State Machine 1 general timer
#define THRS2_1						0x56 //State Machine 1 threshold value 1
#define THRS1_1						0x57 //State Machine 1 threshold value 2
#define MASK1_B						0x59 //State Machine 1 axis and sign mask
#define MASK1_A						0x5A //State Machine 1 axis and sign mask
#define SETT1						0x5B //State Machine 1 detection settings

#define PR1							0x5C //Program-reset pointer
#define TC1							0x5D //-5E //Timer counter
#define OUTS1						0x5F //Main set flag

#define ST2_X						0x60 // a¿ do 6F //State Machine 2 code register
#define TIM4_2						0x70 //State Machine 2 general timer
#define TIM3_2						0x71 //State Machine 2 general timer
#define TIM2_2						0x72 //-0x73 //State Machine 2 general timer
#define TIM1_2						0x74 //-0x75 //State Machine 2 general timer
#define THRS2_2						0x76 //State Machine 2 threshold value 1
#define THRS1_2						0x77 //State Machine 2 threshold value 2
#define DES2						0x78 //Decimation factor
#define MASK2_B						0x79 //State Machine 2 axis and sign mask
#define MASK2_A						0x7A //State Machine 2 axis and sign mask
#define SETT2						0x7B //State Machine 2 detection settings

#define PR2							0x7C //Program-reset pointer
#define TC2							0x7D //-7E //Timer counter
#define OUTS2						0x7F //Main set flag

#define CS_LIS_ON		HAL_GPIO_WritePin(CS_LIS_GPIO_Port, CS_LIS_Pin, GPIO_PIN_SET)
#define CS_LIS_OFF		HAL_GPIO_WritePin(CS_LIS_GPIO_Port, CS_LIS_Pin, GPIO_PIN_RESET)

int ZYXOR, ZOR, YOR, XOR, ZYXDA, ZDA, YDA, XDA; //bity STATUS_REG
uint8_t outx_l, outx_h, outy_l, outy_h, outz_l, outz_h, outt_l, outt_h, outt;
int16_t outx, outy, outz;
int8_t off_x, off_y, off_z;
int t;
float x, y, z;
char bufor_x[sizeof(outx)], bufor_y[sizeof(outy)], bufor_z[sizeof(outz)];
uint8_t status[8];
int wszystko_ok;

float angle;
#define pi 3.14159265

uint8_t whoami, info1, info2;
bool who_am_i, info_1, info_2;
bool fifo_mode, drdy_mode;

bool LIS3DSH_Test();
void LIS3DSH_Init(bool normal, bool drdy, bool fifo);
void LIS3DSH_Write(uint16_t adres, uint16_t dane);
uint16_t LIS3DSH_Read(uint16_t adres);
void LIS3DSH_Measure(); //Do trybu normalnego g³ównie, sprawdzane s¹ bity STATUSU, czy s¹ dostêpne dane
void LIS3DSH_Status(uint8_t stat); //G³ównie do odczytywania bitów STATUS, ale ogólnie zmienia na liczbê binarn¹ i wyœwietla j¹
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin); //Funkcja Przerwania od pinu EXTI0
void LIS3DSH_TiltToAngle(float x_axis, float y_axis, float z_axis);
//void LIS3DSH_COM_commands();

#endif /* INC_LIS3DSH_H_ */
