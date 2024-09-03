#ifndef _PPU_H_
#define _PPU_H_

#include "hal.h"
#include "mmu.h"
#include "platform.h"

namespace emu::gameboy {
namespace detail::PixelProcessor {
class PixelFetcher
{
private:
    using GameboyMemory = emu::gameboy::GameboyMemory;

public:
    struct config_t
    {
        using tiledata = emu::gameboy::io::ppu::lcd::tiledata;
        using tilemap = emu::gameboy::io::ppu::lcd::tilemap;

        tiledata tiledataId;
        tilemap  tilemapId;
    };
    struct packed(1) pixelInfo
    {
        uint8_t encoding  : 2;
        uint8_t isBgPixel : 1;
        uint8_t objectPalette : 1;
        uint8_t priority  : 1;
    };

public:
    PixelFetcher(std::shared_ptr<GameboyMemory> memory);
    ~PixelFetcher();
    
    void reset(config_t config, point_t point);
    void enterObjectMode(GameboyMemory::objectAttributes *object, uint8_t row);
    circular_queue<pixelInfo, 16> *getBackgroundQueue();
    circular_queue<pixelInfo, 16> *getObjectQueue();
    void update();
    bool inObjectRender();

private:
    uint8_t getTileId(io::ppu::lcd::tilemap tilemapBlock, uint8_t x, uint8_t y);
    uint8_t getTileDataIndex(uint8_t positionY);
    void insertPixels(circular_queue<pixelInfo, 16> *queue, uint8_t priority, uint8_t palette, bool flipX, bool isBg);

private:
    /** Current state machine state */
    enum state: uint8_t
    {
        getTileID,
        getDataLow,
        getDataHigh,
        sleep,
        push,
    };
    /** Background pixel fifo */
    circular_queue<pixelInfo, 16>    _backgroundQueue;
    circular_queue<pixelInfo, 16>    _objectQueue;
    state                            _state;
    point_t                          _currentPixel;
    config_t                         _config;
    GameboyMemory::objectAttributes *_object = nullptr;
    GameboyMemory::videoRAM *        _videoRam = nullptr;
    uint8_t                          _objectRow;
    /** Tile data from VRAM */
    struct
    {
        Word16 data;
        uint8_t tileId;
    } _info;
};
} // namespace detail::PixelProcessor

class PixelProcessor
{
public:
    using tilemap = io::ppu::lcd::tilemap;
    using tiledata = io::ppu::lcd::tiledata;
    using objectSize = io::ppu::lcd::objectSize;
    using lcdMode = io::ppu::lcd::mode;
    using lcdControl = io::ppu::lcd::control;
    using lcdStatus = io::ppu::lcd::status;
    using objectAttributes = GameboyMemory::objectAttributes;

private:
    using PixelFetcher = detail::PixelProcessor::PixelFetcher;

public:
    PixelProcessor();
    PixelProcessor(std::shared_ptr<GameboyMemory> memory);
    ~PixelProcessor();
    
    void reset();
    void configureInterrupts(uint8_t vblankInterruptFlag, uint8_t statInterruptFlag);
    
    void enable();
    void disable();
    void setBackgroundPalette(uint8_t index0, uint8_t index1, uint8_t index2, uint8_t index3);
    void setBGWindowTileDataArea(tiledata block);
    
    uint8_t getCurrentRow() const;

    void mCycleUpdate(uint8_t mCycles);
    
private:
    void sync();

    bool inWindow(uint8_t x, uint8_t y) const;
    bool inWindow(point_t point) const { return inWindow(point.x, point.y); }
    
    HAL_PixelColor colorPaletteToValue(PixelFetcher::pixelInfo pxiel);
    void sortObjectQueue();
    
    void objectAttributeMemorySearchAction();
    void pixelTransferAction();
    void hBlankAction();
    void vBlankAction();

private:
    PixelFetcher                   _pixelFetcher;
    std::shared_ptr<GameboyMemory> _memory = nullptr;
    io::ppu::registers *           _registers = nullptr;
    std::queue<uint8_t>            _backgroundPixelFifo;
    std::queue<uint8_t>            _objectPixelFifo;
    std::optional<uint8_t>         _vblankInterruptFlag = std::nullopt;
    std::optional<uint8_t>         _statInterruptFlag = std::nullopt;
    uint32_t                       _stateMachineCycle = 0;
    uint8_t                        _currentX = 0;
    uint8_t                        _objectsOnLineCount = 0;
    objectAttributes *             _objectsOnLine[10];
};
} // namespace emu::gameboy
#endif /* _PPU_H_ */
