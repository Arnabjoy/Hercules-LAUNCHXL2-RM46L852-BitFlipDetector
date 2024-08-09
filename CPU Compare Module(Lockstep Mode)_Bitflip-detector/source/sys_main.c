/**
 * @file sys_main.c
 * @brief Main application file for CCM-R4F lockstep mode monitoring
 * @date 1-August-2024
 * @version 1.0
 *
 * This application initializes the CCM-R4F module to operate in lockstep mode.
 * It continuously monitors the CCM Status Register (CCMSR) for compare errors,
 * logs the results via serial communication, and controls two LEDs to indicate
 * the status of the comparison.
 *
 * Author: Arnab Chakraborty
 */

#include "sys_common.h"
#include "system.h"
#include "sci.h"
#include "gio.h"
#include "esm.h"
#include "sys_core.h"
#include "sys_selftest.h"
#include <stdio.h>
#include <string.h>

/* Define Base Addresses and Offsets */
#define CCMR4F_BASE_ADDR  0xFFFFF600  /**< Base address for the CCM-R4F module */
#define CCMKEYR_OFFSET    0x04        /**< Offset for the MKEY register */
#define CCMSR_OFFSET      0x00        /**< Offset for the CCMSR register */

/* Define Mode Values */
#define CCMKEYR_LOCKSTEP  0x0         /**< Value to set CCM-R4F to lockstep mode */
#define CCMSR_CMPE_MASK   (1 << 16)   /**< Mask for the Compare Error flag bit in CCMSR */

/* Define LED Port and Pins */
#define LED_PORT gioPORTB             /**< GIO port for LEDs */
#define LED_PIN_SUCCESS 1             /**< GIO pin for success LED */
#define LED_PIN_ERROR 2               /**< GIO pin for error LED */

/**
 * @brief Delay function
 *
 * Simple delay loop to introduce a time delay.
 *
 * @param count Number of iterations for the delay loop
 */
void delay(uint32_t count) {
    uint32_t i;
    for (i = 0; i < count; i++);
}

/**
 * @brief Log message to serial
 *
 * Sends a message string via the SCI (UART) interface.
 *
 * @param message Null-terminated string to send
 */
void logToSerial(const char* message) {
    while (!sciIsTxReady(scilinREG));
    sciSend(scilinREG, strlen(message), (unsigned char*)message);
}

/**
 * @brief Main application entry point
 *
 * Initializes the CCM-R4F in lockstep mode, continuously checks for compare errors,
 * and logs the results. It also controls LEDs based on the comparison results.
 */
void main(void) {
    sciInit(); /**< Initialize SCI for serial communication */
    gioInit(); /**< Initialize GIO for LED control */

    /* Set LED pins as output */
    gioSetDirection(LED_PORT, (1 << LED_PIN_SUCCESS) | (1 << LED_PIN_ERROR));
    _enable_IRQ(); /**< Enable interrupts */

    /* Initialize CCM-R4F and set to lockstep mode */
    *((volatile uint32_t *)(CCMR4F_BASE_ADDR + CCMKEYR_OFFSET)) = CCMKEYR_LOCKSTEP;

    while (1) {
        /* Checking the CCM Status Register's 16th bit using a bitwise AND operation
         * to detect a CPU signal compare mismatch; an error is detected if the result
         * of the operation is non-zero (meaning that the 16th bit is set to 1)
         */
        if (*((volatile uint32_t *)(CCMR4F_BASE_ADDR + CCMSR_OFFSET)) & CCMSR_CMPE_MASK) {
            logToSerial("\rCCM-R4F Lockstep Mode: Error Detected!\r\n");
            gioSetBit(LED_PORT, LED_PIN_SUCCESS, 0);    /**< Turn off success LED */
            gioToggleBit(LED_PORT, LED_PIN_ERROR);      /**< Toggle error LED */

            /* Clear error flags */
            *((volatile uint32_t *)(CCMR4F_BASE_ADDR + CCMSR_OFFSET)) = CCMSR_CMPE_MASK; /**< Clear compare error status bit */
        } else {
            logToSerial("\rCCM-R4F Lockstep Mode: No Error Detected\r\n");
            gioSetBit(LED_PORT, LED_PIN_ERROR, 0);      /**< Turn off error LED */
            gioToggleBit(LED_PORT, LED_PIN_SUCCESS);    /**< Toggle success LED */
        }

        /* Delay before the next iteration */
        delay(10000000); /**< Add a delay between tests */
    }
}
