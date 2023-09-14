#ifndef __PCF8575_H__
#define __PCF8575_H__

#include <stddef.h>
#include <i2cdev.h>
#include <esp_err.h>

#define PCF8575_I2C_ADDR_BASE 0x20

/**
 * @brief Initialize device descriptor
 *
 * @param dev Pointer to I2C device descriptor
 * @return `ESP_OK` on success
 */
esp_err_t pcf8575_init_desc(i2c_dev_t *dev);

/**
 * @brief Free device descriptor
 *
 * @param dev Pointer to I2C device descriptor
 * @return `ESP_OK` on success
 */
esp_err_t pcf8575_free_desc(i2c_dev_t *dev);

/**
 * @brief Read GPIO port value
 *
 * @param dev Pointer to I2C device descriptor
 * @param val 8-bit GPIO port value
 * @return `ESP_OK` on success
 */
esp_err_t pcf8575_port_read(i2c_dev_t *dev, uint16_t *val);

/**
 * @brief Write value to GPIO port
 *
 * @param dev Pointer to I2C device descriptor
 * @param value GPIO port value
 * @return ESP_OK on success
 */
esp_err_t pcf8575_port_write(i2c_dev_t *dev, uint16_t value);

#endif /* __PCF8575_H__ */
