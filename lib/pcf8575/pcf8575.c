#include <esp_err.h>
#include "pcf8575.h"

static esp_err_t read_port(i2c_dev_t *dev, uint16_t *val)
{
    if (!(dev && val))
        return ESP_ERR_INVALID_ARG;

    I2C_DEV_TAKE_MUTEX(dev);
    I2C_DEV_CHECK(dev, i2c_dev_read(dev, NULL, 0, val, 2));
    I2C_DEV_GIVE_MUTEX(dev);

    return ESP_OK;
}

static esp_err_t write_port(i2c_dev_t *dev, uint16_t val)
{
    if (!dev)
        return ESP_ERR_INVALID_ARG;

    I2C_DEV_TAKE_MUTEX(dev);
    I2C_DEV_CHECK(dev, i2c_dev_write(dev, NULL, 0, &val, 2));
    I2C_DEV_GIVE_MUTEX(dev);

    return ESP_OK;
}

esp_err_t pcf8575_init_desc(i2c_dev_t *dev)
{
    if (!dev)
        return ESP_ERR_INVALID_ARG;
    return i2c_dev_create_mutex(dev);
}

esp_err_t pcf8575_free_desc(i2c_dev_t *dev)
{
    if (!dev)
        return ESP_ERR_INVALID_ARG;
    return i2c_dev_delete_mutex(dev);
}

esp_err_t pcf8575_port_read(i2c_dev_t *dev, uint16_t *val)
{
    return read_port(dev, val);
}

esp_err_t pcf8575_port_write(i2c_dev_t *dev, uint16_t val)
{
    return write_port(dev, val);
}
