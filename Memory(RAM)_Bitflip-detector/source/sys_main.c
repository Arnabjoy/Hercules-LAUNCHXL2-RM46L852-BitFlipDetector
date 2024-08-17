/**
* @file sys_main.c
* @brief Application main file
* @Space Science-Special Assignment
* @date 21st July, 2024
* @version 01
* @author Arnab Chakraborty
*
* This application initializes a custom RAM region with PRBS-7 generated data,
* calculates a checksum, and continuously verifies the checksum to detect any
* bit flips. If bit flips are detected, the information is logged over SCI (Serial
* Communication Interface) and indicated via LED 3 (GIOB_2) blinking. If no bit flips
* are detected, this information is also logged over SCI and indicated via
* LED 2 (GIOB_1) blinking.
*
* The purpose of this application is to test the robustness of the memory
* against radiation-induced Single Event Effects (SEEs) by irradiating the chip
* in a particle accelerator and counting the number of SEEs.
*
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/* Include Files */
#include "sys_common.h"
#include "sci.h"
#include "gio.h"
#include "reg_sci.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Defines the start and size of the memory regions */
#define DATA_STORE_START 0x08001600  // Start address of data store
#define DATA_STORE_SIZE  0x000174FC  // Each region size: 93.25 KB

#define CUSTOM_RAM_START 0x08018AFC  // Start address of custom RAM
#define CUSTOM_RAM_SIZE  0x000174FC  // Each region size: 93.25 KB and end address: 0x0802FFF0

/* LED port and pin definitions */
#define LED_PORT gioPORTB
#define LED_PIN_SUCCESS 1
#define LED_PIN_ERROR 2

/* Memory region declarations */
__attribute__((section(".custom_data_section"))) uint64_t custom_ram[CUSTOM_RAM_SIZE / sizeof(uint64_t)];
__attribute__((section(".data_store_section"))) uint64_t original_ram[DATA_STORE_SIZE / sizeof(uint64_t)];

/**
 * @brief Generate PRBS-7 sequence
 *
 * This function generates a pseudo-random binary sequence (PRBS) of length 7.
 * It is used to fill the custom RAM with random data.
 *
 * @return 7-bit PRBS value
 */
uint8_t prbs7() {
    static uint8_t state = 0x7F; // Initial state
    int newbit = (((state >> 6) ^ (state >> 5)) & 1); // New bit from the 7th and 6th bits
    state = ((state << 1) | newbit) & 0x7F; // Shift left and insert the new bit at LSB, mask to keep 7 bits
    return state;
}

/**
 * @brief Delay function
 *
 * This function provides a simple delay loop.
 *
 * @param count Number of iterations for delay
 */
void delay(uint32_t count) {
    uint32_t i;
    for (i = 0; i < count; i++) {
    }
}

/**
 * @brief Log messages to serial (UART)
 *
 * This function sends a message over UART.
 *
 * @param message The message to be logged
 */
void logToSerial(const char* message) {
    while (!sciIsTxReady(scilinREG));  // Wait until the TX buffer is ready
    sciSend(scilinREG, strlen(message), (unsigned char*)message);
}

/**
 * @brief Main function
 *
 * This is the entry point of the application. It initializes the custom RAM
 * with PRBS-7 data, calculates the expected checksum, and continuously
 * verifies the checksum to detect any bit flips.
 *
 * @return (never returns-continously checks for bit flips in a while loop)
 */
int main(void) {
    sciInit(); // Initialize the SCI (UART)
    gioInit(); // Initialize GIO
    // Set the direction for LED pins
    gioSetDirection(LED_PORT, (1 << LED_PIN_SUCCESS) | (1 << LED_PIN_ERROR));

    _enable_IRQ(); // Enable interrupts

    uint64_t data = 0;
    uint8_t prbs_output;
    uint64_t expected_checksum = 0;

    // Initializing custom RAM with generated PRBS-7 data and calculating expected checksum
    uint64_t i;
    for (i = 0; i < CUSTOM_RAM_SIZE / sizeof(uint64_t); i++) {
        data = 0;
        int j;
        for (j = 0; j < 64; j += 8) {
            prbs_output = prbs7();
            data |= ((uint64_t)prbs_output << j);
        }
        custom_ram[i] = data;
        original_ram[i] = data; // Store original data
        expected_checksum += data;
    }

    // Simulate bit flips for testing
    //custom_ram[10] ^= 0x1;     // Flip the least significant bit of the 11th element
    //custom_ram[10] ^= 0x2;     // Flip the second least significant bit of the 11th element

    // Infinite loop to verify the checksum and correct errors
    while (1) {
        uint64_t calculated_checksum = 0;
        uint64_t bit_flip_count = 0;

        // Calculate the checksum and count the number of bit flips
        for (i = 0; i < CUSTOM_RAM_SIZE / sizeof(uint64_t); i++) {
            calculated_checksum += custom_ram[i];
            uint64_t xor_val = custom_ram[i] ^ original_ram[i];
            if (xor_val != 0) {
                // Correct the bit flip by restoring the stored original value from original_ram
                custom_ram[i] = original_ram[i];
                // Count the number of bit flips
                while (xor_val) {                    //Loop continues as long as xor_val is non-zero
                    bit_flip_count += xor_val & 1;  // Incrementing bit_flip_count for each bit flip
                    xor_val >>= 1; // Right shifting xor_val to check the next bit
                }
            }
        }

        // Log and indicate the result
        if (expected_checksum == calculated_checksum && bit_flip_count == 0) {
            char log_entry[200];
            snprintf(log_entry, sizeof(log_entry), "\rChecksum matches. No bit flip was detected!\r\n");
            logToSerial(log_entry);
            gioSetBit(LED_PORT, LED_PIN_ERROR, 0);    // Turning off Error LED
            gioToggleBit(LED_PORT, LED_PIN_SUCCESS); // Toggling Success LED for creating a blinking effect
        } else {
            char log_entry[200];
            snprintf(log_entry, sizeof(log_entry), "\rChecksum mismatch! %llu bit flips were detected and corrected.\r\n", bit_flip_count);
            logToSerial(log_entry);
            gioSetBit(LED_PORT, LED_PIN_SUCCESS, 0);   // Turning off Success LED
            gioToggleBit(LED_PORT, LED_PIN_ERROR);    // Toggling Error LED for creating a blinking effect

        }

        delay(10000000);  // Delay to avoid flooding the terminal
    }

}




