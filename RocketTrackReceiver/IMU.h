
#pragma once

int SetupIMU(void);
void PollIMU(void);

int IMUCommandHandler(uint8_t *cmd,uint16_t cmdptr);

typedef enum
{
    NO_SENSORS=0,
    SENSOR_MPU6500_QMC5883L_BME180,
    SENSOR_ICM_20948_BME180,
    SENSOR_MPU9250_BME180

} sensor_setup_types;

extern float roll;
extern float pitch;
extern float yaw;
extern float heading;

extern float Mag_A11,Mag_A12,Mag_A13;
extern float Mag_A21,Mag_A22,Mag_A23;
extern float Mag_A31,Mag_A32,Mag_A33;
extern float Mag_B1,Mag_B2,Mag_B3;
