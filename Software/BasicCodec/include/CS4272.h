#ifndef CS4272_H
#define CS4272_H

#include "hardware/i2c.h"
#include "hardware/clocks.h"

#include "i2s.pio.h"


// I2S PIO definitions
#define PIN_MCLK   16
#define PIN_LRCK   17
#define PIN_SCLK   18
#define PIN_SDIN   19
#define PIN_SDOUT  20

// Codec address
#define CS4272_ADDR 0x10

// Codec register amount
#define CS4272_REG_NUM 8

// Codec clock ratio definitions
#define CS4272_MCLK_RATIO 256
#define CS4272_SCLK_RATIO 64

// Mode Control registers
#define CS4272_MODE_CONTROL			(uint8_t)0x01
#define CS4272_MC_FUNC_MODE(x)		(uint8_t)(((x) & 0x03) << 6)
#define CS4272_MC_RATIO_SEL(x)		(uint8_t)(((x) & 0x03) << 4)
#define CS4272_MC_MASTER_SLAVE		(uint8_t)0x08
#define CS4272_MC_SERIAL_FORMAT(x)	(uint8_t)(((x) & 0x07) << 0)

// DAC Control registers
#define CS4272_DAC_CTRL			        (uint8_t)0x02
#define CS4272_DAC_CTRL_AUTO_MUTE		(uint8_t)0x80
#define CS4272_DAC_CTRL_FILTER_SEL		(uint8_t)0x40
#define CS4272_DAC_CTRL_DE_EMPHASIS(x)	(uint8_t)(((x) & 0x03) << 4)
#define CS4272_DAC_CTRL_VOL_RAMP_UP		(uint8_t)0x08
#define CS4272_DAC_CTRL_VOL_RAMP_DN		(uint8_t)0x04
#define CS4272_DAC_CTRL_INV_POL(x)		(uint8_t)(((x) & 0x03) << 0)

// DAC Volume and Mixing registers
#define CS4272_DAC_VOL				    (uint8_t)0x03
#define CS4272_DAC_VOL_CH_VOL_TRACKING	(uint8_t)0x40
#define CS4272_DAC_VOL_SOFT_RAMP(x)		(uint8_t)(((x) & 0x03) << 4)
#define CS4272_DAC_VOL_ATAPI(x)			(uint8_t)(((x) & 0x0F) << 0)

// DAC Channel A registers
#define CS4272_DAC_CHA_VOL			    (uint8_t)0x04
#define CS4272_DAC_CHA_VOL_MUTE			(uint8_t)0x80
#define CS4272_DAC_CHA_VOL_VOLUME(x)    (uint8_t)(((x) & 0x7F) << 0)

// DAC Channel B registers
#define CS4272_DAC_CHB_VOL			    (uint8_t)0x05
#define CS4272_DAC_CHB_VOL_MUTE			(uint8_t)0x80
#define CS4272_DAC_CHB_VOL_VOLUME(x)    (uint8_t)(((x) & 0x7F) << 0)

// ADC Control registers
#define CS4272_ADC_CTRL				(uint8_t)0x06
#define CS4272_ADC_CTRL_DITHER		(uint8_t)0x20
#define CS4272_ADC_CTRL_SER_FORMAT	(uint8_t)0x10
#define CS4272_ADC_CTRL_MUTE(x)		(uint8_t)(((x) & 0x03) << 2)
#define CS4272_ADC_CTRL_HPF(x)		(uint8_t)(((x) & 0x03) << 0)

// Mode Control 2 registers
#define CS4272_MODE_CTRL2			    (uint8_t)0x07
#define CS4272_MODE_CTRL2_LOOP			(uint8_t)0x10
#define CS4272_MODE_CTRL2_MUTE_TRACK	(uint8_t)0x08
#define CS4272_MODE_CTRL2_CTRL_FREEZE	(uint8_t)0x04
#define CS4272_MODE_CTRL2_CTRL_PORT_EN	(uint8_t)0x02
#define CS4272_MODE_CTRL2_POWER_DOWN	(uint8_t)0x01

// Chip ID registers
#define CS4272_CHIP_ID		    (uint8_t)0x08
#define CS4272_CHIP_ID_PART(x)  (uint8_t)(((x) & 0x0F) << 4)
#define CS4272_CHIP_ID_REV(x)	(uint8_t)(((x) & 0x0F) << 0)

// Codec reset definition
#define CS4272_RESET_PIN 22


class CS4272
{
public:
    CS4272(PIO pio = pio0, i2c_inst_t *i2c = i2c1);

    void init();
    bool setup();

	bool volume(uint8_t vol_left, uint8_t vol_right);

	bool muteOutput(bool mute);
	bool muteInput(bool mute);

    bool loopMode(bool loop);

	bool setDither(bool dither);

    bool readInput(uint32_t &val_left, uint32_t &val_right);
    bool writeOutput(uint32_t val_left, uint32_t val_right);

private:
    PIO pio_port;
    uint sm_mclk;
    uint sm_tx;
    uint sm_rx;

    i2c_inst_t *i2c_port;

    uint8_t reg_local[CS4272_REG_NUM];

    bool read_reg(uint8_t reg, uint8_t *value);
    bool write_reg(uint8_t reg, uint8_t value);
};

#endif