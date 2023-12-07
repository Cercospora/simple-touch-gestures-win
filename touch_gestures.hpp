#pragma once

#include <windows.h>
#include <cstdint>
#include <complex>

#include <chrono>
using time_abs = std::chrono::time_point<std::chrono::high_resolution_clock>;
using time_rel = std::chrono::duration<double, std::micro>;
static inline auto duration_ms(time_rel x) { return std::chrono::duration_cast<time_rel>(x); }
static inline auto TIC() { return std::chrono::high_resolution_clock::now(); }
static inline auto TOC(time_abs t) { return duration_ms(TIC() - t); }

constexpr size_t MAX_TOUCH_INPUTS = 10;

struct POINTD
{
    double x, y;
    friend POINTD operator+(POINTD a, const POINTD &b)
    {
        a.x += b.x;
        a.y += b.y;
        return a;
    }
    friend POINTD operator-(POINTD a, const POINTD &b)
    {
        a.x -= b.x;
        a.y -= b.y;
        return a;
    }
};

struct TSTATE
{
    POINTD pos = {0};
    POINT client = {0};
    DWORD event = 0;
    DWORD dwTime = 0;
};
