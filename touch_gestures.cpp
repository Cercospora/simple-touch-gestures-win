#include "touch_gestures.hpp"
#include "gestures.hpp"
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cmath>

static TSTATE mbstate;
static std::vector<TSTATE> mbDownPattern;

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_MOUSEACTIVATE:
        printf("WM_MOUSEACTIVATE\n");
        PostQuitMessage(0);
        break;
    case WM_TOUCH:
    {
        UINT cInputs = LOWORD(wParam); // Number of touch points
        if (cInputs > 1)
        {
            PostQuitMessage(0);
            break;
        }

        TOUCHINPUT pInputs[MAX_TOUCH_INPUTS];
        cInputs = std::min<size_t>(MAX_TOUCH_INPUTS, cInputs);
        if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT)))
        {
            mbstate.cInputs = cInputs;
            // Process the touch points
            for (UINT i = 0; i < 1; i++)
            {
                const TOUCHINPUT &ti = pInputs[i];
                mbstate.pos[i] = {ti.x / 100, ti.y / 100};
                ScreenToClient(hwnd, &mbstate.pos[i]);
                mbstate.dwTime = ti.dwTime;
                mbDownPattern.push_back(mbstate);

                // Determine the type of touch event
                if (ti.dwFlags & TOUCHEVENTF_DOWN)
                {
                    // printf("Touch Down %u at (%ld, %ld)\n", i, mbstate.pos[i].x, mbstate.pos[i].y);
                }
                if (ti.dwFlags & TOUCHEVENTF_MOVE)
                {
                    // printf("Touch Move %u at (%ld, %ld)\n", i, mbstate.pos[i].x, mbstate.pos[i].y);
                }
                if (ti.dwFlags & TOUCHEVENTF_UP)
                {
                    // printf("Touch Up %u at (%ld, %ld)\n", i, mbstate.pos[i].x, mbstate.pos[i].y);
                    HWND nextWindow = GetNextWindow(hwnd, GW_HWNDNEXT);
                    ShowWindow(hwnd, SW_MINIMIZE);
                    TGESTURE ges = DecodeGesture(mbDownPattern, i);
                    switch (ges)
                    {
                    case TGESTURE::SWIPE_LEFT:
                        printf("LEFT\n");
                        MBack(nextWindow);
                        break;
                    case TGESTURE::SWIPE_RIGHT:
                        printf("RIGHT\n");
                        MForward(nextWindow);
                        break;
                    case TGESTURE::SWIPE_UP:
                        printf("UP\n");
                        MReOpenTab(nextWindow);
                        break;
                    case TGESTURE::SWIPE_DOWN:
                        printf("DOWN\n");
                        MCloseTab(nextWindow);
                        break;
                    default:
                        printf("NONE\n");
                        SetActiveWindow(nextWindow);
                        break;
                    }
                    mbDownPattern.clear();
                }
            }
        }

        CloseTouchInputHandle((HTOUCHINPUT)lParam);
        break;
    }
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProcedure;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "TransparentWindowClass";
    wcex.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_APPWINDOW,
        wcex.lpszClassName,
        TEXT("Transparent Window"),
        WS_POPUP | WS_VISIBLE,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), // primary display size
        NULL, NULL, wcex.hInstance, NULL);
    printf("Primary Display Size %dx%d\n", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

    SetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), 16, LWA_ALPHA);
    RegisterTouchWindow(hwnd, TWF_FINETOUCH);
    if (!hwnd)
    {
        MessageBox(NULL, "RegisterTouchWindow Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, SW_MINIMIZE);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}