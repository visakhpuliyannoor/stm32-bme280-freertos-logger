/*
 * BME280.c
 *
 *  Created on: 05-Oct-2026
 *      Author: visakh.p
 */

#include "BME280.h"
#include "main.h"

Reg_BME_280 sensor_data;
//uint8_t ID;
uint8_t rxbuffer[18];
int32_t t_fine;
void BME280_Init(I2C_HandleTypeDef *hi2c);
//int32_t BME_Temperature_Calculation(I2C_HandleTypeDef *hi2c);
//int32_t BME_Pressure_Calculation(I2C_HandleTypeDef *hi2c);
//int32_t BME_Humidity_Calculation(I2C_HandleTypeDef *hi2c);

void BME280_Init(I2C_HandleTypeDef *hi2c)
{
//	HAL_I2C_Mem_Read(hi2c, 0xEE, 0xD0, 1, &sensor_data.Chip_ID, 1, 500);
	HAL_StatusTypeDef Status;
	uint8_t  retries=0;
////	Status = HAL_I2C_IsDeviceReady(hi2c, Status, 2, 100)

	sensor_data.ctrl_meas = (BME280_OSRS_T_X1 | BME280_OSRS_P_X1 | BME280_MODE_NORMAL);
  sensor_data.ctrl_hum = BME280_OSRS_H_X1;

	do
	{
    Status = HAL_I2C_IsDeviceReady(hi2c, 0xEE, 2, 100);
		if(Status == HAL_OK) break;
		retries++;
    HAL_Delay(10);
	}while (retries < BME280_MAX_RETRIES);
	if(Status == HAL_OK)
	{
		HAL_I2C_Mem_Read(hi2c, 0xEE, 0x88, 1, rxbuffer, 6, 500);

		sensor_data.dig_T1 = ((rxbuffer[1]<<8)|rxbuffer[0]);
		sensor_data.dig_T2 = (signed short)((rxbuffer[3]<<8)|rxbuffer[2]);
		sensor_data.dig_T3 = (signed short)((rxbuffer[5]<<8)|rxbuffer[4]);
		HAL_Delay(100);

		HAL_I2C_Mem_Read(hi2c, 0xEE, 0x8E, 1, rxbuffer, 18, 500);

		sensor_data.dig_P1 = ((rxbuffer[1]<<8)|rxbuffer[0]);
		sensor_data.dig_P2 = (signed short)((rxbuffer[3]<<8)|rxbuffer[2]);
		sensor_data.dig_P3 = (signed short)((rxbuffer[5]<<8)|rxbuffer[4]);
		sensor_data.dig_P4 = (signed short)((rxbuffer[7]<<8)|rxbuffer[6]);
		sensor_data.dig_P5 = (signed short)((rxbuffer[9]<<8)|rxbuffer[8]);
		sensor_data.dig_P6 = (signed short)((rxbuffer[11]<<8)|rxbuffer[10]);
		sensor_data.dig_P7 = (signed short)((rxbuffer[13]<<8)|rxbuffer[12]);
		sensor_data.dig_P8 = (signed short)((rxbuffer[15]<<8)|rxbuffer[14]);
		sensor_data.dig_P9 = (signed short)((rxbuffer[17]<<8)|rxbuffer[16]);
		HAL_Delay(100);

		HAL_I2C_Mem_Read(hi2c, 0xEE, 0xA1, 1, rxbuffer, 1, 500);
		sensor_data.dig_H1 = rxbuffer[0];
		HAL_Delay(100);
		HAL_I2C_Mem_Read(hi2c, 0xEE, 0xE1, 1, rxbuffer, 7, 500);
		sensor_data.dig_H2 = (signed short)((rxbuffer[1]<<8)|rxbuffer[0]);
		sensor_data.dig_H3 = rxbuffer[2];
//		sensor_data.dig_H4 = (signed)((rxbuffer[4]<<8)|rxbuffer[3]);
//		sensor_data.dig_H5 = (signed)((rxbuffer[6]<<8)|rxbuffer[5]);
		sensor_data.dig_H4 = (signed short)((rxbuffer[3] << 4) | (rxbuffer[4] & 0x0F));
		sensor_data.dig_H5 = (signed short)((rxbuffer[5] << 4) | (rxbuffer[4] >> 4));
		sensor_data.dig_H6 = rxbuffer[6];
		HAL_Delay(100);

		HAL_I2C_Mem_Write(hi2c, 0xEE, bme_ctrl_hum, 1, &sensor_data.ctrl_hum, 1, 500);
		HAL_I2C_Mem_Write(hi2c, 0xEE, bme_ctrl_meas, 1, &sensor_data.ctrl_meas, 1, 500);
	}
}


/*Function for Calculation of Temperature*/
int32_t BME_Temperature_Calculation(I2C_HandleTypeDef *hi2c)
{
	int32_t raw_temp, var1, var2, Temp;
	HAL_I2C_Mem_Read(hi2c, 0xEE, bme_temp_msb, 1, rxbuffer, 3, 500);

	raw_temp = ((int32_t)rxbuffer[0] << 12) | ((int32_t)rxbuffer[1] << 4) | (rxbuffer[2] >> 4);
	var1 = (((raw_temp>>3) - ((int32_t)sensor_data.dig_T1<<1)) * ((int32_t)sensor_data.dig_T2)) >> 11;
	var2 = (((((raw_temp>>4) - ((int32_t)sensor_data.dig_T1)) * ((raw_temp>>4) - ((int32_t)sensor_data.dig_T1)))>> 12) *((int32_t)sensor_data.dig_T3)) >> 14;
	t_fine = var1 + var2;
	Temp = ((t_fine * 5 + 128) >> 8);
	return (Temp);
}


/*Function for Calculation of Pressure*/

int32_t BME_Pressure_Calculation(I2C_HandleTypeDef *hi2c)
{
	int64_t raw_press, var1, var2, Pressure;
	HAL_I2C_Mem_Read(hi2c, 0xEE, bme_press_msb, 1, rxbuffer, 3, 500);
	raw_press = ((int32_t)rxbuffer[0] << 12) | ((int32_t)rxbuffer[1] << 4) | (rxbuffer[2] >> 4);

	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)sensor_data.dig_P6;
	var2 = var2 + ((var1*(int64_t)sensor_data.dig_P5)<<17);
	var2 = var2 + (((int64_t)sensor_data.dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)sensor_data.dig_P3)>>8) + ((var1 * (int64_t)sensor_data.dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)sensor_data.dig_P1)>>33;
	if (var1 == 0)
	{
	return 0; // avoid exception caused by division by zero
	}
	Pressure = 1048576-raw_press;
	Pressure = (((Pressure<<31)-var2)*3125)/var1;
	var1 = (((int64_t)sensor_data.dig_P9) * (Pressure>>13) * (Pressure>>13)) >> 25;
	var2 = (((int64_t)sensor_data.dig_P8) * Pressure) >> 19;
	Pressure = ((Pressure + var1 + var2) >> 8) + (((int64_t)sensor_data.dig_P7)<<4);
	return ((int32_t)Pressure);
}

/*Function for Calculation of Pressure*/

int32_t BME_Humidity_Calculation(I2C_HandleTypeDef *hi2c)
{
//	int64_t raw_hum, var1, var2, Humidity;
	int32_t v_x1_u32r,raw_hum;
	HAL_I2C_Mem_Read(hi2c, 0xEE, bme_hum_msb, 1, rxbuffer, 2, 500);
	raw_hum = ((int32_t)rxbuffer[0] << 8) | ((int32_t)rxbuffer[1]);


	v_x1_u32r = (t_fine - ((int32_t)76800));

	v_x1_u32r = (((((raw_hum << 14) - (((int32_t)sensor_data.dig_H4) << 20) - (((int32_t)sensor_data.dig_H5) *
	v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *
	((int32_t)sensor_data.dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)sensor_data.dig_H3)) >> 11) +
	((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)sensor_data.dig_H2) +
	8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
	((int32_t)sensor_data.dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return ((int32_t)(v_x1_u32r>>12));
}

