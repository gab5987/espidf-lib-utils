/**
 * @file  epd4i2.h
 * @brief Library to work with WaveShare's 4.2 inch B/W E-Paper Display
 * @date  2023-10--12
 * @copyright Gabriel Novalski
 */
#ifndef _4i2_EPD_H_
#define _4i2_EPD_H_

#include <driver/gpio.h>
#include <driver/spi_master.h>

#define EPD_WIDTH 400
#define EPD_HEIGHT 300

#define EPD_BUFFER_SIZE ((uint32_t)(EPD_WIDTH) * (uint32_t)(EPD_HEIGHT) / 8)

/*
 * Struct refering to the display configuration
 * */
struct EpdDevice
{
    // Attached GPIO Pins
    gpio_num_t busy, rst, dc, cs, clk, din;
    // Refer to the pins used, defaults to SPI 3
    spi_host_device_t spi_host;
    spi_device_handle_t spi_handler;
    u_int8_t *buffer;
};

typedef struct EpdDevice EpdDevice_t;

/*
 * @brief Initializes and clears the epd displays
 * @param device A pointer to a EpdDevice
 * @returns ESP_OK if setup correctly
 * */
#ifdef __cplusplus
extern "C"
{
#endif
    esp_err_t Epd_Initilize(EpdDevice_t *device);
#ifdef __cplusplus
}
#endif

/*
 * @brief Renders the buffer stored into the device
 * @param device A pointer to a EpdDevice
 * */
#ifdef __cplusplus
extern "C"
{
#endif
    void Epd_Render(EpdDevice_t *device);
#ifdef __cplusplus
}
#endif

#endif // !_4i2_EPD_H_
