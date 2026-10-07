/*
 * BME280.h
 *
 *  Created on: 05-Oct-2026
 *      Author: visakh.p
 */

#ifndef INC_BME280_H_
#define INC_BME280_H_

#include "main.h"
#include <stdint.h>



/* mode field — bits [1:0], no shift needed */
#define BME280_MODE_SLEEP    (0x00U)
#define BME280_MODE_FORCED   (0x01U)
#define BME280_MODE_NORMAL   (0x03U)

/* osrs_t field — bits [7:5], shift left by 5 */
#define BME280_OSRS_T_X1     (0x01U << 5)
/* osrs_p field — bits [4:2], shift left by 2 */
#define BME280_OSRS_P_X1     (0x01U << 2)
/* osrs_h field — bits [2:0], shift left by 0*/
#define BME280_OSRS_H_X1     (0x01U << 0)

#define BME280_MAX_RETRIES 10


typedef struct{
	uint8_t Chip_ID;
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t 	dig_H6;
	uint8_t ctrl_hum;
	uint8_t status;
	uint8_t ctrl_meas;
	uint8_t config;
	uint8_t press_msb;
	uint8_t press_lsb;
	uint8_t press_xlsb;
	uint8_t temp_msb;
	uint8_t temp_lsb;
	uint8_t temp_xlsb;
	uint8_t hum_msb;
	uint8_t hum_lsb;

}Reg_BME_280;



typedef enum
{
	bme_id		   = (uint8_t)0xD0,
	bme_reset	   = (uint8_t)0xE0,
	bme_ctrl_hum   = (uint8_t)0xF2,
	bme_status     = (uint8_t)0xF3,
	bme_ctrl_meas  = (uint8_t)0xF4,
	bme_config	   = (uint8_t)0xF5,
	bme_press_msb  = (uint8_t)0xF7,
	bme_press_lsb  = (uint8_t)0xF8,
	bme_press_xlsb = (uint8_t)0xF9,
	bme_temp_msb   = (uint8_t)0xFA,
	bme_temp_lsb   = (uint8_t)0xFB,
	bme_temp_xlsb  = (uint8_t)0xFC,
	bme_hum_msb	   = (uint8_t)0xFD,
	bme_hum_lsb    = (uint8_t)0xFE
} BME280_registers;

extern Reg_BME_280 sensor_data;
//extern uint8_t ID;
extern uint8_t rxbuffer[18];
extern void BME280_Init(I2C_HandleTypeDef *hi2c);
int32_t BME_Temperature_Calculation(I2C_HandleTypeDef *hi2c);
int32_t BME_Pressure_Calculation(I2C_HandleTypeDef *hi2c);
int32_t BME_Humidity_Calculation(I2C_HandleTypeDef *hi2c);


#endif /* INC_BME280_H_ */
