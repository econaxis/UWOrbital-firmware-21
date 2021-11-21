#include "ringbuffer.h"

#include "stdio.h"

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>




#include "FreeRTOS.h"
#include "task.h"

struct SampleTelemetry {
    long timestamp;
    float data1;
    float data2;
    char strings[10];
};

struct RingBuffer mm;


// Push/pop operations since program start
int ops = 0;

void push_task() {
    // Simple LFSR PRNG to generate data to push into the queue
    uint32_t hasher = 3906821083;
    for (long i = 1; i < 100000; i++) {
        uint32_t input = (hasher >> 31) ^ (hasher >> 29) ^ (hasher >> 11) ^ (hasher >> 10);
        hasher <<= 1;
        hasher |= (input & 1);
        struct SampleTelemetry telem = {i, (float) hasher, (float) hasher * 0.823820, "abfgh"};
        push_queue(&mm, (void *) &telem);
        ops++;
        taskYIELD();
    }
    printf("Push exited\n");
    for(;;) {}
}

void recv_task() {
    TickType_t start = xTaskGetTickCount();
    struct SampleTelemetry received;
    long max_time = 0;
    while (true) {
        if (pop_queue(&mm, (char *) &received)) {
            ops++;
            assert(received.timestamp > max_time);
            max_time = received.timestamp;
            if (received.timestamp % 10 == 0) {
                float elapsed = (float) (xTaskGetTickCount() - start) / configTICK_RATE_HZ;
                printf("Ops per sec: %d %f %f\n",ops, elapsed,  ops /elapsed);
//                printf("Received: %d %f %s\n", received.timestamp, received.data1, received.strings);
            }

        } else {
            // Queue is empty
            taskYIELD();
        }
    }
}

int main() {
    mm = create_ring_buf(sizeof(struct SampleTelemetry));
    TaskHandle_t pusher, receiver, pusher1, pusher2;
    BaseType_t returned = xTaskCreate(push_task, "Main", 4096, NULL, tskIDLE_PRIORITY + 1, &pusher);
    assert(returned == pdPASS);
    returned = xTaskCreate(recv_task, "Main", 4096, NULL, tskIDLE_PRIORITY + 1, &receiver);
    assert(returned == pdPASS);
    vTaskStartScheduler();
}


static void example() {
    // Create a ringbuffer struct
    struct RingBuffer rb = create_ring_buf(sizeof (struct SampleTelemetry));

    // To push an item, we make the item on the stack first
    struct SampleTelemetry telemetry = {28802, 28.3, 69.420, "abcdef"};

    // Then, cast the item to a const void* pointer, and pass it to push_queue function
    // The length of the item is automatically known when the RingBuffer was created.
    push_queue(&rb, (const void*) &telemetry);

    // Items are pushed via memcpy, so we can push the same telemetry again after modifying it.
    telemetry.timestamp += 5;
    telemetry.strings = "ghijkl";
    push_queue(&rb, (const void*) &telemetry);


    // To pop an item off the queue, first allocate enough space on the stack for the item to live.
    struct SampleTelemetry popped; // Does not have to be initialized memory

    // pop_queue returns true on error, false on failure
    assert(pop_queue(&rb, (char*) &popped));

    // pop_queue pops the first item off. That means we should expect these values.
    assert(popped.timestamp == 28802);
    assert(popped.strings == "abcdef");

    struct SampleTelemetry popped2;
    assert(pop_queue(&rb, (char*) &popped2));
    assert(popped2.timestamp == 28807);
    assert(popped2.strings == "ghijkl");

    // We have popped all items off the queue now. The next time we call pop, it should return an error.
    // Must check pop_queue return result! If not, you may be accessing uninitialized memory.
    struct SampleTelemetry popped3;
    assert(pop_queue(&rb, (char*) &popped3) == false);
    // popped3 still contains uninitialized memory...don't touch it.
}

