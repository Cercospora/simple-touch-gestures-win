#include "touch_gestures.hpp"
#include "gestures.hpp"
#include <vector>
#include <algorithm>
#include <cstdio>

static std::vector<TSTATE> mbDownPattern[MAX_TOUCH_INPUTS];
static UINT numMaxInputs = 0;

#define PRINTACTION(x)      \
    {                       \
        (x);                \
        printf("%s\n", #x); \
    }
void TouchEventDone(HWND hwnd)
{
    HWND nextWindow = GetNextWindow(hwnd, GW_HWNDNEXT);
    ShowWindow(hwnd, SW_MINIMIZE);
    
    const TGESTURE ges = DecodeGesture(mbDownPattern, numMaxInputs);
    switch (ges)
    {
        // 1 finger
    case TGESTURE::TAP1:
        break;
    case TGESTURE::SWIPE1_LEFT:
        PRINTACTION(MBack(nextWindow));
        break;
    case TGESTURE::SWIPE1_RIGHT:
        PRINTACTION(MForward(nextWindow));
        break;
    case TGESTURE::SWIPE1_UP:
        PRINTACTION(MOpenTab(nextWindow));
        break;
    case TGESTURE::SWIPE1_DOWN:
        PRINTACTION(MCloseTab(nextWindow));
        break;

        // 2 finger
    case TGESTURE::TAP2:
        break;
    case TGESTURE::SWIPE2_LEFT:
        PRINTACTION(MPreviousTab(nextWindow));
        break;
    case TGESTURE::SWIPE2_RIGHT:
        PRINTACTION(MNextTab(nextWindow));
        break;
    case TGESTURE::SWIPE2_UP:
        PRINTACTION(MReOpenTab(nextWindow));
        break;
    case TGESTURE::SWIPE2_DOWN:
        PRINTACTION(MCloseApp(nextWindow));
        break;

    case TGESTURE::NONE:
        SetActiveWindow(nextWindow);
        printf("NONE\n");
        break;

    default:
        printf("Unhandled code %d\n", (int)ges);
        break;
    }

    // clear globals
    for (auto &it : mbDownPattern)
    {
        it.clear();
    }
    numMaxInputs = 0;
}
#undef PRINTACTION

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_MOUSEACTIVATE:
        break;
    case WM_TOUCH:
    {
        UINT cInputs = LOWORD(wParam); // Number of touch points
        TOUCHINPUT pInputs[MAX_TOUCH_INPUTS];
        cInputs = std::min<UINT>(MAX_TOUCH_INPUTS, cInputs);
        numMaxInputs = std::max<UINT>(numMaxInputs, cInputs);

        if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT)))
        {
            for (size_t i = 0; i < MAX_TOUCH_INPUTS; i++)
            {
                if (i < cInputs)
                {
                    TOUCHINPUT &ti = pInputs[i];
                    TSTATE mbstate;
                    mbstate.pos = {ti.x / 100., ti.y / 100.};
                    mbstate.client = {ti.x / 100, ti.y / 100};
                    ScreenToClient(hwnd, &mbstate.client);
                    mbstate.dwTime = ti.dwTime;
                    mbstate.event = ti.dwFlags;
                    mbDownPattern[i].push_back(std::move(mbstate));

                    if ((ti.dwFlags & TOUCHEVENTF_UP) && cInputs == 1)
                    {
                        TouchEventDone(hwnd);
                    }
                }
                else
                {
                    mbDownPattern[i].emplace_back();
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

    SetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), 32, LWA_ALPHA);
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