/**
 * @file  ads7828.cpp
 * @brief ADS7828 library, a 8-channel single-ended/4-channel differential
 * 12-bit ADC from TI with an I2C interface.
 * @date  2023-08-12
 * @copyright Gabriel Novalski @ MarineTelematics Equipamentos Nauticos
 */

#include "ads7828.h"

uint16_t read_data(i2c_dev_t *dev, uint8_t channel, bool single)
{
    uint16_t buffer = 0x0000;

    uint8_t cmd = single ? SINGLE_ENDED : DIFFERENTIAL;
    // force channel into 3 bits in case channel is out of range
    channel = channel & 0x7;
    uint8_t csel = (channel >> 1) | ((channel & 0x1) << 2);
    cmd = cmd | csel << 4 | IREF_ON_AD_ON;
    i2c_dev_write(dev, NULL, 0, &cmd, 1);

    i2c_dev_read(dev, NULL, 0, &buffer, 2);
    return buffer;
}

esp_err_t ads7828_init_desc(i2c_dev_t *dev)
{
    if (!dev)
        return ESP_ERR_INVALID_ARG;
    return i2c_dev_create_mutex(dev);
}

uint16_t ads7828_read(i2c_dev_t *dev, uint8_t channel)
{
    return read_data(dev, channel, true);
}

uint16_t ads7828_readdif(i2c_dev_t *dev, uint8_t channel)
{
    return read_data(dev, channel, false);
}
