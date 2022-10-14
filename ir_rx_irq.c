#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "ir_rx_irq.h"

struct _ir_data ir_data;

uint16_t __last_address = 0x0;
uint16_t __last_command = 0x0;

void (*user_function_callback) (uint16_t address, uint16_t command, int type) = NULL;

void reset_ir_data() {
    ir_data.cnt = 0;

    size_t i = 0;
    for (; i < 34; ++i) {
        ir_data.rises[i] = 0;
    }
}

void process_ir_data(int type) {
    // If it's a repeat code, just send the previous command.
    if (type == REPEAT) {
        user_function_callback(__last_address, __last_command, type);
        return;
    }

    uint64_t diff;
    uint32_t raw = 0x0;

    size_t i = 2;
    for (; i < 34; ++i) {
        // Compute differences between pulses
        diff = ir_data.rises[i] - ir_data.rises[i - 1];

        // Should be a zero
        if (diff > 0.85 * ZERO_SPACE && diff < 1.15 * ZERO_SPACE)
            raw >>= 1;
        // Should be a one
        else if (diff > 0.85 * ONE_SPACE && diff < 1.15 * ONE_SPACE) {
            raw >>= 1;
            raw |= 0x80000000;
        }
        // Bad transmission
        else {
            return;
        }
    }

    // Use a union for bit access
    union {
        uint32_t raw;
        struct {
            uint8_t adr, inv_adr, cmd, inv_cmd;
        };
    } data;

    data.raw = raw;

    // Error checking defined by the NEC protocol
    if (data.adr != (data.inv_adr ^ 0xff) || data.cmd != (data.inv_cmd ^ 0xff)) {
        return;
    }

    __last_address = data.adr;
    __last_command = data.cmd;
    user_function_callback(data.adr, data.cmd, NORMAL);
}

void irq_callback(uint gpio, uint32_t events) {
    uint64_t current_time = time_us_64();

    // The space between the pulses is too big
    if (ir_data.cnt > 0) {
        uint64_t diff = current_time - ir_data.rises[ir_data.cnt - 1];

        // Check if this is a bad message
        if (diff > MAXIMUM_SPACE) {
            reset_ir_data();
            ir_data.rises[0] = current_time;
        }
        // Check if it is a repeat message
        else if (ir_data.cnt == 1 && diff > 0.85 * REPEAT_SPACE && diff < 1.15 * REPEAT_SPACE) {
            process_ir_data(REPEAT);
            reset_ir_data();
        }
    }

    ir_data.rises[ir_data.cnt++] = current_time;

    // The transmission ended
    if (ir_data.cnt == 34) {
        process_ir_data(NORMAL);
        reset_ir_data();
    }
}

void init_ir_irq_receiver(uint gpio, void (*callback) (uint16_t address, uint16_t command, int type)) {
    // Init the ir_data struct
    reset_ir_data();

    // Set the user callback function
    user_function_callback = callback;

    // Init the sdk
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_FALL, true, &irq_callback);
}
