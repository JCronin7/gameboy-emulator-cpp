#ifndef _CONFIG_H_
#define _CONFIG_H_

#define kSoftwareVersionMajor 0
#define kSoftwareVersionMinor 0

#define kConfigSingleSpeedMode 0
#define kConfigDoubleSpeedMode 1
// A " dot " = one 222 Hz (≅ 4.194 MHz)
#define kConfigDotCycleHz (1 << 22)
#define kConfigMCycleHz(speed) (((speed) == kConfigSingleSpeedMode) ? kConfigDotCycleHz >> 2 : kConfigDotCycleHz >> 1)

#define kConfigStartupCodeFile "bin/DMG_ROM.bin"

// Video Graphics configs
#define kConfigDisplayWindowTitle "gameboy"

#define kConfigBackgroundSizeX 256
#define kConfigBackgroundSizeY 256

#define kConfigScreenPixelsX 160
#define kConfigScreenPixelsY 144

#define xConfigPixelSize 4

#define CONFIG_PIXEL_SIZE 1
#define CONFIG_TILE_SIZE (PIXEL_SIZE << 3)

#define kConfigPixelColorBlack      0
#define kConfigPixelColorDarkGray   85
#define kConfigPixelColorLightGray  171
#define kConfigPixelColorWhite      255

#define kConfigJoypadUp     'W'
#define kConfigJoypadDown   'S'
#define kConfigJoypadLeft   'A'
#define kConfigJoypadRight  'D'

#define kConfigJoypadSelect '\r'
#define kConfigJoypadStart  '1'
#define kConfigJoypadA      'O'
#define kConfigJoypadB      'P'

#define kConfigVBlankInterruptFlag  0x01
#define kConfigLCDInterruptFlag     0x02
#define kConfigTimerInterruptFlag   0x04
#define kConfigSerialInterruptFlag  0x08
#define kConfigJoypadInterruptFlag  0x10

extern uint32_t system_error;

#endif /* _CONFIG_H_ */