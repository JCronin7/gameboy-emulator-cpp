#if (USE_SDL_GRAPHICS == 1 && USE_OPENGL_GRAPHICS == 1)
#error "Cannot select OpenGL and SDL graphics drivers at the same time."
#endif
#if (USE_SDL_AUDIO == 1 && USE_OPENAL_AUDIO == 1)
#error "Cannot select OpenAL and SDL audio drivers at the same time."
#endif

#if (LINUX == 1)
#if (USE_SDL_GRAPHICS == 1 || USE_SDL_AUDIO == 1)
/**
 * @brief Install commands for SDL
 *
 *  # install sdl2
 *  sudo apt install libsdl2-dev libsdl2-2.0-0 -y;
 *
 *  # install sdl image  - if you want to display images
 *  sudo apt install libjpeg-dev libwebp-dev libtiff5-dev libsdl2-image-dev libsdl2-image-2.0-0 -y;
 *
 *  # install sdl mixer  - if you want sound
 *  sudo apt install libmikmod-dev libfishsound1-dev libsmpeg-dev liboggz2-dev libflac-dev libfluidsynth-dev libsdl2-mixer-dev libsdl2-mixer-2.0-0 -y;
 *
 *  # install sdl true type fonts - if you want to use text
 *  sudo apt install libfreetype6-dev libsdl2-ttf-dev libsdl2-ttf-2.0-0 -y;
 *
 *  # linker flags
 *  `sdl2-config --cflags --libs` -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf
*/
#include <SDL2/SDL.h>
#endif
#if (USE_OPENGL_GRAPHICS == 1)
/// @todo install command
#include <GL/glut.h>
#endif
#if (USE_OPENAL_AUDIO == 1)
// sudo apt-get install libopenal-dev
#include <AL/al.h>
#include <AL/alc.h>
#endif
#elif (OSX == 1)
#include <dispatch/dispatch.h>
#if (USE_SDL_GRAPHICS == 1 || USE_SDL_AUDIO == 1)
#include <SDL.h>
#endif
#if (USE_OPENGL_GRAPHICS == 1)
/// @todo add push/pop for warning supression
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <GLUT/glut.h>
#endif
#if (USE_OPENAL_AUDIO == 1)
// sudo apt-get install libopenal-dev
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif
#endif

#include "hal.h"

HAL_PixelBuffer *
HAL_PixelBufferCreate(uint16_t sizeX, uint16_t sizeY)
{
    HAL_PixelBuffer *pixelBuffer = (HAL_PixelBuffer *)malloc(sizeof(HAL_PixelBuffer));
    require_or(pixelBuffer, return NULL);

    pixelBuffer->sizeX = sizeX;
    pixelBuffer->sizeY = sizeY;
    pixelBuffer->state = HAL_STATE_CREATED;

    pixelBuffer->buffer = (uint8_t **)malloc(sizeY * sizeof(uint8_t *));
    require_or(pixelBuffer->buffer, free(pixelBuffer); return NULL);
    pthread_mutex_init(&pixelBuffer->lock, NULL);

    for (uint16_t y = 0; y < sizeY; y++)
    {
        pixelBuffer->buffer[y] = (uint8_t *)malloc(sizeX * sizeof(uint8_t));
        require_or(pixelBuffer->buffer[y], HAL_PixelBufferDestroy(pixelBuffer); return NULL);
        memset(pixelBuffer->buffer[y], HAL_LCD_PIXEL_COLOR_WHITE, sizeX);
    }

    return pixelBuffer;
}

void
HAL_PixelBufferDestroy(HAL_PixelBuffer *pixelBuffer)
{
    assert(pixelBuffer);
    require_or(pixelBuffer->buffer, return);
    require_or(pixelBuffer->state != HAL_STATE_INACTIVE, return);

    HAL_PixelBufferReserve(pixelBuffer);

    // free all buffers
    for (uint16_t y = 0; y < pixelBuffer->sizeY; y++)
    {
        require_or(pixelBuffer->buffer[y], continue);
        free(pixelBuffer->buffer[y]);
        pixelBuffer->buffer[y] = NULL;
    }

    free(pixelBuffer->buffer);
    pixelBuffer->buffer = NULL;
    pixelBuffer->sizeX = pixelBuffer->sizeY = 0;
    pixelBuffer->state = HAL_STATE_INACTIVE;
    pthread_mutex_destroy(&pixelBuffer->lock);
}

void
HAL_PixelBufferReserve(HAL_PixelBuffer *pixelBuffer)
{
    assert(pixelBuffer->buffer && pixelBuffer->state != HAL_STATE_INACTIVE);
    LOCK(pixelBuffer->lock);
}

void
HAL_PixelBufferRelease(HAL_PixelBuffer *pixelBuffer)
{
    assert(pixelBuffer->buffer && pixelBuffer->state != HAL_STATE_INACTIVE);
    UNLOCK(pixelBuffer->lock);
}

uint32_t
HAL_PixelBufferSetPixel(HAL_PixelBuffer *pixelBuffer, uint8_t positionX, uint8_t positionY, uint8_t brightness)
{
    require_or(pixelBuffer->buffer && pixelBuffer->state != HAL_STATE_INACTIVE, return kReturnEPixelBufferNull);
    require_or(positionX < pixelBuffer->sizeX, return kReturnEPixelBufferInvalidX);
    require_or(positionY < pixelBuffer->sizeY, return kReturnEPixelBufferInvalidY);
    require_or(pthread_mutex_trylock(&pixelBuffer->lock) != 0, return kReturnEPixelBufferNotLocked);

    /// @todo can we write directly to SDL here?
    pixelBuffer->buffer[positionY][positionX] = brightness;
    return kReturnOK;
}

uint32_t
HAL_PixelBufferGetPixel(HAL_PixelBuffer *pixelBuffer, uint8_t positionX, uint8_t positionY, uint8_t *brightness)
{
    require_or(brightness, return kReturnEBadArgument);
    *brightness = 0;
    require_or(pixelBuffer->buffer && pixelBuffer->state != HAL_STATE_INACTIVE, return kReturnEPixelBufferNull);
    require_or(positionX < pixelBuffer->sizeX, return kReturnEPixelBufferInvalidX);
    require_or(positionY < pixelBuffer->sizeY, return kReturnEPixelBufferInvalidY);
    require_or(pthread_mutex_trylock(&pixelBuffer->lock) != 0, return kReturnEPixelBufferNotLocked);

    emu::assign(brightness, pixelBuffer->buffer[positionY][positionX]);
    return kReturnOK;
}

uint16_t
HAL_PixelBufferGetSizeX(HAL_PixelBuffer *pixelBuffer)
{
    require_or(pixelBuffer->buffer && pixelBuffer->state != HAL_STATE_INACTIVE, return 0);
    return pixelBuffer->sizeX;
}

uint16_t
HAL_PixelBufferGetSizeY(HAL_PixelBuffer *pixelBuffer)
{
    require_or(pixelBuffer->buffer && pixelBuffer->state != HAL_STATE_INACTIVE, return 0);
    return pixelBuffer->sizeY;
}

HAL_State
HAL_PixelBufferGetState(HAL_PixelBuffer *pixelBuffer)
{
    assert(pixelBuffer != NULL);
    return pixelBuffer->state;
}

uint32_t
HAL_PixelBufferSetReady(HAL_PixelBuffer *pixelBuffer, bool ready)
{
    require_or(pixelBuffer->buffer && pixelBuffer->state != HAL_STATE_INACTIVE, return kReturnEPixelBufferNull);
    pixelBuffer->state = (ready) ? HAL_STATE_RUNNING : HAL_STATE_CREATED;
    return kReturnOK;
}

typedef struct HAL_Display
{
#if (USE_SDL_GRAPHICS == 1)
    SDL_Window *            window;
    SDL_Renderer *          renderer;
    HAL_KeyboardEventPtr    keyboardEventCallback;
#elif (USE_OPENGL_GRAPHICS == 1)
    const char *            windowName;
    int32_t                 windowID;
#endif
    HAL_PixelBuffer *       pixelBuffer;
    HAL_State               state;
    pthread_t               thread;
    bool                    isAsyc;
} HAL_Display;

static HAL_Display halDisplay;

static void
HAL_DisplayDrawPixel(uint16_t positionX, uint16_t positionY, uint8_t brightness)
{
    require_or(halDisplay.state != HAL_STATE_INACTIVE, return);
#if (USE_SDL_GRAPHICS == 1)
    SDL_SetRenderDrawColor(halDisplay.renderer, brightness, brightness, brightness, 0xFF);
    SDL_RenderDrawPoint(halDisplay.renderer, positionX, positionY);
    SDL_SetRenderDrawColor(halDisplay.renderer, 0, 0, 0, 0xFF);
#elif (USE_OPENGL_GRAPHICS == 1)
    glBegin(GL_QUADS);
        glColor3ub(brightness, brightness, brightness);
        //glVertex2f((float)x + 0.5, (float)y + 0.5);
        glVertex2i(positionX, positionY);
        glVertex2i(positionX, positionY + 1);
        glVertex2i(positionX + 1, positionY + 1);
        glVertex2i(positionX + 1, positionY);
    glEnd();
#endif
}

static void
HAL_DisplayDrawLine(uint16_t startX, uint16_t endX, uint16_t startY, uint16_t endY)
{
    require_or(halDisplay.state != HAL_STATE_INACTIVE, return);
#if (USE_SDL_GRAPHICS == 1)
    SDL_SetRenderDrawColor(halDisplay.renderer, 0xFF, 0, 0, 0xFF);
    SDL_RenderDrawLine(halDisplay.renderer, startX, startY, endX, endY);
    SDL_SetRenderDrawColor(halDisplay.renderer, 0, 0, 0, 0xFF);
#elif (USE_OPENGL_GRAPHICS == 1)
    glBegin(GL_LINES);
        glColor3ub(0xFFD, 0, 0);
        glVertex2i(startX, startY);
        glVertex2i(endX, endY);
    glEnd();
#endif
}

static void
HAL_DisplayDrawSquare(uint16_t startX, uint16_t endX, uint16_t startY, uint16_t endY)
{
    require_or(halDisplay.state != HAL_STATE_INACTIVE, return);
#if (USE_SDL_GRAPHICS == 1)
    SDL_SetRenderDrawColor(halDisplay.renderer, 0xFF, 0, 0, 0xFF);
    SDL_Point points[4] = {
        { .x = startX, .y = startY },
        { .x = startX, .y = endY },
        { .x = endX, .y = endY },
        { .x = endX, .y = startY },
    };
    SDL_RenderDrawLines(halDisplay.renderer, points, 4);
    SDL_SetRenderDrawColor(halDisplay.renderer, 0, 0, 0, 0xFF);
#elif (USE_OPENGL_GRAPHICS == 1)
    glBegin(GL_LINES);
    glColor3ub(0, 0, 255);
        glVertex2i(startX, startY);
        glVertex2i(endX, startY);
        glVertex2i(endX, endY);
        glVertex2i(endY, startX);
    glEnd();
#endif
}

static uint32_t
HAL_DisplayCheckForEvents(void)
{
#if (USE_SDL_GRAPHICS == 1)
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            halDisplay.state = HAL_STATE_INACTIVE;
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (event.key.repeat == 0)
            {
                halDisplay.keyboardEventCallback(event.key.keysym.sym, 0, 0);
            }
            break;
        }
    }
#endif
    return kReturnOK;
}

static void
HAL_DisplaySetFrame(void)
{
    HAL_State pixel_buffer_state = HAL_PixelBufferGetState(halDisplay.pixelBuffer);
#if (USE_SDL_GRAPHICS == 1)
    require_or(pixel_buffer_state == HAL_STATE_RUNNING, return);
    SDL_RenderClear(halDisplay.renderer);
#elif (USE_OPENGL_GRAPHICS == 1)
    require_or(pixel_buffer_state == HAL_STATE_RUNNING, glutPostRedisplay(); return);
    // Set background color to black and opaque
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
#endif
    // gain exclusive access to pixel buffer
    HAL_PixelBufferReserve(halDisplay.pixelBuffer);
    uint16_t size_x = HAL_PixelBufferGetSizeX(halDisplay.pixelBuffer);
    uint16_t size_y = HAL_PixelBufferGetSizeY(halDisplay.pixelBuffer);
    for (uint16_t y = 0; y < size_y; y++)
    {
        for (uint16_t x = 0; x < size_x; x++)
        {
            uint8_t brightness;
            HAL_PixelBufferGetPixel(halDisplay.pixelBuffer, x, y, &brightness);
            HAL_DisplayDrawPixel(x, y, brightness);
        }
    }
#if (SHOW_TILE_OUTLINES == 1)
    for (uint16_t tile_x = 0; tile_x < size_x; tile_x += 8)
    {
        HAL_DisplayDrawLine(tile_x, tile_x, 0, size_y);
    }
    for (uint16_t tile_y = 0; tile_y < size_y; tile_y += 8)
    {
        HAL_DisplayDrawLine(0, size_x, tile_y, tile_y);
    }
#endif
    HAL_PixelBufferSetReady(halDisplay.pixelBuffer, false);
    HAL_PixelBufferRelease(halDisplay.pixelBuffer);

#if (USE_SDL_GRAPHICS == 1)
    SDL_RenderPresent(halDisplay.renderer);
#elif (USE_OPENGL_GRAPHICS == 1)
    glFlush();
    glutPostRedisplay();
#endif
}

uint32_t
HAL_DisplayInit(const char *windowName, uint16_t sizeX, uint16_t sizeY)
{
    halDisplay.pixelBuffer = HAL_PixelBufferCreate(sizeX, sizeY);
    if (halDisplay.pixelBuffer == NULL)
    {
        log_error("Failed to create pixel buffer");
        return kReturnEPixelBufferInit;
    }

#if (USE_SDL_GRAPHICS == 1)
    /// @todo don't init everything here
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        log_error("Error in SDL: %s", SDL_GetError());
		return kReturnEVideoDriverInit;
	}

    uint16_t window_size_x = kHalWindowScaleFactor * sizeX;
    uint16_t window_size_y = kHalWindowScaleFactor * sizeY;
    uint32_t window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    halDisplay.window = SDL_CreateWindow(windowName, 100, 100, window_size_x, window_size_y, window_flags);
    if (halDisplay.window == NULL)
    {
        log_error("Error in SDL: %s", SDL_GetError());
        return kReturnEWindowInitError;
    }

    uint32_t renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    halDisplay.renderer = SDL_CreateRenderer(halDisplay.window, -1, renderer_flags);
    if (halDisplay.renderer == NULL)
    {
        log_error("Error in SDL: %s", SDL_GetError());
        SDL_DestroyWindow(halDisplay.window);
        SDL_Quit();
        return kReturnERendererInitError;
    }

    SDL_RenderSetScale(halDisplay.renderer, kHalWindowScaleFactor, kHalWindowScaleFactor);
#elif (USE_OPENGL_GRAPHICS == 1)
    int glut_argc = 0;
    char **glut_argv = NULL;

    glutInit(&glut_argc, glut_argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    // Allows the display window to be stretched while retaining pixel count
    uint16_t window_size_x = kHalWindowScaleFactor * sizeX;
    uint16_t window_size_y = kHalWindowScaleFactor * sizeY;
    glutInitWindowSize(window_size_x, window_size_y);

    // Create a window with the given title
    halDisplay.windowID = glutCreateWindow(windowName);
    halDisplay.windowName = windowName;
    glPushMatrix();
	glLoadIdentity();
    gluOrtho2D(0, sizeX, sizeY, 0);

    // Position the window's initial top-left corner
    glutInitWindowPosition(0, 0);
    glutDisplayFunc(HAL_DisplaySetFrame);
    glPointSize(xConfigPixelSize);
#endif
    halDisplay.state = HAL_STATE_CREATED;
    halDisplay.isAsyc = false;
    return kReturnOK;
}

void
HAL_DisplayTerminate(void)
{
    require_or(halDisplay.state != HAL_STATE_INACTIVE, return);
    HAL_DisplayCancel();
    /// @todo destroy thread
    //halDisplay.thread
    if (halDisplay.pixelBuffer != NULL)
    {
        HAL_PixelBufferDestroy(halDisplay.pixelBuffer);
        halDisplay.pixelBuffer = NULL;
    }
#if (USE_SDL_GRAPHICS == 1)
    halDisplay.keyboardEventCallback = NULL;
    if (halDisplay.renderer != NULL)
    {
	    SDL_DestroyRenderer(halDisplay.renderer);
        halDisplay.renderer = NULL;
    }

    if (halDisplay.window != NULL)
    {
	    SDL_DestroyWindow(halDisplay.window);
        halDisplay.window = NULL;
    }
	SDL_Quit();
#elif (USE_OPENGL_GRAPHICS == 1)
    glutDestroyWindow(halDisplay.windowID);
#endif
    halDisplay.state = HAL_STATE_INACTIVE;
}

void
HAL_DisplaySetKeyboardEventCallback(HAL_KeyboardEventPtr callback)
{
#if (USE_SDL_GRAPHICS == 1)
    halDisplay.keyboardEventCallback = callback;
#elif (USE_OPENGL_GRAPHICS == 1)
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
    glutKeyboardUpFunc(callback);
    glutKeyboardFunc(callback);
#endif
}

void *
HAL_DisplayBackgroundTask(void *args)
{
    (void)args;
    halDisplay.state = HAL_STATE_RUNNING;
#if (USE_SDL_GRAPHICS == 1)
    while (true)
    {
        HAL_DisplayCheckForEvents();
        require_or(halDisplay.state == HAL_STATE_RUNNING, return NULL)
        HAL_DisplaySetFrame();
        SDL_Delay(kHalWindowDelayMs60FPS);
    }
#elif (USE_OPENGL_GRAPHICS == 1)
    // Enter the event-processing loop
    glutMainLoop();
#endif
    return NULL;
}

uint32_t
HAL_DisplayStart(void)
{
    require_or(HAL_PixelBufferGetState(halDisplay.pixelBuffer) != HAL_STATE_INACTIVE, return kReturnEFailure);
    require_or(halDisplay.state != HAL_STATE_INACTIVE, return kReturnEDisplayNotInit);
    halDisplay.state = HAL_STATE_RUNNING;
    int ret = pthread_create(&halDisplay.thread, NULL, HAL_DisplayBackgroundTask, NULL);
    halDisplay.isAsyc = true;
    require_or(ret == 0, log_error("Failed to create thread!"); return kReturnEPThreadError);
    return kReturnOK;
}

uint32_t
HAL_DisplayJoin(void)
{
    require_or(HAL_PixelBufferGetState(halDisplay.pixelBuffer) != HAL_STATE_INACTIVE, return kReturnEFailure);
    require_or(halDisplay.isAsyc == true, return kReturnOK);
    require_or(halDisplay.state != HAL_STATE_INACTIVE, return kReturnEDisplayNotInit);
    int ret = pthread_join(halDisplay.thread, NULL);
    require_or(ret == 0, log_error("Failed to join thread!"); return kReturnEPThreadError);
    halDisplay.state = HAL_STATE_CREATED;
    return kReturnOK;
}

uint32_t
HAL_DisplayCancel(void)
{
    require_or(HAL_PixelBufferGetState(halDisplay.pixelBuffer) != HAL_STATE_INACTIVE, return kReturnEFailure);
    if (halDisplay.isAsyc)
    {
        require_or(halDisplay.state != HAL_STATE_INACTIVE, return kReturnEDisplayNotInit);
        int ret = pthread_cancel(halDisplay.thread);
        require_or(ret == 0, log_error("Failed to join thread!"); return kReturnEPThreadError);
    }
    halDisplay.state = HAL_STATE_CREATED;
    return kReturnOK;
}

HAL_State
HAL_DisplayGetState(void)
{
    return halDisplay.state;
}

void
HAL_DisplayReserve(void)
{
    HAL_PixelBufferReserve(halDisplay.pixelBuffer);
}

void
HAL_DisplayRelease(void)
{
    HAL_PixelBufferRelease(halDisplay.pixelBuffer);
}

uint32_t
HAL_DisplaySetPixel(uint8_t positionX, uint8_t positionY, uint8_t brightness)
{
    return HAL_PixelBufferSetPixel(halDisplay.pixelBuffer, positionX, positionY, brightness);
}

uint32_t
HAL_DisplayGetPixel(uint8_t positionX, uint8_t positionY, uint8_t *brightness)
{
    return HAL_PixelBufferGetPixel(halDisplay.pixelBuffer, positionX, positionY, brightness);
}

uint16_t
HAL_DisplayGetSizeX(void)
{
    return HAL_PixelBufferGetSizeX(halDisplay.pixelBuffer);
}

uint16_t
HAL_DisplayGetSizeY(void)
{
    return HAL_PixelBufferGetSizeY(halDisplay.pixelBuffer);
}

HAL_State
HAL_DisplayGetPixelBufferState(void)
{
    return HAL_PixelBufferGetState(halDisplay.pixelBuffer);
}

void
HAL_DisplaySetReady(bool ready)
{
    HAL_PixelBufferSetReady(halDisplay.pixelBuffer, ready);
}

typedef struct HAL_TimerPrivate
{
#if (LINUX == 1)
    struct sigevent     signalEvent;
    struct itimerspec   timerSpec;
    timer_t             timerID;
    pthread_attr_t      pthreadAttr;
#elif (OSX == 1)
    dispatch_source_t   dispatchSource;
#endif
    pthread_mutex_t     lock;
    HAL_TimerCallbackPtr callback;
    void *              args;
    HAL_State           state;
    bool                interval;
} HAL_TimerPrivate;

#if (OSX == 1)
typedef struct HAL_TimerQueue
{
    dispatch_queue_t queue;
    uint16_t activeTimers;
} HAL_TimerQueue;

static HAL_TimerQueue halTimerQueue = { NULL, 0 };
#endif

#if (LINUX == 1)
static void
HAL_TimerExpired(union sigval sv)
{
    HAL_TimerPrivate *timer = (HAL_TimerPrivate *)sv.sival_ptr;
    assert(timer);
    if (timer->interval == false)
    {
        timer->state = HAL_STATE_IDLE;
        timer->timerSpec.it_value.tv_sec = timer->timerSpec.it_value.tv_nsec = 0;
    }
    require_or(timer->callback != NULL, return);
    timer->callback(timer->args);
}
#elif (OSX == 1)
static void
HAL_TimerExpried(HAL_TimerPrivate *timer)
{
    assert(timer);
    if (timer->interval == false)
    {
        timer->state = HAL_STATE_IDLE;
        dispatch_suspend(timer->dispatchSource);
    }
    require_or(timer->callback != NULL, return);
    timer->callback(timer->args);
}
#endif

HAL_Timer *
HAL_TimerCreate(HAL_TimerCallbackPtr callback, void *args)
{
    HAL_TimerPrivate *timer = (HAL_TimerPrivate *)malloc(sizeof(HAL_TimerPrivate));
    require_or(timer, return NULL);
    timer->callback = callback;
    timer->args = args;

    int ret = pthread_mutex_init(&timer->lock, NULL);
    require_or(ret == 0, HAL_TimerDestroy((HAL_Timer *)timer); return NULL);
#if (LINUX == 1)
    // Create a separate thread for the callback
    ret = pthread_attr_init(&timer->pthreadAttr);
    require_or(ret == 0, HAL_TimerDestroy((HAL_Timer *)timer); return NULL);
    ret = pthread_attr_setdetachstate(&timer->pthreadAttr, PTHREAD_CREATE_DETACHED);
    require_or(ret == 0, HAL_TimerDestroy((HAL_Timer *)timer); return NULL);

    timer->signalEvent.sigev_notify = SIGEV_THREAD;
    timer->signalEvent.sigev_notify_function = HAL_TimerExpired;
    timer->signalEvent.sigev_value.sival_ptr = timer;
    timer->signalEvent.sigev_notify_attributes = &timer->pthreadAttr;

    ret = timer_create(CLOCK_REALTIME, &timer->signalEvent, &timer->timerID);
    require_or(ret == 0, HAL_TimerDestroy((HAL_Timer *)timer); return NULL);

    timer->timerSpec.it_value.tv_sec = 0;
    timer->timerSpec.it_value.tv_nsec = 0;
    timer->timerSpec.it_interval.tv_sec = 0;
    timer->timerSpec.it_interval.tv_nsec = 0;
#elif (OSX == 1)
    if (halTimerQueue.activeTimers == 0)
    {
        halTimerQueue.queue = dispatch_queue_create("EmulatorTimerQueue", 0);
    }
    // Create dispatch timer source
    timer->dispatchSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, halTimerQueue.queue);
    // Set block for dispatch source when catched events
    dispatch_source_set_event_handler(timer->dispatchSource, ^{ HAL_TimerExpried(timer); });
    // Set block for dispatch source when cancel source
    dispatch_source_set_cancel_handler(timer->dispatchSource, ^{
        dispatch_release(timer->dispatchSource);
        dispatch_release(halTimerQueue.queue);
    });
    halTimerQueue.activeTimers++;
#endif
    timer->state = HAL_STATE_IDLE;
    return (HAL_Timer *)timer;
}

void
HAL_TimerDestroy(HAL_Timer *timer)
{
    HAL_TimerPrivate *timer_info = (HAL_TimerPrivate *)timer;
    require_or(timer && timer_info->state != HAL_STATE_INACTIVE, return);
    if (timer_info->state == HAL_STATE_RUNNING)
    {
        HAL_TimerStop(timer);
    }
    pthread_mutex_destroy(&timer_info->lock);
#if (LINUX == 1)
    timer_delete(timer_info->timerID);
    pthread_attr_destroy(&timer_info->pthreadAttr);
#elif (OSX == 1)
    dispatch_source_cancel(timer_info->dispatchSource);
#endif
    timer_info->state = HAL_STATE_INACTIVE;
    free(timer);
}

uint32_t
HAL_TimerStart(HAL_Timer *timer, HAL_TimerStartOpts option, uint64_t duration)
{
    assert(timer);
    HAL_TimerPrivate *timer_info = (HAL_TimerPrivate *)timer;
    LOCK(timer_info->lock);
#if (LINUX == 1)
    switch (option)
    {
    case HAL_TIMER_START_INTERVAL_NSEC:
        timer_info->timerSpec.it_interval.tv_nsec = duration;
    case HAL_TIMER_START_VALUE_NSEC:
        timer_info->timerSpec.it_value.tv_nsec = duration;
        timer_info->timerSpec.it_value.tv_sec = 0;
        timer_info->timerSpec.it_interval.tv_sec = 0;
        break;
    case HAL_TIMER_START_INTERVAL_SEC:
        timer_info->timerSpec.it_interval.tv_sec = duration;
    case HAL_TIMER_START_VALUE_SEC:
        timer_info->timerSpec.it_value.tv_sec = duration;
        timer_info->timerSpec.it_value.tv_nsec = 0;
        timer_info->timerSpec.it_interval.tv_nsec = 0;
        break;
    }
    int ret = timer_settime(timer_info->timerID, 0, &timer_info->timerSpec, NULL);
    require_or(ret == 0, UNLOCK(timer_info->lock); return kReturnETimerCantStart);
#elif (OSX == 1)
    dispatch_time_t start_value = 0;
    dispatch_time_t interval_value = DISPATCH_TIME_FOREVER;
    timer_info->interval = false;
    switch (option)
    {
    case HAL_TIMER_START_INTERVAL_NSEC:
        timer_info->interval = true;
        interval_value = duration;
    case HAL_TIMER_START_VALUE_NSEC:
        start_value = dispatch_time(DISPATCH_TIME_NOW, duration);
        break;
    case HAL_TIMER_START_INTERVAL_SEC:
        timer_info->interval = true;
        interval_value = NSEC_PER_SEC * duration;
    case HAL_TIMER_START_VALUE_SEC:
        start_value = dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * duration);
        break;
    }

    dispatch_source_set_timer(timer_info->dispatchSource, start_value, interval_value, 0);
    if (timer_info->state != HAL_STATE_RUNNING)
    {
        dispatch_resume(timer_info->dispatchSource);
    }
#endif
    timer_info->state = HAL_STATE_RUNNING;
    UNLOCK(timer_info->lock);
    return kReturnOK;
}

bool
HAL_TimerIsRunning(HAL_Timer *timer)
{
    assert(timer);
    HAL_TimerPrivate *timer_info = (HAL_TimerPrivate *)timer;
    return timer_info->state == HAL_STATE_RUNNING;
}

uint32_t
HAL_TimerStop(HAL_Timer *timer)
{
    assert(timer);
    HAL_TimerPrivate *timer_info = (HAL_TimerPrivate *)timer;
    require_or(timer_info->state == HAL_STATE_RUNNING, return kReturnOK);
    LOCK(timer_info->lock);
#if (LINUX == 1)
    timer_info->timerSpec.it_value.tv_sec = 0;
    timer_info->timerSpec.it_value.tv_nsec = 0;
    timer_info->timerSpec.it_interval.tv_sec = 0;
    timer_info->timerSpec.it_interval.tv_nsec = 0;
    int ret = timer_settime(timer_info->timerID, 0, &timer_info->timerSpec, NULL);
    require_or(ret == 0, UNLOCK(timer_info->lock); return kReturnETimerCantStop);
#elif (OSX == 1)
    dispatch_suspend(timer_info->dispatchSource);
#endif
    timer_info->state = HAL_STATE_IDLE;
    UNLOCK(timer_info->lock);
    return kReturnOK;
}

typedef struct HAL_AudioDevice
{
#if (USE_OPENAL_AUDIO == 1)
    pthread_mutex_t lock;
    const ALCchar * defname;
    ALCdevice *     audioDevice;
    ALCcontext *    context;
#endif
    HAL_State  state;
} HAL_AudioDevice;

static HAL_AudioDevice halAudioDevice;

uint32_t
HAL_AudioDeviceInit(void)
{
#if (USE_SDL_AUDIO == 1)
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
    {
        log_error("Error in SDL: %s", SDL_GetError());
		return kReturnEAudioDriverInit;
	}
#elif (USE_OPENAL_AUDIO == 1)
    // Find default audio device
    halAudioDevice.defname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    log_debug("Audio device name %s", halAudioDevice.defname);
    if (halAudioDevice.defname == NULL)
    {
        log_error("Error in openGL");
		return kReturnEAudioDriverInit;
	}

    // Open audio device
    halAudioDevice.audioDevice = alcOpenDevice(halAudioDevice.defname);
    if (halAudioDevice.audioDevice == NULL)
    {
        log_error("Error in openGL");
		return kReturnEAudioDriverInit;
	}

    // Create context
    halAudioDevice.context = alcCreateContext(halAudioDevice.audioDevice, NULL);
    if (halAudioDevice.context == NULL)
    {
        log_error("Error in openGL");
		return kReturnEAudioDriverInit;
	}
    alcMakeContextCurrent(halAudioDevice.context);
    if (alGetError() != AL_NO_ERROR)
    {
        halAudioDevice.state = HAL_STATE_CREATED;
        log_error("Error in openAL driver: %s", alGetString(alGetError()));
        return kReturnEAudioDriverInit;
    }
#endif
    halAudioDevice.state = HAL_STATE_CREATED;
    return kReturnOK;
}

void
HAL_AudioDeviceTerminate(void)
{
    require_or(halAudioDevice.state != HAL_STATE_INACTIVE, return);
#if (USE_SDL_AUDIO == 1)
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
#elif (USE_OPENAL_AUDIO == 1)
    alcMakeContextCurrent(NULL);
    alcDestroyContext(halAudioDevice.context);
    halAudioDevice.context = NULL;
    alcCloseDevice(halAudioDevice.audioDevice);
    halAudioDevice.audioDevice = NULL;
    halAudioDevice.defname = NULL;
#endif
}

HAL_State
HAL_AudioDeviceGetState(void)
{
    return halAudioDevice.state;
}

#if (USE_SDL_AUDIO == 1)
void HAL_AudioChannelCallback(void *channel_ptr, Uint8 *stream, int streamLength)
{
    HAL_AudioChannel *channel = (HAL_AudioChannel *)channel_ptr;
    require_or(channel, log_error("Channel is NULL"); return);
    require_or(channel->sampleBuffer && channel->sampleBufferSize > 0, return);
    uint16_t *stream_samples = (uint16_t *)stream;

    LOCK(channel->lock);
    for (size_t i = 0; i < channel->sampleBufferSize; i++)
    {
        stream_samples[i] = (int)(channel->gain * (float)channel->sampleBuffer[i]);
    }
    UNLOCK(channel->lock);
}
#endif

HAL_AudioChannel *
HAL_AudioChannelOpen(uint32_t bufferSize)
{
    HAL_AudioChannel *channel = (HAL_AudioChannel *)malloc(sizeof(HAL_AudioChannel));
    require_or(channel, return NULL);
    require_or(pthread_mutex_init(&channel->lock, NULL) == 0, free(channel); return NULL);
    channel->sampleBufferSize = bufferSize;

#if (USE_SDL_AUDIO == 1)
    SDL_AudioSpec *audio_spec = (SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
    require_or(audio_spec, HAL_AudioChannelClose(channel); return NULL);

    audio_spec->format = AUDIO_S16SYS;      // Audio format (16-bit signed)
    audio_spec->channels = 1;               // Number of audio channels (stereo)
    audio_spec->samples = bufferSize;       // Audio buffer size (samples)
    audio_spec->callback = HAL_AudioChannelCallback;
    audio_spec->userdata = channel;
    audio_spec->size = sizeof(int16_t) * bufferSize;

    channel->gain = 1;
    channel->config = audio_spec;
    channel->sampleBufferSize = bufferSize;
    channel->sampleBuffer = (int16_t *)malloc(bufferSize * sizeof(int16_t));
    if (channel->sampleBuffer == NULL)
    {
        log_error("Failed to craete sample buffer");
        HAL_AudioChannelClose(channel);
		return NULL;
	}
#elif (USE_OPENAL_AUDIO == 1)
    alGenBuffers(1, &channel->bufferID);
    if (alGetError() != AL_NO_ERROR)
    {
        log_error("failed call to alGenBuffer, (%s)", alGetString(alGetError()));
    }
    alGenSources(1, &channel->streamingSource);
    alSourcei(channel->streamingSource, AL_LOOPING, AL_TRUE);
#endif
    channel->state = HAL_STATE_CREATED;
    return channel;
}

void
HAL_AudioChannelClose(HAL_AudioChannel *channel)
{
    assert(channel);
    require_or(channel->state != HAL_STATE_INACTIVE, return);
    require_or(channel, return);
#if (USE_SDL_AUDIO == 1)
    if (channel->state == HAL_STATE_RUNNING)
    {
        HAL_AudioChannelPause(channel);
    }
    if (channel->sampleBuffer)
    {
        free(channel->sampleBuffer);
    }
    if (channel->config)
    {
        free(channel->config);
    }
#elif (USE_OPENAL_AUDIO == 1)
    alSourceStop(channel->streamingSource);
    alDeleteBuffers(1, &channel->bufferID);
    alDeleteSources(1, &channel->streamingSource);
#endif
    pthread_mutex_destroy(&channel->lock);
    free(channel);
}

void
HAL_AudioChannelSetSamples(HAL_AudioChannel *channel, int16_t *buffer, size_t bufferSize, uint32_t frequency)
{
    assert(channel);
    size_t copy_size = (bufferSize < channel->sampleBufferSize) ? bufferSize : channel->sampleBufferSize;
    HAL_State current_state = channel->state;
    HAL_AudioChannelPause(channel);
    require_or(buffer && bufferSize > 0, return);
    require_or(frequency > 0, return);

    LOCK(channel->lock);
#if (USE_SDL_AUDIO == 1)
    memcpy(channel->sampleBuffer, buffer, sizeof(int16_t) * copy_size);
    if (bufferSize < channel->sampleBufferSize)
    {
        int16_t *fill_from = &channel->sampleBuffer[bufferSize];
        size_t fill_size = sizeof(int16_t) * (channel->sampleBufferSize - bufferSize);
        memset(fill_from, 0, fill_size);
    }
    SDL_AudioSpec *audio_spec = (SDL_AudioSpec *)channel->config;
    channel->frequency = audio_spec->freq = frequency;
#elif (USE_OPENAL_AUDIO == 1)
    channel->frequency = frequency;
    alSourcei(channel->streamingSource, AL_BUFFER, 0);
    alBufferData(channel->bufferID, AL_FORMAT_MONO16, buffer, bufferSize * sizeof(int16_t), channel->frequency);
    alSourcei(channel->streamingSource, AL_BUFFER, channel->bufferID);
#endif
    UNLOCK(channel->lock);

    channel->state = HAL_STATE_IDLE;
    require_or(current_state == HAL_STATE_RUNNING, return);
    HAL_AudioChannelPlay(channel);
}

void
HAL_AudioChannelSetPitch(HAL_AudioChannel *channel, float pitch)
{
    assert(channel);
    require_or(pitch > kHalAudioMinPitch, HAL_AudioChannelPause(channel); return);
    LOCK(channel->lock);
#if (USE_SDL_AUDIO == 1)
    if (channel->deviceID)
    {
        SDL_CloseAudioDevice(channel->deviceID);
    }

    SDL_AudioSpec *audio_spec = (SDL_AudioSpec *)channel->config;
    audio_spec->freq = (int)(pitch * (float)channel->frequency);
    require_or(channel->state == HAL_STATE_RUNNING, UNLOCK(channel->lock); return);

    channel->deviceID = SDL_OpenAudioDevice(NULL, 0, channel->config, NULL, 0);
    if (channel->deviceID == 0)
    {
        log_error("Error in SDL: %s", SDL_GetError());
        UNLOCK(channel->lock);
        return;
	}
    SDL_PauseAudioDevice(channel->deviceID, 0);
#elif (USE_OPENAL_AUDIO == 1)
    alSourcef(channel->streamingSource, AL_PITCH, pitch);
#endif
    UNLOCK(channel->lock);
}

void
HAL_AudioChannelSetGain(HAL_AudioChannel *channel, float gain)
{
    assert(channel);
    require_or(gain > kHalAudioMinGain, HAL_AudioChannelPause(channel); return);
    LOCK(channel->lock);
#if (USE_SDL_AUDIO == 1)
    channel->gain = gain;
#elif (USE_OPENAL_AUDIO == 1)
    alSourcef(channel->streamingSource, AL_GAIN, gain);
#endif
    UNLOCK(channel->lock);
}

void
HAL_AudioChannelPlay(HAL_AudioChannel *channel)
{
    assert(channel);
    require_or(channel->state == HAL_STATE_IDLE, return);
    LOCK(channel->lock);
#if (USE_SDL_AUDIO == 1)
    channel->deviceID = SDL_OpenAudioDevice(NULL, 0, channel->config, NULL, 0);
    if (channel->deviceID == 0)
    {
        log_error("Error in SDL: %s", SDL_GetError());
        return;
	}
    SDL_PauseAudioDevice(channel->deviceID, 0);
#elif (USE_OPENAL_AUDIO == 1)
    alSourcePlay(channel->streamingSource);
#endif
    channel->state = HAL_STATE_RUNNING;
    UNLOCK(channel->lock);
}

void
HAL_AudioChannelPause(HAL_AudioChannel *channel)
{
    assert(channel);
    require_or(channel->state == HAL_STATE_RUNNING, return);
    LOCK(channel->lock);
#if (USE_SDL_AUDIO == 1)
    require_or(channel->deviceID, return);
    SDL_PauseAudioDevice(channel->deviceID, 1);
    SDL_CloseAudioDevice(channel->deviceID);
    channel->deviceID = 0;
#elif (USE_OPENAL_AUDIO == 1)
    alSourceStop(channel->streamingSource);
#endif
    channel->state = HAL_STATE_IDLE;
    UNLOCK(channel->lock);
}
