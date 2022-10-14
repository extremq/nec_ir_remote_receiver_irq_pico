#include <stdio.h>

#include "ir_rx_irq.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// type = 1 if it's a normal command, type = 2 if it's a repeat command 
// (sent by holding the button down for a long time)
void callback_ir (uint16_t address, uint16_t command, int type) {
    printf("Got 0x%04X 0x%04X %d\n", address, command, type);
}

int main() {
    stdio_init_all();
    
    // This is all you do.
    init_ir_irq_receiver(0, &callback_ir);

    while (true);
}
