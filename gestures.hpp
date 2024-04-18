#pragma once

#include "touch_gestures.hpp"
#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>
#include <numbers>

constexpr double PI = std::numbers::pi;

enum class TGESTURE
{
    NONE = 0,

    INPUT1 = 1 << 8,
    INPUT2 = 2 << 8,
    INPUT3 = 3 << 8,
    INPUT4 = 4 << 8,
    INPUT5 = 5 << 8,
    INPUT_MASK = 0xff << 8,

    POS_TOP_HALF = 1 << 16,
    POS_BOTTOM_HALF = 2 << 16,
    POS_LEFT_HALF = 3 << 16,
    POS_RIGHT_HALF = 4 << 16,
    POS_TOP_EDGE = 5 << 16,
    POS_BOTTOM_EDGE = 6 << 16,
    POS_LEFT_EDGE = 7 << 16,
    POS_RIGHT_EDGE = 8 << 16,
    POS_MASK = 0xff << 16,

    TAP = 1,
    SWIPE_LEFT = 2,
    SWIPE_RIGHT = 3,
    SWIPE_UP = 4,
    SWIPE_DOWN = 5,
    ACTION_MASK = 0xff,

    TAP1 = TAP | INPUT1,
    SWIPE1_LEFT = SWIPE_LEFT | INPUT1,
    SWIPE1_RIGHT = SWIPE_RIGHT | INPUT1,
    SWIPE1_UP = SWIPE_UP | INPUT1,
    SWIPE1_DOWN = SWIPE_DOWN | INPUT1,

    TAP2 = TAP | INPUT2,
    SWIPE2_LEFT = SWIPE_LEFT | INPUT2,
    SWIPE2_RIGHT = SWIPE_RIGHT | INPUT2,
    SWIPE2_UP = SWIPE_UP | INPUT2,
    SWIPE2_DOWN = SWIPE_DOWN | INPUT2,

    TAP3 = TAP | INPUT3,
    SWIPE3_LEFT = SWIPE_LEFT | INPUT3,
    SWIPE3_RIGHT = SWIPE_RIGHT | INPUT3,
    SWIPE3_UP = SWIPE_UP | INPUT3,
    SWIPE3_DOWN = SWIPE_DOWN | INPUT3,
};

inline double cdistance2(const POINTD &a, const POINTD &b)
{
    double x = a.x - b.x;
    double y = a.y - b.y;
    return x * x + y * y;
}

inline POINTD xy2rt(const POINTD &d)
{
    return {sqrt(d.x * d.x + d.y * d.y), atan2(d.y, d.x)};
}

inline std::vector<std::pair<const TSTATE *, const TSTATE *>> FindStartEnd(
    const std::vector<TSTATE> pattern[MAX_TOUCH_INPUTS],
    const int num_max_inputs)
{
    std::vector<std::pair<const TSTATE *, const TSTATE *>> x(num_max_inputs);
    x[0].first = &pattern[0].front();
    x[0].second = &pattern[0].back();

    for (int n = 1; n < num_max_inputs; n++)
    {
        size_t i = 0;
        for (; i < pattern[n].size(); i++)
        {
            if (pattern[n][i].dwTime != 0)
            {
                x[n].first = &pattern[n][i];
                x[n].second = &pattern[n][i];
                break;
            }
        }
        for (; i < pattern[n].size(); i++)
        {
            if (pattern[n][i].dwTime == 0)
            {
                break;
            }
            x[n].second = &pattern[n][i];
        }
    }

    if (num_max_inputs == 2)
    {
        double d1 = cdistance2(x[0].first->pos, x[0].second->pos) + cdistance2(x[1].first->pos, x[1].second->pos);
        double d2 = cdistance2(x[0].first->pos, x[1].second->pos) + cdistance2(x[1].first->pos, x[0].second->pos);
        if (d2 < d1)
        {
            std::swap(x[0].second, x[1].second);
        }
    }

    return x;
}

inline TGESTURE DecodeGesture(
    const std::vector<TSTATE> pattern[MAX_TOUCH_INPUTS],
    const int num_max_inputs,
    const int tap_tolerance = 10)
{
    if (num_max_inputs == 0 || pattern[0].size() < 2)
    {
        return TGESTURE::NONE;
    }

    auto startend = FindStartEnd(pattern, num_max_inputs);

    // const int cx = GetSystemMetrics(SM_CXSCREEN);
    // const int cy = GetSystemMetrics(SM_CYSCREEN);

    TGESTURE ges[MAX_TOUCH_INPUTS] = {TGESTURE::NONE};

    for (int n = 0; n < num_max_inputs; n++)
    {
        const auto &first = *startend[n].first;
        const auto &last = *startend[n].second;
        POINTD delta = xy2rt(last.pos - first.pos);
        printf("Delta R,theta = (%lf, %lf)\n", delta.x, delta.y);
        // x: left -, right +, y: up -, down +
        if (abs(delta.x) < tap_tolerance)
        {
            ges[n] = TGESTURE::TAP;
        }
        else if (delta.y >= -PI / 4 && delta.y < PI / 4)
        {
            ges[n] = TGESTURE::SWIPE_LEFT;
        }
        else if (delta.y >= PI / 4 && delta.y < 3 * PI / 4)
        {
            ges[n] = TGESTURE::SWIPE_DOWN;
        }
        else if (delta.y >= -3 * PI / 4 && delta.y < -PI / 4)
        {
            ges[n] = TGESTURE::SWIPE_UP;
        }
        else
        {
            ges[n] = TGESTURE::SWIPE_RIGHT;
        }
    }

    bool alleq = true;
    for (int n = 1; n < num_max_inputs; n++)
    {
        alleq &= ges[n] == ges[n - 1];
    }
    if (alleq)
    {
        return (TGESTURE)((int)ges[0] | (num_max_inputs << 8));
    }

    return TGESTURE::NONE;
}

// Actions

inline void MBack(HWND nextWindow)
{
    SetActiveWindow(nextWindow);

    // Alt + Left Arrow
    keybd_event(VK_MENU, 0, 0, 0);
    keybd_event(VK_LEFT, 0, 0, 0);
    keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
}

inline void MForward(HWND nextWindow)
{
    SetActiveWindow(nextWindow);

    // Alt + Right Arrow
    keybd_event(VK_MENU, 0, 0, 0);
    keybd_event(VK_RIGHT, 0, 0, 0);
    keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
}

inline void MTaskView()
{
    // Win + Tab
    keybd_event(VK_LWIN, 0, 0, 0);
    keybd_event(VK_TAB, 0, 0, 0);
    keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
}

inline void MStartMenu()
{
    // Win
    keybd_event(VK_LWIN, 0, 0, 0);
    keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
}

inline void MOnScreenKeyboardLegacy()
{
    // Win
    system(R"(%windir%\system32\osk.exe)");
}

inline void MCloseTab(HWND nextWindow)
{
    SetActiveWindow(nextWindow);

    // Ctrl + W
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event('W', 0, 0, 0);
    keybd_event('W', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

inline void MOpenTab(HWND nextWindow)
{
    SetActiveWindow(nextWindow);

    // Ctrl + T
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event('T', 0, 0, 0);
    keybd_event('T', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

inline void MReOpenTab(HWND nextWindow)
{
    SetActiveWindow(nextWindow);

    // Ctrl + Shift + T
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event(VK_SHIFT, 0, 0, 0);
    keybd_event('T', 0, 0, 0);
    keybd_event('T', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

inline void MCloseApp(HWND nextWindow)
{
    SetActiveWindow(nextWindow);

    // Alt + F4
    keybd_event(VK_MENU, 0, 0, 0);
    keybd_event(VK_F4, 0, 0, 0);
    keybd_event(VK_F4, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
}

inline void MNextTab(HWND nextWindow)
{
    SetActiveWindow(nextWindow);

    // Ctrl + Tab
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event(VK_TAB, 0, 0, 0);
    keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

inline void MPreviousTab(HWND nextWindow)
{
    SetActiveWindow(nextWindow);

    // Ctrl + Shift + Tab
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event(VK_SHIFT, 0, 0, 0);
    keybd_event(VK_TAB, 0, 0, 0);
    keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}