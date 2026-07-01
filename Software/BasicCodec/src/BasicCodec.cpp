#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "CS4272.h"

// I2C port definitions
#define I2C_PORT    i2c1
#define I2C_SDA     14
#define I2C_SCL     15
#define I2C_BAUD    100000


void init_i2c()
{
    i2c_init(I2C_PORT, I2C_BAUD);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
}

static void enable_ftz(void)
{
	uint32_t fpscr;

	fpscr = __builtin_arm_get_fpscr();
	fpscr |= 1u << 24;
	__builtin_arm_set_fpscr(fpscr);
}

int main()
{
    // Set the system clock
    set_sys_clock_khz(OC_CLK_KHZ, true);

    // Enable flush-to-zero for performance
    enable_ftz();

    stdio_init_all();

    // Wait for console connection
    while (!stdio_usb_connected()) {
        sleep_ms(1000);
    }

    // Welcome message
    printf("-- CS4272 CODEC TEST --\n");

    init_i2c();

    // Codec configuration
    CS4272 codec(pio0, I2C_PORT);
    codec.init();

    if (!codec.setup())
        printf("Codec setup error!\n");

    float sys_clk = clock_get_hz(clk_sys) / 1000000.0f;
    printf("Running passthrough at clock: %.2f MHz\n", sys_clk);

    int sample = 0, size = SAMPLES_PER_SEC / 1000;
    int32_t sine_table[size];

    for (int i = 0; i < size; ++i) {
        int32_t sample = (int32_t)(0x7FFFFF * sin(2.0 * M_PI * i / size));
        sine_table[i] = sample << 8;
    }

    while (true) {
        uint32_t value = (uint32_t)sine_table[sample];

        codec.writeOutput(value, value);

        sample++;
        if (sample >= size) {
            sample = 0;
        }
    }
}
