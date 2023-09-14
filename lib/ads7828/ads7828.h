/**
 * @file  ads7828.h
 * @brief ADS7828 library, a 8-channel single-ended/4-channel differential
 * 12-bit ADC from TI with an I2C interface.
 * @date  2023-08-12
 * @copyright Gabriel Novalski @ MarineTelematics Equipamentos Nauticos
 */

#ifndef ADS7828_h
#define ADS7828_h

// Base I2C address, A1,A0 can give 0x48, 0x49, 0x4A, 0x4B
#define ADS7828_I2CADDR 0x48

// Definitions for the command byte.
// OR the various bits together to construct the byte
// MSB 7 SD bit
#define SINGLE_ENDED 0x80 // Single ended
#define DIFFERENTIAL 0x00 // Differential

// Bits 6:4 C2, C1, C0 Channel selectors
// No definitions needed since we will generate from channel
// and whether differential or single-ended

// Bits 3:2 PD1,PD0 Power down , refernece selection
#define PD_BTWN_CONV 0x0    // Power down betweed A/D conversions
#define IREF_OFF_AD_ON 0x04 // Internal reference OFF, A/D converter ON
#define IREF_ON_AD_OFF 0x08 // Internal reference ON, A/D conveter OFF
#define IREF_ON_AD_ON 0x0C  // Internal reference ON, A/D converter ON
// LSB 1:0 not used XX

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include <i2cdev.h>

/**
 * @brief Initialize ADS7828 device descriptor.
 * @param dev I2C device descriptor.
 * @return ESP_OK on success.
 */
esp_err_t ads7828_init_desc(i2c_dev_t *dev);

/**
 * @brief Read single-ended channel data.
 * @param dev I2C device descriptor.
 * @param channel Channel to read.
 * @return uint16_t Channel data.
 */
uint16_t ads7828_read(i2c_dev_t *dev, uint8_t channel);

/**
 * @brief Read differential channel data.
 * @param dev I2C device descriptor.
 * @param channel Channel to read.
 * @return uint16_t Channel data.
 */
uint16_t ads7828_readdif(i2c_dev_t *dev, uint8_t channel);

#endif /* ADS7828_h */
