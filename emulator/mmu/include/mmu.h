#ifndef _MMU_H_
#define _MMU_H_

#include "platform.h"

#include <list>

#define kMemoryMapSize (0x10000)
#define kMemoryMap4KB  (4U << 10)
#define kMemoryMap8KB  (8U << 10)
#define kMemoryMap16KB (16U << 10)

namespace emu::gameboy {
namespace detail::GameboyMemory {
struct romBank00
{
    uint64_t restartVectors[0x8];
    uint64_t verticalBlankInterrupt;
    uint64_t lcdcStatusInterrupt;
    uint64_t timerOverflowInterrupt;
    uint64_t serialTransferCompleteInterrupt;
    uint16_t p1xInterrupt[4];
    uint8_t  unsued[0x98];
    uint16_t code_begin[0x2];
    uint8_t  nintendo_graphic[0x30];
    char     gameTitle[0xF];
    uint8_t  colorGameboy;
    uint8_t  licenseCode[2];
    uint8_t  xgbIndicator;
    uint8_t  cartridgeType;
    uint8_t  romSize;
    uint8_t  ramSize;
    uint8_t  language;
    uint8_t  license_code_old;
    uint8_t  mask_rom_version_num;
    uint8_t  complement_check;
    uint16_t checksum;
    uint8_t  data[0x3EB0];
};
static_assert(sizeof(romBank00) == kMemoryMap16KB);

struct romBank01
{
    uint8_t region[kMemoryMap16KB];
};
static_assert(sizeof(romBank01) == kMemoryMap16KB);

struct videoRAM
{
    // Array of 16-byte buffers (x3)
    uint8_t tiledata[3][0x800 >> 4][0x10];
    // 32 x 32 tilemaps
    uint8_t tilemap[2][0x20][0x20];
};
static_assert(sizeof(videoRAM) == kMemoryMap8KB);

struct externalRAM
{
    uint8_t region[kMemoryMap8KB];
};
static_assert(sizeof(externalRAM) == kMemoryMap8KB);

struct workRAM00
{
    uint8_t region[kMemoryMap4KB];
};
static_assert(sizeof(workRAM00) == kMemoryMap4KB);

struct workRAM01
{
    uint8_t region[kMemoryMap4KB];
};
static_assert(sizeof(workRAM01) == kMemoryMap4KB);

struct workRamMirror
{
    uint8_t region[0x1E00];
};
static_assert(sizeof(workRamMirror) == 0x1E00);

struct objectAttributes
{
    point_t position;
    uint8_t tileId;
    struct
    {
#if 0 /* GBC macro */
        uint8_t palette : 3;
        uint8_t bank    : 1;
        uint8_t         : 1;
#else
        uint8_t         : 4;
        uint8_t palette : 1;
#endif
        uint8_t flipX   : 1;
        uint8_t flipY   : 1;
        uint8_t priority: 1;
    } flags;
};
static_assert(sizeof(objectAttributes) == 4);

struct highRAM
{
    uint8_t region[0x7F];
};
static_assert(sizeof(highRAM) == 0x7F);

struct interruptEnable
{
    uint8_t value;
};
static_assert(sizeof(interruptEnable) == 0x1);

struct DirectMemoryAccess
{
    static constexpr uint8_t  kDurationMCycles = 160;
    static constexpr uint16_t kDestinationAddress = 0xFE00;
    static constexpr uint8_t  kBufferLength = 0x9F;
    static constexpr uint16_t kStartAddress = 0xFF46;

    uint16_t sourceAddress = 0;
    uint8_t  count = 0;
    bool     inProgress = false;
};
} // namespace detail::GameboyMemory

namespace io {
namespace ctrl {
union packed(1) controlBase
{
    uint8_t base;
    struct packed(1) {
        uint8_t buttons               : 4;
        const uint8_t directionSelect : 1;
        const uint8_t actionSelect    : 1;
    };
};
union packed(1) controlDirectionInputs
{
    uint8_t base;
    struct packed(1) {
        uint8_t right                 : 1;
        uint8_t left                  : 1;
        uint8_t up                    : 1;
        uint8_t down                  : 1;
    };
};
static_assert(sizeof(controlDirectionInputs) == 1);
union packed(1) controlActionInputs
{
    uint8_t base;
    struct packed(1) {
        uint8_t a                     : 1;
        uint8_t b                     : 1;
        uint8_t select                : 1;
        uint8_t start                 : 1;
    };
};
static_assert(sizeof(controlActionInputs) == 1);
} // namespace ctrl
namespace ppu {
namespace lcd {
enum class tilemap : uint8_t
{
    /// @brief Block 0x9800 to 0x9BFF
    block0 = 0,
    /// @brief Block 0x9C00 to 0x9FFF
    block1 = 1,
};
enum class tiledata : uint8_t
{
    /// @brief Block 0x8800 to 0x97FF
    block0 = 0,
    /// @brief Block 0x8000 to 0x8FFF
    block1 = 1,
};
enum class objectSize : uint8_t
{
    /// @brief Regular sized object (8x8 pixel)
    standard = 0,
    /// @brief Tall sized object (8x16 pixel)
    tall = 1,
};
/// @brief See `emu::gameboy::detail::PixelProcessor::lcdMode`
enum class mode : uint8_t
{
    hBlank = 0,
    vBlank = 1,
    searchingObjectAttributeMemory = 2,
    transferringToLcd = 3,
};
/// @brief LCDC: LCD control (0xFF40)
struct packed(1) control
{
    uint8_t backgroundWindowEnable : 1;
    uint8_t objectEnable : 1;
    objectSize objectSize : 1;
    tilemap backgroundTileMapArea : 1;
    tiledata backgroundWindowTiledataArea : 1;
    uint8_t windowEnable : 1;
    tilemap windowTilemapArea : 1;
    uint8_t enable : 1;
};
static_assert(sizeof(control) == 1);
/// @brief LCD Status register (0xFF41)
struct status
{
    mode    mode                : 2;
    uint8_t lycEqualsLY         : 1;
    uint8_t hblankInterruptMode : 1;
    uint8_t vblankInterruptMode : 1;
    uint8_t oamInterruptMode    : 1;
    uint8_t lycInterruptMode    : 1;
};
static_assert(sizeof(status) == 1);
namespace y {
/// @brief LY:  LCD Y coordinate (0xFF44)
///        LYC: LY compare (0xFF45)
struct packed(1) status
{
    uint8_t coordinate;
    uint8_t compare;
};
static_assert(sizeof(status) == 2);
} // namespace y
} // namespace lcd
/// @brief Scroll Y/X registers (0xFF42-0xFF43)
using scroll = point_t;

/// @brief DMA: OAM DMA source address & start (0xFF46)
struct packed(1) dma
{
    uint8_t sourceAddress; // 0xXX00 - 0xXX9F
};
static_assert(sizeof(dma) == 1);
/// @brief BGP (Non-CGB Mode only): BG palette data (0xFF47)
///        OBP0, OBP1 (Non-CGB Mode only): OBJ palette 0, 1 data (0xFF48–0xFF49)
struct palettes
{
    uint8_t monochrome;
    uint8_t object[2];
};
/// @brief WY: Window Y (0xFF4A)
///        WX: Window X (FF4B)
struct packed(1) window
{
    uint8_t y;
    uint8_t x;
};
struct packed(1) registers
{
    lcd::control     lcdControl;
    lcd::status      lcdStatus;
    scroll           scroll;
    lcd::y::status   y;
    dma              dmaStart;
    palettes         palettes;
    window           window;
};
static_assert(sizeof(registers) == 0x0C);
} // namespace ppu
struct packed(1) registers
{
    uint8_t ctrl;
    /// @todo Confirm this is right.. 
    uint8_t serialTransferData;
    uint8_t serialTransferControl;
    uint8_t : 8;
    struct timerRegisters
    {
        uint8_t divider;
        uint8_t counter;
        uint8_t modulo;
        struct
        {
            uint8_t clockSelect : 2;
            uint8_t enable      : 1;
            uint8_t             : 5;
        } control;
        uint8_t rsvd[7];
    };
    static_assert(sizeof(timerRegisters) == 0xB);
    timerRegisters timer;
    uint8_t interruptFlag;
    uint8_t audio[0x20];
    uint8_t wave[0x10];
    ppu::registers ppu;
    uint8_t rsvd[3];
    uint8_t vramSelect;
    uint8_t bootROMDisable;
    uint8_t vramDMA[0x17];
    uint8_t bgObjectPalettes[0x8];
    uint8_t wramSelect[0x10];
};
static_assert(sizeof(registers) == 0x80);
} // namespace io

class GameboyMemory
{
public:
    using romBank00 = detail::GameboyMemory::romBank00;
    using romBank01 = detail::GameboyMemory::romBank01;
    using videoRAM = detail::GameboyMemory::videoRAM;
    using externalRAM = detail::GameboyMemory::externalRAM;
    using workRAM00 = detail::GameboyMemory::workRAM00;
    using workRAM01 = detail::GameboyMemory::workRAM01;
    using workRamMirror = detail::GameboyMemory::workRamMirror;
    using objectAttributes = detail::GameboyMemory::objectAttributes;
    using highRAM = detail::GameboyMemory::highRAM;
    using interruptEnable = detail::GameboyMemory::interruptEnable;
    
    using DirectMemoryAccess = detail::GameboyMemory::DirectMemoryAccess;

private:
    static constexpr const char *kDMGROMFilename = "/Users/croninj/Personal/gameboy-emulator-cpp/bin/DMG_ROM.bin";
    
private:
    struct IOSignaler
    {
        using CallbackFn = std::function<void(uint16_t, std::optional<uint8_t>)>;
        CallbackFn  callback = nullptr;
        uint16_t    compare;
        uint16_t    mask;
        uint8_t    id; 
    };
    
public:
    GameboyMemory() = default;
    ~GameboyMemory();
    
    bool activate(const char *gamefile);
    
    /**
     * @brief Read a segment of memory from the game data buffer. 
     * 
     * @param[in] address The base address to read from (big endian?).
     * @returns The data located at the address given.
     */
    template <typename T = uint8_t, typename = std::enable_if_t<std::is_integral<T>::value>> 
    T read(uint16_t address) const
    {
        const uint8_t *byte_ptr = &_memory->raw[address];
        if (in_range(address, 0xFF00, 0xFF80))
        {
            signal(address);
        }
        return *reinterpret_cast<const T *>(byte_ptr);
    }
    /**
     * @brief Read a buffer from memory.
     * 
     * @param[in] address The base address to write to. 
     * @param[out] bufferOut Buffer to write memory data to. 
     * @param[in] nbytes Number of bytes to read
     */
    void read(uint16_t address, uint8_t * const bufferOut, uint16_t nbytes) const;

    template <typename T = uint8_t, typename = std::enable_if_t<std::is_integral<T>::value>> 
    void write(uint16_t address, T value)
    {
        *reinterpret_cast<T *>(&_memory->raw[address]) = value; 
        if (in_range(address, 0xFF00, 0xFF80))
        {
            signal(address, static_cast<uint8_t>(value));
        }
    }
    void write(uint16_t address, const uint8_t * const buffer, uint16_t nbytes);
    
    void mCycleUpdate(uint8_t cycles);
    void dumpToFile();
    
    /// @todo How to clear this? 
    uint8_t registerIOSignaler(IOSignaler::CallbackFn callback, uint16_t compare, uint16_t mask);
    void deregisterIOSignaler(uint8_t id);
    void requestInterrupt(uint8_t interruptFlag);
    
    auto getROMBank00() { return &_memory->layout.romBank00; }
    auto getROMBank01() { return &_memory->layout.romBank01; }
    auto getVideoRAM() { return &_memory->layout.videoRam; }
    auto getExternalRAM() { return &_memory->layout.externalRam; }
    auto getWorkRAM00() { return &_memory->layout.workRam00; }
    auto getWorkRAM01() { return &_memory->layout.workRam01; }
    auto getWorkRamMirror() { return &_memory->layout.workRamMirror; }
    auto getObjectAttributeMemory() { return &_memory->layout.objectAttributeMemory; }
    auto getIORegisters() { return &_memory->layout.ioRegisters; }
    auto getHighRAM() { return &_memory->layout.highRAM; }
    auto getInterruptEnable() { return &_memory->layout.interruptEnable; }
    
    void print(uint16_t offset, uint16_t length);
    
    static void print(uint8_t *buffer, uint16_t offset, uint16_t length);
    
private:
    void signal(uint16_t address, std::optional<uint8_t> writeValue = std::nullopt) const;
    bool loadFile(const char *filename, size_t size);

private:
    union memory
    {
        struct memoryLayout
        {
            romBank00        romBank00;
            romBank01        romBank01;
            videoRAM         videoRam;
            externalRAM      externalRam;
            workRAM00        workRam00;
            workRAM01        workRam01;
            workRamMirror    workRamMirror;
            objectAttributes objectAttributeMemory[40];
            uint8_t          rsvd[0x60];
            io::registers    ioRegisters;
            highRAM          highRAM;
            interruptEnable  interruptEnable;
        } layout;
        static_assert(sizeof(memoryLayout) == kMemoryMapSize);
        uint8_t raw[kMemoryMapSize];
    };
    memory *              _memory = nullptr;
    DirectMemoryAccess    _dma;
    std::list<IOSignaler> _ioSignalerList;
    const char *          _gamefile = nullptr;
    bool                  _activated = false;
};
} // namespace emu::gameboy

/// @todo Move this into ioRegisters struct (need to confirm offset)
typedef struct mmu_timer
{
    uint8_t counter;
    uint8_t modulo;
    uint8_t control;
} packed(1) mmu_timer;

#endif /* _MMU_H_ */