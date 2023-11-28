#pragma once

#include "touch_gestures.hpp"
#include <vector>
#include <thread>

enum class TGESTURE
{
    NONE,
    SWIPE_LEFT,
    SWIPE_RIGHT,
    SWIPE_UP,
    SWIPE_DOWN
};

inline TGESTURE DecodeGesture(const std::vector<TSTATE> &pattern, UINT i = 0)
{
    if (pattern.empty())
    {
        return TGESTURE::NONE;
    }

    const auto &first = pattern.front();
    const auto &last = pattern.back();
    POINT delta;
    delta.x = last.pos[i].x - first.pos[i].x;
    delta.y = last.pos[i].y - first.pos[i].y;
    // printf("Touch Delta %u at (%ld, %ld)\n", i, delta.x, delta.y);
    // x: left -, right +, up -, down +
    if (abs(delta.x) > abs(delta.y))
    {
        if (delta.x < -10)
        {
            return TGESTURE::SWIPE_RIGHT;
        }
        else if (delta.x > 10)
        {
            return TGESTURE::SWIPE_LEFT;
        }
    }
    else
    {
        if (delta.y < -10)
        {
            return TGESTURE::SWIPE_UP;
        }
        else if (delta.y > 10)
        {
            return TGESTURE::SWIPE_DOWN;
        }
    }
    return TGESTURE::NONE;
}

inline void MBack(HWND nextWindow)
{
    SetActiveWindow(nextWindow);
    auto f = [nextWindow]()
    {
        Sleep(10);
        // Alt + Left Arrow
        keybd_event(VK_MENU, 0, 0, 0);
        keybd_event(VK_LEFT, 0, 0, 0);
        keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
    };
    std::thread thr(f);
    thr.detach();
}

inline void MForward(HWND nextWindow)
{
    SetActiveWindow(nextWindow);
    auto f = [nextWindow]()
    {
        Sleep(10);
        // Alt + Right Arrow
        keybd_event(VK_MENU, 0, 0, 0);
        keybd_event(VK_RIGHT, 0, 0, 0);
        keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
    };
    std::thread thr(f);
    thr.detach();
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

inline void MOnScreenKeyboard()
{
    // Win
    system(R"(%windir%\system32\osk.exe)");
}

inline void MCloseTab(HWND nextWindow)
{
    SetActiveWindow(nextWindow);
    auto f = [nextWindow]()
    {
        Sleep(10);
        // Ctrl + W
        keybd_event(VK_CONTROL, 0, 0, 0);
        keybd_event('W', 0, 0, 0);
        keybd_event('W', 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    };
    std::thread thr(f);
    thr.detach();
}



inline void MReOpenTab(HWND nextWindow)
{
    SetActiveWindow(nextWindow);
    auto f = [nextWindow]()
    {
        Sleep(10);
        // Ctrl + Shift + T
        keybd_event(VK_CONTROL, 0, 0, 0);
        keybd_event(VK_SHIFT, 0, 0, 0);
        keybd_event('T', 0, 0, 0);
        keybd_event('T', 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    };
    std::thread thr(f);
    thr.detach();
}