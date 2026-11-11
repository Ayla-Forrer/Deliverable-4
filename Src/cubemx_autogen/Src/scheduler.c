#include "scheduler.h"
#include "ring_buffer.h"
#include "main.h"
#include <stdio.h>
#include <math.h>

static uint16_t sensor_enabled = 0;             // sets the initial state of the sensor to disabled
static uint16_t last_button_state = 0;          //sets the initial state of the button to not pressed

extern I2C_HandleTypeDef hi2c1;                 // declare the i2c handle as an external variable so it can be used 
#define ADXL_ADDR   (0x53 << 1)                 // shifting 0x53 left by 1 gives 0xA6, the i2c address of the accelerometer

//task one: enable/disable sensor based on button press and store in buffer one
void task_one(void)
{uint8_t button_state = HAL_GPIO_ReadPin(button_GPIO_Port, button_Pin);  // read the current state of the button, if it is pressed or not
    if (button_state == GPIO_PIN_RESET && last_button_state == 1) {     // check if the button was just pressed, if 1 it has not been pressed, if 0 it has been pressed. will tell if sensor is active
        sensor_enabled = !sensor_enabled; // Toggle sensor state       // if the button was just pressed, toggle the sensor state
    }
    last_button_state = button_state;               // update the last button state

    if (sensor_enabled) {                           // if enabled, read data from the accelerometer and store it in buffer one
        uint8_t raw_bytes[6];
        accel_data current_sample;

        if (HAL_I2C_Mem_Read(&hi2c1, ADXL_ADDR, 0x32, I2C_MEMADD_SIZE_8BIT, raw_bytes, 6, 10) == HAL_OK) {  //reads the data, if successful, it will store the data in raw_bytes array
            current_sample.x = (int16_t)(raw_bytes[1] << 8 | raw_bytes[0]);
            current_sample.y = (int16_t)(raw_bytes[3] << 8 | raw_bytes[2]);
            current_sample.z = (int16_t)(raw_bytes[5] << 8 | raw_bytes[4]);       // raw to real data conversion

            bufferone_push_overwrite(current_sample);       // acceleration data is pushed into buffer one, if buffer is full, it will overwrite the oldest data
        }
        
    }
}

//task two: check accelerometer data in buffer one, apply a simple low-pass filter, and store the filtered data in buffer two
void task_two(void)
{accel_data popped_data;
    while(bufferone_pull(&popped_data) == 0) {          // pulls data from buffer one and converts raw acceleration values to g's
        double x_g = popped_data.x * 0.0039;            // convert to g
        double y_g = popped_data.y * 0.0039; 
        double z_g = popped_data.z * 0.0039; 

        double magnitude = sqrt((x_g * x_g) + (y_g * y_g) + (z_g * z_g));   // calculate the magnitude of the acceleration vector

        if (magnitude > 1.5) {                                              // 1.5 Gs is a standard baseline choice, exceeds the normal force of Earth's gravity
         HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_RESET);         // turn on LED if magnitude exceeds threshold
        } else {
         HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_SET);           // turn off LED if magnitude is below threshold
        }
        buffertwo_push_discard(popped_data);                                // discards new data if buffer two is full
    }
}

//task 3: Print the current acceleration vector.
void task_three(void)
{accel_data out_data;                                                       // takes the data from buffer two and prints it the current acceleration vector
    while (buffertwo_pull(&out_data) == 0) {
        printf(" Current Acceleration Vector: X: %d, Y: %d, Z: %d\n", out_data.x, out_data.y, out_data.z);
    }
}   
