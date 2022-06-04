/*
 * LIS3DSH.c
 *
 *  Created on: 12.11.2021
 *      Author: Maria B¹k
 */
#include "LIS3DSH.h"

uint8_t received_buffor[128];
uint8_t send_buffor[64];
uint8_t len;
uint8_t result;

int flaga=0;
float sensitivity = 0.06; //mg //FS = 000, scale +- 2g

void LIS3DSH_Write(uint16_t adres, uint16_t dane){
	uint16_t wyslane = ((0x00 | adres) << 8) | dane; //RW=0

	CS_LIS_OFF;
	HAL_SPI_Transmit_DMA(&hspi1,(uint16_t*)&wyslane, 1); //16 cykli zegara, , HAL_MAX_DELAY
	CS_LIS_ON;
}
uint16_t LIS3DSH_Read(uint16_t adres){
	uint16_t odebrane;
	uint16_t wyslane = (0x80 | adres) << 8; //RW=1

	CS_LIS_OFF;
	HAL_SPI_TransmitReceive_DMA(&hspi1,(uint16_t*)&wyslane, (uint16_t*)&odebrane, 1); //16 cykli zegara
	CS_LIS_ON;

	return odebrane;
}
bool LIS3DSH_Test(){
	uint8_t dummy;
	dummy = LIS3DSH_Read(WHO_AM_I); // ¿eby cykl zegara siê dobrze zgrywa³, pierwsze wywo³anie SPI to œmiec

	whoami = LIS3DSH_Read(WHO_AM_I);
	if(whoami == 0x3F){
		who_am_i = true;
	}else{
		who_am_i = false;
	}

	info1 = LIS3DSH_Read(INFO1);
	if(info1 == 0x21){
		info_1 = true;
	}else{
		info_1 = false;
	}

	info2 = LIS3DSH_Read(INFO2);
	if(info2 == 0x00){
		info_2 = true;
	}else{
		info_2 = false;
	}

	if (who_am_i && info_1 && info_2 == true){
		wszystko_ok=1;
		return true;
	}else{
		wszystko_ok=0;
		return false;
	}
}
void LIS3DSH_Init(bool normal, bool drdy, bool fifo){
	CS_LIS_ON;
	if(LIS3DSH_Test() == true){
		if(normal == true){
			//***NORMAL mode***
			LIS3DSH_Write(CTRL_REG4, 0b01101111); //ODR = 100Hz, BDU = 1, ZYX enabled
			LIS3DSH_Write(CTRL_REG1, 0x00);
			LIS3DSH_Write(CTRL_REG2, 0x00);
			LIS3DSH_Write(CTRL_REG3, 0x00);
			LIS3DSH_Write(CTRL_REG5, 0b00000000); //BW = 00 anti aliasing 800Hz, Scale +- 2g, Self test disabled, SPI 4 wire
			LIS3DSH_Write(CTRL_REG6, 0x00);

		}else if(drdy == true){
			//***Interrupt with DRDY signal***
			LIS3DSH_Write(CTRL_REG4, 0b01101111); //ODR = 100Hz, BDU = 1, ZYX enabled
			LIS3DSH_Write(CTRL_REG1, 0x00);
			LIS3DSH_Write(CTRL_REG2, 0x00);
			LIS3DSH_Write(CTRL_REG3, 0b10001000); //DR_EN signal enable to INT1, Interrupt polarity active LOW, signal latched, DRDY signal enabled
			LIS3DSH_Write(CTRL_REG5, 0b00000000); //BW = 00 anti aliasing 800Hz, Scale +- 2g, Self test disabled, SPI 4 wire
			LIS3DSH_Write(CTRL_REG6, 0x00);

			LIS3DSH_Write(OFF_X, 0x03);
			LIS3DSH_Write(OFF_Y, 0x03);
			LIS3DSH_Write(OFF_Z, 0x03);

			drdy_mode = true;
			//printf("Tryb DRDY.\n");
		}else if(fifo == true){
			//***FIFO mode***
			//To correctly retrieve data from FIFO, BDU = 0! (10.5 App Note)
			LIS3DSH_Write(CTRL_REG4, 0b01100111); //ODR = 100Hz, BDU = 0, ZYX enabled
			LIS3DSH_Write(CTRL_REG1, 0x00);
			LIS3DSH_Write(CTRL_REG2, 0x00);
			LIS3DSH_Write(CTRL_REG3, 0b00001000); //Interrupt signals active LOW, Interrupt signal latched, INT1 signal enabled
			LIS3DSH_Write(CTRL_REG5, 0b00000000); //BW = 00 anti aliasing 800Hz, Scale +- 2g, Self test disabled, SPI 4 wire
			LIS3DSH_Write(CTRL_REG6, 0b01000010); //FIFO enable, P1_OVERRUN enable (Interrupt on INT1 from OVERRUN flag)
			LIS3DSH_Write(FIFO_CTRL,  0b01000000); //Stream mode

			fifo_mode = true;
			//printf("Tryb FIFO.\n");
		}
	}
}
void LIS3DSH_Measure(){

	if(wszystko_ok == 1){
		outx_l	= 	LIS3DSH_Read(OUT_X_L);
		outx_h	= 	LIS3DSH_Read(OUT_X_H); //XDA=0 po odczytaniu!

		outx = ( outx_h << 8 ) | outx_l;
		//x = (float) outx * sensitivity; //FS=00 + Normal mode, SENSITIVITY = 0.06 mg // mg

		outy_l	= 	LIS3DSH_Read(OUT_Y_L);
		outy_h	= 	LIS3DSH_Read(OUT_Y_H);

		outy = ( outy_h << 8 ) | outy_l;
		//y = (float) outy * sensitivity; //FS=00 + Normal mode, SENSITIVITY = 0.06 mg // mg

		outz_l	= 	LIS3DSH_Read(OUT_Z_L);
		outz_h	=	LIS3DSH_Read(OUT_Z_H);

		outz = ( outz_h << 8 ) | outz_l;
		//z = (float) outz * sensitivity; //FS=00 + Normal mode, SENSITIVITY = 0.06 mg // mg
	}
}
/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == INT_LIS_Pin){
		if(fifo_mode == true){
			for(int i = 0; i < 32; i++){
				//X
				outx_l	= 	LIS3DSH_Read(OUT_X_L);
				outx_h	= 	LIS3DSH_Read(OUT_X_H);
				outx = ( outx_h << 8 ) | outx_l;

				//Y
				outy_l	= 	LIS3DSH_Read(OUT_Y_L);
				outy_h	= 	LIS3DSH_Read(OUT_Y_H);
				outy = ( outy_h << 8 ) | outy_l;

				//Z
				outz_l	= 	LIS3DSH_Read(OUT_Z_L);
				outz_h	=	LIS3DSH_Read(OUT_Z_H);
				outz = ( outz_h << 8 ) | outz_l;

				LIS3DSH_TiltToAngle(x, y, z);

				x = (float) outx * sensitivity - off_x * 32; //Application Note 4.6
				y = (float) outy * sensitivity - off_y * 32; //Application Note 4.6
				z = (float) outz * sensitivity - off_z * 32; //Application Note 4.6
				//printf("X: %9.6f mg\nY: %9.6f mg\nZ: %9.6f mg\n\n", x, y, z);
			}
			//LIS3DSH_Write(FIFO_CTRL, 0b00000000); //BYPASS mode, ¿eby wyzerowac bufor FIFO
			//LIS3DSH_Write(FIFO_CTRL, 0b00100000); //FIFO mode
		}else if(drdy_mode == true){
			//X
			outx_l	= 	LIS3DSH_Read(OUT_X_L);
			outx_h	= 	LIS3DSH_Read(OUT_X_H);
			outx = ( outx_h << 8 ) | outx_l;
			//Y
			outy_l	= 	LIS3DSH_Read(OUT_Y_L);
			outy_h	= 	LIS3DSH_Read(OUT_Y_H);
			outy = ( outy_h << 8 ) | outy_l;
			//Z
			outz_l	= 	LIS3DSH_Read(OUT_Z_L);
			outz_h	=	LIS3DSH_Read(OUT_Z_H);
			outz = ( outz_h << 8 ) | outz_l;

			x = (float) outx * sensitivity; //Application Note 4.6
			y = (float) outy * sensitivity; //Application Note 4.6
			z = (float) outz * sensitivity; //Application Note 4.6
			//printf("X: %9.6f mg\nY: %9.6f mg\nZ: %9.6f mg\n\n", x, y, z);
			//LIS3DSH_TiltToAngle(x, y, z);
		}
	}
}
*/
void LIS3DSH_TiltToAngle(float x_axis, float y_axis, float z_axis){
	if(z_axis < 0){
		if(x_axis >= 0 && y_axis >= 0){
			//pierwsza cwiartka (0,90)
			angle = atan2(abs(x_axis), abs(y_axis)) * 180/pi ; //radiany * 180/pi = stopnie
			printf("Pierwsza cwiartka\n");
		}else if (x_axis >= 0 && y_axis <= 0){
			//druga cwiartka (90,180)
			angle = atan2(abs(y_axis), abs(x_axis)) * 180/pi + 90;
			printf("Druga cwiartka\n");
		}else if(x_axis <= 0 && y_axis <= 0){
			//trzecia cwiartka (180, 270)
			angle = atan2(abs(x_axis), abs(y_axis)) * 180/pi + 180;
			printf("Trzecia cwiartka\n");
		}else if(x_axis <= 0 && y_axis >= 0){
			//czwarta cwiartka (270, 360)
			angle = atan2(abs(y_axis), abs(x_axis)) * 180/pi + 270;
			printf("Czwarta cwiartka\n");
		}
	}else if(z_axis >= 0){
		if(x_axis <= 0 && y_axis >= 0){
			//pierwsza cwiartka (0,90)
			angle = atan2(abs(x_axis), abs(y_axis)) * 180/pi ; //radiany * 180/pi = stopnie
			printf("Pierwsza cwiartka\n");
		}else if (x_axis <= 0 && y_axis <= 0){
			//druga cwiartka (90,180)
			angle = atan2(abs(y_axis), abs(x_axis)) * 180/pi + 90;
			printf("Druga cwiartka\n");
		}else if(x_axis >= 0 && y_axis <= 0){
			//trzecia cwiartka (180, 270)
			angle = atan2(abs(x_axis), abs(y_axis)) * 180/pi + 180;
			printf("Trzecia cwiartka\n");
		}else if(x_axis >= 0 && y_axis >= 0){
			//czwarta cwiartka (270, 360)
			angle = atan2(abs(y_axis), abs(x_axis)) * 180/pi + 270;
			printf("Czwarta cwiartka\n");
		}
	}
	printf("Kat: %f \n\n\n", angle);
}
void LIS3DSH_Status(uint8_t stat){
	//zamiana na liczbe binarn¹
	int temp,count=0;

	for(int i=7;i>=0;i--){
		temp= stat >> i;

		if(temp & 1){
			status[count]=1;
			}else{
			status[count]=0;
		}
		count++;
	}

	//Czy dane siê nadpisa³y?
	ZYXOR  = status[0];
	ZOR		 = status[1];
	YOR		 = status[2];
	XOR		 = status[3];
	//Czy s¹ nowe dane dostêpne do odczytu?
	ZYXDA = status[4];
	ZDA		 = status[5];
	YDA		 = status[6];
	XDA		 = status[7];

	for(int i = 0; i <= sizeof(status); i++){
		if(i < 4){
			printf("%d", status[i]);
		}else if(i == 4){
			printf(" ");
		}else if(i > 4){
			printf("%d", status[i-1]);
		}
	 }

	 printf("\n");

}
/*
void LIS3DSH_COM_commands(){
	if(strcmp(received_buffor, "alive") == 0){
		if(LIS3DSH_Test() == true){
			len = sprintf(send_buffor, "LA1");
			result = CDC_Transmit_FS(send_buffor, len);
		}else{
			len = sprintf(send_buffor, "LA0");
			result = CDC_Transmit_FS(send_buffor, len);
		}
	}else if(strcmp(received_buffor, "measure") == 0){
		outx_l	= 	LIS3DSH_Read(OUT_X_L);
		outx_h	= 	LIS3DSH_Read(OUT_X_H);

		outy_l	= 	LIS3DSH_Read(OUT_Y_L);
		outy_h	= 	LIS3DSH_Read(OUT_Y_H);

		outz_l	= 	LIS3DSH_Read(OUT_Z_L);
		outz_h	=	LIS3DSH_Read(OUT_Z_H);

		outx = ( outx_h << 8 ) | outx_l;
		outy = ( outy_h << 8 ) | outy_l;
		outz = ( outz_h << 8 ) | outz_l;

		x = (float) outx * sensitivity;
		y = (float) outy * sensitivity;
		z = (float) outz * sensitivity;

		len = sprintf(send_buffor, "L%9.3f:%9.3f:%9.3f\n\r", x, y, z);
		result = CDC_Transmit_FS(send_buffor, len);

	}
}
*/
