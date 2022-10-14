/**
 * @file ir_rx_irq.h
 * @author extremq
 * @date 14 Oct 2022
 * @brief Header for the NEC infrared protocol. Used only for receiving.
 * @see https://techdocs.altium.com/display/FPGA/NEC+Infrared+Transmission+Protocol
 */

#ifndef __IR_RX_IRQ_H__
#define __IR_RX_IRQ_H__

#define NORMAL 1
#define REPEAT 2

#define ZERO_SPACE     1125
#define ONE_SPACE      2250
#define MAXIMUM_SPACE 15000
#define REPEAT_SPACE  11250

/**
 * @brief Function that zeroes the ir_data struct.
 */
void reset_ir_data();

/**
 * @param type Type of processing. Can be REPEAT or NORMAL.
 * @brief Function that computes the differences between rises and decodes them.
 */
void process_ir_data(int type);

/**
 * @brief Function called automatically by the irq. Used for triggering the processing.
 */
void irq_callback(uint gpio, uint32_t events);

/**
 * @param gpio The input gpio pin for the infrared sensor.
 * @param callback Function that gets called when the sensor receives a valid command.
 * @brief Initializes the irq callback used to process the pulses.
 */
void init_ir_irq_receiver(uint gpio, void (*callback) (uint16_t address, uint16_t command, int type));

#endif /* __IR_RX_IRQ_H__ */
