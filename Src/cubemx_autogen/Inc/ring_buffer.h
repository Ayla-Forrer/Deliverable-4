#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>

typedef struct                  
{
    uint16_t x;                                                  // structure that holds acceleration along the x,y, and z axes
    uint16_t y;
    uint16_t z;
} accel_data;

#define  buffer_size  10;

int bufferone_push_overwrite(accel_data sample);                // declares the functions that will be used to push and pull data from the ring buffers
int bufferone_pull(accel_data *sample);
int buffertwo_push_discard(accel_data sample);
int buffertwo_pull(accel_data *sample);

#endif /* RING_BUFFER_H */


