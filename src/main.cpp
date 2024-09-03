#include "apu.h"
#include "cpu.h"
#include "hal.h"
#include "joypad.h"
#include "mmu.h"
#include "ppu.h"
#include "timer.h"
#include "Emulator.hpp"

#include <thread>

using namespace std;
using namespace emu::gameboy;

const char *gameFilename = nullptr;
int         systemExitStatus = 1;

unique_ptr<emu::Emulator> emulator; 

int
main(int argc, char **argv)
{
    emulator = unique_ptr<emu::Emulator>(new emu::Emulator(emu::Emulator::Type::gameboy));
    // create and initialize game window and keyboard function
    HAL_DisplayInit("gameboy", kConfigScreenPixelsX, kConfigScreenPixelsY);
    // Setup exit functions
    atexit([]() { 
        log_debug("Gameboy exiting.");
        if (systemExitStatus)
        {
            //CPU_PrintState(cpu);
            emulator->DumpState();
        }

        // When `HAL_DisplayBackgroundTask` returns, we can teardown the entire system
        //apu_destroy();
        HAL_AudioDeviceTerminate();
        /// @todo can't destroy window off main thread
        //HAL_DisplayTerminate();
        HAL_AudioDeviceTerminate();
    });
    log_error("Emulator version %d.%d", kSoftwareVersionMajor, kSoftwareVersionMinor);

    // create and initialize game window and keyboard function
    HAL_DisplayInit("gameboy", kConfigScreenPixelsX, kConfigScreenPixelsY);

#if 0
    /// @todo meant to enforce system timing, but processing takes too long with gameboy cpu freqency
    HAL_Timer *system_timer = HAL_TimerCreate(system_step, NULL);
    uint64_t system_timer_interval = (1000 * 1000) / start_opts->cpu_frequency;
    HAL_TimerStart(system_timer, HAL_TIMER_START_INTERVAL_NSEC, system_timer_interval);
#endif
    
    // Parse game file from start args
    while (true)
    {
        int opt = 0;
        require_or((opt = getopt(argc, argv, "g:")) != -1, break);
        switch (opt)
        {
        case 'g':
            gameFilename = optarg;
            break;
        }
    }
    require_or(gameFilename, log_error("No game file specified!"); exit(1));
    require_or(emulator->Activate(gameFilename), return 1);
    
    thread emulatorMainLoop([&]() -> void {
        uint32_t audio_device_status = HAL_AudioDeviceInit();
        
        if (audio_device_status != kReturnOK)
        {
            log_error("Failed to start audio device with code %u", audio_device_status);
            exit(0);
        }
        //apu_create(&*memory);

        while (HAL_DisplayGetState() != HAL_STATE_INACTIVE)
        {
            emulator->Run();
        }
    });

    //CPU_PrintAllInstructions();
    HAL_DisplayBackgroundTask(NULL);
#if 0
    HAL_TimerDestroy(system_timer);
#endif
    exit(0);
}