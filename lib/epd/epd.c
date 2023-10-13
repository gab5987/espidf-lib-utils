#include "driver/gpio.h"
#include "epd4i2.h"
#include "freertos/portmacro.h"
#include "stdbool.h"
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/projdefs.h>
#include <freertos/task.h>
#include <hal/gpio_types.h>
#include <stdlib.h>
#include <string.h>

#define SEND_DEV_COMM(x) Epd_WriteTransaction(device, x, true);
#define SEND_DEV_DATA(x) Epd_WriteTransaction(device, x, false);

static const char *TAG = "EP Display";

static const unsigned char lut_vcom0_full[] = {
    0x40, 0x17, 0x00, 0x00, 0x00, 0x02, 0x00, 0x17, 0x17, 0x00, 0x00, 0x02, 0x00, 0x0A, 0x01,
    0x00, 0x00, 0x01, 0x00, 0x0E, 0x0E, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char lut_ww_full[] = {
    0x40, 0x17, 0x00, 0x00, 0x00, 0x02, 0x90, 0x17, 0x17, 0x00, 0x00, 0x02, 0x40, 0x0A,
    0x01, 0x00, 0x00, 0x01, 0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char lut_bw_full[] = {
    0x40, 0x17, 0x00, 0x00, 0x00, 0x02, 0x90, 0x17, 0x17, 0x00, 0x00, 0x02, 0x40, 0x0A,
    0x01, 0x00, 0x00, 0x01, 0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char lut_wb_full[] = {
    0x80, 0x17, 0x00, 0x00, 0x00, 0x02, 0x90, 0x17, 0x17, 0x00, 0x00, 0x02, 0x80, 0x0A,
    0x01, 0x00, 0x00, 0x01, 0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char lut_bb_full[] = {
    0x80, 0x17, 0x00, 0x00, 0x00, 0x02, 0x90, 0x17, 0x17, 0x00, 0x00, 0x02, 0x80, 0x0A,
    0x01, 0x00, 0x00, 0x01, 0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#define T1 25 // color change charge balance pre-phase
#define T2 1  // color change or sustain charge balance pre-phase
#define T3 2  // color change or sustain phase
#define T4 25 // color change phase

static const unsigned char lut_20_vcom0_partial[] = {
    0x00, T1, T2, T3, T4, 1, // 00 00 00 00
    0x00, 1,  0,  0,  0,  1, // gnd phase
};

static const unsigned char lut_21_ww_partial[] = {
    // 10 w
    0x18, T1, T2, T3, T4, 1, // 00 01 10 00
    0x00, 1,  0,  0,  0,  1, // gnd phase
};

static const unsigned char lut_22_bw_partial[] = {
    // 10 w
    0x5A, T1, T2, T3, T4, 1, // 01 01 10 10
    0x00, 1,  0,  0,  0,  1, // gnd phase
};

static const unsigned char lut_23_wb_partial[] = {
    // 01 b
    0xA5, T1, T2, T3, T4, 1, // 10 10 01 01
    0x00, 1,  0,  0,  0,  1, // gnd phase
};

static const unsigned char lut_24_bb_partial[] = {
    // 01 b
    0x24, T1, T2, T3, T4, 1, // 00 10 01 00
    0x00, 1,  0,  0,  0,  1, // gnd phase
};

static void Epd_WriteTransaction(EpdDevice_t *device, const uint8_t data, bool cmd)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &data;

    // t.flags = SPI_TRANS_CS_KEEP_ACTIVE; // Keep CS active after data transfer

    if (cmd)
        gpio_set_level(device->dc, 0);
    esp_err_t ret = spi_device_transmit(device->spi_handler, &t);
    if (cmd)
        gpio_set_level(device->dc, 1);
    assert(ret == ESP_OK); // Should have had no issues.
}

static void Epd_Reset(EpdDevice_t *device)
{
    for (int i = 0; i < 4; i++)
    {
        gpio_set_level(device->rst, i % 2 == 0);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    gpio_set_level(device->rst, 1);
}

inline static void Epd_WaitBusy(EpdDevice_t *dev)
{
    while (!gpio_get_level(dev->busy))
        vTaskDelay(pdTICKS_TO_MS(1));
}

void Epd_WriteDataPGM(EpdDevice_t *device, const uint8_t *data, uint16_t n, int16_t fill_with_zeroes)
{
    for (uint16_t i = 0; i < n; i++)
        Epd_WriteTransaction(device, *data++, false);
    while (fill_with_zeroes > 0)
    {
        Epd_WriteTransaction(device, 0x00, false);
        fill_with_zeroes--;
    }
}

static void Epd_InitFullUpdate(EpdDevice_t *device)
{
    ESP_LOGI(TAG, "Initializing full display update...");
    Epd_WriteTransaction(device, 0x00, true);
    Epd_WriteTransaction(device, 0x3f, false); // 300x400 B/W mode, LUT set by register

    unsigned int count;
    Epd_WriteTransaction(device, 0x20, true); // vcom
    for (count = 0; count < 44; count++)
        Epd_WriteTransaction(device, lut_vcom0_full[count], false);

    Epd_WriteTransaction(device, 0x21, true); // ww --
    for (count = 0; count < 42; count++)
        Epd_WriteTransaction(device, lut_ww_full[count], false);

    Epd_WriteTransaction(device, 0x22, true); // bw r
    for (count = 0; count < 42; count++)
        Epd_WriteTransaction(device, lut_bw_full[count], false);

    Epd_WriteTransaction(device, 0x23, true); // wb w
    for (count = 0; count < 42; count++)
        Epd_WriteTransaction(device, lut_wb_full[count], false);

    Epd_WriteTransaction(device, 0x24, true); // bb b
    for (count = 0; count < 42; count++)
        Epd_WriteTransaction(device, lut_bb_full[count], false);
}

static void Epd_InitPartialUpdate(EpdDevice_t *device)
{
    ESP_LOGI(TAG, "Initializing partial display update...");
    Epd_WriteTransaction(device, 0x00, true);
    Epd_WriteTransaction(device, 0x3F, false); // 300x400 B/W mode, LUT set by register
    Epd_WriteTransaction(device, 0x20, true);
    Epd_WriteDataPGM(device, lut_20_vcom0_partial, sizeof(lut_20_vcom0_partial), 44 - sizeof(lut_20_vcom0_partial));
    Epd_WriteTransaction(device, 0x21, true);
    Epd_WriteDataPGM(device, lut_21_ww_partial, sizeof(lut_21_ww_partial), 42 - sizeof(lut_21_ww_partial));
    Epd_WriteTransaction(device, 0x22, true);
    Epd_WriteDataPGM(device, lut_22_bw_partial, sizeof(lut_22_bw_partial), 42 - sizeof(lut_22_bw_partial));
    Epd_WriteTransaction(device, 0x23, true);
    Epd_WriteDataPGM(device, lut_23_wb_partial, sizeof(lut_23_wb_partial), 42 - sizeof(lut_23_wb_partial));
    Epd_WriteTransaction(device, 0x24, true);
    Epd_WriteDataPGM(device, lut_24_bb_partial, sizeof(lut_24_bb_partial), 42 - sizeof(lut_24_bb_partial));
}

void Epd_WakeUp(EpdDevice_t *device)
{
    ESP_LOGI(TAG, "Waking up display...");
    Epd_Reset(device);

    Epd_WriteTransaction(device, 0x01, true);  // POWER SETTING
    Epd_WriteTransaction(device, 0x03, false); // VDS_EN, VDG_EN internal
    Epd_WriteTransaction(device, 0x00, false); // VCOM_HV, VGHL_LV=16V
    Epd_WriteTransaction(device, 0x2b, false); // VDH=11V
    Epd_WriteTransaction(device, 0x2b, false); // VDL=11V
    Epd_WriteTransaction(device, 0x06, true);  // boost soft start
    Epd_WriteTransaction(device, 0x17, false); // A
    Epd_WriteTransaction(device, 0x17, false); // B
    Epd_WriteTransaction(device, 0x17, false); // C
    Epd_WriteTransaction(device, 0x00, true);  // panel setting
    Epd_WriteTransaction(device, 0x3f, false); // 300x400 B/W mode, LUT set by register
    Epd_WriteTransaction(device, 0x30, true);  // PLL setting
    Epd_WriteTransaction(device, 0x3a, false); // 3a 100HZ   29 150Hz 39 200HZ 31 171HZ
    Epd_WriteTransaction(device, 0x61, true);  // resolution setting
    Epd_WriteTransaction(device, EPD_WIDTH / 256, false);
    Epd_WriteTransaction(device, EPD_WIDTH % 256, false);
    Epd_WriteTransaction(device, EPD_HEIGHT / 256, false);
    Epd_WriteTransaction(device, EPD_HEIGHT % 256, false);
    Epd_WriteTransaction(device, 0x82, true); // vcom_DC setting
    Epd_WriteTransaction(device, 0x12,
                         false); // -0.1 + 18 * -0.05 = -1.0V from OTP, slightly better
    // Epd_WriteTransaction(device, 0x1c, false);   // -0.1 + 28 * -0.05 = -1.5V test, worse
    Epd_WriteTransaction(device, 0x50, true);  // VCOM AND DATA INTERVAL SETTING
    Epd_WriteTransaction(device, 0xd7, false); // border floating to avoid flashing
    Epd_WriteTransaction(device, 0x04, true);

    Epd_WaitBusy(device);
    Epd_InitFullUpdate(device);
}

uint16_t Epd_SetPartialRamArea(EpdDevice_t *device, uint16_t x, uint16_t y, uint16_t xe, uint16_t ye)
{
    ESP_LOGI(TAG, "Setting up display partial RAM area...");

    x &= 0xFFF8;                              // byte boundary
    xe = (xe - 1) | 0x0007;                   // byte boundary - 1
    Epd_WriteTransaction(device, 0x90, true); // partial window
    Epd_WriteTransaction(device, x / 256, false);
    Epd_WriteTransaction(device, x % 256, false);
    Epd_WriteTransaction(device, xe / 256, false);
    Epd_WriteTransaction(device, xe % 256, false);
    Epd_WriteTransaction(device, y / 256, false);
    Epd_WriteTransaction(device, y % 256, false);
    Epd_WriteTransaction(device, ye / 256, false);
    Epd_WriteTransaction(device, ye % 256, false);
    Epd_WriteTransaction(device, 0x01, false);
    // Epd_WriteTransaction(device 0x00); // don't see any difference
    return (7 + xe - x) / 8; // number of bytes to transfer
}

void Epd_Render(EpdDevice_t *device)
{
    Epd_WriteTransaction(device, 0x13, true);
    for (uint32_t i = 0; i < EPD_BUFFER_SIZE; i++)
    {
        uint8_t data = i < EPD_BUFFER_SIZE ? device->buffer[i] : 0x00;
        Epd_WriteTransaction(device, data, false);
    }
    Epd_WriteTransaction(device, 0x12, true); // display refresh
    Epd_WaitBusy(device);
}

esp_err_t Epd_Initilize(EpdDevice_t *device)
{
    esp_err_t ret;

    gpio_set_direction(device->busy, GPIO_MODE_INPUT);
    gpio_set_direction(device->rst, GPIO_MODE_OUTPUT);
    gpio_set_direction(device->dc, GPIO_MODE_OUTPUT);

    gpio_set_level(device->rst, 1);
    gpio_set_level(device->dc, 1);

    device->buffer = malloc(EPD_BUFFER_SIZE);
    if (device->buffer == NULL)
    {
        ret = ESP_ERR_NO_MEM;
        return ret;
    }

    ESP_LOGI(TAG, "Initializing bus SPI%d...", device->spi_host + 1);

    spi_device_handle_t spi;
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = device->din,
        .sclk_io_num = device->clk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 128,
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 4000000,
        .mode = 0,
        .spics_io_num = device->cs,
        .queue_size = 7,
    };

    ESP_ERROR_CHECK(ret = spi_bus_initialize(device->spi_host, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(ret = spi_bus_add_device(device->spi_host, &devcfg, &spi));

    device->spi_handler = spi;

    ESP_LOGI(TAG, "Attach to main flash bus...");

    memset(device->buffer, 0xff, EPD_BUFFER_SIZE); // Set display to full blank

    Epd_WakeUp(device);
    Epd_WriteTransaction(device, 0x13, true);
    for (uint32_t i = 0; i < EPD_BUFFER_SIZE; i++)
    {
        uint8_t data = i < EPD_BUFFER_SIZE ? device->buffer[i] : 0x00;
        Epd_WriteTransaction(device, ~data, false);
    }
    Epd_WriteTransaction(device, 0x12, true); // display refresh
    Epd_WaitBusy(device);

    Epd_InitPartialUpdate(device);
    Epd_WriteTransaction(device, 0x91, true); // partial in

    Epd_SetPartialRamArea(device, 0, 0, EPD_WIDTH, EPD_HEIGHT);

    Epd_Render(device);
    Epd_WriteTransaction(device, 0x92, true); // partial out

    return ret;
}

