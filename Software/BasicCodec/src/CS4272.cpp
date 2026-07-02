#include "CS4272.h"


CS4272::CS4272(PIO pio, i2c_inst_t *i2c) : pio_port(pio), i2c_port(i2c)
{
    for (int i = 0; i < CS4272_REG_NUM; ++i)
        reg_local[i] = 0x00;
}

void CS4272::init()
{
    // Get the system clock frequency
    const uint32_t sys_clk = clock_get_hz(clk_sys);

    // Set the state machine numbers
    sm_mclk = 0;
    sm_tx   = 1;
    sm_rx   = 2;

    // Add the PIO programs
    uint off_mclk = pio_add_program(pio_port, &mclk_program);
    uint off_tx   = pio_add_program(pio_port, &i2s_tx_program);
    uint off_rx   = pio_add_program(pio_port, &i2s_rx_program);

    // Calculate the PIO clocks
    float div_mclk = sys_clk / (2 * SAMPLES_PER_SEC * CS4272_MCLK_RATIO);
    float div_sclk = sys_clk / (2 * SAMPLES_PER_SEC * CS4272_SCLK_RATIO);

    // Initialize the PIO programs
    mclk_program_init(pio_port, sm_mclk, off_mclk, PIN_MCLK, div_mclk);
    i2s_tx_program_init(pio_port, sm_tx, off_tx, PIN_SDOUT, PIN_LRCK, div_sclk);
    i2s_rx_program_init(pio_port, sm_rx, off_rx, PIN_SDIN, PIN_LRCK, div_sclk);

    pio_enable_sm_mask_in_sync(pio_port, (1u << sm_mclk) | (1u << sm_tx) | (1u << sm_rx));
}

bool CS4272::setup()
{
    bool result = true;

    // Codec reset pin
    gpio_init(CS4272_RESET_PIN);
    gpio_set_dir(CS4272_RESET_PIN, GPIO_OUT);
    gpio_disable_pulls(CS4272_RESET_PIN);

    // Enable the codec and wait (datasheet)
    gpio_put(CS4272_RESET_PIN, 1);

    // Set power down mode and then control port as enabled
    result &= writeReg(CS4272_MODE_CTRL2, CS4272_MODE_CTRL2_POWER_DOWN | CS4272_MODE_CTRL2_CTRL_PORT_EN);

    // Set single speed clock ratio and I2S format
    result &= writeReg(CS4272_MODE_CONTROL, CS4272_MC_RATIO_SEL(0) | CS4272_MC_SERIAL_FORMAT(1));

    // Set the ADC for I2S format
    result &= writeReg(CS4272_ADC_CTRL, CS4272_ADC_CTRL_SER_FORMAT);

    // Release the power down mode
    result &= writeReg(CS4272_MODE_CTRL2, CS4272_MODE_CTRL2_CTRL_PORT_EN);

    return result;
}

bool CS4272::volume(uint8_t vol_left, uint8_t vol_right)
{
    bool result = true;

	reg_local[CS4272_DAC_CHA_VOL] = CS4272_DAC_CHA_VOL_VOLUME(0x7F - (vol_left & 0x7F));
	result &= writeReg(CS4272_DAC_CHA_VOL, reg_local[CS4272_DAC_CHA_VOL]);
	
	reg_local[CS4272_DAC_CHB_VOL] = CS4272_DAC_CHB_VOL_VOLUME(0x7F - (vol_right & 0x7F));
	result &= writeReg(CS4272_DAC_CHB_VOL, reg_local[CS4272_DAC_CHB_VOL]);

	return result;
}

bool CS4272::muteOutput(bool mute)
{
    if (mute) {
        reg_local[CS4272_DAC_CHA_VOL] |= CS4272_DAC_CHA_VOL_MUTE;
        reg_local[CS4272_DAC_CHB_VOL] |= CS4272_DAC_CHB_VOL_MUTE;
    }
    else {
        reg_local[CS4272_DAC_CHA_VOL] &= ~CS4272_DAC_CHA_VOL_MUTE;
        reg_local[CS4272_DAC_CHB_VOL] &= ~CS4272_DAC_CHB_VOL_MUTE;
    }

    bool result = true;

    result &= writeReg(CS4272_DAC_CHA_VOL, reg_local[CS4272_DAC_CHA_VOL]);
    result &= writeReg(CS4272_DAC_CHB_VOL, reg_local[CS4272_DAC_CHB_VOL]);

	return result;
}

bool CS4272::muteInput(bool mute)
{
    if (mute)
        reg_local[CS4272_ADC_CTRL] |= CS4272_ADC_CTRL_MUTE(3);
    else
        reg_local[CS4272_ADC_CTRL] &= ~CS4272_ADC_CTRL_MUTE(3);

	return writeReg(CS4272_ADC_CTRL, reg_local[CS4272_ADC_CTRL]);
}

bool CS4272::loopMode(bool loop)
{
    if (loop)
        reg_local[CS4272_MODE_CTRL2] |= CS4272_MODE_CTRL2_LOOP;
    else
        reg_local[CS4272_MODE_CTRL2] &= ~CS4272_MODE_CTRL2_LOOP;

	return writeReg(CS4272_MODE_CTRL2, reg_local[CS4272_MODE_CTRL2]);
}

bool CS4272::setDither(bool dither)
{
    if (dither)
        reg_local[CS4272_ADC_CTRL] |= CS4272_ADC_CTRL_DITHER;
    else
        reg_local[CS4272_ADC_CTRL] &= ~CS4272_ADC_CTRL_DITHER;

	return writeReg(CS4272_ADC_CTRL, reg_local[CS4272_ADC_CTRL]);
}

bool CS4272::readInput(uint32_t &val_left, uint32_t &val_right)
{
    val_left = pio_sm_get_blocking(pio_port, sm_rx);

    val_right = pio_sm_get_blocking(pio_port, sm_rx);

    return true;
}

bool CS4272::writeOutput(uint32_t val_left, uint32_t val_right)
{
    pio_sm_put_blocking(pio_port, sm_tx, val_left << 8);

    pio_sm_put_blocking(pio_port, sm_tx, val_right << 8);

    return true;
}

bool CS4272::readReg(uint8_t reg, uint8_t *value)
{
    bool result = true;

    // Write the adress of the register to read
    result &= i2c_write_blocking(i2c_port, CS4272_ADDR, &reg, 1, true) == 1;
    if (result)
        // Read the response from the codec
        result &= i2c_read_blocking(i2c_port, CS4272_ADDR, value, 1, false) == 1;

    return result;
}

bool CS4272::writeReg(uint8_t addr, uint8_t value)
{
    uint8_t buff[2] = { addr, value };

    bool result = i2c_write_blocking(i2c_port, CS4272_ADDR, buff, 2, false) == 2;

    if (result)
        reg_local[addr] = value;

    sleep_ms(10);

    return result;
}
