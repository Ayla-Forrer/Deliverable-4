#include "ring_buffer.h"

#define buffer_size 1024  // Define buffer_size with an appropriate value

static accel_data raw_space[buffer_size];
static accel_data filtered_space[buffer_size];

static struct                                           // making two ring buffers, one for raw data and one for filtered data
{
    accel_data *const buffer;                           // pointer to array holding acceleration data
    int head;                                           
    int tail;
    const int maxlen;
}   bufferone = {raw_space, 0, 0, buffer_size},
    buffertwo = {filtered_space, 0, 0, buffer_size};

int bufferone_push_overwrite(accel_data sample)        // the following codes the behavior of the ring buffers, one will overwrite the oldest data if full, the other will discard new data if full
{
    int next = bufferone.head + 1;
    if (next >= bufferone.maxlen)
        next = 0;
    if (next == bufferone.tail) {
        bufferone.tail++;
        if (bufferone.tail >= bufferone.maxlen)
            bufferone.tail = 0;
    }
    bufferone.buffer[bufferone.head] = sample;
    bufferone.head = next;
    return 0;
}

int bufferone_pull(accel_data *sample) 
{
    if (bufferone.head == bufferone.tail)
        return -1;
    *sample = bufferone.buffer[bufferone.tail];
    bufferone.tail++;
    if (bufferone.tail >= bufferone.maxlen)
        bufferone.tail = 0;
    return 0;
}

int buffertwo_push_discard(accel_data sample)
{
    int next = buffertwo.head + 1;
    if (next >= buffertwo.maxlen)
        next = 0;
    if (next == buffertwo.tail) 
        return -1;
    buffertwo.buffer[buffertwo.head] = sample;
    buffertwo.head = next;
    return 0;
}

int buffertwo_pull(accel_data *sample)
{
    if (buffertwo.head == buffertwo.tail)
        return -1;
    *sample = buffertwo.buffer[buffertwo.tail];
    buffertwo.tail++;
    if (buffertwo.tail >= buffertwo.maxlen)
        buffertwo.tail = 0;
    return 0;
}