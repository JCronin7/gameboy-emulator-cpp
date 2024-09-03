#ifndef _HAL_H_
#define _HAL_H_

#include "platform.h"

#define kHalWindowScaleFactor   (4ul)
#define kHalWindowDelayMs60FPS  (17)
#define kHalNSecToMSec          (1000ull * 1000ull)
#define kHalAudioMinPitch       (0.01)
#define kHalAudioMinGain        (0.01)

#define HAL_VOLUME_SCALE(var)   ((var) << 8)

/**
 * @brief State enum for hardware abstraction layer (HAL) components.
 *
 */
typedef enum HAL_State
{
    HAL_STATE_INACTIVE = 0,
    HAL_STATE_CREATED,
    HAL_STATE_IDLE,
    HAL_STATE_RUNNING,
} HAL_State;

/**
 * @brief Pixel color mapping.
 *
 */
typedef enum HAL_PixelColor
{
    HAL_LCD_PIXEL_COLOR_BLACK       = kConfigPixelColorBlack,
    HAL_LCD_PIXEL_COLOR_DARK_GRAY   = kConfigPixelColorDarkGray,
    HAL_LCD_PIXEL_COLOR_LIGHT_GRAY  = kConfigPixelColorLightGray,
    HAL_LCD_PIXEL_COLOR_WHITE       = kConfigPixelColorWhite,
} HAL_PixelColor;

/** @brief keyboard event callback function type. */
using HAL_KeyboardEventPtr = std::function<void(char, int, int)>;

/** @brief Pixel buffer used by emulator to set pixels, then read by window driver. */
typedef struct HAL_PixelBuffer
{
    pthread_mutex_t lock;       /// Lock primative
    uint8_t         **buffer;   /// buffer pointer
    uint16_t        sizeX;      /// horizontal size
    uint16_t        sizeY;      /// vertical size
    HAL_State       state;      /// current state
} HAL_PixelBuffer;

/**
 * @brief Create a new pixel buffer struct.
 *
 * @param[in] sizeX Horizontal size of the pixel buffer.
 * @param[in] sizeY Vertical size of the pixel buffer.
 * @return HAL_PixelBuffer *New pixel buffer.
 */
HAL_PixelBuffer *HAL_PixelBufferCreate(uint16_t sizeX, uint16_t sizeY);
/**
 * @brief Destroys a pixel buffer struct.
 *
 * @param[in] pixelBuffer Pixel buffer struct to destroy.
 */
void HAL_PixelBufferDestroy(HAL_PixelBuffer *pixelBuffer);
/**
 * @brief Locks the pixel buffer, granting exclusive read/write access to pixels.
 *
 * @param[in] pixelBuffer Pixel buffer object.
 */
void HAL_PixelBufferReserve(HAL_PixelBuffer *pixelBuffer);
/**
 * @brief Unlocks the pixel buffer.
 *
 * @param[in] pixelBuffer Pixel buffer object.
 */
void HAL_PixelBufferRelease(HAL_PixelBuffer *pixelBuffer);
/**
 * @brief Write a pixel in the pixel buffer.
 *
 * @param[in] pixelBuffer Pixel buffer object.
 * @param[in] positionX Horizontal position of the pixel
 * @param[in] positionY Vertical position of the pixel
 * @param[in] brightness brightness of the pixel (0-255)
 * @return uint32_t Return code, 0 on success
 */
uint32_t HAL_PixelBufferSetPixel(HAL_PixelBuffer *pixelBuffer, uint8_t positionX, uint8_t positionY,
                                 uint8_t brightness);
/**
 * @brief Read a pixel in the pixel buffer
 *
 * @param[in] pixelBuffer Pixel buffer object.
 * @param[in] positionX Horizontal position of the pixel
 * @param[in] positionY Vertical position of the pixel
 * @param[out] brightness brightness of the pixel (0-255)
 * @return uint32_t Return code, 0 on success
 */
uint32_t HAL_PixelBufferGetPixel(HAL_PixelBuffer *pixelBuffer, uint8_t positionX, uint8_t positionY,
                                 uint8_t *brightness);
/**
 * @brief Get the pixel buffer horizontal size.
 *
 * @param[in] pixelBuffer Pixel buffer object.
 * @return uint16_t Pixel buffer horizontal size, 0 if not initialized.
 */
uint16_t HAL_PixelBufferGetSizeX(HAL_PixelBuffer *pixelBuffer);
/**
 * @brief Get the pixel buffer vertical size.
 *
 * @param[in] pixelBuffer Pixel buffer object.
 * @return uint16_t Pixel buffer vertical size, 0 if not initialized.
 */
uint16_t HAL_PixelBufferGetSizeY(HAL_PixelBuffer *pixelBuffer);
/**
 * @brief Get the state of the pixel buffer.
 *
 * @param[in] pixelBuffer Pixel buffer object.
 * @return HAL_State Pixel buffer state.
 */
HAL_State HAL_PixelBufferGetState(HAL_PixelBuffer *pixelBuffer);
/**
 * @brief Marks the pixel buffer as running.
 *
 * @param[in] pixelBuffer Pixel buffer object.
 * @param[in] ready Bool indicating if the pixel buffer is running.
 */
uint32_t HAL_PixelBufferSetReady(HAL_PixelBuffer *pixelBuffer, bool ready);

/**
 * @brief Initialize the display driver.
 *
 * @param[in] windowName Name of the created window.
 * @param[in] sizeX Horizontal size of the window.
 * @param[in] sizeY Vertical size of the window.
 * @return uint32_t Return code, 0 on success.
 */
uint32_t HAL_DisplayInit(const char *windowName, uint16_t sizeX, uint16_t sizeY);
/**
 * @brief Terminates the display driver
 *
 */
void HAL_DisplayTerminate(void);
/**
 * @brief Set the keyboard event callback
 *
 * @param[in] callback Keyboard event callback
 */
void HAL_DisplaySetKeyboardEventCallback(HAL_KeyboardEventPtr callback);
/**
 * @brief Display background operation, this function blocks.
 *
 * @param[in] args Task arguments, not used.
 * @return void * Return is always NULL.
 */
void *HAL_DisplayBackgroundTask(void *args);
/**
 * @brief Start the display background thread.
 *
 * @return uint32_t Return code, 0 on success.
 */
uint32_t HAL_DisplayStart(void);
/**
 * @brief Join the display background thread.
 *
 * @return uint32_t Return code, 0 on success.
 */
uint32_t HAL_DisplayJoin(void);
/**
 * @brief Cancel the display background thread.
 *
 * @return uint32_t Return code, 0 on success.
 */
uint32_t HAL_DisplayCancel(void);
/**
 * @brief Get the current state of the display.
 *
 * @return HAL_State State of the display
 */
HAL_State HAL_DisplayGetState(void);
/**
 * @brief Locks the pixel buffer, granting exclusive read/write access to pixels.
 *
 */
void HAL_DisplayReserve(void);
/**
 * @brief Unlocks the pixel buffer.
 *
 */
void HAL_DisplayRelease(void);
/**
 * @brief Write a pixel in the pixel buffer.
 *
 * @param[in] positionX Horizontal position of the pixel
 * @param[in] positionY Vertical position of the pixel
 * @param[in] brightness brightness of the pixel (0-255)
 * @return uint32_t Return code, 0 on success
 */
uint32_t HAL_DisplaySetPixel(uint8_t positionX, uint8_t positionY, uint8_t brightness);
/**
 * @brief Read a pixel in the pixel buffer
 *
 * @param[in] positionX Horizontal position of the pixel
 * @param[in] positionY Vertical position of the pixel
 * @param[out] brightness brightness of the pixel (0-255)
 * @return uint32_t Return code, 0 on success
 */
uint32_t HAL_DisplayGetPixel(uint8_t positionX, uint8_t positionY, uint8_t *brightness);
/**
 * @brief Get the pixel buffer horizontal size.
 *
 * @return uint16_t Pixel buffer horizontal size, 0 if not initialized.
 */
uint16_t HAL_DisplayGetSizeX(void);
/**
 * @brief Get the pixel buffer vertical size.
 *
 * @return uint16_t Pixel buffer vertical size, 0 if not initialized.
 */
uint16_t HAL_DisplayGetSizeY(void);
/**
 * @brief Get the state of the pixel buffer.
 *
 * @return HAL_State Pixel buffer state.
 */
HAL_State HAL_DisplayGetState(void);
/**
 * @brief Marks the pixel buffer as running.
 *
 * @param[in] ready Bool indicating if the pixel buffer is running.
 */
void HAL_DisplaySetReady(bool ready);

/** @brief Timer callback type. */
typedef void (*HAL_TimerCallbackPtr)(void *);
/** @brief Public Timer type. */
typedef struct HAL_Timer { } HAL_Timer;
/** @brief Timer start options */
typedef enum HAL_TimerStartOpts
{
    HAL_TIMER_START_VALUE_NSEC = 0,
    HAL_TIMER_START_VALUE_SEC,
    HAL_TIMER_START_INTERVAL_NSEC,
    HAL_TIMER_START_INTERVAL_SEC,
} HAL_TimerStartOpts;

/**
 * @brief Create a new timer object.
 *
 * @param[in] callback Callback function for when the timer expires.
 * @param[in] args Arguments to callback function
 * @return HAL_Timer *
 */
HAL_Timer *HAL_TimerCreate(HAL_TimerCallbackPtr callback, void *args);
/**
 * @brief Destroys a timer object.
 *
 * @param[in] timer Timer to destroy.
 */
void HAL_TimerDestroy(HAL_Timer *timer);
/**
 * @brief Start a timer, using one of the re-arm options given.
 *
 * @param[in] timer Timer object.
 * @param[in] option Timer start option (see HAL_TimerStartOpts enum).
 * @param[in] duration Timer duration.
 * @return uint32_t Return code, zero on success.
 */
uint32_t HAL_TimerStart(HAL_Timer *timer, HAL_TimerStartOpts option, uint64_t duration);
/**
 * @brief Checks if a timer is running.
 *
 * @param[in] timer Timer object.
 * @return true Timer is running.
 * @return false Timer is not running.
 */
bool HAL_TimerIsRunning(HAL_Timer *timer);
/**
 * @brief Stops a timer.
 *
 * @param[in] timer Timer object.
 * @return uint32_t Return code, zero on success.
 */
uint32_t HAL_TimerStop(HAL_Timer *timer);

/**
 * @brief Initialize the audio driver.
 *
 * @return uint32_t Return code, 0 on success.
 */
uint32_t HAL_AudioDeviceInit(void);
/**
 * @brief Terminate the audio driver.
 *
 */
void HAL_AudioDeviceTerminate(void);
/**
 * @brief Get the audio driver state.
 *
 * @return HAL_State Audio driver state.
 */
HAL_State HAL_AudioDeviceGetState(void);

/**
 * @brief Audio channel struct.
 *
 */
typedef struct HAL_AudioChannel
{
#if (USE_SDL_AUDIO == 1)
    /// @brief Holds audio samples.
    int16_t *       sampleBuffer;
    /// @brief Audio channel configuration.
    void *          config;
    /// @brief Gain factor.
    float           gain;
    /// @brief Audio channel ID.
    uint32_t        deviceID;
#elif (USE_OPENAL_AUDIO == 1)
    /// @brief OpenAL buffer identifier.
    uint32_t        bufferID;
    /// @brief Streaming source identifier.
    uint32_t        streamingSource;
#endif
    /// @brief Lock for exclusive channel access.
    pthread_mutex_t lock;
    /// @brief Size of the sample buffer.
    size_t          sampleBufferSize;
    /// @brief Playing frequency.
    int32_t         frequency;
    /// @brief Current state of the channel.
    HAL_State       state;
} HAL_AudioChannel;

/**
 * @brief Open a new audio channel, must be freed using `HAL_AudioChannelClose`.
 *
 * @param[in] bufferSize Number of samples used by the audio channel.
 * @return HAL_AudioChannel *A new audio channel object.
 */
HAL_AudioChannel *HAL_AudioChannelOpen(uint32_t bufferSize);
/**
 * @brief Close and free an audio channel.
 *
 * @param[in] channel Audio channel to close.
 */
void HAL_AudioChannelClose(HAL_AudioChannel *channel);
/**
 * @brief Set the sample buffer and frequency for an audio channel.
 *
 * @param[in] channel Audio channel to configure.
 * @param[in] buffer Sample buffer.
 * @param[in] bufferSize Sample buffer size.
 * @param[in] frequency Frequency to play samples at in Hz
 */
void HAL_AudioChannelSetSamples(HAL_AudioChannel *channel, int16_t *buffer, size_t bufferSize, uint32_t frequency);
/**
 * @brief Set the pitch (frequency scaling) for an audio channel that is currently playing.
 *
 * @param[in] channel Channel to set pitch for.
 * @param[in] pitch Pitch scale value.
 */
void HAL_AudioChannelSetPitch(HAL_AudioChannel *channel, float pitch);
/**
 * @brief Set the gain (volume scaling) for an audio channel that is currently playing.
 *
 * @param[in] channel Channel to set gain for.
 * @param[in] gain Gain scale value.
 */
void HAL_AudioChannelSetGain(HAL_AudioChannel *channel, float gain);
/**
 * @brief Play an audio channel.
 *
 * @param[in] channel Audio channel to play.
 */
void HAL_AudioChannelPlay(HAL_AudioChannel *channel);
/**
 * @brief Pause an audio channel.
 *
 * @param[in] channel Audio channel to pause.
 */
void HAL_AudioChannelPause(HAL_AudioChannel *channel);

#endif /* _HAL_H_ */