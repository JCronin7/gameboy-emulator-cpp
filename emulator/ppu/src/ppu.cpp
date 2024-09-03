#include "ppu.h"

#define kPPUCyclesOAMSearch 80ul
#define kPPUCyclesPerLine 456ul

using namespace std;
using namespace emu; 
using namespace emu::gameboy;
using namespace emu::gameboy::detail::PixelProcessor;
using namespace emu::gameboy::io::ppu;

PixelFetcher::PixelFetcher(std::shared_ptr<GameboyMemory> memory)
{
    _videoRam = memory->getVideoRAM();
    reset(config_t {
        .tiledataId = lcd::tiledata::block0,
        .tilemapId = lcd::tilemap::block0,
    }, point_t { 0, 0 });
}

PixelFetcher::~PixelFetcher()
{ }

void 
PixelFetcher::reset(config_t config, point_t point)
{
    _state = state::getTileID;
    _config = config;
    _object = NULL;
    _currentPixel = point;
    _backgroundQueue.clear();
    _objectQueue.clear();
}

void 
PixelFetcher::enterObjectMode(GameboyMemory::objectAttributes *object, uint8_t row)
{
    _object = object;
    _objectRow = row;
    _state = state::getDataLow;
}

circular_queue<PixelFetcher::pixelInfo, 16> *
PixelFetcher::getBackgroundQueue()
{
    return &_backgroundQueue;
}

circular_queue<PixelFetcher::pixelInfo, 16> *
PixelFetcher::getObjectQueue()
{
    return &_objectQueue;
}

void 
PixelFetcher::update()
{
    switch (_state)
    {
    case state::getTileID:
        _info.tileId = getTileId(_config.tilemapId, _currentPixel.x, _currentPixel.y);
        _state = state::getDataLow;
        break;
    case state::getDataLow:
    {
        uint8_t index, tile_id;
        uint8_t *tile_bytes;
        if (_object)
        {
            uint8_t tile_row = (_object->flags.flipY) ? 7 - _objectRow : _objectRow;
            index = getTileDataIndex(tile_row);
            assert(index < 16);
            tile_id = _object->tileId;
            tile_bytes = _videoRam->tiledata[0][tile_id];
        }
        else
        {
            index = getTileDataIndex(_currentPixel.y);
            assert(index < 16);
            tile_id = _info.tileId;
            tile_bytes = (_config.tiledataId == lcd::tiledata::block0) ?
                _videoRam->tiledata[2][static_cast<int8_t>(tile_id)] : _videoRam->tiledata[0][tile_id];
        }
        _info.data.lo = tile_bytes[index];
        _info.data.hi = tile_bytes[index + 1];
        _state = state::getDataHigh;
        break;
    }
    // Optimize state machine by getting high and low bytes in the same state.
    case state::getDataHigh:
        _state = state::push;
        break;
#if 0 // Not needed
    case state::sleep:
        _state = state::push;
        break;
#endif
    case state::push:
        if (_object)
        {
            require_or(_objectQueue.size() <= 8, break);
            insertPixels(&_objectQueue, _object->flags.priority, _object->flags.palette, _object->flags.flipX, false);
            _object = NULL;
        }
        else
        {
            require_or(_backgroundQueue.size() <= 8, break);
            insertPixels(&_backgroundQueue, 0, 0, false, true);
            _currentPixel.x += 8;
        }
        _state = state::getTileID;
        break;
    default:
        assert(false);
    }
}

bool 
PixelFetcher::inObjectRender()
{
    return static_cast<bool>(_object);
}

uint8_t 
PixelFetcher::getTileId(lcd::tilemap tilemapBlock, uint8_t x, uint8_t y)
{
    uint8_t idx = static_cast<uint8_t>(tilemapBlock);
    return _videoRam->tilemap[idx][y >> 3][x >> 3];
}

uint8_t 
PixelFetcher::getTileDataIndex(uint8_t positionY)
{
    return (positionY & 0x7) << 1;
}

void 
PixelFetcher::insertPixels(circular_queue<PixelFetcher::pixelInfo, 16> *queue, uint8_t priority, uint8_t palette, bool flipX, bool isBgPixel)
{
    assert(queue);
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t x = (flipX) ? i : 7 - i;
        uint8_t encoding = (get_bit(_info.data.hi, x) << 1) | get_bit(_info.data.lo, x);
        pixelInfo pixel = {
            .encoding = encoding,
            .isBgPixel = isBgPixel,
            .objectPalette = palette,
            .priority = priority
        };
        queue->enqueue(pixel);
    }
}

PixelProcessor::PixelProcessor(shared_ptr<GameboyMemory> memory): _pixelFetcher(memory), _memory(memory)
{
    _registers = &_memory->getIORegisters()->ppu;
    _registers->y.coordinate = 0;
    _registers->lcdStatus.mode = lcd::mode::searchingObjectAttributeMemory;
    sync();
}

PixelProcessor::~PixelProcessor()
{ }

void
PixelProcessor::reset()
{
    PixelFetcher::config_t newline_config = {
        .tiledataId = _registers->lcdControl.backgroundWindowTiledataArea,
        .tilemapId = _registers->lcdControl.backgroundTileMapArea,
    };
    _pixelFetcher.reset(newline_config, point_t { 
        .x = 0, 
        .y = static_cast<uint8_t>(_registers->y.coordinate + _registers->scroll.y) 
    });
    _currentX = 0;
    _stateMachineCycle = 0;

    _registers->y.coordinate = 0;
    _registers->lcdStatus.mode = lcd::mode::searchingObjectAttributeMemory;
    _objectsOnLineCount = 0;
    sync();
}

void
PixelProcessor::configureInterrupts(uint8_t vblankInterruptFlag, uint8_t statInterruptFlag)
{
    _vblankInterruptFlag = vblankInterruptFlag;
    _statInterruptFlag = statInterruptFlag;
}

void
PixelProcessor::enable()
{
    assert(_registers != nullptr);
    _registers->lcdControl.enable = true;
}

void
PixelProcessor::disable()
{
    assert(_registers != nullptr);
    _registers->lcdControl.enable = false;
}

void
PixelProcessor::setBackgroundPalette(uint8_t index0, uint8_t index1, uint8_t index2, uint8_t index3)
{
    assert(index0 <= 3);
    assert(index1 <= 3);
    assert(index2 <= 3);
    assert(index3 <= 3);
    assert(_registers != nullptr);
    _registers->palettes.monochrome = index0 | (index1 << 2) | (index2 << 4) | (index3 << 6);
}

void
PixelProcessor::setBGWindowTileDataArea(lcd::tiledata block)
{
    assert(_registers != nullptr);
    _registers->lcdControl.backgroundWindowTiledataArea = block;
}

uint8_t
PixelProcessor::getCurrentRow() const
{
    assert(_registers != nullptr);
    return _registers->y.coordinate;
}

void 
PixelProcessor::mCycleUpdate(uint8_t mCycles)
{
    /// @todo shouldn't hard-code `4 *` here (based on CPU speed setting)
    uint16_t dotCycles = 4 * mCycles;
    while (dotCycles--)
    {
        require_or(_registers->lcdControl.enable, return);
        _stateMachineCycle++;
        switch (_registers->lcdStatus.mode)
        {
        // OAM Search Stage
        case lcd::mode::searchingObjectAttributeMemory:
            objectAttributeMemorySearchAction();
            break;
        // Pixel transfer stage
        case lcd::mode::transferringToLcd:
            pixelTransferAction();
            break;
        // Horizontal blank stage
        case lcd::mode::hBlank:
            hBlankAction();
            break;
        // Vertical blank stage
        case lcd::mode::vBlank:
            vBlankAction();
            break;
        }
        // Make sure register state is always up to date
        sync();
    }
}

void
PixelProcessor::sync()
{
    /// @todo Not sure if this logic is correct, need to make sure we only generate an interrupt when lyc==ly: 0 -> 1
    bool lycIsTrue = (_registers->y.coordinate == _registers->y.compare);
    /// Generate STAT interrupt logic
    auto &status = _registers->lcdStatus;
    if ((status.lycInterruptMode && lycIsTrue && (status.lycEqualsLY == false))                 ||
        (status.oamInterruptMode && (status.mode == lcd::mode::searchingObjectAttributeMemory)) ||
        (status.vblankInterruptMode && (status.mode == lcd::mode::vBlank))                      ||
        (status.hblankInterruptMode && (status.mode == lcd::mode::hBlank)))
    {
        assert(_statInterruptFlag != nullopt);
        _memory->requestInterrupt(*_statInterruptFlag);
    }
    status.lycEqualsLY = lycIsTrue;
}

bool
PixelProcessor::inWindow(uint8_t x, uint8_t y) const
{
    assert(_registers);
    /// @todo replace CONFIG_VISUAL_SCREEN_SIZE_ with pixel buffer size
    return in_range(x, _registers->window.x, _registers->window.x + kConfigScreenPixelsX) &&
           in_range(x, _registers->window.y, _registers->window.y + kConfigScreenPixelsY);
}

HAL_PixelColor
PixelProcessor::colorPaletteToValue(PixelFetcher::pixelInfo pixel)
{
    static const HAL_PixelColor values[] = {
        HAL_LCD_PIXEL_COLOR_WHITE,
        HAL_LCD_PIXEL_COLOR_LIGHT_GRAY,
        HAL_LCD_PIXEL_COLOR_DARK_GRAY,
        HAL_LCD_PIXEL_COLOR_BLACK
    };
    assert(_registers);
    assert(pixel.encoding <= 3);
    palettes &pal = _registers->palettes;
    uint8_t color_palette = (pixel.isBgPixel) ? pal.monochrome : pal.object[pixel.objectPalette];
    return values[0x3 & (color_palette >> (2 * pixel.encoding))];
}

void
PixelProcessor::sortObjectQueue(void)
{
    for (uint8_t i = 0; i < _objectsOnLineCount - 1; i++)
    {
        uint8_t maxIdx = i;
        for (uint8_t j = i + 1; j < _objectsOnLineCount; j++)
        {
            require_or(_objectsOnLine[j]->position.x > _objectsOnLine[maxIdx]->position.x, continue);
            maxIdx = j;
        }
        require_or(maxIdx != i, continue);
        // only works if objectAttributes is 4 bytes
        static_assert(sizeof(*_objectsOnLine[0]) == 4);
        swap(*reinterpret_cast<uint32_t *>(_objectsOnLine[maxIdx]), *reinterpret_cast<uint32_t *>(_objectsOnLine[i]));
    }
}

void
PixelProcessor::objectAttributeMemorySearchAction()
{
    assert(_registers);
    assert(_registers->lcdStatus.mode == lcd::mode::searchingObjectAttributeMemory);
    if (_stateMachineCycle & 1)
    {
        uint8_t index = _stateMachineCycle >> 1;
        
        auto attr = (*_memory->getObjectAttributeMemory())[_stateMachineCycle >> 1];
        const uint8_t objectHeight = (_registers->lcdControl.objectSize == lcd::objectSize::standard) ? 8 : 16;
        if ((attr.position.x != 0) &&
            in_range(_registers->y.coordinate + 16, attr.position.y, attr.position.y + objectHeight))
        {
            _objectsOnLine[_objectsOnLineCount++] = &attr;
        }
    }
    if (_stateMachineCycle >= kPPUCyclesOAMSearch)
    {
        PixelFetcher::config_t newline_config = {
            .tiledataId = _registers->lcdControl.backgroundWindowTiledataArea,
            .tilemapId = _registers->lcdControl.backgroundTileMapArea,
        };
        _pixelFetcher.reset(newline_config, point_t { 
            .x = 0, 
            .y = static_cast<uint8_t>(_registers->y.coordinate + _registers->scroll.y) 
        });
        _registers->lcdStatus.mode = lcd::mode::transferringToLcd;
        sortObjectQueue();
    }
}

void
PixelProcessor::pixelTransferAction(void)
{
    assert(_registers && _registers->lcdStatus.mode == lcd::mode::transferringToLcd);
    const uint16_t max_pixel_x = HAL_DisplayGetSizeX();

    _pixelFetcher.update();

    auto bgQueue = _pixelFetcher.getBackgroundQueue();
    require_or(bgQueue->size() >= 8, return);
    require_or(_pixelFetcher.inObjectRender() == false, return);

    const uint8_t object_index = _objectsOnLineCount - 1;
    if ((_objectsOnLineCount > 0) &&
        (_currentX + 8) == _objectsOnLine[object_index]->position.x)
    {
        uint8_t object_row = _registers->y.coordinate - _objectsOnLine[object_index]->position.y;
        _pixelFetcher.enterObjectMode(_objectsOnLine[object_index], object_row);
        _objectsOnLineCount--;
        return;
    }

    HAL_DisplayReserve();
    auto pixel = bgQueue->dequeue();
    auto objQueue = _pixelFetcher.getObjectQueue();
    if (objQueue->size() > 0)
    {
        auto obejct_pixel = objQueue->dequeue();
        if (obejct_pixel.encoding && obejct_pixel.priority == 0)
        {
            pixel = obejct_pixel;
        }
    }

    HAL_DisplaySetPixel(_currentX, _registers->y.coordinate, colorPaletteToValue(pixel));
    _currentX++;
    HAL_DisplayRelease();

    if (_currentX == max_pixel_x)
    {
        _currentX = 0;
        _objectsOnLineCount = 0;
        HAL_DisplaySetReady(true);
        _registers->lcdStatus.mode = lcd::mode::hBlank;
    }
}

void
PixelProcessor::hBlankAction()
{
    assert(_registers && _registers->lcdStatus.mode == lcd::mode::hBlank);
    require_or(_stateMachineCycle >= kPPUCyclesPerLine, return);
    _stateMachineCycle = 0;
    _currentX = 0;

    _registers->y.coordinate++;
    if (HAL_DisplayGetSizeY() == _registers->y.coordinate)
    {
        _registers->lcdStatus.mode = lcd::mode::vBlank;
        assert(_vblankInterruptFlag != nullopt);
        _memory->requestInterrupt(*_vblankInterruptFlag);
    }
    else
    {
        _registers->lcdStatus.mode = lcd::mode::searchingObjectAttributeMemory;
        _objectsOnLineCount = 0;
    }
}

void
PixelProcessor::vBlankAction()
{
    assert(_registers && _registers->lcdStatus.mode == lcd::mode::vBlank);
    require_or(_stateMachineCycle >= kPPUCyclesPerLine, return);
    _stateMachineCycle = 0;
    _registers->y.coordinate++;
    if ((HAL_DisplayGetSizeY() + 10) == _registers->y.coordinate)
    {
        _registers->y.coordinate = 0;
        _registers->lcdStatus.mode = lcd::mode::searchingObjectAttributeMemory;
        _objectsOnLineCount = 0;
    }
}
