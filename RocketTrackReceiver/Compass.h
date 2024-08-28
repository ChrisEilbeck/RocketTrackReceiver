
#pragma once

int SetupCompass(void);
void PollCompass(void);

int CompassCommandHandler(uint8_t *cmd,uint16_t cmdptr);

typedef enum
{
    NO_SENSORS=0,
    SENSOR_MPU6500_QMC5883L_BME180,
    SENSOR_ICM_20948_BME180,
    SENSOR_MPU9250_BME180

} sensor_setup_types;