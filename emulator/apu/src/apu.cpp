#include "apu.h"
#include "apu_registers.h"
#include "mmu.h"

#define APU_CONSTANT_VOLUME_STEP 0.017f

typedef struct apu_struct
{
    HAL_Timer *        length_timer;
    // Channel 1 - Pulse with period sweep
    struct
    {
        HAL_AudioChannel *device;
        HAL_Timer *      period_timer;
        HAL_Timer *      volume_timer;
        int16_t *         sample_buffer;
        size_t            buffer_size;
        float             period_gain;
        float             period_step;
        float             volume_gain;
        float             volume_step;
    } channel1;
    //hal_audio_channel_t channel2_pulse;
    //hal_audio_channel_t channel3_wave;
    //hal_audio_channel_t channel4_random;
} apu_struct;
static apu_struct apu;
static apu_registers_t apu_registers = NULL;

static inline uint16_t
apu_channel1_period_get(void)
{
    return (apu_registers->channel1_period_high_and_control.period << 8) | apu_registers->channel1_period_low.value;
}

static void
apu_process_channel1_update(uint16_t address, uint8_t value)
{
    require_or(apu_registers->audio_master_control.audio_on, return);

    if (apu_registers->channel1_period_high_and_control.trigger)
    {
        // Generate a pusle signal using the configurated duty and volume (32, 16-bit samples)
        apu_pulse_generate(apu_registers->channel1_length_timer_and_duty_cycle.wave_duty,
                           apu_registers->channel1_volume_and_envelope.initial_volume,
                           apu.channel1.sample_buffer, apu.channel1.buffer_size);
        uint32_t frequency = 1048576 / (2048 - apu_channel1_period_get());
        HAL_AudioChannelSetSamples(apu.channel1.device, apu.channel1.sample_buffer, apu.channel1.buffer_size, frequency);
        HAL_AudioChannelPlay(apu.channel1.device);

        if (apu_registers->channel1_sweep.individual_step)
        {
            uint64_t period_timer_interval = 7800000ull * apu_registers->channel1_sweep.pace;
            apu.channel1.period_gain = 1;
            float period_offset = 1.0f / (float)(1 << apu_registers->channel1_sweep.individual_step);
            apu.channel1.period_step = (1 + ((apu_registers->channel1_sweep.direction == 0) ? period_offset : -period_offset));
            HAL_TimerStart(apu.channel1.period_timer, HAL_TIMER_START_INTERVAL_NSEC, period_timer_interval);
        }

        apu.channel1.volume_gain = 1;
        apu.channel1.volume_step = APU_CONSTANT_VOLUME_STEP;
        uint64_t volume_timer_interval = 3900000ull * apu_registers->channel1_volume_and_envelope.sweep_pace;
        HAL_TimerStart(apu.channel1.volume_timer, HAL_TIMER_START_INTERVAL_NSEC, volume_timer_interval);
        apu_registers->audio_master_control.channel1_on = true;
    }
    else
    {
        HAL_AudioChannelPause(apu.channel1.device);
        if (HAL_TimerIsRunning(apu.channel1.period_timer))
        {
            HAL_TimerStop(apu.channel1.period_timer);
        }
        if (HAL_TimerIsRunning(apu.channel1.volume_timer))
        {
            HAL_TimerStop(apu.channel1.volume_timer);
        }
        apu_registers->audio_master_control.channel1_on = false;
    }
}

/// @todo this is tied to DIV-APU?
static void
apu_length_timer(void *args)
{
    if (apu_registers->channel1_length_timer_and_duty_cycle.initial_length_timer)
    {
        apu_registers->channel1_length_timer_and_duty_cycle.initial_length_timer++;
        apu_registers->channel1_length_timer_and_duty_cycle.initial_length_timer &= 0x3F;
    }
}

static void
apu_channel1_period_timer_callback(void *args)
{
    apu.channel1.period_gain *= apu.channel1.period_step;
    HAL_AudioChannelSetPitch(apu.channel1.device, apu.channel1.period_gain);
}

static void
apu_channel1_volume_timer_callback(void *args)
{
    apu.channel1.volume_gain += (apu_registers->channel1_volume_and_envelope.envelope_direction) ? apu.channel1.volume_step : -apu.channel1.volume_step;
    apu.channel1.volume_gain = (apu.channel1.volume_gain > 0) ? apu.channel1.volume_gain : 0.0f;
    HAL_AudioChannelSetGain(apu.channel1.device, apu.channel1.volume_gain);
}

void
apu_create(emu::gameboy::GameboyMemory *memory)
{
    assert(HAL_AudioDeviceGetState() != HAL_STATE_INACTIVE);
    apu_registers = reinterpret_cast<apu_registers_t>(memory->getIORegisters()->audio);
    apu.length_timer  = HAL_TimerCreate(apu_length_timer, NULL);

    // Channel 1 initialization
    apu.channel1.buffer_size = 16;
    apu.channel1.sample_buffer = (int16_t *)malloc(apu.channel1.buffer_size * sizeof(int16_t));
    apu.channel1.device = HAL_AudioChannelOpen(apu.channel1.buffer_size);
    apu.channel1.period_timer = HAL_TimerCreate(apu_channel1_period_timer_callback, NULL);
    apu.channel1.volume_timer = HAL_TimerCreate(apu_channel1_volume_timer_callback, NULL);
    //memory->registerIOSignaler(apu_process_channel1_update, 0xFF14, UINT16_MAX);

    //apu.channel2_pulse  = HAL_AudioChannelOpen(8);
    //apu.channel3_wave   = HAL_AudioChannelOpen(sizeof(apu_channel3_wave_pattern_ram_struct));
    //apu.channel4_random = HAL_AudioChannelOpen(32);

    HAL_TimerStart(apu.length_timer, HAL_TIMER_START_INTERVAL_NSEC, 3906250);
}

void apu_destroy(void)
{
    require_or(HAL_AudioDeviceGetState() != HAL_STATE_INACTIVE, return);
    HAL_AudioChannelClose(apu.channel1.device);
    free(apu.channel1.sample_buffer);
    apu.channel1.sample_buffer = NULL;
    apu.channel1.buffer_size = 0;
    HAL_TimerStop(apu.channel1.period_timer);
    HAL_TimerDestroy(apu.channel1.period_timer);
    HAL_TimerStop(apu.channel1.volume_timer);
    HAL_TimerDestroy(apu.channel1.volume_timer);
    /// @todo API to clear mmu_memory_watcher

    //HAL_AudioChannelClose(apu.channel2_pulse);
    //HAL_AudioChannelClose(apu.channel3_wave);
    //HAL_AudioChannelClose(apu.channel4_random);
    HAL_TimerStop(apu.length_timer);
    HAL_TimerDestroy(apu.length_timer);
    apu_registers = NULL;
}

void
apu_pulse_generate(uint8_t duty, uint8_t volume, int16_t *buffer, size_t buffer_size)
{
    static uint8_t on_samples[4] = {7, 6, 4, 6};
    uint8_t on_counter = on_samples[duty];

    for (size_t byte = 0; byte < buffer_size; byte++)
    {
        if ((byte & 0x7) == 0)
        {
            on_counter = on_samples[duty];
        }

        uint8_t is_sample_on = false;
        if (on_counter)
        {
            is_sample_on = true;
            on_counter--;
        }

        if (duty == APU_DUTY_CYCLE_QUARTER_N)
        {
            is_sample_on ^= 1;
        }

        buffer[byte] = (is_sample_on) ? HAL_VOLUME_SCALE((uint16_t)volume) : 0;
    }
}
