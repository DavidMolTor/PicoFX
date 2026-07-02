#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"

#include "CS4272.h"

// I2C port definitions
#define I2C_PORT    i2c1
#define I2C_SDA     14
#define I2C_SCL     15
#define I2C_BAUD    100000


void printBits(uint32_t value)
{
	char str[33];
	str[32] = 0;

	for (int i = 31; i >= 0; i--)
	{
		str[i] = '0' + (value & 1);
		value >>= 1;
	}

	printf(str);
}

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

    int sample = 0;
    uint32_t rx_left, rx_right;

    while (true) {
        codec.readInput(rx_left, rx_right);

        if (sample < SAMPLES_PER_SEC)
        {
            sample++;
            continue;
        }

        int32_t value_left  = ((int32_t)(rx_left << 8)) >> 8;
        int32_t value_right = ((int32_t)(rx_right << 8)) >> 8;

        float mv_left   = (float)value_left * 5000.0f / 8388608.0f;
        float mv_right  = (float)value_right * 5000.0f / 8388608.0f;

        printf("Values: %.2f, %.2f\n", mv_left, mv_right);

        sample = 0;
    }
}
