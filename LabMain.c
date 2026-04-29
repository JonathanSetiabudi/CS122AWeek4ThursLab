#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"

#define MASTER_LED0 2
#define MASTER_LED1 3
#define MASTER_LED2 4
#define MASTER_LED3 5

#define PATTERN_SWITCH 20
#define POT_PIN 26

#define SPI_CS_FPGA 17
#define SPI_CLK_PIN 18
#define SPI_MOSI_PIN 19

#define SPI_PORT spi0

#ifndef GPIO_FUNC_SPI
#define GPIO_FUNC_SPI 1
#endif

uint8_t pattern0[] = {
    0b00000000, 0b00000001, 0b00000011, 0b00000111, 0b00001111, 0b00011111, 0b00111111, 0b01111111, 0b11111111
};
#define PATTERN0_LEN (sizeof(pattern0) / sizeof(uint8_t))
int pattern0_index = 0;

uint8_t pattern1[] = {
    0b01010101, 0b10101010
};
#define PATTERN1_LEN (sizeof(pattern1) / sizeof(uint8_t))
int pattern1_index = 0;

long map(long x, long in_min, long in_max, long out_min, long out_max) 
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void update_leds(uint8_t bits) 
{
    gpio_put(MASTER_LED0, (bits >> 0) & 1);
    gpio_put(MASTER_LED1, (bits >> 1) & 1);
    gpio_put(MASTER_LED2, (bits >> 2) & 1);
    gpio_put(MASTER_LED3, (bits >> 3) & 1);
}

void spi_send_to_fpga(uint8_t bits) 
{
    uint8_t data = (bits & 0x0F);
    gpio_put(SPI_CS_FPGA, 0);
    sleep_us(2);
    spi_write_blocking(SPI_PORT, &data, 1);
    sleep_us(2);
    gpio_put(SPI_CS_FPGA, 1);
}

void setup_spi_master() {
    // Start slower for signal-integrity/timing margin during lab bring-up.
    spi_init(SPI_PORT, 250);
    gpio_set_function(SPI_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MOSI_PIN, GPIO_FUNC_SPI);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_init(SPI_CS_FPGA);
    gpio_set_dir(SPI_CS_FPGA, true);
    gpio_put(SPI_CS_FPGA, 1);
}

int main()
{
    // Set up of pins
    stdio_init_all();
    sleep_ms(1000);

    for (int i = MASTER_LED0; i <= MASTER_LED3; i++)
    {
        gpio_init(i);
        gpio_set_dir(i, true);
        gpio_put(i, 0);
    }

    gpio_init(PATTERN_SWITCH);
    gpio_set_dir(PATTERN_SWITCH, false);
    gpio_pull_down(PATTERN_SWITCH);

    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(0);
    
    setup_spi_master();

    int frame_index = 0;
    absolute_time_t last_update = get_absolute_time();
    
    while (true) 
    {
        bool pattern = gpio_get(PATTERN_SWITCH);
        uint16_t adc_value = adc_read();
        int delay_ms = map(adc_value, 0, 4095, 100, 1000);

        absolute_time_t now = get_absolute_time();
        if (absolute_time_diff_us(last_update, now) >= (int64_t)delay_ms * 1000)
        {
            last_update = now;
            uint8_t val = 0;

            if (!pattern)
            {
                val = pattern0[pattern0_index];
                pattern0_index = (pattern0_index + 1) % PATTERN0_LEN;
            }
            else if(pattern)
            {
                val = pattern1[pattern1_index];
                pattern1_index = (pattern1_index + 1) % PATTERN1_LEN;
                
            }
            update_leds(val);
            spi_send_to_fpga(val >> 4);
        }
            
        sleep_ms(10);
        
    }
}