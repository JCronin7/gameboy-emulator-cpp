#ifndef _APU_H_
#define _APU_H_

#include "hal.h"
#include "mmu.h"
#include "platform.h"

typedef enum apu_duty_cycle_enum
{
    APU_DUTY_CYCLE_EIGHTH = 0,
    APU_DUTY_CYCLE_QUARTER = 1,
    APU_DUTY_CYCLE_HALF = 2,
    APU_DUTY_CYCLE_QUARTER_N = 3
} apu_duty_cycle_enum;

void apu_create(emu::gameboy::GameboyMemory *memory);
void apu_destroy(void);

void apu_pulse_generate(uint8_t duty, uint8_t volume, int16_t *buffer, size_t buffer_size);

#endif /* _APU_H_ */
