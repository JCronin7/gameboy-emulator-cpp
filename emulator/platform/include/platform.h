#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include <queue>

#include "config.h"

#if (OSX == 1 && LINUX == 1)
#error "OS macro is invalid"
#endif

/** Count arguments from https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/preprocessor/macros/__VA_ARGS__/count-arguments*/
#define __COUNT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define __ARG_CNT(...) __COUNT(dummy, ##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define __CONCAT__(a, b) a##b
#define CONCAT(a, b) __CONCAT__(a, b)

#define kConvertByteToKB (1 << 10)

/**
 * @brief Return Codes
 *
 */
#define kReturnOK                       0
#define kReturnEFailure                 0x001
#define kReturnEBadArgument             0x002
#define kReturnEPThreadError            0x003
#define kReturnENoMemory                0x004

#define kReturnEPixelBufferNull         0x100
#define kReturnEPixelBufferNotLocked    0x101
#define kReturnEPixelBufferUnlock       0x102
#define kReturnEPixelBufferInvalidX     0x103
#define kReturnEPixelBufferInvalidY     0x104

#define kReturnEVideoDriverInit         0x200
#define kReturnEPixelBufferInit         0x201
#define kReturnEWindowInitError         0x202
#define kReturnERendererInitError       0x203
#define kReturnEDisplayNotInit          0x204

#define kReturnEAudioDriverInit         0x300

#define kReturnEFifoFull                0x400
#define kReturnEFifoEmpty               0x401

#define kReturnETimerCantStart          0x500
#define kReturnETimerCantStop           0x501

#define kReturnENotFound                0x600
#define kReturnECantInsert              0x601

#define kLogLevelDebug 3
#define kLogLevelInfo  2
#define kLogLevelError 1
#define kLogLevelFatal 0

/**
 * @brief Alias for packed attribute.
 */
#define packed(align) __attribute__((packed, aligned(align)))
/**
 * @brief Alias' for expect_true/expect_false conditionals. 
 */
#define expect_true(x)  __builtin_expect(!!(x), 1)
#define expect_false(x) __builtin_expect(!!(x), 0)

/// @brief Custom `assert` macro
#ifdef assert
#undef assert
#endif
#define assert(cond) if (!expect_true(cond)) { log_error("Assert Failure! " #cond); exit(1); }

#define require_or(condition, else_action) { if (!expect_true(condition)) { else_action; int __ = __ + 1; }}

/// @brief lock macro, assert on failure.
#define LOCK(lock) assert(pthread_mutex_lock(&(lock)) == 0)
/// @brief unlock macro, assert on failure.
#define UNLOCK(lock) assert(pthread_mutex_unlock(&(lock)) == 0)

/**
 * @brief Logging
 *
 */
#if (EMULATOR_LOG_LEVEL >= kLogLevelDebug)
#define log_debug(format, ...) printf("%s:%d:%s " format "\n", __FILE__, __LINE__, __FUNCTION__ __VA_OPT__(,) __VA_ARGS__)
#else
#define log_debug(format, ...)
#endif
#if (EMULATOR_LOG_LEVEL >= kLogLevelInfo)
#define log_info(format, ...) printf("%s:%d:%s " format "\n", __FILE__, __LINE__, __FUNCTION__ __VA_OPT__(,) __VA_ARGS__)
#else
#define log_info(format, ...)
#endif
#if (EMULATOR_LOG_LEVEL >= kLogLevelError)
#define log_error(format, ...) fprintf(stderr, "%s:%d:%s " format "\n", __FILE__, __LINE__, __FUNCTION__ __VA_OPT__(,) __VA_ARGS__)
#else
#define log_error(format, ...)
#endif
#define log_fatal(format, ...)                                                                                          \
    do {                                                                                                                \
        fprintf(stderr, "%s:%d:%s " format "\n", __FILE__, __LINE__, __FUNCTION__ __VA_OPT__(,) __VA_ARGS__);   \
        assert(!"Fatal Error!");                                                                                        \
    } while (false)
#define set_error(error) systemError = (error); log_error("error %u", (error)); (error)

#if (TEST == 1)
#define exit(status) exit(status);
#else
#define exit(status) { systemExitStatus = (status); exit(systemExitStatus); }
#endif

#define UNUSED(first, ...) (void)first __VA_OPT__(;(void))__VA_ARGS__

extern int systemExitStatus;

typedef union Word16
{
    struct
    {
        uint8_t lo;
        uint8_t hi;
    };
    uint16_t value;
} packed(1) Word16;

namespace emu {
struct packed(1) point_t
{
    uint8_t y;
    uint8_t x;
};

template <typename T, typename U>
constexpr bool get_bit(T value, U bit)
{
    return static_cast<bool>(value & (1 << bit));
}

template <typename T, typename U>
constexpr void set_bit(T& value, U bit)
{ 
    value |= (1 << bit);
}

template <typename T, typename U>
constexpr void clear_bit(T& value, U bit)
{ 
    value &= ~(1 << bit); 
}

template <typename T, typename U> 
void assign(T *dst, U src)
{
    require_or(dst, return);
    *dst = static_cast<T>(src);
}

template <typename T, typename U, typename V>
constexpr bool in_range(T value, U min, V max)
{
    return ((value >= static_cast<T>(min)) && (value < static_cast<T>(max)));
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
bool add_overflow(T op0, T op1, T *res = nullptr)
{
    T tmp_res;
    auto ovf = __builtin_add_overflow(op0, op1, &tmp_res);
    assign(res, tmp_res);
    return ovf;
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
bool add3_overflow(T op0, T op1, T op2, T *res = nullptr)
{
    T tmp_res;
    auto ovf = __builtin_add_overflow(op0, op1, &tmp_res) | __builtin_add_overflow(tmp_res, op2, &tmp_res);
    assign(res, tmp_res);
    return ovf;
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
bool sub_overflow(T op0, T op1, T *res = nullptr)
{
    T tmp_res;
    auto ovf = __builtin_sub_overflow(op0, op1, &tmp_res);
    assign(res, tmp_res);
    return ovf;
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
bool sub3_overflow(T op0, T op1, T op2, T *res = nullptr)
{
    T tmp_res;
    auto ovf = __builtin_sub_overflow(op0, op1, &tmp_res) | __builtin_sub_overflow(tmp_res, op2, &tmp_res);
    assign(res, tmp_res);
    return ovf;
}

template <typename T>
void swap(T u, T v)
{
    u ^= v;
    u ^= v;
    u ^= v;
}

template <typename T, typename U> 
T min(T u, U v)
{
    return (u < static_cast<T>(v)) ? u : static_cast<T>(v);
}

inline char to_upper(char c)
{
    return ('a' <= c && c <= 'z') ? c - 'a' + 'A' : c;
}

template <typename T, size_t kCapacity>
class circular_queue: public std::queue<T>
{
public:
    void enqueue(T newValue)
    {
        if (this->size() == kCapacity)
        {
            this->pop();
        }
        this->push(newValue);
    }
    
    T dequeue()
    {
        auto fnt = this->front();
        this->pop();
        return std::move(fnt);
    }
    
    void clear()
    {
        std::queue<T> empty;
        std::swap(*this, empty);
    }
};
} // namespace emu

#endif /* _PLATFORM_H_ */