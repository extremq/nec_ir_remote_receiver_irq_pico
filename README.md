# nec_ir_remote_receiver_irq_pico
A simple, IRQ-based implementation of the IR NEC receiver protocol in C using the Raspberry Pi Pico SDK.

# How to install the module (for beginners)
The minimal way of adding this module to your CMake project is by downloading the `ir_rx_irq.c` and `ir_rx_irq.h` files
and placing them in the same folder as your main source.

From there, you have to add the files inside `add_executable`. For example:
```cmake
add_executable(my_project
        main.c
        ir_rx_irq.c
        ir_rx_irq.h
        )
```

# Usage
Check `example.c` for a basic starting point.
```c
// type = 1 if it's a normal command, type = 2 if it's a repeat command 
// (sent by holding the button down for a long time)
// This function will be called when you press a button on the remote
// and the sensor receives it correctly.
void callback_ir (uint16_t address, uint16_t command, int type) {
    printf("Got 0x%04X 0x%04X %d\n", address, command, type);
}

int main() {
    ...
    // This is all you do.
    init_ir_irq_receiver(YOUR_GPIO_PIN, &callback_ir);
    ...
}
```
