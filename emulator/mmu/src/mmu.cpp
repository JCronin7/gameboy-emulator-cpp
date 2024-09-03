#include "mmu.h"

using namespace std;
using namespace emu::gameboy;
using namespace emu::gameboy::detail::GameboyMemory;

GameboyMemory::~GameboyMemory()
{
    _ioSignalerList.clear();
    if (_memory)
    {
        delete _memory;
    }
}

bool
GameboyMemory::activate(const char *gamefile)
{
    require_or(gamefile != nullptr, return false);
    assert(_memory = new memory);

    require_or(loadFile(gamefile, kMemoryMapSize), return false);
    // Write ROM file onto the first 256 bytes of memory
    require_or(loadFile(kDMGROMFilename, 0x100), return false);
    
    _gamefile = gamefile;
    registerIOSignaler([&](uint16_t address, optional<uint8_t> value) {
        require_or(value != nullopt, return);
        _dma.inProgress = true;
        _dma.sourceAddress = static_cast<uint16_t>(*value) << 8;
        _dma.count = 0;
    }, DirectMemoryAccess::kStartAddress, UINT16_MAX);
    registerIOSignaler([&](uint16_t address, optional<uint8_t> value) {
        require_or(value != nullopt, return);
        /// @todo Shouldn't assume file is `kMemoryMapSize` bytes
        loadFile(_gamefile, kMemoryMapSize);
        memset(_memory->layout.objectAttributeMemory, 0, sizeof(_memory->layout.objectAttributeMemory));
    }, 0xFF50, UINT16_MAX);

    return true;
}

void
GameboyMemory::read(uint16_t address, uint8_t * const bufferOut, uint16_t nbytes) const
{
    assert((static_cast<uint32_t>(nbytes) + address) <= kMemoryMapSize);
    memcpy(bufferOut, _memory->raw + address, nbytes);
}

void
GameboyMemory::write(uint16_t address, const uint8_t * const buffer, uint16_t nbytes)
{
    assert((static_cast<uint32_t>(nbytes) + address) <= kMemoryMapSize);
    memcpy(&_memory->raw[address], buffer, nbytes);
}

void
GameboyMemory::mCycleUpdate(uint8_t cycles)
{
    if (_dma.inProgress)
    {
        static constexpr uint8_t kLoopEndValue = DirectMemoryAccess::kDurationMCycles;
        assert(_dma.count < kLoopEndValue);
        for (; _dma.count < emu::min(kLoopEndValue, cycles + _dma.count); _dma.count++)
        {
            uint16_t dst = DirectMemoryAccess::kDestinationAddress + _dma.count;
            uint16_t src = _dma.sourceAddress + _dma.count;
            _memory->raw[dst] = _memory->raw[src]; 
        }
        if (_dma.count == kLoopEndValue)
        {
            _dma.inProgress = false;
            for (auto attr: _memory->layout.objectAttributeMemory)
            {
                printf("attr.position.y = %x\n", attr.position.y);
                printf("attr.position.x = %x\n", attr.position.x);
                printf("attr.tileId = %x\n", attr.tileId);
                printf("attr.flags.palette = %x\n", attr.flags.palette);
                printf("attr.flags.flipX = %x\n", attr.flags.flipX);
                printf("attr.flags.flipY = %x\n", attr.flags.flipY);
                printf("attr.flags.priority = %x\n", attr.flags.priority);
            }
        }
    }
}

void
GameboyMemory::dumpToFile()
{
    time_t ltime = time(NULL);
    char buffer[64] = { '\0' };
    snprintf(buffer, 64, "bin/memory_%sbin", asctime(localtime(&ltime)));
    int i = 0;
    // Replace bad characters in filename
    while (buffer[i] != '\0')
    {
        if (buffer[i] == ' ')
        {
            buffer[i] = '_';
        }
        if (buffer[i] == '\n')
        {
            buffer[i] = '.';
        }
        i++;
    }
    FILE *file = fopen(buffer, "w");
    size_t bytes_written = fwrite(&_memory->raw, 1, kMemoryMapSize, file);
    log_debug("Wrote 0x%08x bytes to file.", (uint32_t)bytes_written);
    fclose(file);
}

uint8_t
GameboyMemory::registerIOSignaler(IOSignaler::CallbackFn callback, uint16_t compare, uint16_t mask)
{
    IOSignaler info = {
        .callback = std::move(callback),
        .compare = compare,
        .mask = mask,
        .id = static_cast<uint8_t>(_ioSignalerList.size()),
    };
    IOSignaler tmp = info;
    _ioSignalerList.insert(_ioSignalerList.end(), info);
    return info.id;
}

void 
GameboyMemory::deregisterIOSignaler(uint8_t id)
{
    static auto remove_predicate = [id](const IOSignaler &info) -> bool {
        return info.id == id;
    };
    _ioSignalerList.remove_if(remove_predicate);
}

void
GameboyMemory::requestInterrupt(uint8_t interruptFlag)
{
    _memory->layout.ioRegisters.interruptFlag |= interruptFlag;
}

void
GameboyMemory::print(uint16_t offset, uint16_t length)
{
    print(_memory->raw, offset, length);
}

/* static */
void
GameboyMemory::print(uint8_t *buffer, uint16_t offset, uint16_t length)
{
    static constexpr auto header = "\t\t  00   01   02   03   04   05   06   07   08   09   0A   0B   0C   0D   0E   0F\n";
    static constexpr auto printBytes = [](uint8_t *data, size_t size) {
        for (int i = 0; i < size; i++)
        {
            printf("0x%02x ", data[i]);
        }
        printf("\n");
    };
    uint16_t row = 0;
    printf(header);
    while (row + 16 <= length)
    {
        printf("0x%04x: ", row);
        printBytes(&buffer[row], 16);
        row += 16;
    }
    require_or(length > row, return);
    printf("0x%04x: ", row);
    printBytes(&buffer[row], length - row);
}

void 
GameboyMemory::signal(uint16_t address, std::optional<uint8_t> writeValue) const
{
    for (auto &itr : _ioSignalerList)
    {
        require_or(itr.compare == (address & itr.mask), continue);
        itr.callback(address, writeValue);
    }
}

bool
GameboyMemory::loadFile(const char *filename, size_t size)
{
    assert(_memory);
    log_debug("Loading game file \"%s\"", filename);
    
    FILE *stream = fopen(filename, "rb");
    if (stream == NULL)
    {
        fprintf(stderr, "Failed to open file! (%s)\n", strerror(errno));
        return false;
    }

    size_t bytesRead = fread(_memory->raw, sizeof(uint8_t), size, stream);
    fclose(stream);
    if (bytesRead == 0)
    {
        fprintf(stderr, "Failed to initialize file! (%s)\n", strerror(errno));
        return false;
    }
    // clear unset memory
    memset(_memory->raw + bytesRead, 0, size - bytesRead);
    return true;
}
