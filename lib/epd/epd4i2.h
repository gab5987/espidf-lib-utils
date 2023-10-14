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

#define EPD_COLOR_WHITE 0xff
#define EPD_COLOR_BLACK 0x00

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

#ifdef __cplusplus
extern "C"
{
#endif
    /*
     * @brief Initializes and clears the epd displays
     * @param device A pointer to a EpdDevice
     * @returns ESP_OK if setup correctly
     * */
    esp_err_t Epd_Initilize(EpdDevice_t *device);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    /*
     * @brief Renders the buffer stored into the device
     * @param device A pointer to a EpdDevice
     * */
    void Epd_Render(EpdDevice_t *device);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    /*
     * @brief Stores EPD_COLOR_WHITE in the devices buffer
     * @param device A pointer to a EpdDevice
     * */
    void Epd_ClearDisplay(EpdDevice_t *device);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    /*
     * @brief Draw and stores a bitmap into the devices buffer
     * @param device A pointer to a EpdDevice
     * @param x X axis offset
     * @param y Y axis offset
     * @param w Bitmat width
     * @param h Bitmat height
     * @param color_invert Inverts the bg/fg color
     * */
    void Epd_DisplayBmp(EpdDevice_t *device, int x, int y, int w, int h, const unsigned char bmp[], bool color_invert);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    /*
     * @brief Receives a text and draws it into the devices buffer, might overrite
     *        the buffer since it automatically breaks line
     * @param device A pointer to a EpdDevice
     * @param x X axis offset
     * @param y Y axis offset
     * @param txt Text to draw
     * @param font Font to be used(8, 12, 16 are available)
     * @param color_invert Inverts the bg/fg color
     * */
    void Epd_DrawText(EpdDevice_t *device, u_int16_t x, u_int16_t y, const char *txt, int font, bool color_invert);
#ifdef __cplusplus
}
#endif

#endif // !_4i2_EPD_H_
