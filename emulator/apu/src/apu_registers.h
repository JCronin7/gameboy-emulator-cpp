#ifndef _APU_REGISTERS_H_
#define _APU_REGISTERS_H_

#include "platform.h"

/**
 * @brief FF10 — NR10: Channel 1 sweep
 *
 */
typedef struct apu_channel1_sweep_struct
{
    uint8_t individual_step : 3;
    uint8_t direction       : 1;
    uint8_t pace            : 3;
} apu_channel1_sweep_struct;

typedef apu_channel1_sweep_struct *apu_channel1_sweep_t;

/**
 * @brief FF11 — NR11: Channel 1 length timer & duty cycle
 *
 */
typedef struct apu_channel1_length_timer_and_duty_cycle_struct
{
    uint8_t initial_length_timer    : 6;
    uint8_t wave_duty               : 2;
} apu_channel1_length_timer_and_duty_cycle_struct;

typedef apu_channel1_length_timer_and_duty_cycle_struct *apu_channel1_length_timer_and_duty_cycle_t;

/**
 * @brief FF12 — NR12: Channel 1 volume & envelope
 *
 */
typedef struct apu_channel1_volume_and_envelope_struct
{
    uint8_t sweep_pace          : 3;
    uint8_t envelope_direction  : 1;
    uint8_t initial_volume      : 4;
} apu_channel1_volume_and_envelope_struct;

typedef apu_channel1_volume_and_envelope_struct *apu_channel1_volume_and_envelope_t;

/**
 * @brief FF13 — NR13: Channel 1 period low [write-only]
 *
 */
typedef struct apu_channel1_period_low_struct
{
    uint8_t value;
} apu_channel1_period_low_struct;

typedef apu_channel1_period_low_struct *apu_channel1_period_low_t;

/**
 * @brief FF14 — NR14: Channel 1 period high & control
 *
 */
typedef struct apu_channel1_period_high_and_control_struct
{
    uint8_t period          : 3;
    uint8_t rsvd            : 3;
    uint8_t length_enable   : 1;
    uint8_t trigger         : 1;
} apu_channel1_period_high_and_control_struct;

typedef apu_channel1_period_high_and_control_struct *apu_channel1_period_high_and_control_t;

/**
 * @brief FF16 — NR21: Channel 2 length timer & duty cycle
 *
 */
typedef struct apu_channel2_length_timer_and_duty_cycle_struct
{
    uint8_t initial_length_timer    : 6;
    uint8_t wave_duty               : 2;
} apu_channel2_length_timer_and_duty_cycle_struct;

typedef apu_channel2_length_timer_and_duty_cycle_struct *apu_channel2_length_timer_and_duty_cycle_t;

/**
 * @brief FF17 — NR22: Channel 2 volume & envelope
 *
 */
typedef struct apu_channel2_volume_and_envelope_struct
{
    uint8_t sweep_pace          : 3;
    uint8_t envelope_direction  : 1;
    uint8_t initial_volume      : 4;
} apu_channel2_volume_and_envelope_struct;

typedef apu_channel2_volume_and_envelope_struct *apu_channel2_volume_and_envelope_t;

/**
 * @brief FF18 — NR23: Channel 2 period low [write-only]
 *
 */
typedef struct apu_channel2_period_low_struct
{
    uint8_t value;
} apu_channel2_period_low_struct;

typedef apu_channel2_period_low_struct *apu_channel2_period_low_t;

/**
 * @brief FF19 — NR24: Channel 2 period high & control
 *
 */
typedef struct apu_channel2_period_high_and_control_struct
{
    uint8_t period          : 3;
    uint8_t rsvd            : 3;
    uint8_t length_enable   : 1;
    uint8_t trigger         : 1;
} apu_channel2_period_high_and_control_struct;

typedef apu_channel2_period_high_and_control_struct *apu_channel2_period_high_and_control_t;

/**
 * @brief FF1A — NR30: Channel 3 DAC enable
 *
 */
typedef struct apu_channel3_dac_enable_struct
{
    uint8_t rsvd   : 7;
    uint8_t dac_on : 1;
} apu_channel3_dac_enable_struct;

typedef apu_channel3_dac_enable_struct *apu_channel3_dac_enable_t;

/**
 * @brief FF1B — NR31: Channel 3 length timer [write-only]
 *
 */
typedef struct apu_channel3_length_timer_struct
{
    uint8_t initial_length_timer;
} apu_channel3_length_timer_struct;

typedef apu_channel3_length_timer_struct *apu_channel3_length_timer_t;

/**
 * @brief FF1C — NR32: Channel 3 output level
 *
 */
typedef struct apu_channel3_output_level_struct
{
    uint8_t rsvd         : 5;
    uint8_t output_level : 2;
} apu_channel3_output_level_struct;

typedef apu_channel3_output_level_struct *apu_channel3_output_level_t;

/**
 * @brief FF1D — NR33: Channel 3 period low [write-only]
 *
 */
typedef struct apu_channel3_period_low_struct
{
    uint8_t value;
} apu_channel3_period_low_struct;

typedef apu_channel3_period_low_struct *apu_channel3_period_low_t;

/**
 * @brief FF1E — NR34: Channel 3 period high & control
 *
 */
typedef struct apu_channel3_period_high_and_control_struct
{
    uint8_t period          : 3;
    uint8_t rsvd            : 3;
    uint8_t length_enable   : 1;
    uint8_t trigger         : 1;
} apu_channel3_period_high_and_control_struct;

typedef apu_channel3_period_high_and_control_struct *apu_channel3_period_high_and_control_t;

/**
 * @brief FF30–FF3F — Wave pattern RAM
 *
 */
typedef struct apu_channel3_wave_pattern_ram_struct
{
    uint8_t region[0x10];
} apu_channel3_wave_pattern_ram_struct;

typedef apu_channel3_wave_pattern_ram_struct *apu_channel3_wave_pattern_ram_t;

/**
 * @brief FF20 — NR41: Channel 4 length timer [write-only]
 *
 */
typedef struct apu_channel4_length_timer_struct
{
    uint8_t initial_length_timer;
} apu_channel4_length_timer_struct;

typedef apu_channel4_length_timer_struct *apu_channel4_length_timer_t;

/**
 * @brief FF21 — NR42: Channel 4 volume & envelope
 *
 */
typedef struct apu_channel4_volume_and_envelope_struct
{
    uint8_t sweep_pace          : 3;
    uint8_t envelope_direction  : 1;
    uint8_t initial_volume      : 4;
} apu_channel4_volume_and_envelope_struct;

typedef apu_channel4_volume_and_envelope_struct *apu_channel4_volume_and_envelope_t;

/**
 * @brief FF22 — NR43: Channel 4 frequency & randomness
 *
 */
typedef struct apu_channel4_frequency_and_randomness_struct
{
    uint8_t clock_divider   : 3;
    uint8_t lfsr_width      : 1;
    uint8_t clock_shift     : 4;
} apu_channel4_frequency_and_randomness_struct;

typedef apu_channel4_frequency_and_randomness_struct *apu_channel4_frequency_and_randomness_t;

/**
 * @brief FF23 — NR44: Channel 4 control
 *
 */
typedef struct apu_channel4_control_struct
{
    uint8_t rsvd            : 6;
    uint8_t length_enable   : 1;
    uint8_t trigger         : 1;
} apu_channel4_control_struct;

typedef apu_channel4_control_struct *apu_channel4_control_t;

/**
 * @brief FF25 — NR51: Sound panning
 *
 */
typedef struct apu_sound_panning_struct
{
    uint8_t channel1_right : 1;
    uint8_t channel2_right : 1;
    uint8_t channel3_right : 1;
    uint8_t channel4_right : 1;
    uint8_t channel1_left : 1;
    uint8_t channel2_left : 1;
    uint8_t channel3_left : 1;
    uint8_t channel4_left : 1;
} apu_sound_panning_struct;

typedef apu_sound_panning_struct *apu_sound_panning_t;

/**
 * @brief FF26 — NR52: Audio master control
 *
 */
typedef struct apu_audio_master_control_struct
{
    uint8_t channel1_on : 1;
    uint8_t channel2_on : 1;
    uint8_t channel3_on : 1;
    uint8_t channel4_on : 1;
    uint8_t rsvd        : 3;
    uint8_t audio_on    : 1;
} apu_audio_master_control_struct;

typedef apu_audio_master_control_struct *apu_audio_master_control_t;

// base reg is 0xFF10
typedef struct apu_registers_struct
{
    apu_channel1_sweep_struct                       channel1_sweep;
    apu_channel1_length_timer_and_duty_cycle_struct channel1_length_timer_and_duty_cycle;
    apu_channel1_volume_and_envelope_struct         channel1_volume_and_envelope;
    apu_channel1_period_low_struct                  channel1_period_low;
    apu_channel1_period_high_and_control_struct     channel1_period_high_and_control;
    uint8_t                                         rsvd0;
    apu_channel2_length_timer_and_duty_cycle_struct channel2_length_timer_and_duty_cycle;
    apu_channel2_volume_and_envelope_struct         channel2_volume_and_envelope;
    apu_channel2_period_low_struct                  channel2_period_low;
    apu_channel2_period_high_and_control_struct     channel2_period_high_and_control;
    apu_channel3_dac_enable_struct                  channel3_dac_enable;
    apu_channel3_length_timer_struct                channel3_length_timer;
    apu_channel3_output_level_struct                channel3_output_level;
    apu_channel3_period_low_struct                  channel3_period_low;
    apu_channel3_period_high_and_control_struct     channel3_period_high_and_control;
    uint8_t                                         rsvd1;
    apu_channel4_length_timer_struct                channel4_length_timer;
    apu_channel4_volume_and_envelope_struct         channel4_volume_and_envelope;
    apu_channel4_frequency_and_randomness_struct    channel4_frequency_and_randomness;
    apu_channel4_control_struct                     channel4_control;
    uint8_t                                         master_volume_and_vin_panning; // 0xFF24
    apu_sound_panning_struct                        sound_panning;
    apu_audio_master_control_struct                 audio_master_control;
    uint8_t                                         rsvd2[0x09];
    apu_channel3_wave_pattern_ram_struct            channel3_wave_pattern_ram;
} packed(1) apu_registers_struct;

typedef apu_registers_struct *apu_registers_t;
static_assert(sizeof(apu_registers_struct) == 0x30);

#endif /* _APU_REGISTERS_H_ */